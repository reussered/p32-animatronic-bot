// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/test_head.hdr"
#include "components/goblin_left_eye.hdr"
#include "components/goblin_eye.hdr"
#include "components/gc9a01.hdr"
#include "components/spi_bus.hdr"
#include "components/goblin_right_eye.hdr"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    test_head_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_bus_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    test_head_act,    // [0] Test head subsystem with single left display for hardware validation
    goblin_left_eye_act,    // [1] Left eye display animation
    goblin_eye_act,    // [2] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [3] <describe the components purpose and functionality here>
    spi_bus_act,    // [4] ESP32 VSPI bus interface for SPI communication
    goblin_right_eye_act,    // [5] Right eye display with bird animation
    goblin_eye_act,    // [6] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [7] <describe the components purpose and functionality here>
    spi_bus_act     // [8] ESP32 VSPI bus interface for SPI communication
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] test_head - every 1 loops
    1,    // [1] goblin_left_eye - every 1 loops
    1,    // [2] goblin_eye - every 1 loops
    1,    // [3] gc9a01 - every 1 loops
    1,    // [4] spi_bus - every 1 loops
    1,    // [5] goblin_right_eye - every 1 loops
    1,    // [6] goblin_eye - every 1 loops
    1,    // [7] gc9a01 - every 1 loops
    1     // [8] spi_bus - every 1 loops
};
