// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/heartbeat.hdr"
#include "components/network_monitor.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/generic_spi_display.hdr"
#include "components/goblin_right_eye.hdr"
#include "components/goblin_nose.hdr"
#include "components/hc_sr04_ultrasonic_distance_sensor.hdr"
#include "components/goblin_mouth.hdr"
#include "components/goblin_speaker.hdr"
#include "components/speaker.hdr"
#include "components/goblin_left_ear.hdr"
#include "components/servo_sg90_micro.hdr"
#include "components/goblin_right_ear.hdr"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    generic_spi_display_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    generic_spi_display_init,
    generic_spi_display_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    generic_spi_display_init,
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
    goblin_left_eye_act,    // [2] Left eye display animation
    goblin_eye_act,    // [3] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [4] gc9a01 component
    generic_spi_display_act,    // [5] generic_spi_display component
    generic_spi_display_act,    // [6] generic_spi_display component
    goblin_right_eye_act,    // [7] Right eye display animation
    goblin_eye_act,    // [8] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [9] gc9a01 component
    generic_spi_display_act,    // [10] generic_spi_display component
    generic_spi_display_act,    // [11] generic_spi_display component
    goblin_nose_act,    // [12] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [13] hc_sr04_ultrasonic_distance_sensor component
    goblin_mouth_act,    // [14] Mouth display animation
    goblin_mouth_act,    // [15] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [16] gc9a01 component
    generic_spi_display_act,    // [17] generic_spi_display component
    generic_spi_display_act,    // [18] generic_spi_display component
    goblin_speaker_act,    // [19] Audio output processing
    speaker_act,    // [20] speaker component
    goblin_left_ear_act,    // [21] goblin_left_ear component
    servo_sg90_micro_act,    // [22] Micro servo motor for precise position control in small animatronics
    goblin_right_ear_act,    // [23] goblin_right_ear component
    servo_sg90_micro_act     // [24] Micro servo motor for precise position control in small animatronics
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    1,    // [2] goblin_left_eye - every 1 loops
    1,    // [3] goblin_eye - every 1 loops
    1,    // [4] gc9a01 - every 1 loops
    1,    // [5] generic_spi_display - every 1 loops
    1,    // [6] generic_spi_display - every 1 loops
    60000,    // [7] goblin_right_eye - every 60000 loops
    1,    // [8] goblin_eye - every 1 loops
    1,    // [9] gc9a01 - every 1 loops
    1,    // [10] generic_spi_display - every 1 loops
    1,    // [11] generic_spi_display - every 1 loops
    180000,    // [12] goblin_nose - every 180000 loops
    1,    // [13] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    36000,    // [14] goblin_mouth - every 36000 loops
    1,    // [15] goblin_mouth - every 1 loops
    1,    // [16] gc9a01 - every 1 loops
    1,    // [17] generic_spi_display - every 1 loops
    1,    // [18] generic_spi_display - every 1 loops
    84000,    // [19] goblin_speaker - every 84000 loops
    1,    // [20] speaker - every 1 loops
    1,    // [21] goblin_left_ear - every 1 loops
    1,    // [22] servo_sg90_micro - every 1 loops
    1,    // [23] goblin_right_ear - every 1 loops
    1     // [24] servo_sg90_micro - every 1 loops
};
