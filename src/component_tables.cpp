// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/goblin_personality.hdr"
#include "components/goblin_behavior_engine.hdr"
#include "components/goblin_head.hdr"
#include "components/spi_bus.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_right_eye.hdr"
#include "components/goblin_nose.hdr"
#include "components/hc_sr04_ultrasonic_distance_sensor.hdr"
#include "components/gpio_bus.hdr"
#include "components/gpio_pair_driver.hdr"
#include "components/gpio_single_driver.hdr"
#include "components/goblin_mouth.hdr"
#include "components/gc9a01.hdr"
#include "components/goblin_speaker.hdr"
#include "components/speaker.hdr"
#include "components/goblin_left_ear.hdr"
#include "components/servo_sg90_micro.hdr"
#include "components/pwm_bus.hdr"
#include "components/pwm_driver.hdr"
#include "components/hw496_microphone.hdr"
#include "components/goblin_right_ear.hdr"
#include "components/goblin_left_cheek.hdr"
#include "components/goblin_right_cheek.hdr"
#include "components/goblin_left_eyebrow.hdr"
#include "components/goblin_right_eyebrow.hdr"
#include "components/goblin_jaw.hdr"
#include "components/goblin_forehead.hdr"
#include "components/goblin_torso.hdr"
#include "components/spine_flexion_servo.hdr"
#include "components/spine_extension_servo.hdr"
#include "components/waist_rotation_servo.hdr"
#include "components/torso_status_led.hdr"
#include "components/torso_speaker.hdr"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    goblin_personality_init,
    goblin_behavior_engine_init,
    goblin_head_init,
    spi_bus_init,
    goblin_left_eye_init,
    goblin_right_eye_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    gpio_bus_init,
    gpio_pair_driver_init,
    gpio_single_driver_init,
    goblin_mouth_init,
    gc9a01_init,
    spi_bus_init,
    goblin_speaker_init,
    speaker_init,
    goblin_left_ear_init,
    servo_sg90_micro_init,
    pwm_bus_init,
    pwm_driver_init,
    hw496_microphone_init,
    goblin_right_ear_init,
    goblin_left_cheek_init,
    goblin_right_cheek_init,
    goblin_left_eyebrow_init,
    goblin_right_eyebrow_init,
    goblin_jaw_init,
    goblin_forehead_init,
    goblin_torso_init,
    spine_flexion_servo_init,
    spine_extension_servo_init,
    waist_rotation_servo_init,
    torso_status_led_init,
    torso_speaker_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    goblin_personality_act,    // [0] Goblin family personality traits - mischievous, aggressive, curious
    goblin_behavior_engine_act,    // [1] Goblin behavior selection engine - chooses actions based on mood and personality
    goblin_head_act,    // [2] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    spi_bus_act,    // [3] ESP32 HSPI bus interface for SPI communication
    goblin_left_eye_act,    // [4] Left eye display animation
    goblin_right_eye_act,    // [5] Right eye display with bird animation
    goblin_nose_act,    // [6] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [7] <describe the components purpose and functionality here>
    gpio_bus_act,    // [8] GPIO bus component containing GPIO drivers for digital input/output operations
    gpio_pair_driver_act,    // [9] Generic GPIO pair driver for ESP32 supporting trigger/echo pairs commonly used for ultrasonic sensors
    gpio_single_driver_act,    // [10] Generic single GPIO driver for ESP32 supporting digital input/output with interrupt capabilities
    goblin_mouth_act,    // [11] Mouth display animation
    gc9a01_act,    // [12] <describe the components purpose and functionality here>
    spi_bus_act,    // [13] SPI bus component containing SPI display driver for high-speed serial communication
    goblin_speaker_act,    // [14] Audio output processing
    speaker_act,    // [15] <describe the components purpose and functionality here>
    goblin_left_ear_act,    // [16] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [17] Micro servo motor for precise position control in small animatronics
    pwm_bus_act,    // [18] PWM bus component containing PWM driver for servo and LED control
    pwm_driver_act,    // [19] Generic PWM driver for ESP32 supporting multiple channels with configurable frequency and duty cycle
    hw496_microphone_act,    // [20] HW-496 MEMS microphone with MAX4466 pre-amplifier
    goblin_right_ear_act,    // [21] <describe the components purpose and functionality here>
    goblin_left_cheek_act,    // [22] Left cheek muscle for facial expressions and emotions
    goblin_right_cheek_act,    // [23] Right cheek muscle for facial expressions and emotions
    goblin_left_eyebrow_act,    // [24] Left eyebrow for emotional expressions and eye manipulation
    goblin_right_eyebrow_act,    // [25] Right eyebrow for emotional expressions and eye manipulation
    goblin_jaw_act,    // [26] Jaw/chin servo for mouth opening/closing and speech animation
    goblin_forehead_act,    // [27] Forehead servo for wrinkle effects and concentration expressions
    goblin_torso_act,    // [28] Goblin torso subsystem - Master controller with WiFi, and system coordination
    spine_flexion_servo_act,    // [29] <describe the components purpose and functionality here>
    spine_extension_servo_act,    // [30] <describe the components purpose and functionality here>
    waist_rotation_servo_act,    // [31] <describe the components purpose and functionality here>
    torso_status_led_act,    // [32] <describe the components purpose and functionality here>
    torso_speaker_act     // [33] <describe the components purpose and functionality here>
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    25,    // [0] goblin_personality - every 25 loops
    10,    // [1] goblin_behavior_engine - every 10 loops
    25,    // [2] goblin_head - every 25 loops
    1,    // [3] spi_bus_vspi - every 1 loops
    1,    // [4] goblin_left_eye - every 1 loops
    1,    // [5] goblin_right_eye - every 1 loops
    180000,    // [6] goblin_nose - every 180000 loops
    1,    // [7] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    1,    // [8] gpio_bus - every 1 loops
    1,    // [9] gpio_pair_driver - every 1 loops
    1,    // [10] gpio_single_driver - every 1 loops
    36000,    // [11] goblin_mouth - every 36000 loops
    1,    // [12] gc9a01 - every 1 loops
    1,    // [13] spi_bus - every 1 loops
    84000,    // [14] goblin_speaker - every 84000 loops
    1,    // [15] speaker - every 1 loops
    1,    // [16] goblin_left_ear - every 1 loops
    1,    // [17] servo_sg90_micro - every 1 loops
    1,    // [18] pwm_bus - every 1 loops
    1,    // [19] pwm_driver - every 1 loops
    1,    // [20] HW496_MICROPHONE - every 1 loops
    1,    // [21] goblin_right_ear - every 1 loops
    15,    // [22] goblin_left_cheek - every 15 loops
    15,    // [23] goblin_right_cheek - every 15 loops
    20,    // [24] goblin_left_eyebrow - every 20 loops
    20,    // [25] goblin_right_eyebrow - every 20 loops
    30,    // [26] goblin_jaw - every 30 loops
    10,    // [27] goblin_forehead - every 10 loops
    50,    // [28] goblin_torso - every 50 loops
    1,    // [29] spine_flexion_servo - every 1 loops
    1,    // [30] spine_extension_servo - every 1 loops
    1,    // [31] waist_rotation_servo - every 1 loops
    1,    // [32] torso_status_led - every 1 loops
    1     // [33] torso_speaker - every 1 loops
};
