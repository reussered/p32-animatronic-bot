// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/goblin_personality.hdr"
#include "components/goblin_behavior_engine.hdr"
#include "components/goblin_head.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/spi_bus.hdr"
#include "components/generic_spi_display.hdr"
#include "components/goblin_right_eye.hdr"
#include "components/goblin_left_ear.hdr"
#include "components/servo_sg90_micro.hdr"
#include "components/pwm_bus.hdr"
#include "components/pwm_driver.hdr"
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
    goblin_personality_init,
    goblin_behavior_engine_init,
    goblin_head_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    goblin_left_ear_init,
    servo_sg90_micro_init,
    pwm_bus_init,
    pwm_driver_init,
    hw496_microphone_init,
    goblin_right_ear_init,
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
    goblin_left_eye_act,    // [3] Left eye display animation
    goblin_eye_act,    // [4] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [5] <describe the components purpose and functionality here>
    spi_bus_act,    // [6] ESP32 HSPI bus interface for SPI communication
    generic_spi_display_act,    // [7] <describe the components purpose and functionality here>
    goblin_right_eye_act,    // [8] Right eye display with bird animation
    goblin_left_ear_act,    // [9] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [10] Micro servo motor for precise position control in small animatronics
    pwm_bus_act,    // [11] PWM bus component containing PWM driver for servo and LED control
    pwm_driver_act,    // [12] Generic PWM driver for ESP32 supporting multiple channels with configurable frequency and duty cycle
    hw496_microphone_act,    // [13] HW-496 MEMS microphone with MAX4466 pre-amplifier
    goblin_right_ear_act,    // [14] <describe the components purpose and functionality here>
    goblin_torso_act,    // [15] Goblin torso subsystem - Master controller with WiFi, and system coordination
    system_core_act,    // [16] Core system management - initialization, health checks, error handling
    debug_controller_act,    // [17] System-level debug flag controller that manages debugging state via Environment SharedMemory
    network_monitor_act,    // [18] Network connectivity monitoring - WiFi signal strength, connection status
    wifi_station_act,    // [19] WiFi station connectivity for cloud integration and remote control
    bluetooth_central_act,    // [20] Bluetooth Low Energy (BLE) central controller - mobile app connectivity
    telemetry_hub_act,    // [21] Central telemetry aggregation and publishing hub - collects data from all subsystems
    watchdog_act,    // [22] Hardware watchdog timer - system health monitoring and automatic recovery
    power_monitor_act,    // [23] Power monitoring - battery voltage, current consumption, remaining capacity
    spine_flexion_servo_act,    // [24] <describe the components purpose and functionality here>
    spine_extension_servo_act,    // [25] <describe the components purpose and functionality here>
    waist_rotation_servo_act,    // [26] <describe the components purpose and functionality here>
    torso_status_led_act,    // [27] <describe the components purpose and functionality here>
    torso_speaker_act     // [28] <describe the components purpose and functionality here>
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    25,    // [0] goblin_personality - every 25 loops
    10,    // [1] goblin_behavior_engine - every 10 loops
    25,    // [2] goblin_head - every 25 loops
    1,    // [3] goblin_left_eye - every 1 loops
    1,    // [4] goblin_eye - every 1 loops
    1,    // [5] gc9a01 - every 1 loops
    1,    // [6] spi_bus_vspi - every 1 loops
    1,    // [7] generic_spi_display - every 1 loops
    1,    // [8] goblin_right_eye - every 1 loops
    1,    // [9] goblin_left_ear - every 1 loops
    1,    // [10] servo_sg90_micro - every 1 loops
    1,    // [11] pwm_bus - every 1 loops
    1,    // [12] pwm_driver - every 1 loops
    1,    // [13] HW496_MICROPHONE - every 1 loops
    1,    // [14] goblin_right_ear - every 1 loops
    50,    // [15] goblin_torso - every 50 loops
    100,    // [16] system_core - every 100 loops
    1,    // [17] debug_controller - every 1 loops
    50,    // [18] network_monitor - every 50 loops
    50,    // [19] wifi_station - every 50 loops
    75,    // [20] bluetooth_central - every 75 loops
    75,    // [21] telemetry_hub - every 75 loops
    500,    // [22] watchdog - every 500 loops
    200,    // [23] power_monitor - every 200 loops
    1,    // [24] spine_flexion_servo - every 1 loops
    1,    // [25] spine_extension_servo - every 1 loops
    1,    // [26] waist_rotation_servo - every 1 loops
    1,    // [27] torso_status_led - every 1 loops
    1     // [28] torso_speaker - every 1 loops
};
