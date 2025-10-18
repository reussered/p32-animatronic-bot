#include "p32_component_tables.hpp"

// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

// Global loop counter defined in p32_shared_state.cpp
extern uint64_t g_loopCount;

// ============================================================================
// Initialization Table
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_init,
    p32_comp_network_monitor_init,
    p32_comp_left_eye_init,
    gsm_test_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_act,        // [0] System heartbeat
    p32_comp_network_monitor_act,  // [1] Network monitoring and loop timing  
    p32_comp_left_eye_act,         // [2] Left eye display animation
    gsm_test_act                   // [3] GlobalSharedMemory test component
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    60000,    // [0] heartbeat - every 60000 loops (2.0 Hz)
    60000,    // [1] network_monitor - every 60000 loops (2.0 Hz)
    60000,    // [2] left_eye - every 60000 loops (2.0 Hz)
    30000     // [3] gsm_test - every 30000 loops (4.0 Hz) - faster for testing
};
