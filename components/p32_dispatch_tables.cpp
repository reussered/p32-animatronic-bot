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
    goblin_left_eye_init,
    goblin_nose_init,
    goblin_right_eye_init,
    mg996r_servo_init,
    servo_sg90_micro_init,
    torso_integrated_power_system_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    goblin_left_eye_act,    // [0] goblin_left_eye component
    goblin_nose_act,    // [1] goblin_nose component
    goblin_right_eye_act,    // [2] goblin_right_eye component
    mg996r_servo_act,    // [3] mg996r_servo component
    servo_sg90_micro_act,    // [4] servo_sg90_micro component
    torso_integrated_power_system_act    // [5] torso_integrated_power_system component
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // [0] goblin_left_eye - every 1 loops (120000 Hz)
    1,    // [1] goblin_nose - every 1 loops (120000 Hz)
    1,    // [2] goblin_right_eye - every 1 loops (120000 Hz)
    1,    // [3] mg996r_servo - every 1 loops (120000 Hz)
    1,    // [4] servo_sg90_micro - every 1 loops (120000 Hz)
    1    // [5] torso_integrated_power_system - every 1 loops (120000 Hz)
};
