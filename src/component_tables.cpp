// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/goblin_head.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/goblin_right_eye.hdr"
#include "components/goblin_nose.hdr"
#include "components/hc_sr04_ultrasonic_distance_sensor.hdr"
#include "components/goblin_mouth.hdr"
#include "components/goblin_speaker.hdr"
#include "components/speaker.hdr"
#include "components/goblin_left_ear.hdr"
#include "components/servo_sg90_micro.hdr"
#include "components/hw496_microphone.hdr"
#include "components/goblin_right_ear.hdr"
#include "components/goblin_torso.hdr"
#include "components/system_core.hdr"
#include "components/debug_controller.hdr"
#include "components/network_monitor.hdr"
#include "components/wifi_station.hdr"
#include "components/bluetooth_central.hdr"
#include "components/telemetry_hub.hdr"
#include "components/watchdog.hdr"
#include "components/power_monitor.hdr"
#include "components/spine_flexion_servo.hdr"
#include "components/spine_extension_servo.hdr"
#include "components/waist_rotation_servo.hdr"
#include "components/torso_status_led.hdr"
#include "components/torso_speaker.hdr"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    goblin_head_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    goblin_speaker_init,
    speaker_init,
    goblin_left_ear_init,
    servo_sg90_micro_init,
    hw496_microphone_init,
    goblin_right_ear_init,
    servo_sg90_micro_init,
    hw496_microphone_init,
    goblin_torso_init,
    system_core_init,
    debug_controller_init,
    network_monitor_init,
    wifi_station_init,
    bluetooth_central_init,
    telemetry_hub_init,
    watchdog_init,
    power_monitor_init,
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
    goblin_head_act,    // [0] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    goblin_left_eye_act,    // [1] Left eye display animation
    goblin_eye_act,    // [2] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [3] <describe the components purpose and functionality here>
    goblin_right_eye_act,    // [4] Right eye display with bird animation
    goblin_eye_act,    // [5] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [6] <describe the components purpose and functionality here>
    goblin_nose_act,    // [7] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [8] <describe the components purpose and functionality here>
    goblin_mouth_act,    // [9] Mouth display animation
    goblin_mouth_act,    // [10] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [11] <describe the components purpose and functionality here>
    goblin_speaker_act,    // [12] Audio output processing
    speaker_act,    // [13] <describe the components purpose and functionality here>
    goblin_left_ear_act,    // [14] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [15] Micro servo motor for precise position control in small animatronics
    hw496_microphone_act,    // [16] HW-496 MEMS microphone with MAX4466 pre-amplifier
    goblin_right_ear_act,    // [17] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [18] Micro servo motor for precise position control in small animatronics
    hw496_microphone_act,    // [19] HW-496 MEMS microphone with MAX4466 pre-amplifier
    goblin_torso_act,    // [20] Goblin torso subsystem - Master controller with WiFi, and system coordination
    system_core_act,    // [21] Core system management - initialization, health checks, error handling
    debug_controller_act,    // [22] System-level debug flag controller that manages debugging state via Environment SharedMemory
    network_monitor_act,    // [23] Network connectivity monitoring - WiFi signal strength, connection status
    wifi_station_act,    // [24] WiFi station connectivity for cloud integration and remote control
    bluetooth_central_act,    // [25] Bluetooth Low Energy (BLE) central controller - mobile app connectivity
    telemetry_hub_act,    // [26] Central telemetry aggregation and publishing hub - collects data from all subsystems
    watchdog_act,    // [27] Hardware watchdog timer - system health monitoring and automatic recovery
    power_monitor_act,    // [28] Power monitoring - battery voltage, current consumption, remaining capacity
    spine_flexion_servo_act,    // [29] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [30] Micro servo motor for precise position control in small animatronics
    spine_extension_servo_act,    // [31] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [32] Micro servo motor for precise position control in small animatronics
    waist_rotation_servo_act,    // [33] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [34] Micro servo motor for precise position control in small animatronics
    torso_status_led_act,    // [35] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [36] Micro servo motor for precise position control in small animatronics
    torso_speaker_act,    // [37] <describe the components purpose and functionality here>
    servo_sg90_micro_act     // [38] Micro servo motor for precise position control in small animatronics
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    25,    // [0] goblin_head - every 25 loops
    1,    // [1] goblin_left_eye - every 1 loops
    1,    // [2] goblin_eye - every 1 loops
    1,    // [3] gc9a01 - every 1 loops
    1,    // [4] goblin_right_eye - every 1 loops
    1,    // [5] goblin_eye - every 1 loops
    1,    // [6] gc9a01 - every 1 loops
    180000,    // [7] goblin_nose - every 180000 loops
    1,    // [8] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    36000,    // [9] goblin_mouth - every 36000 loops
    1,    // [10] goblin_mouth - every 1 loops
    1,    // [11] gc9a01 - every 1 loops
    84000,    // [12] goblin_speaker - every 84000 loops
    1,    // [13] speaker - every 1 loops
    1,    // [14] goblin_left_ear - every 1 loops
    1,    // [15] servo_sg90_micro - every 1 loops
    1,    // [16] HW496_MICROPHONE - every 1 loops
    1,    // [17] goblin_right_ear - every 1 loops
    1,    // [18] servo_sg90_micro - every 1 loops
    1,    // [19] HW496_MICROPHONE - every 1 loops
    50,    // [20] goblin_torso - every 50 loops
    100,    // [21] system_core - every 100 loops
    1,    // [22] debug_controller - every 1 loops
    50,    // [23] network_monitor - every 50 loops
    50,    // [24] wifi_station - every 50 loops
    75,    // [25] bluetooth_central - every 75 loops
    75,    // [26] telemetry_hub - every 75 loops
    500,    // [27] watchdog - every 500 loops
    200,    // [28] power_monitor - every 200 loops
    1,    // [29] spine_flexion_servo - every 1 loops
    1,    // [30] servo_sg90_micro - every 1 loops
    1,    // [31] spine_extension_servo - every 1 loops
    1,    // [32] servo_sg90_micro - every 1 loops
    1,    // [33] waist_rotation_servo - every 1 loops
    1,    // [34] servo_sg90_micro - every 1 loops
    1,    // [35] torso_status_led - every 1 loops
    1,    // [36] servo_sg90_micro - every 1 loops
    1,    // [37] torso_speaker - every 1 loops
    1     // [38] servo_sg90_micro - every 1 loops
};
