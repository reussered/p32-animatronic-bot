// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "components/heartbeat.hpp"
#include "components/network_monitor.hpp"
#include "components/test_head.hpp"
#include "components/goblin_eye.hpp"
#include "components/gc9a01.hpp"
#include "components/goblin_right_eye.hpp"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    test_head_init,
    goblin_eye_init,
    gc9a01_init,
    goblin_right_eye_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    test_head_act,    // [2] Test head subsystem with dual displays for hardware validation
    goblin_eye_act,    // [3] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [4] gc9a01 component
    goblin_right_eye_act     // [5] Right eye display animation
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    1,    // [2] test_head - every 1 loops
    1,    // [3] goblin_eye - every 1 loops
    1,    // [4] gc9a01 - every 1 loops
    60000     // [5] goblin_right_eye - every 60000 loops
};
