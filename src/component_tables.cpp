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
    generic_spi_display_init
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
    generic_spi_display_act     // [8] generic_spi_display component
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
    1     // [8] generic_spi_display - every 1 loops
};
