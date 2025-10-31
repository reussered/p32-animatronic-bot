// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/goblin_head.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/spi_bus.hdr"
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
    spi_bus_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    spi_bus_init,
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
    spi_bus_act,    // [4] ESP32 VSPI bus interface for SPI communication
    goblin_right_eye_act,    // [5] Right eye display with bird animation
    goblin_eye_act,    // [6] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [7] <describe the components purpose and functionality here>
    spi_bus_act,    // [8] ESP32 VSPI bus interface for SPI communication
    goblin_nose_act,    // [9] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [10] <describe the components purpose and functionality here>
    goblin_mouth_act,    // [11] Mouth display animation
    goblin_mouth_act,    // [12] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [13] <describe the components purpose and functionality here>
    spi_bus_act,    // [14] ESP32 VSPI bus interface for SPI communication
    goblin_speaker_act,    // [15] Audio output processing
    speaker_act,    // [16] <describe the components purpose and functionality here>
    goblin_left_ear_act,    // [17] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [18] Micro servo motor for precise position control in small animatronics
    hw496_microphone_act,    // [19] HW-496 MEMS microphone with MAX4466 pre-amplifier
    goblin_right_ear_act,    // [20] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [21] Micro servo motor for precise position control in small animatronics
    hw496_microphone_act,    // [22] HW-496 MEMS microphone with MAX4466 pre-amplifier
    goblin_torso_act,    // [23] Goblin torso subsystem - Master controller with WiFi, and system coordination
    system_core_act,    // [24] Core system management - initialization, health checks, error handling
    debug_controller_act,    // [25] System-level debug flag controller that manages debugging state via Environment SharedMemory
    network_monitor_act,    // [26] Network connectivity monitoring - WiFi signal strength, connection status
    wifi_station_act,    // [27] WiFi station connectivity for cloud integration and remote control
    bluetooth_central_act,    // [28] Bluetooth Low Energy (BLE) central controller - mobile app connectivity
    telemetry_hub_act,    // [29] Central telemetry aggregation and publishing hub - collects data from all subsystems
    watchdog_act,    // [30] Hardware watchdog timer - system health monitoring and automatic recovery
    power_monitor_act,    // [31] Power monitoring - battery voltage, current consumption, remaining capacity
    spine_flexion_servo_act,    // [32] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [33] Micro servo motor for precise position control in small animatronics
    spine_extension_servo_act,    // [34] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [35] Micro servo motor for precise position control in small animatronics
    waist_rotation_servo_act,    // [36] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [37] Micro servo motor for precise position control in small animatronics
    torso_status_led_act,    // [38] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [39] Micro servo motor for precise position control in small animatronics
    torso_speaker_act,    // [40] <describe the components purpose and functionality here>
    servo_sg90_micro_act     // [41] Micro servo motor for precise position control in small animatronics
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    25,    // [0] goblin_head - every 25 loops
    1,    // [1] goblin_left_eye - every 1 loops
    1,    // [2] goblin_eye - every 1 loops
    1,    // [3] gc9a01 - every 1 loops
    1,    // [4] spi_bus - every 1 loops
    1,    // [5] goblin_right_eye - every 1 loops
    1,    // [6] goblin_eye - every 1 loops
    1,    // [7] gc9a01 - every 1 loops
    1,    // [8] spi_bus - every 1 loops
    180000,    // [9] goblin_nose - every 180000 loops
    1,    // [10] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    36000,    // [11] goblin_mouth - every 36000 loops
    1,    // [12] goblin_mouth - every 1 loops
    1,    // [13] gc9a01 - every 1 loops
    1,    // [14] spi_bus - every 1 loops
    84000,    // [15] goblin_speaker - every 84000 loops
    1,    // [16] speaker - every 1 loops
    1,    // [17] goblin_left_ear - every 1 loops
    1,    // [18] servo_sg90_micro - every 1 loops
    1,    // [19] HW496_MICROPHONE - every 1 loops
    1,    // [20] goblin_right_ear - every 1 loops
    1,    // [21] servo_sg90_micro - every 1 loops
    1,    // [22] HW496_MICROPHONE - every 1 loops
    50,    // [23] goblin_torso - every 50 loops
    100,    // [24] system_core - every 100 loops
    1,    // [25] debug_controller - every 1 loops
    50,    // [26] network_monitor - every 50 loops
    50,    // [27] wifi_station - every 50 loops
    75,    // [28] bluetooth_central - every 75 loops
    75,    // [29] telemetry_hub - every 75 loops
    500,    // [30] watchdog - every 500 loops
    200,    // [31] power_monitor - every 200 loops
    1,    // [32] spine_flexion_servo - every 1 loops
    1,    // [33] servo_sg90_micro - every 1 loops
    1,    // [34] spine_extension_servo - every 1 loops
    1,    // [35] servo_sg90_micro - every 1 loops
    1,    // [36] waist_rotation_servo - every 1 loops
    1,    // [37] servo_sg90_micro - every 1 loops
    1,    // [38] torso_status_led - every 1 loops
    1,    // [39] servo_sg90_micro - every 1 loops
    1,    // [40] torso_speaker - every 1 loops
    1     // [41] servo_sg90_micro - every 1 loops
};
