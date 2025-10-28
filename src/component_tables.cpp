// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/heartbeat.hpp"
#include "components/network_monitor.hpp"
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

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
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
    servo_sg90_micro_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    spi_bus_act,    // [2] spi_bus component
    goblin_left_eye_act,    // [3] Left eye display animation
    goblin_eye_act,    // [4] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [5] gc9a01 component
    generic_spi_display_act,    // [6] generic_spi_display component
    goblin_right_eye_act,    // [7] Right eye display animation
    goblin_eye_act,    // [8] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [9] gc9a01 component
    generic_spi_display_act,    // [10] generic_spi_display component
    goblin_nose_act,    // [11] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [12] hc_sr04_ultrasonic_distance_sensor component
    goblin_mouth_act,    // [13] Mouth display animation
    goblin_mouth_act,    // [14] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [15] gc9a01 component
    generic_spi_display_act,    // [16] generic_spi_display component
    goblin_speaker_act,    // [17] Audio output processing
    speaker_act,    // [18] speaker component
    goblin_left_ear_act,    // [19] goblin_left_ear component
    servo_sg90_micro_act,    // [20] Micro servo motor for precise position control in small animatronics
    goblin_right_ear_act,    // [21] goblin_right_ear component
    servo_sg90_micro_act     // [22] Micro servo motor for precise position control in small animatronics
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    1,    // [2] spi_bus - every 1 loops
    1,    // [3] goblin_left_eye - every 1 loops
    1,    // [4] goblin_eye - every 1 loops
    1,    // [5] gc9a01 - every 1 loops
    1,    // [6] generic_spi_display - every 1 loops
    60000,    // [7] goblin_right_eye - every 60000 loops
    1,    // [8] goblin_eye - every 1 loops
    1,    // [9] gc9a01 - every 1 loops
    1,    // [10] generic_spi_display - every 1 loops
    180000,    // [11] goblin_nose - every 180000 loops
    1,    // [12] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    36000,    // [13] goblin_mouth - every 36000 loops
    1,    // [14] goblin_mouth - every 1 loops
    1,    // [15] gc9a01 - every 1 loops
    1,    // [16] generic_spi_display - every 1 loops
    84000,    // [17] goblin_speaker - every 84000 loops
    1,    // [18] speaker - every 1 loops
    1,    // [19] goblin_left_ear - every 1 loops
    1,    // [20] servo_sg90_micro - every 1 loops
    1,    // [21] goblin_right_ear - every 1 loops
    1     // [22] servo_sg90_micro - every 1 loops
};
