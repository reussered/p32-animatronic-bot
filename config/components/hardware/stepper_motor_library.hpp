/**
 * @file stepper_motor_library.hpp
 * @brief Comprehensive stepper motor library with 30+ motor configurations
 * 
 * Covers NEMA sizes, voltages, gear ratios, and control modes.
 * All calculations pre-computed to avoid runtime overhead.
 * 
 * ARCHITECTURE:
 * - Motor specs stored as static const structs
 * - No allocations, all ROM-based
 * - Speed/torque tradeoff is preset per motor type
 * - Driver interface is controller-agnostic (GPIO + timing)
 * 
 * USAGE:
 *   const StepperSpec& motor = STEPPER_MOTORS[NEMA17_STANDARD];
 *   if (motor.steps_per_revolution == 200) { ... }
 *   delay_us = motor.min_step_interval_us;  // Timing for max speed
 */

#pragma once

#include <cstdint>

// Step/direction control modes
enum StepperMode {
    STEP_DIR_MODE = 0,      // Step + Direction pins (STEP HIGH->LOW, DIR static)
    DUAL_COIL_MODE = 1,     // Direct coil A/B control (wave/full step patterns)
    PWM_SERVO_MODE = 2,     // PWM-based pseudo-servo (angle control)
};

// Resolution modes
enum StepResolution {
    FULL_STEP = 1,          // 200 steps/rev (NEMA17), 48 steps/rev (NEMA8)
    HALF_STEP = 2,          // 2x resolution
    QUARTER_STEP = 4,       // 4x resolution (microstepping)
    EIGHTH_STEP = 8,        // 8x resolution
    SIXTEENTH_STEP = 16,    // 16x resolution (precision)
};

// Stepper motor specification
struct StepperSpec {
    const char* name;                    // "NEMA17 Standard", "NEMA23 HighTorque", etc
    uint16_t nema_size;                  // 17, 23, 24, 34, 42 (motor frame height)
    uint16_t rated_voltage_mv;           // 12000, 24000 mV
    uint16_t rated_current_ma;           // Per coil, mA
    uint16_t holding_torque_ncm;         // Holding torque in 0.01 N·cm units
    uint16_t max_torque_ncm;             // Peak torque
    uint16_t base_steps_per_rev;         // Usually 200 (1.8°) or 48 (7.5°)
    uint16_t nominal_rpm_at_rated;       // Typical RPM at rated current
    uint16_t max_rpm_no_load;            // Max RPM with no load
    float inertia_gcm2;                  // Rotational inertia
    uint16_t length_mm;                  // Motor body length
    uint16_t shaft_diameter_mm_x10;      // Shaft OD × 10 (e.g., 50 = 5mm)
    StepperMode control_mode;            // Default control method
    StepResolution default_resolution;   // Default step resolution
    uint16_t min_step_interval_us;       // Max speed timing (microseconds)
    uint16_t recommended_step_interval_us; // Comfortable sustained speed
    bool has_gearbox;                    // Geared variant available
    uint16_t gear_ratio_x100;            // Gear ratio × 100 (e.g., 250 = 2.5:1)
    const char* mounting_type;           // "NEMA17_FLANGE", "CAPTIVE_SHAFT", etc
    const char* typical_application;     // "Eye movement", "Jaw rotation", etc
};

// ============================================================================
// MOTOR CATALOG: 30 STEPPER MOTOR CONFIGURATIONS
// ============================================================================

constexpr StepperSpec STEPPER_MOTORS[] = {
    // ========== NEMA8 (20.5 × 20.5mm, micro-positioning)
    {
        "NEMA8 Micro Standard",
        8, 12000, 400,
        20,   // 0.2 N·cm holding
        30,   // 0.3 N·cm max
        48,   // 7.5° steps (48 rev)
        200, 2000,  // 200 RPM @ 12V, 2000 no-load
        4.5f, 20, 30, STEP_DIR_MODE, FULL_STEP,
        100, 500, false, 0, "CAPTIVE_SHAFT", "Finger articulation"
    },
    {
        "NEMA8 Micro Geared 5:1",
        8, 12000, 400,
        100,  // 1.0 N·cm holding (5× gear)
        150,  // 1.5 N·cm max
        48, 200, 400,  // Geared down
        4.5f, 35, 30, STEP_DIR_MODE, HALF_STEP,
        50, 250, true, 500, "CAPTIVE_SHAFT", "Finger articulation (heavy)"
    },

    // ========== NEMA11 (28 × 28mm, light articulation)
    {
        "NEMA11 Standard",
        11, 12000, 700,
        50,   // 0.5 N·cm
        100,  // 1.0 N·cm
        200, 300, 1500,
        12.0f, 26, 30, STEP_DIR_MODE, FULL_STEP,
        80, 400, false, 0, "NEMA11_FLANGE", "Light articulation"
    },

    // ========== NEMA14 (35 × 35mm, general purpose)
    {
        "NEMA14 Standard",
        14, 12000, 1000,
        100,  // 1.0 N·cm
        200,  // 2.0 N·cm
        200, 400, 2000,
        20.0f, 33, 40, STEP_DIR_MODE, FULL_STEP,
        60, 300, false, 0, "NEMA14_FLANGE", "Head rotation"
    },
    {
        "NEMA14 High-Speed",
        14, 24000, 800,
        80, 160,
        200, 600, 3500,
        20.0f, 33, 40, STEP_DIR_MODE, QUARTER_STEP,
        30, 150, false, 0, "NEMA14_FLANGE", "Rapid repositioning"
    },

    // ========== NEMA17 (42.3 × 42.3mm, standard workhorses)
    {
        "NEMA17 Standard",
        17, 12000, 1500,
        400,  // 4.0 N·cm
        800,  // 8.0 N·cm
        200, 500, 3000,
        40.0f, 38, 50, STEP_DIR_MODE, FULL_STEP,
        50, 250, false, 0, "NEMA17_FLANGE", "Eye movement, jaw"
    },
    {
        "NEMA17 Precision",
        17, 24000, 1200,
        350, 700,
        200, 600, 4000,
        40.0f, 38, 50, STEP_DIR_MODE, SIXTEENTH_STEP,
        20, 100, false, 0, "NEMA17_FLANGE", "Precision articulation"
    },
    {
        "NEMA17 High-Torque",
        17, 24000, 2000,
        1200, 2500,
        200, 300, 1500,
        40.0f, 38, 50, STEP_DIR_MODE, HALF_STEP,
        40, 200, false, 0, "NEMA17_FLANGE", "Heavy jaw/cheek"
    },
    {
        "NEMA17 Geared 3:1",
        17, 12000, 1500,
        1200, 2400,
        200, 150, 1000,
        40.0f, 50, 50, STEP_DIR_MODE, FULL_STEP,
        80, 400, true, 300, "NEMA17_FLANGE", "Jaw opening (heavy)"
    },
    {
        "NEMA17 Geared 5:1",
        17, 12000, 1500,
        2000, 4000,
        200, 100, 600,
        40.0f, 55, 50, STEP_DIR_MODE, FULL_STEP,
        120, 600, true, 500, "NEMA17_FLANGE", "Mouth corner pull"
    },

    // ========== NEMA23 (56.4 × 56.4mm, heavy load)
    {
        "NEMA23 Standard",
        23, 24000, 2500,
        1500, 3000,
        200, 800, 4500,
        75.0f, 51, 60, STEP_DIR_MODE, FULL_STEP,
        40, 200, false, 0, "NEMA23_FLANGE", "Neck rotation"
    },
    {
        "NEMA23 High-Torque",
        23, 48000, 3500,
        3000, 6000,
        200, 500, 2500,
        75.0f, 51, 60, STEP_DIR_MODE, HALF_STEP,
        50, 250, false, 0, "NEMA23_FLANGE", "Torso articulation"
    },
    {
        "NEMA23 Geared 10:1",
        23, 24000, 2500,
        15000, 30000,
        200, 80, 450,
        75.0f, 70, 60, STEP_DIR_MODE, FULL_STEP,
        200, 1000, true, 1000, "NEMA23_FLANGE", "Heavy load positioning"
    },

    // ========== NEMA24 (60 × 60mm, high precision)
    {
        "NEMA24 Precision",
        24, 36000, 2800,
        2000, 4000,
        200, 900, 5000,
        120.0f, 55, 65, STEP_DIR_MODE, SIXTEENTH_STEP,
        30, 150, false, 0, "NEMA24_FLANGE", "Precision eye tracking"
    },

    // ========== NEMA34 (86 × 86mm, heavy industrial)
    {
        "NEMA34 Standard",
        34, 48000, 5000,
        8000, 16000,
        200, 1000, 5000,
        250.0f, 65, 90, STEP_DIR_MODE, FULL_STEP,
        40, 200, false, 0, "NEMA34_FLANGE", "Leg/arm articulation"
    },
    {
        "NEMA34 High-Speed",
        34, 48000, 3500,
        5000, 10000,
        200, 1500, 7500,
        250.0f, 65, 90, STEP_DIR_MODE, QUARTER_STEP,
        20, 100, false, 0, "NEMA34_FLANGE", "Rapid full-body motion"
    },
    {
        "NEMA34 Geared 20:1",
        34, 48000, 5000,
        160000, 320000,
        200, 50, 250,
        250.0f, 85, 90, STEP_DIR_MODE, FULL_STEP,
        300, 1500, true, 2000, "NEMA34_FLANGE", "Extreme torque positioning"
    },

    // ========== NEMA42 (110 × 110mm, maximum torque)
    {
        "NEMA42 Standard",
        42, 48000, 6000,
        20000, 40000,
        200, 800, 4000,
        400.0f, 75, 110, STEP_DIR_MODE, FULL_STEP,
        50, 250, false, 0, "NEMA42_FLANGE", "Full-body base rotation"
    },

    // ========== Pancake Motors (shallow, wide torque)
    {
        "NEMA17 Pancake",
        17, 24000, 1500,
        600, 1200,
        200, 400, 2500,
        35.0f, 20, 50, STEP_DIR_MODE, FULL_STEP,
        60, 300, false, 0, "CAPTIVE_SHAFT", "Compact articulation"
    },
    {
        "NEMA23 Pancake",
        23, 36000, 2800,
        2000, 4000,
        200, 600, 3500,
        80.0f, 28, 60, STEP_DIR_MODE, HALF_STEP,
        40, 200, false, 0, "CAPTIVE_SHAFT", "Compact heavy load"
    },

    // ========== Linear Actuator Steppers (leadscrew integrated)
    {
        "NEMA17 Linearized 5mm",
        17, 12000, 1500,
        800, 1600,  // Effective when converted to linear force
        200, 500, 3000,
        40.0f, 60, 50, STEP_DIR_MODE, FULL_STEP,
        50, 250, true, 100, "LEADSCREW_5MM", "Eyelid opening"
    },
    {
        "NEMA17 Linearized 8mm",
        17, 12000, 1500,
        1200, 2400,
        200, 350, 2000,
        40.0f, 65, 50, STEP_DIR_MODE, FULL_STEP,
        75, 375, true, 150, "LEADSCREW_8MM", "Jaw opening (linear)"
    },
    {
        "NEMA23 Linearized 12mm",
        23, 24000, 2500,
        3000, 6000,
        200, 400, 2400,
        75.0f, 80, 60, STEP_DIR_MODE, HALF_STEP,
        60, 300, true, 200, "LEADSCREW_12MM", "Heavy eyelid"
    },

    // ========== Hollow Shaft Steppers (through-bore for cables)
    {
        "NEMA23 Hollow 25mm",
        23, 24000, 2500,
        1500, 3000,
        200, 800, 4500,
        75.0f, 51, 250, STEP_DIR_MODE, FULL_STEP,
        40, 200, false, 0, "HOLLOW_25MM", "Neck rotation (cable pass)"
    },

    // ========== Dual-Shaft Steppers
    {
        "NEMA17 Dual-Shaft",
        17, 12000, 1500,
        400, 800,
        200, 500, 3000,
        40.0f, 48, 50, DUAL_COIL_MODE, FULL_STEP,
        50, 250, false, 0, "DUAL_SHAFT", "Symmetric load distribution"
    },

    // ========== Low-Speed High-Torque (LSHT) Geared
    {
        "NEMA17 LSHT 100:1",
        17, 12000, 1500,
        40000, 80000,
        200, 30, 200,
        40.0f, 75, 50, STEP_DIR_MODE, FULL_STEP,
        300, 1500, true, 10000, "NEMA17_FLANGE", "Ultra-slow smooth"
    },

    // ========== Servo-Like Steppers (PWM capable)
    {
        "NEMA14 PWM Servo",
        14, 24000, 1000,
        150, 300,
        200, 500, 3000,
        20.0f, 33, 40, PWM_SERVO_MODE, QUARTER_STEP,
        25, 125, false, 0, "NEMA14_FLANGE", "Smooth servo-like motion"
    },
    {
        "NEMA17 PWM Servo",
        17, 24000, 1500,
        500, 1000,
        200, 600, 4000,
        40.0f, 38, 50, PWM_SERVO_MODE, SIXTEENTH_STEP,
        20, 100, false, 0, "NEMA17_FLANGE", "Smooth articulation"
    },

    // ========== Outdoor-Rated Steppers (sealed bearings)
    {
        "NEMA17 IP54 Sealed",
        17, 12000, 1500,
        400, 800,
        200, 500, 3000,
        40.0f, 48, 50, STEP_DIR_MODE, FULL_STEP,
        50, 250, false, 0, "SEALED_IP54", "Weather-resistant motion"
    },

    // ========== Flat-Face Motors (compact depth)
    {
        "NEMA17 Flat 15mm",
        17, 12000, 1500,
        400, 800,
        200, 500, 3000,
        40.0f, 15, 50, STEP_DIR_MODE, FULL_STEP,
        50, 250, false, 0, "FLAT_MOUNT", "Space-constrained"
    },
};

// Stepper count
constexpr uint16_t STEPPER_MOTOR_COUNT = sizeof(STEPPER_MOTORS) / sizeof(StepperSpec);

// Convenience indexes
enum StepperMotorIndex {
    NEMA8_STANDARD = 0,
    NEMA8_GEARED_5 = 1,
    NEMA11_STANDARD = 2,
    NEMA14_STANDARD = 3,
    NEMA14_HIGHSPEED = 4,
    NEMA17_STANDARD = 5,
    NEMA17_PRECISION = 6,
    NEMA17_HIGHTORQUE = 7,
    NEMA17_GEARED_3 = 8,
    NEMA17_GEARED_5 = 9,
    NEMA23_STANDARD = 10,
    NEMA23_HIGHTORQUE = 11,
    NEMA23_GEARED_10 = 12,
    NEMA24_PRECISION = 13,
    NEMA34_STANDARD = 14,
    NEMA34_HIGHSPEED = 15,
    NEMA34_GEARED_20 = 16,
    NEMA42_STANDARD = 17,
    NEMA17_PANCAKE = 18,
    NEMA23_PANCAKE = 19,
    NEMA17_LINEAR_5MM = 20,
    NEMA17_LINEAR_8MM = 21,
    NEMA23_LINEAR_12MM = 22,
    NEMA23_HOLLOW_25MM = 23,
    NEMA17_DUAL_SHAFT = 24,
    NEMA17_LSHT_100 = 25,
    NEMA14_PWM_SERVO = 26,
    NEMA17_PWM_SERVO = 27,
    NEMA17_IP54_SEALED = 28,
    NEMA17_FLAT_15MM = 29,
};

// ============================================================================
// STEPPER DRIVER INTERFACE (Hardware abstraction)
// ============================================================================

/**
 * Generic stepper controller - works with any motor from the library
 */
class StepperController {
public:
    typedef void (*StepCallback)(uint32_t step_count);  // Timing callback
    
    StepperController(const StepperSpec& spec)
        : motor_spec(spec), current_position(0), 
          target_position(0), is_moving(false),
          step_interval_us(spec.recommended_step_interval_us),
          step_callback(nullptr)
    {}
    
    // Configure GPIO pins for step/direction control
    bool initStepDir(uint8_t step_pin, uint8_t dir_pin) {
        // TODO: GPIO setup in actual .src implementation
        return true;
    }
    
    // Configure coil control pins for dual-coil mode
    bool initDualCoil(uint8_t coil_a_pin, uint8_t coil_b_pin) {
        // TODO: GPIO setup in actual .src implementation
        return true;
    }
    
    // Set movement speed (RPM)
    void setSpeed(uint16_t rpm) {
        if (rpm == 0) rpm = 1;
        // steps_per_sec = (rpm * steps_per_rev) / 60
        uint32_t steps_per_sec = (rpm * motor_spec.base_steps_per_rev) / 60;
        step_interval_us = (steps_per_sec > 0) ? 1000000 / steps_per_sec : 1000;
    }
    
    // Move to absolute position (steps)
    void moveTo(int32_t position) {
        target_position = position;
        is_moving = (position != current_position);
    }
    
    // Move relative (steps)
    void move(int32_t steps) {
        moveTo(current_position + steps);
    }
    
    // Get current position
    int32_t getPosition() const { return current_position; }
    
    // Check if moving
    bool isMoving() const { return is_moving; }
    
    // Step once (called from ISR or main loop)
    void step() {
        if (!is_moving) return;
        
        if (current_position < target_position) {
            current_position++;
        } else if (current_position > target_position) {
            current_position--;
        } else {
            is_moving = false;
        }
        
        if (step_callback) {
            step_callback(current_position);
        }
    }
    
    // Get motor specification
    const StepperSpec& getSpec() const { return motor_spec; }
    
    // Set timing callback for custom ISR integration
    void setStepCallback(StepCallback cb) { step_callback = cb; }
    
    uint32_t getStepIntervalUs() const { return step_interval_us; }
    
private:
    const StepperSpec& motor_spec;
    int32_t current_position;
    int32_t target_position;
    bool is_moving;
    uint32_t step_interval_us;
    StepCallback step_callback;
};

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * Find stepper by name
 */
inline const StepperSpec* findStepperByName(const char* name) {
    for (uint16_t i = 0; i < STEPPER_MOTOR_COUNT; ++i) {
        if (name && STEPPER_MOTORS[i].name && 
            strcmp(STEPPER_MOTORS[i].name, name) == 0) {
            return &STEPPER_MOTORS[i];
        }
    }
    return nullptr;
}

/**
 * Find best motor for application
 * @param min_torque_ncm Minimum torque needed
 * @param max_rpm Maximum speed needed
 * @param prefer_compact If true, prefer smaller NEMA sizes
 */
inline const StepperSpec* selectMotor(uint16_t min_torque_ncm, 
                                       uint16_t max_rpm, 
                                       bool prefer_compact = false) {
    const StepperSpec* best = nullptr;
    uint32_t best_score = 0xFFFFFFFF;
    
    for (uint16_t i = 0; i < STEPPER_MOTOR_COUNT; ++i) {
        const StepperSpec& motor = STEPPER_MOTORS[i];
        
        // Must meet torque requirement
        if (motor.max_torque_ncm < min_torque_ncm) continue;
        
        // Must meet speed requirement
        if (motor.max_rpm_no_load < max_rpm) continue;
        
        // Score: minimize oversizing, prefer compact if requested
        uint32_t score = motor.holding_torque_ncm * 100 + 
                         (prefer_compact ? motor.nema_size : 0);
        
        if (score < best_score) {
            best_score = score;
            best = &motor;
        }
    }
    
    return best;
}

#endif // STEPPER_MOTOR_LIBRARY_HPP
