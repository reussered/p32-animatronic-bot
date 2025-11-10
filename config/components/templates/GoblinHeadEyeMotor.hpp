/**
 * @file goblin_head_eye_motor.hpp
 * @brief Stepper-controlled eye movement with mood integration
 * 
 * SUBSYSTEM: goblin_head (ESP32-S3 R8N16)
 * 
 * ARCHITECTURE:
 * - Left/right eye pan: NEMA17_STANDARD stepper (dual-axis)
 * - Up/down eye tilt: NEMA14_STANDARD stepper
 * - Blink: NEMA8_GEARED_5 linear actuator (eyelid)
 * - Display: GC9A01 240×240 RGB565
 * - Mood affects eye color, pupil dilation
 * 
 * MEMORY:
 * - Frame buffer: 115.2 KB (240×240×2 RGB565)
 * - Motor state: 144 bytes
 * - Display state: 32 bytes
 * Total: ~115.5 KB (within 300 KB budget)
 * 
 * TIMING:
 * - Eye update: 30 Hz (32 ms)
 * - Motor step: variable (depends on speed)
 * - Display refresh: 60 Hz (via display driver)
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "config/components/templates/goblin_eye_mood_display_v2.hpp"
#include "config/components/hardware/stepper_motor_library.hpp"
#include "shared/Mood.hpp"

class GoblinHeadEyeMotor {
public:
    // Pan/tilt limits (degrees)
    static constexpr int16_t PAN_MIN = -45;
    static constexpr int16_t PAN_MAX = 45;
    static constexpr int16_t TILT_MIN = -30;
    static constexpr int16_t TILT_MAX = 30;
    
    // Blink parameters
    static constexpr uint16_t BLINK_CLOSE_SPEED_MS = 100;  // Close eyelid in 100ms
    static constexpr uint16_t BLINK_OPEN_SPEED_MS = 80;    // Open in 80ms
    static constexpr uint16_t BLINK_HOLD_MS = 50;          // Hold closed for 50ms
    
    struct State {
        int16_t pan_angle;          // Current pan angle (degrees)
        int16_t tilt_angle;         // Current tilt angle (degrees)
        int16_t pan_target;         // Target pan angle
        int16_t tilt_target;        // Target tilt angle
        uint8_t eyelid_position;    // 0=open, 255=closed
        uint8_t eyelid_target;
        uint32_t last_blink_time;   // Timestamp of last blink (milliseconds)
        uint32_t blink_interval_ms; // Time between blinks (randomized)
        bool is_blinking;           // Currently executing blink
        uint8_t gaze_style;         // 0=neutral, 1=focused, 2=wandering
    };
    
    GoblinHeadEyeMotor()
        : initialized(false), frame_buffer(nullptr),
          pan_controller(STEPPER_MOTORS[NEMA17_STANDARD]),
          tilt_controller(STEPPER_MOTORS[NEMA14_STANDARD]),
          blink_controller(STEPPER_MOTORS[NEMA8_GEARED_5])
    {
        memset(&state, 0, sizeof(state));
        state.pan_angle = 0;
        state.tilt_angle = 0;
        state.eyelid_position = 0;  // Eyes open
        state.eyelid_target = 0;
        state.blink_interval_ms = 3000;  // Blink every 3 seconds
        state.gaze_style = 0;  // Neutral
    }
    
    /**
     * Initialize eye motor system
     * @param display_callback Function to send frame to display
     * @param pan_step_pin GPIO pin for pan stepper STEP
     * @param pan_dir_pin GPIO pin for pan stepper DIRECTION
     * @param tilt_step_pin GPIO pin for tilt stepper STEP
     * @param tilt_dir_pin GPIO pin for tilt stepper DIRECTION
     * @param blink_step_pin GPIO pin for blink stepper STEP
     * @param blink_dir_pin GPIO pin for blink stepper DIRECTION
     */
    bool init(
        GoblinEyeMoodDisplay::SendFrameCallback display_callback,
        uint8_t pan_step_pin, uint8_t pan_dir_pin,
        uint8_t tilt_step_pin, uint8_t tilt_dir_pin,
        uint8_t blink_step_pin, uint8_t blink_dir_pin
    ) {
        if (initialized) return false;
        if (!display_callback) return false;
        
        // Initialize display (240×240 GC9A01 RGB565)
        if (!eye_display.init(240, 240, RGB565_FORMAT, display_callback)) {
            return false;
        }
        
        // Initialize stepper controllers
        if (!pan_controller.initStepDir(pan_step_pin, pan_dir_pin)) {
            return false;
        }
        pan_controller.setSpeed(100);  // 100 RPM default
        
        if (!tilt_controller.initStepDir(tilt_step_pin, tilt_dir_pin)) {
            return false;
        }
        tilt_controller.setSpeed(80);
        
        if (!blink_controller.initStepDir(blink_step_pin, blink_dir_pin)) {
            return false;
        }
        blink_controller.setSpeed(200);  // Faster for blink
        
        // Calculate stepper positions from angle limits
        // NEMA17: 200 steps/rev → ~1.8° per step
        // Pan range: -45° to +45° = 90° total = ~50 steps
        // Position 0 = center
        updateMotorTargets();
        
        initialized = true;
        return true;
    }
    
    /**
     * Main activity loop - call every 30ms or so
     */
    void act(const Mood& current_mood, uint32_t now_ms) {
        if (!initialized) return;
        
        // Update blink state machine
        updateBlink(now_ms);
        
        // Update motor positions (in component loop, handled by dispatcher)
        // TODO: Motor steps dispatched via GPIO ISR
        
        // Render eye display with current mood
        eye_display.renderFrame(current_mood, 0x00FF00);  // Green base
        eye_display.sendToDisplay();
    }
    
    /**
     * Set target gaze position (degrees)
     */
    void lookAt(int16_t pan_deg, int16_t tilt_deg) {
        // Clamp to limits
        state.pan_target = (pan_deg < PAN_MIN) ? PAN_MIN : 
                          (pan_deg > PAN_MAX) ? PAN_MAX : pan_deg;
        state.tilt_target = (tilt_deg < TILT_MIN) ? TILT_MIN : 
                           (tilt_deg > TILT_MAX) ? TILT_MAX : tilt_deg;
        
        updateMotorTargets();
    }
    
    /**
     * Trigger blink animation
     */
    void blink() {
        if (!state.is_blinking) {
            state.is_blinking = true;
            state.eyelid_target = 255;  // Close
        }
    }
    
    /**
     * Set gaze behavior
     * 0 = neutral/fixed, 1 = focused/intense, 2 = wandering/curious
     */
    void setGazeStyle(uint8_t style) {
        state.gaze_style = style % 3;
    }
    
    /**
     * Get current eye state
     */
    const State& getState() const { return state; }
    
    /**
     * Get display object
     */
    GoblinEyeMoodDisplay& getDisplay() { return eye_display; }
    
    /**
     * Get stepper controller (for advanced control)
     */
    StepperController& getPanController() { return pan_controller; }
    StepperController& getTiltController() { return tilt_controller; }
    StepperController& getBlinkController() { return blink_controller; }
    
    ~GoblinHeadEyeMotor() {
        // Display auto-cleans up frame_buffer
    }
    
private:
    bool initialized;
    uint8_t* frame_buffer;
    State state;
    
    GoblinEyeMoodDisplay eye_display;
    StepperController pan_controller;
    StepperController tilt_controller;
    StepperController blink_controller;
    
    /**
     * Convert target angles to motor step positions
     */
    void updateMotorTargets() {
        // NEMA17 Standard: 200 steps/rev, 1.8° per step
        // Range: -45° to +45° → 25 to -25 steps from center (1.8° per step ≈ 25 steps per 45°)
        // Tilt: ±30° → ±16.67 steps (NEMA14 similar)
        
        int32_t pan_steps = (state.pan_target * 100) / 180;  // Rough conversion
        int32_t tilt_steps = (state.tilt_target * 100) / 180;
        
        pan_controller.moveTo(pan_steps);
        tilt_controller.moveTo(tilt_steps);
    }
    
    /**
     * Blink state machine
     */
    void updateBlink(uint32_t now_ms) {
        if (state.is_blinking) {
            // Move eyelid toward target
            if (state.eyelid_position < state.eyelid_target) {
                state.eyelid_position++;
            } else if (state.eyelid_position > state.eyelid_target) {
                state.eyelid_position--;
            } else {
                // Reached target
                if (state.eyelid_target == 255) {
                    // Eyes closed, hold for BLINK_HOLD_MS then open
                    static uint32_t blink_close_time = 0;
                    if (blink_close_time == 0) {
                        blink_close_time = now_ms;
                    }
                    if (now_ms - blink_close_time > BLINK_HOLD_MS) {
                        state.eyelid_target = 0;  // Open
                        blink_close_time = 0;
                    }
                } else {
                    // Eyes fully open, blink complete
                    state.is_blinking = false;
                    state.last_blink_time = now_ms;
                    // Randomize next blink (2.5 - 5.0 seconds)
                    state.blink_interval_ms = 2500 + (now_ms % 2500);
                }
            }
        } else {
            // Check if time for auto-blink
            if (now_ms - state.last_blink_time > state.blink_interval_ms) {
                blink();
            }
        }
    }
};

#endif // GOBLIN_HEAD_EYE_MOTOR_HPP
