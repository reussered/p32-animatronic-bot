/**
 * @file goblin_head_mouth_motor.hpp
 * @brief Stepper-controlled mouth animation with mood integration
 * 
 * SUBSYSTEM: goblin_head (ESP32-S3 R8N16)
 * 
 * ARCHITECTURE:
 * - Jaw rotation: NEMA17_HIGHTORQUE stepper (vertical hinge)
 * - Mouth corner L/R: NEMA14_STANDARD stepper each (smile/frown)
 * - Cheek L/R puff: NEMA8_GEARED_5 stepper each (linear)
 * - Display: ILI9341 480×320 RGB666 (chunked rendering)
 * - Mood affects mouth color, expression intensity
 * 
 * MEMORY:
 * - Chunk buffer: 115.2 KB (480×80×3 RGB666)
 * - Motor state: 192 bytes (5 motors)
 * - Display state: 48 bytes
 * Total: ~115.5 KB (within 300 KB budget)
 * 
 * TIMING:
 * - Mouth update: 30 Hz (32 ms)
 * - Motor step: variable per motor
 * - Display refresh: 4 chunks × 30 Hz = 120 chunk updates/sec
 * 
 * EXPRESSIONS:
 * - Smile: corners up, cheeks puff, jaw neutral
 * - Frown: corners down, cheeks deflate, jaw neutral
 * - Open: jaw down, corners center, normal cheeks
 * - Shocked: jaw dropped, corners wide, eyes dilated
 * - Angry: jaw tight, corners down, cheeks up
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "config/components/templates/goblin_mouth_mood_display_v2.hpp"
#include "config/components/hardware/stepper_motor_library.hpp"
#include "shared/Mood.hpp"

class GoblinHeadMouthMotor {
public:
    // Motion ranges (degrees or mm)
    static constexpr int16_t JAW_OPEN_MIN = 0;
    static constexpr int16_t JAW_OPEN_MAX = 60;
    static constexpr int16_t CORNER_DEFLECT_MIN = -30;
    static constexpr int16_t CORNER_DEFLECT_MAX = 30;
    static constexpr int16_t CHEEK_PUFF_MIN = 0;
    static constexpr int16_t CHEEK_PUFF_MAX = 20;
    
    // Expression presets
    enum Expression {
        NEUTRAL = 0,
        SMILE = 1,
        FROWN = 2,
        OPEN_LAUGH = 3,
        SHOCK = 4,
        ANGRY = 5,
        CONFUSED = 6,
        SKEPTICAL = 7,
    };
    
    struct State {
        int16_t jaw_open;           // 0=closed, 60=wide open (degrees)
        int16_t corner_left;        // Left corner position (-30=down to +30=up)
        int16_t corner_right;       // Right corner position
        int16_t cheek_left_puff;    // Left cheek puff (0=normal, 20=puffed)
        int16_t cheek_right_puff;   // Right cheek puff
        
        // Targets
        int16_t jaw_target;
        int16_t corner_left_target;
        int16_t corner_right_target;
        int16_t cheek_left_target;
        int16_t cheek_right_target;
        
        uint32_t last_expression_time;
        Expression current_expression;
        uint8_t expression_intensity;  // 0-255
    };
    
    GoblinHeadMouthMotor()
        : initialized(false),
          jaw_controller(STEPPER_MOTORS[NEMA17_HIGHTORQUE]),
          corner_l_controller(STEPPER_MOTORS[NEMA14_STANDARD]),
          corner_r_controller(STEPPER_MOTORS[NEMA14_STANDARD]),
          cheek_l_controller(STEPPER_MOTORS[NEMA8_GEARED_5]),
          cheek_r_controller(STEPPER_MOTORS[NEMA8_GEARED_5])
    {
        memset(&state, 0, sizeof(state));
        state.current_expression = NEUTRAL;
        state.expression_intensity = 128;
    }
    
    /**
     * Initialize mouth motor system
     * @param display_callback Function to send chunks to display
     * @param jaw_step_pin GPIO pin for jaw stepper STEP
     * @param jaw_dir_pin GPIO pin for jaw stepper DIRECTION
     * @param corner_l_step_pin GPIO pin for left corner stepper STEP
     * @param corner_l_dir_pin GPIO pin for left corner stepper DIRECTION
     * @param corner_r_step_pin GPIO pin for right corner stepper STEP
     * @param corner_r_dir_pin GPIO pin for right corner stepper DIRECTION
     * @param cheek_l_step_pin GPIO pin for left cheek stepper STEP
     * @param cheek_l_dir_pin GPIO pin for left cheek stepper DIRECTION
     * @param cheek_r_step_pin GPIO pin for right cheek stepper STEP
     * @param cheek_r_dir_pin GPIO pin for right cheek stepper DIRECTION
     */
    bool init(
        GoblinMouthMoodDisplay::SendChunkCallback display_callback,
        uint8_t jaw_step_pin, uint8_t jaw_dir_pin,
        uint8_t corner_l_step_pin, uint8_t corner_l_dir_pin,
        uint8_t corner_r_step_pin, uint8_t corner_r_dir_pin,
        uint8_t cheek_l_step_pin, uint8_t cheek_l_dir_pin,
        uint8_t cheek_r_step_pin, uint8_t cheek_r_dir_pin
    ) {
        if (initialized) return false;
        if (!display_callback) return false;
        
        // Initialize display (480×320 ILI9341 RGB666 with chunking)
        // Chunk height: 80 pixels (480×80×3 = 115.2 KB buffer)
        if (!mouth_display.init(480, 320, RGB666_FORMAT, 
                                true, 80, display_callback)) {
            return false;
        }
        
        // Initialize jaw controller (main jaw rotation)
        if (!jaw_controller.initStepDir(jaw_step_pin, jaw_dir_pin)) {
            return false;
        }
        jaw_controller.setSpeed(120);  // 120 RPM
        
        // Initialize corner controllers (L/R mouth corners)
        if (!corner_l_controller.initStepDir(corner_l_step_pin, corner_l_dir_pin)) {
            return false;
        }
        corner_l_controller.setSpeed(100);
        
        if (!corner_r_controller.initStepDir(corner_r_step_pin, corner_r_dir_pin)) {
            return false;
        }
        corner_r_controller.setSpeed(100);
        
        // Initialize cheek controllers (puff effects)
        if (!cheek_l_controller.initStepDir(cheek_l_step_pin, cheek_l_dir_pin)) {
            return false;
        }
        cheek_l_controller.setSpeed(150);  // Faster for puff
        
        if (!cheek_r_controller.initStepDir(cheek_r_step_pin, cheek_r_dir_pin)) {
            return false;
        }
        cheek_r_controller.setSpeed(150);
        
        initialized = true;
        return true;
    }
    
    /**
     * Main activity loop - call every 30ms or so
     */
    void act(const Mood& current_mood, uint32_t now_ms) {
        if (!initialized) return;
        
        // Update expression based on mood
        updateExpressionFromMood(current_mood);
        
        // Smooth interpolation toward targets
        interpolateMotors();
        
        // Update motor positions
        updateMotorTargets();
        
        // Render mouth display chunks (if needed)
        mouth_display.resetChunkIndex();
        while (mouth_display.renderNextChunk(current_mood, 0xFF6600)) {
            // TODO: Chunk callback dispatched by display
            mouth_display.sendChunkToDisplay();
        }
    }
    
    /**
     * Set expression with intensity
     */
    void setExpression(Expression expr, uint8_t intensity = 200) {
        state.current_expression = expr;
        state.expression_intensity = intensity;
        state.last_expression_time = 0;  // Will transition immediately
        
        // Set motor targets based on expression
        updateExpressionMotorTargets(expr, intensity);
    }
    
    /**
     * Speak animation (jaw bobbing for phoneme)
     */
    void speakPhoneme(uint8_t phoneme_id) {
        // phoneme_id: 0-20 maps to phonemes (A, E, I, O, U, etc.)
        // Each phoneme has characteristic jaw open/corner positions
        
        switch (phoneme_id % 5) {
            case 0:  // AH - wide open
                state.jaw_target = 50;
                state.corner_left_target = 0;
                state.corner_right_target = 0;
                break;
            case 1:  // EH/IH - medium open
                state.jaw_target = 20;
                state.corner_left_target = 10;
                state.corner_right_target = 10;
                break;
            case 2:  // OH - rounded
                state.jaw_target = 30;
                state.corner_left_target = -5;
                state.corner_right_target = -5;
                break;
            case 3:  // OO - tight
                state.jaw_target = 15;
                state.corner_left_target = -10;
                state.corner_right_target = -10;
                break;
            case 4:  // M/B/P - closed
                state.jaw_target = 5;
                state.corner_left_target = 0;
                state.corner_right_target = 0;
                break;
        }
    }
    
    /**
     * Get current mouth state
     */
    const State& getState() const { return state; }
    
    /**
     * Get display object
     */
    GoblinMouthMoodDisplay& getDisplay() { return mouth_display; }
    
    /**
     * Get stepper controllers (for advanced control)
     */
    StepperController& getJawController() { return jaw_controller; }
    StepperController& getCornerLeftController() { return corner_l_controller; }
    StepperController& getCornerRightController() { return corner_r_controller; }
    StepperController& getCheekLeftController() { return cheek_l_controller; }
    StepperController& getCheekRightController() { return cheek_r_controller; }
    
    ~GoblinHeadMouthMotor() {
        // Display auto-cleans up chunk_buffer
    }
    
private:
    bool initialized;
    State state;
    
    GoblinMouthMoodDisplay mouth_display;
    StepperController jaw_controller;
    StepperController corner_l_controller;
    StepperController corner_r_controller;
    StepperController cheek_l_controller;
    StepperController cheek_r_controller;
    
    /**
     * Map mood to expression with smooth blending
     */
    void updateExpressionFromMood(const Mood& mood) {
        // Determine dominant mood component
        Expression expr = NEUTRAL;
        int8_t dominance = 0;
        
        // Check each mood component
        if (mood.happiness() > dominance) {
            dominance = mood.happiness();
            expr = SMILE;
        }
        if (mood.sadness() > dominance) {
            dominance = mood.sadness();
            expr = FROWN;
        }
        if (mood.anger() > dominance) {
            dominance = mood.anger();
            expr = ANGRY;
        }
        if (mood.excitement() > dominance) {
            dominance = mood.excitement();
            expr = OPEN_LAUGH;
        }
        if (mood.curiosity() > dominance) {
            dominance = mood.curiosity();
            expr = CONFUSED;
        }
        if (mood.fear() > dominance) {
            dominance = mood.fear();
            expr = SHOCK;
        }
        
        // Map dominance to intensity
        uint8_t intensity = (dominance * 255) / 127;
        
        if (expr != state.current_expression) {
            setExpression(expr, intensity);
        } else {
            state.expression_intensity = intensity;
        }
    }
    
    /**
     * Set motor targets based on expression
     */
    void updateExpressionMotorTargets(Expression expr, uint8_t intensity) {
        float scale = intensity / 255.0f;
        
        switch (expr) {
            case SMILE:
                state.jaw_target = 5;
                state.corner_left_target = (int16_t)(20 * scale);
                state.corner_right_target = (int16_t)(20 * scale);
                state.cheek_left_target = (int16_t)(15 * scale);
                state.cheek_right_target = (int16_t)(15 * scale);
                break;
            case FROWN:
                state.jaw_target = 5;
                state.corner_left_target = (int16_t)(-20 * scale);
                state.corner_right_target = (int16_t)(-20 * scale);
                state.cheek_left_target = 0;
                state.cheek_right_target = 0;
                break;
            case OPEN_LAUGH:
                state.jaw_target = (int16_t)(50 * scale);
                state.corner_left_target = (int16_t)(25 * scale);
                state.corner_right_target = (int16_t)(25 * scale);
                state.cheek_left_target = (int16_t)(20 * scale);
                state.cheek_right_target = (int16_t)(20 * scale);
                break;
            case SHOCK:
                state.jaw_target = (int16_t)(55 * scale);
                state.corner_left_target = (int16_t)(15 * scale);
                state.corner_right_target = (int16_t)(15 * scale);
                state.cheek_left_target = 0;
                state.cheek_right_target = 0;
                break;
            case ANGRY:
                state.jaw_target = 5;
                state.corner_left_target = (int16_t)(-15 * scale);
                state.corner_right_target = (int16_t)(-15 * scale);
                state.cheek_left_target = (int16_t)(10 * scale);
                state.cheek_right_target = (int16_t)(10 * scale);
                break;
            case CONFUSED:
                state.jaw_target = 10;
                state.corner_left_target = (int16_t)(5 * scale);
                state.corner_right_target = (int16_t)(-5 * scale);
                state.cheek_left_target = 0;
                state.cheek_right_target = 0;
                break;
            case SKEPTICAL:
                state.jaw_target = 8;
                state.corner_left_target = (int16_t)(-10 * scale);
                state.corner_right_target = (int16_t)(15 * scale);
                state.cheek_left_target = (int16_t)(8 * scale);
                state.cheek_right_target = 0;
                break;
            case NEUTRAL:
            default:
                state.jaw_target = 0;
                state.corner_left_target = 0;
                state.corner_right_target = 0;
                state.cheek_left_target = 0;
                state.cheek_right_target = 0;
                break;
        }
    }
    
    /**
     * Smooth interpolation toward targets (easing)
     */
    void interpolateMotors() {
        const float INTERP_FACTOR = 0.15f;  // Smooth easing
        
        state.jaw_open += (int16_t)((state.jaw_target - state.jaw_open) * INTERP_FACTOR);
        state.corner_left += (int16_t)((state.corner_left_target - state.corner_left) * INTERP_FACTOR);
        state.corner_right += (int16_t)((state.corner_right_target - state.corner_right) * INTERP_FACTOR);
        state.cheek_left_puff += (int16_t)((state.cheek_left_target - state.cheek_left_puff) * INTERP_FACTOR);
        state.cheek_right_puff += (int16_t)((state.cheek_right_target - state.cheek_right_puff) * INTERP_FACTOR);
    }
    
    /**
     * Update stepper motor positions from state
     */
    void updateMotorTargets() {
        // Convert angles/positions to stepper steps
        // NEMA17_HIGHTORQUE: 200 steps/rev
        // JAW: 60° max → ~33 steps
        int32_t jaw_steps = (state.jaw_open * 200) / 360;  // steps = (degrees * 200) / 360
        
        // NEMA14_STANDARD: 200 steps/rev
        // CORNER: ±30° → ±16.67 steps
        int32_t corner_l_steps = (state.corner_left * 100) / 180;
        int32_t corner_r_steps = (state.corner_right * 100) / 180;
        
        // NEMA8_GEARED_5: higher resolution, 48 * 5 = 240 steps/rev
        // CHEEK: 20mm max → ~240 steps/rev means ~12 steps per mm
        int32_t cheek_l_steps = state.cheek_left_puff * 12;
        int32_t cheek_r_steps = state.cheek_right_puff * 12;
        
        jaw_controller.moveTo(jaw_steps);
        corner_l_controller.moveTo(corner_l_steps);
        corner_r_controller.moveTo(corner_r_steps);
        cheek_l_controller.moveTo(cheek_l_steps);
        cheek_r_controller.moveTo(cheek_r_steps);
    }
};

#endif // GOBLIN_HEAD_MOUTH_MOTOR_HPP
