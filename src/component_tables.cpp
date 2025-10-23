// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "heartbeat.hpp"
#include "network_monitor.hpp"
#include "test_head.hpp"
#include "goblin_left_eye.hpp"
#include "goblin_eye.hpp"
#include "gc9a01.hpp"
#include "goblin_right_eye.hpp"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    test_head_init,
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    test_head_act,    // [2] Test head subsystem with dual displays for hardware validation
    goblin_left_eye_act,    // [3] Left eye display animation
    goblin_eye_act,    // [4] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act,    // [5] gc9a01 component
    goblin_right_eye_act,    // [6] Right eye display animation
    goblin_eye_act,    // [7] Shared goblin eye processing logic with palette and mood-based rendering
    gc9a01_act     // [8] gc9a01 component
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    1,    // [2] test_head - every 1 loops
    1,    // [3] goblin_left_eye - every 1 loops
    1,    // [4] goblin_eye - every 1 loops
    1,    // [5] gc9a01 - every 1 loops
    60000,    // [6] goblin_right_eye - every 60000 loops
    1,    // [7] goblin_eye - every 1 loops
    1     // [8] gc9a01 - every 1 loops
};
