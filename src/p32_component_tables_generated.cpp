#include "p32_component_tables.hpp"

// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

// ============================================================================
// Initialization Table
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_init,
    p32_comp_network_monitor_init,
    p32_comp_simple_core_test_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_act,    // [0] System heartbeat
    p32_comp_network_monitor_act,    // [1] Network monitoring and loop timing
    p32_comp_simple_core_test_act     // [2] Simple blink test to verify ESP32-S3 can execute code - blinks onboard LED
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops (120000 Hz)
    1,    // [1] network_monitor - every 1 loops (120000 Hz)
    10     // [2] simple_core_test - every 10 loops (12000 Hz)
};
