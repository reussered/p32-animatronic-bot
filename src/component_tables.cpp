// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/heartbeat.hpp"
#include "components/network_monitor.hpp"
#include "components/goblin_head.hpp"
#include "components/spi_bus.hpp"
#include "components/goblin_left_eye.hpp"
#include "components/goblin_eye.hpp"
#include "components/gc9a01.hpp"
#include "components/generic_spi_display.hpp"
#include "components/goblin_right_eye.hpp"
#include "components/goblin_nose.hpp"
#include "components/hc_sr04_ultrasonic_distance_sensor.hpp"
#include "components/goblin_mouth.hpp"
#include "components/goblin_speaker.hpp"
#include "components/speaker.hpp"
#include "components/goblin_left_ear.hpp"
#include "components/servo_sg90_micro.hpp"
#include "components/goblin_right_ear.hpp"
#include "components/goblin_torso.hpp"
#include "components/spine_flexion_servo.hpp"
#include "components/spine_extension_servo.hpp"
#include "components/waist_rotation_servo.hpp"
#include "components/torso_status_led.hpp"
#include "components/torso_speaker.hpp"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    goblin_head_init,
    spi_bus_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    generic_spi_display_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    generic_spi_display_init,
    goblin_speaker_init,
    speaker_init,
    goblin_left_ear_init,
    servo_sg90_micro_init,
    goblin_right_ear_init,
    servo_sg90_micro_init,
    goblin_torso_init,
    spine_flexion_servo_init,
    servo_sg90_micro_init,
    spine_extension_servo_init,
    servo_sg90_micro_init,
    waist_rotation_servo_init,
    servo_sg90_micro_init,
    torso_status_led_init,
    servo_sg90_micro_init,
    torso_speaker_init,
    servo_sg90_micro_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    goblin_head_act,    // [2] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    spi_bus_act,    // [3] spi_bus component
    goblin_left_eye_act,    // [4] Left eye display animation
    goblin_eye_act,    // [5] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [6] gc9a01 component
    generic_spi_display_act,    // [7] generic_spi_display component
    goblin_right_eye_act,    // [8] Right eye display animation
    goblin_eye_act,    // [9] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [10] gc9a01 component
    generic_spi_display_act,    // [11] generic_spi_display component
    goblin_nose_act,    // [12] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [13] hc_sr04_ultrasonic_distance_sensor component
    goblin_mouth_act,    // [14] Mouth display animation
    goblin_mouth_act,    // [15] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [16] gc9a01 component
    generic_spi_display_act,    // [17] generic_spi_display component
    goblin_speaker_act,    // [18] Audio output processing
    speaker_act,    // [19] speaker component
    goblin_left_ear_act,    // [20] goblin_left_ear component
    servo_sg90_micro_act,    // [21] Micro servo motor for precise position control in small animatronics
    goblin_right_ear_act,    // [22] goblin_right_ear component
    servo_sg90_micro_act,    // [23] Micro servo motor for precise position control in small animatronics
    goblin_torso_act,    // [24] Goblin torso subsystem - Master controller with mesh networking, WiFi, and system coordination
    spine_flexion_servo_act,    // [25] spine_flexion_servo component
    servo_sg90_micro_act,    // [26] Micro servo motor for precise position control in small animatronics
    spine_extension_servo_act,    // [27] spine_extension_servo component
    servo_sg90_micro_act,    // [28] Micro servo motor for precise position control in small animatronics
    waist_rotation_servo_act,    // [29] waist_rotation_servo component
    servo_sg90_micro_act,    // [30] Micro servo motor for precise position control in small animatronics
    torso_status_led_act,    // [31] torso_status_led component
    servo_sg90_micro_act,    // [32] Micro servo motor for precise position control in small animatronics
    torso_speaker_act,    // [33] torso_speaker component
    servo_sg90_micro_act     // [34] Micro servo motor for precise position control in small animatronics
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    25,    // [2] goblin_head - every 25 loops
    1,    // [3] spi_bus - every 1 loops
    1,    // [4] goblin_left_eye - every 1 loops
    1,    // [5] goblin_eye - every 1 loops
    1,    // [6] gc9a01 - every 1 loops
    1,    // [7] generic_spi_display - every 1 loops
    60000,    // [8] goblin_right_eye - every 60000 loops
    1,    // [9] goblin_eye - every 1 loops
    1,    // [10] gc9a01 - every 1 loops
    1,    // [11] generic_spi_display - every 1 loops
    180000,    // [12] goblin_nose - every 180000 loops
    1,    // [13] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    36000,    // [14] goblin_mouth - every 36000 loops
    1,    // [15] goblin_mouth - every 1 loops
    1,    // [16] gc9a01 - every 1 loops
    1,    // [17] generic_spi_display - every 1 loops
    84000,    // [18] goblin_speaker - every 84000 loops
    1,    // [19] speaker - every 1 loops
    1,    // [20] goblin_left_ear - every 1 loops
    1,    // [21] servo_sg90_micro - every 1 loops
    1,    // [22] goblin_right_ear - every 1 loops
    1,    // [23] servo_sg90_micro - every 1 loops
    50,    // [24] goblin_torso - every 50 loops
    1,    // [25] spine_flexion_servo - every 1 loops
    1,    // [26] servo_sg90_micro - every 1 loops
    1,    // [27] spine_extension_servo - every 1 loops
    1,    // [28] servo_sg90_micro - every 1 loops
    1,    // [29] waist_rotation_servo - every 1 loops
    1,    // [30] servo_sg90_micro - every 1 loops
    1,    // [31] torso_status_led - every 1 loops
    1,    // [32] servo_sg90_micro - every 1 loops
    1,    // [33] torso_speaker - every 1 loops
    1     // [34] servo_sg90_micro - every 1 loops
};
