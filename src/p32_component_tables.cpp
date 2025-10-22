#include "p32_component_tables.h"

// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

// ============================================================================
// Initialization Table
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    heartbeat_init,
    network_monitor_init,
    goblin_head_init,
    goblin_torso_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    goblin_head_act,    // [2] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    goblin_torso_act     // [3] Goblin torso subsystem - Master controller with mesh networking, WiFi, and system coordination
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops (120000 Hz)
    1,    // [1] network_monitor - every 1 loops (120000 Hz)
    25,    // [2] goblin_head - every 25 loops (4800 Hz)
    50     // [3] goblin_torso - every 50 loops (2400 Hz)
};
