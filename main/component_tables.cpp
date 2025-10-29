// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/goblin_personality.hdr"
#include "components/goblin_mood.hdr"
#include "components/goblin_behavior_engine.hdr"
#include "components/goblin_head.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/spi_bus.hdr"
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
#include "components/goblin_torso.hdr"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    goblin_personality_init,
    goblin_mood_init,
    goblin_behavior_engine_init,
    goblin_head_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init,
    generic_spi_display_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    spi_bus_init,
    generic_spi_display_init,
    goblin_speaker_init,
    speaker_init,
    goblin_left_ear_init,
    servo_sg90_micro_init,
    goblin_right_ear_init,
    servo_sg90_micro_init,
    goblin_torso_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    goblin_personality_act,    // [0] Goblin family personality traits - mischievous, aggressive, curious
    goblin_mood_act,    // [1] Goblin 8-mood emotional state machine - integrates sensor inputs and personality
    goblin_behavior_engine_act,    // [2] Goblin behavior selection engine - chooses actions based on mood and personality
    goblin_head_act,    // [3] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    goblin_left_eye_act,    // [4] Left eye display animation
    goblin_eye_act,    // [5] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [6] <describe the components purpose and functionality here>
    spi_bus_act,    // [7] ESP32 HSPI bus interface for SPI communication
    generic_spi_display_act,    // [8] <describe the components purpose and functionality here>
    goblin_right_eye_act,    // [9] Right eye display with bird animation
    goblin_eye_act,    // [10] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [11] <describe the components purpose and functionality here>
    spi_bus_act,    // [12] ESP32 HSPI bus interface for SPI communication
    generic_spi_display_act,    // [13] <describe the components purpose and functionality here>
    goblin_nose_act,    // [14] Proximity sensor monitoring
    hc_sr04_ultrasonic_distance_sensor_act,    // [15] <describe the components purpose and functionality here>
    goblin_mouth_act,    // [16] Mouth display animation
    goblin_mouth_act,    // [17] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [18] <describe the components purpose and functionality here>
    spi_bus_act,    // [19] ESP32 HSPI bus interface for SPI communication
    generic_spi_display_act,    // [20] <describe the components purpose and functionality here>
    goblin_speaker_act,    // [21] Audio output processing
    speaker_act,    // [22] <describe the components purpose and functionality here>
    goblin_left_ear_act,    // [23] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [24] Micro servo motor for precise position control in small animatronics
    goblin_right_ear_act,    // [25] <describe the components purpose and functionality here>
    servo_sg90_micro_act,    // [26] Micro servo motor for precise position control in small animatronics
    goblin_torso_act     // [27] Goblin torso subsystem - Master controller with WiFi, and system coordination
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    25,    // [0] goblin_personality - every 25 loops
    15,    // [1] goblin_mood - every 15 loops
    10,    // [2] goblin_behavior_engine - every 10 loops
    25,    // [3] goblin_head - every 25 loops
    1,    // [4] goblin_left_eye - every 1 loops
    1,    // [5] goblin_eye - every 1 loops
    1,    // [6] gc9a01 - every 1 loops
    1,    // [7] spi_bus_vspi - every 1 loops
    1,    // [8] generic_spi_display - every 1 loops
    1,    // [9] goblin_right_eye - every 1 loops
    1,    // [10] goblin_eye - every 1 loops
    1,    // [11] gc9a01 - every 1 loops
    1,    // [12] spi_bus_vspi - every 1 loops
    1,    // [13] generic_spi_display - every 1 loops
    180000,    // [14] goblin_nose - every 180000 loops
    1,    // [15] hc_sr04_ultrasonic_distance_sensor - every 1 loops
    36000,    // [16] goblin_mouth - every 36000 loops
    1,    // [17] goblin_mouth - every 1 loops
    1,    // [18] gc9a01 - every 1 loops
    1,    // [19] spi_bus_vspi - every 1 loops
    1,    // [20] generic_spi_display - every 1 loops
    84000,    // [21] goblin_speaker - every 84000 loops
    1,    // [22] speaker - every 1 loops
    1,    // [23] goblin_left_ear - every 1 loops
    1,    // [24] servo_sg90_micro - every 1 loops
    1,    // [25] goblin_right_ear - every 1 loops
    1,    // [26] servo_sg90_micro - every 1 loops
    50     // [27] goblin_torso - every 50 loops
};
