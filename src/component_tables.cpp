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
#include "components/spi_bus_vspi.hdr"
#include "components/generic_spi_display.hdr"
#include "components/goblin_right_eye.hdr"
#include "components/goblin_nose.hdr"
#include "components/goblin_mouth.hdr"
#include "components/goblin_speaker.hdr"
#include "components/goblin_left_ear.hdr"
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
    spi_bus_vspi_init,
    generic_spi_display_init,
    spi_bus_vspi_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_vspi_init,
    generic_spi_display_init,
    spi_bus_vspi_init,
    generic_spi_display_init,
    goblin_nose_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    spi_bus_vspi_init,
    generic_spi_display_init,
    spi_bus_vspi_init,
    generic_spi_display_init,
    goblin_speaker_init,
    goblin_left_ear_init,
    goblin_right_ear_init
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
    spi_bus_vspi_act,    // [5] ESP32 VSPI bus interface for SPI communication
    generic_spi_display_act,    // [6] generic_spi_display component
    spi_bus_vspi_act,    // [7] ESP32 VSPI bus interface for SPI communication
    generic_spi_display_act,    // [8] generic_spi_display component
    goblin_right_eye_act,    // [9] Right eye display animation
    goblin_eye_act,    // [10] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [11] gc9a01 component
    spi_bus_vspi_act,    // [12] ESP32 VSPI bus interface for SPI communication
    generic_spi_display_act,    // [13] generic_spi_display component
    spi_bus_vspi_act,    // [14] ESP32 VSPI bus interface for SPI communication
    generic_spi_display_act,    // [15] generic_spi_display component
    goblin_nose_act,    // [16] Proximity sensor monitoring
    goblin_mouth_act,    // [17] Mouth display animation
    goblin_mouth_act,    // [18] Shared goblin mouth processing logic with speech animation and mood-based rendering
    gc9a01_act,    // [19] gc9a01 component
    spi_bus_vspi_act,    // [20] ESP32 VSPI bus interface for SPI communication
    generic_spi_display_act,    // [21] generic_spi_display component
    spi_bus_vspi_act,    // [22] ESP32 VSPI bus interface for SPI communication
    generic_spi_display_act,    // [23] generic_spi_display component
    goblin_speaker_act,    // [24] Audio output processing
    goblin_left_ear_act,    // [25] goblin_left_ear component
    goblin_right_ear_act     // [26] goblin_right_ear component
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
    1,    // [5] spi_bus_vspi - every 1 loops
    1,    // [6] generic_spi_display - every 1 loops
    1,    // [7] spi_bus_vspi - every 1 loops
    1,    // [8] generic_spi_display - every 1 loops
    60000,    // [9] goblin_right_eye - every 60000 loops
    1,    // [10] goblin_eye - every 1 loops
    1,    // [11] gc9a01 - every 1 loops
    1,    // [12] spi_bus_vspi - every 1 loops
    1,    // [13] generic_spi_display - every 1 loops
    1,    // [14] spi_bus_vspi - every 1 loops
    1,    // [15] generic_spi_display - every 1 loops
    180000,    // [16] goblin_nose - every 180000 loops
    36000,    // [17] goblin_mouth - every 36000 loops
    1,    // [18] goblin_mouth - every 1 loops
    1,    // [19] gc9a01 - every 1 loops
    1,    // [20] spi_bus_vspi - every 1 loops
    1,    // [21] generic_spi_display - every 1 loops
    1,    // [22] spi_bus_vspi - every 1 loops
    1,    // [23] generic_spi_display - every 1 loops
    84000,    // [24] goblin_speaker - every 84000 loops
    1,    // [25] goblin_left_ear - every 1 loops
    1     // [26] goblin_right_ear - every 1 loops
};
