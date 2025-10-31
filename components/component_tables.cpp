// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/goblin_head_left_eye_only.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/spi_bus.hdr"
#include "components/generic_spi_display.hdr"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    goblin_head_left_eye_only_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init,
    generic_spi_display_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    goblin_head_left_eye_only_act,    // [0] Goblin head subsystem with single left eye only - minimal configuration for testing
    goblin_left_eye_act,    // [1] Left eye display animation
    goblin_eye_act,    // [2] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [3] <describe the components purpose and functionality here>
    spi_bus_act,    // [4] ESP32 HSPI bus interface for SPI communication
    generic_spi_display_act     // [5] <describe the components purpose and functionality here>
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    25,    // [0] goblin_head_left_eye_only - every 25 loops
    1,    // [1] goblin_left_eye - every 1 loops
    1,    // [2] goblin_eye - every 1 loops
    1,    // [3] gc9a01 - every 1 loops
    1,    // [4] spi_bus - every 1 loops
    1     // [5] generic_spi_display - every 1 loops
};

