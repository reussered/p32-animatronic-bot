/**
 * @file goblin_head_neck_motor.hpp
 * @brief Stepper-controlled neck articulation with smooth motion
 * 
 * SUBSYSTEM: goblin_head → torso interface (ESP32-S3 R8N16 on head)
 * 
 * ARCHITECTURE:
 * - Neck pan (left-right): NEMA23_STANDARD stepper (heavy duty)
 * - Neck tilt (up-down): NEMA17_HIGHTORQUE stepper
 * - Neck roll (shoulder): NEMA14_STANDARD stepper
 * - Full 3-DOF articulation via high-ratio gearing
 * 
 * MEMORY:
 * - Motor state: 96 bytes
 * - Interpolation buffer: 32 bytes
 * Total: ~130 bytes (negligible)
 * 
 * TIMING:
 * - Update: 30 Hz (32 ms)
 * - Smooth motion: exponential easing over 200-500ms
 * 
 * MOTION RANGES:
 * - Pan: ±60° (left-right turn)
 * - Tilt: -30° to +45° (look down/up)
 * - Roll: ±25° (shoulder tilt)
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <cmath>
#include "config/components/hardware/stepper_motor_library.hpp"

class GoblinHeadNeckMotor {
public:
    // Motion ranges (degrees)
    static constexpr int16_t PAN_MIN = -60;
    static constexpr int16_t PAN_MAX = 60;
    static constexpr int16_t TILT_MIN = -30;
    static constexpr int16_t TILT_MAX = 45;
    static constexpr int16_t ROLL_MIN = -25;
    static constexpr int16_t ROLL_MAX = 25;
    
    // Neck presets (for attention/emotion)
    enum NeckPose {
        CENTER = 0,
        LEFT_TURN = 1,
        RIGHT_TURN = 2,
        DOWN_LOOK = 3,
        UP_LOOK = 4,
        SHY_TILT = 5,
        CONFUSED_TILT = 6,
        AGGRESSIVE = 7,
    };
    
    struct State {
        int16_t pan_angle;          // Left-right rotation (degrees)
        int16_t tilt_angle;         // Up-down rotation (degrees)
        int16_t roll_angle;         // Shoulder tilt (degrees)
        
        int16_t pan_target;
        int16_t tilt_target;
        int16_t roll_target;
        
        uint32_t motion_start_time;
        uint32_t motion_duration_ms;
        bool is_moving;
        
        NeckPose current_pose;
    };
    
    GoblinHeadNeckMotor()
        : initialized(false),
          pan_controller(STEPPER_MOTORS[NEMA23_STANDARD]),
          tilt_controller(STEPPER_MOTORS[NEMA17_HIGHTORQUE]),
          roll_controller(STEPPER_MOTORS[NEMA14_STANDARD])
    {
        memset(&state, 0, sizeof(state));
        state.current_pose = CENTER;
    }
    
    /**
     * Initialize neck motor system
     * @param pan_step_pin GPIO pin for pan stepper STEP
     * @param pan_dir_pin GPIO pin for pan stepper DIRECTION
     * @param tilt_step_pin GPIO pin for tilt stepper STEP
     * @param tilt_dir_pin GPIO pin for tilt stepper DIRECTION
     * @param roll_step_pin GPIO pin for roll stepper STEP
     * @param roll_dir_pin GPIO pin for roll stepper DIRECTION
     */
    bool init(
        uint8_t pan_step_pin, uint8_t pan_dir_pin,
        uint8_t tilt_step_pin, uint8_t tilt_dir_pin,
        uint8_t roll_step_pin, uint8_t roll_dir_pin
    ) {
        if (initialized) return false;
        
        // Initialize pan controller (NEMA23)
        if (!pan_controller.initStepDir(pan_step_pin, pan_dir_pin)) {
            return false;
        }
        pan_controller.setSpeed(150);  // Slower for smooth head motion
        
        // Initialize tilt controller (NEMA17 high-torque)
        if (!tilt_controller.initStepDir(tilt_step_pin, tilt_dir_pin)) {
            return false;
        }
        tilt_controller.setSpeed(100);
        
        // Initialize roll controller (NEMA14)
        if (!roll_controller.initStepDir(roll_step_pin, roll_dir_pin)) {
            return false;
        }
        roll_controller.setSpeed(120);
        
        initialized = true;
        return true;
    }
    
    /**
     * Main activity loop - call every 30ms or so
     */
    void act(uint32_t now_ms) {
        if (!initialized) return;
        
        // Update easing if motion in progress
        if (state.is_moving) {
            updateMotionEasing(now_ms);
        }
        
        // Update motor positions
        updateMotorTargets();
    }
    
    /**
     * Turn head to absolute position
     * @param pan_deg Left-right angle (-60 to +60)
     * @param tilt_deg Up-down angle (-30 to +45)
     * @param roll_deg Shoulder tilt (-25 to +25)
     * @param duration_ms Time to reach position (smooth motion)
     */
    void turnTo(int16_t pan_deg, int16_t tilt_deg, int16_t roll_deg, 
                uint32_t duration_ms = 300) {
        // Clamp inputs
        state.pan_target = clamp(pan_deg, PAN_MIN, PAN_MAX);
        state.tilt_target = clamp(tilt_deg, TILT_MIN, TILT_MAX);
        state.roll_target = clamp(roll_deg, ROLL_MIN, ROLL_MAX);
        
        state.motion_duration_ms = duration_ms;
        state.motion_start_time = 0;  // Will be set on first act()
        
        // Calculate if actually moving
        state.is_moving = (state.pan_angle != state.pan_target) ||
                         (state.tilt_angle != state.tilt_target) ||
                         (state.roll_angle != state.roll_target);
    }
    
    /**
     * Move head to preset pose
     */
    void lookAtPose(NeckPose pose, uint32_t duration_ms = 250) {
        state.current_pose = pose;
        
        int16_t pan = 0, tilt = 0, roll = 0;
        
        switch (pose) {
            case CENTER:
                pan = 0; tilt = 0; roll = 0;
                break;
            case LEFT_TURN:
                pan = -45; tilt = 0; roll = -10;
                break;
            case RIGHT_TURN:
                pan = 45; tilt = 0; roll = 10;
                break;
            case DOWN_LOOK:
                pan = 0; tilt = -25; roll = 0;
                break;
            case UP_LOOK:
                pan = 0; tilt = 35; roll = 0;
                break;
            case SHY_TILT:
                pan = -15; tilt = -10; roll = -20;  // Head down-left with shoulder
                break;
            case CONFUSED_TILT:
                pan = 0; tilt = 15; roll = 20;  // Head up with shoulder tilt
                break;
            case AGGRESSIVE:
                pan = 0; tilt = -15; roll = 5;  // Chin forward, slight shoulder
                break;
        }
        
        turnTo(pan, tilt, roll, duration_ms);
    }
    
    /**
     * Indicate motion (head wobble)
     * @param type 0=nodding yes, 1=shaking no, 2=confused wobble
     * @param amplitude Swing angle (degrees)
     * @param frequency Wobble speed (Hz)
     */
    void performMotion(uint8_t type, uint8_t amplitude, uint8_t frequency) {
        // type 0: pan repeatedly left-right
        // type 1: tilt repeatedly up-down
        // type 2: roll wobble
        
        // TODO: Implement motion sequences via state machine
        // For now, just trigger motion to preset
        (void)type;
        (void)amplitude;
        (void)frequency;
    }
    
    /**
     * Get current neck state
     */
    const State& getState() const { return state; }
    
    /**
     * Get stepper controllers
     */
    StepperController& getPanController() { return pan_controller; }
    StepperController& getTiltController() { return tilt_controller; }
    StepperController& getRollController() { return roll_controller; }
    
    /**
     * Check if motion complete
     */
    bool isMoving() const { return state.is_moving; }
    
    ~GoblinHeadNeckMotor() {}
    
private:
    bool initialized;
    State state;
    
    StepperController pan_controller;
    StepperController tilt_controller;
    StepperController roll_controller;
    
    /**
     * Clamp value to range
     */
    static int16_t clamp(int16_t val, int16_t min, int16_t max) {
        if (val < min) return min;
        if (val > max) return max;
        return val;
    }
    
    /**
     * Easing function: ease-in-out-cubic for smooth motion
     * @param t Time elapsed (0.0 to 1.0)
     */
    static float easeInOutCubic(float t) {
        if (t < 0.5f) {
            return 4.0f * t * t * t;
        } else {
            float f = (2.0f * t) - 2.0f;
            return 0.5f * f * f * f + 1.0f;
        }
    }
    
    /**
     * Update motion easing (interpolate toward targets)
     */
    void updateMotionEasing(uint32_t now_ms) {
        // Initialize motion start time on first call
        if (state.motion_start_time == 0) {
            state.motion_start_time = now_ms;
        }
        
        uint32_t elapsed_ms = now_ms - state.motion_start_time;
        float t = (float)elapsed_ms / (float)state.motion_duration_ms;
        
        if (t >= 1.0f) {
            // Motion complete
            state.pan_angle = state.pan_target;
            state.tilt_angle = state.tilt_target;
            state.roll_angle = state.roll_target;
            state.is_moving = false;
            state.motion_start_time = 0;
        } else {
            // Interpolate with easing
            float ease = easeInOutCubic(t);
            
            // Calculate deltas
            int16_t pan_delta = state.pan_target - (state.pan_angle + 
                                (state.pan_target - state.pan_angle) * ease);
            int16_t tilt_delta = state.tilt_target - (state.tilt_angle + 
                                 (state.tilt_target - state.tilt_angle) * ease);
            int16_t roll_delta = state.roll_target - (state.roll_angle + 
                                 (state.roll_target - state.roll_angle) * ease);
            
            // Apply eased positions
            state.pan_angle = state.pan_angle + (int16_t)(pan_delta);
            state.tilt_angle = state.tilt_angle + (int16_t)(tilt_delta);
            state.roll_angle = state.roll_angle + (int16_t)(roll_delta);
        }
    }
    
    /**
     * Update stepper motor positions from angle state
     */
    void updateMotorTargets() {
        // NEMA23 Standard: 200 steps/rev, geared for smooth motion
        // Pan: ±60° range → ±33 steps (rough estimate with gearing)
        int32_t pan_steps = (state.pan_angle * 200) / 360;
        
        // NEMA17 High-Torque: 200 steps/rev
        // Tilt: ±45° range → ±25 steps
        int32_t tilt_steps = (state.tilt_angle * 200) / 360;
        
        // NEMA14: 200 steps/rev
        // Roll: ±25° range → ±14 steps
        int32_t roll_steps = (state.roll_angle * 200) / 360;
        
        pan_controller.moveTo(pan_steps);
        tilt_controller.moveTo(tilt_steps);
        roll_controller.moveTo(roll_steps);
    }
};

#endif // GOBLIN_HEAD_NECK_MOTOR_HPP
