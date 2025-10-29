// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/heartbeat.hpp"
#include "components/network_monitor.hpp"
#include "components/test_head.hpp"
#include "components/goblin_left_eye.hpp"
#include "components/spi_bus.hpp"
#include "components/goblin_eye.hpp"
#include "components/gc9a01.hpp"
#include "components/generic_spi_display.hpp"
#include "components/goblin_right_eye.hpp"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    test_head_init,
    goblin_left_eye_init,
    spi_bus_init,
    goblin_eye_init,
    gc9a01_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    spi_bus_init,
    goblin_eye_init,
    gc9a01_init,
    generic_spi_display_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    test_head_act,    // [2] Test head subsystem with dual displays for hardware validation
    goblin_left_eye_act,    // [3] Left eye display animation
    spi_bus_act,    // [4] spi_bus component
    goblin_eye_act,    // [5] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [6] gc9a01 component
    generic_spi_display_act,    // [7] generic_spi_display component
    goblin_right_eye_act,    // [8] Right eye display animation
    spi_bus_act,    // [9] spi_bus component
    goblin_eye_act,    // [10] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [11] gc9a01 component
    generic_spi_display_act     // [12] generic_spi_display component
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    1,    // [2] test_head - every 1 loops
    1,    // [3] goblin_left_eye - every 1 loops
    1,    // [4] spi_bus - every 1 loops
    1,    // [5] goblin_eye - every 1 loops
    1,    // [6] gc9a01 - every 1 loops
    1,    // [7] generic_spi_display - every 1 loops
    60000,    // [8] goblin_right_eye - every 60000 loops
    1,    // [9] spi_bus - every 1 loops
    1,    // [10] goblin_eye - every 1 loops
    1,    // [11] gc9a01 - every 1 loops
    1     // [12] generic_spi_display - every 1 loops
};
