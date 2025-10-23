// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "heartbeat.hpp"
#include "network_monitor.hpp"
#include "components/goblin_left_eye.hpp"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[INIT_TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    goblin_left_eye_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[ACT_TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    goblin_left_eye_act     // [2] Goblin left eye display test
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[ACT_TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    100   // [2] goblin_left_eye - every 100 loops (change color every ~10 seconds)
};
