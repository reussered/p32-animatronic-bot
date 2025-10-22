#include "p32_dispatch_tables.hpp"
#include "p32_component_functions.hpp"

// ============================================================================
// Global Loop Counter - Written by main.cpp, read by all components
// ============================================================================

uint64_t g_loopCount = 0;

// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

// ============================================================================
// Initialization Table
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    mg996r_servo_init,
    torso_integrated_power_system_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    mg996r_servo_act,    // [0] mg996r_servo component
    torso_integrated_power_system_act    // [1] torso_integrated_power_system component
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // [0] mg996r_servo - every 1 loops (120000 Hz)
    1    // [1] torso_integrated_power_system - every 1 loops (120000 Hz)
};
