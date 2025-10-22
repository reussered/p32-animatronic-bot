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
    goblin_left_eye_init,
    goblin_right_eye_init,
    goblin_nose_init,
    goblin_mouth_init,
    goblin_speaker_init,
    unknown_init,
    unknown_init,
    goblin_torso_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    goblin_head_act,    // [2] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    goblin_left_eye_act,    // [3] Left eye display animation
    goblin_right_eye_act,    // [4] Right eye display animation
    goblin_nose_act,    // [5] Proximity sensor monitoring
    goblin_mouth_act,    // [6] Mouth display animation
    goblin_speaker_act,    // [7] Audio output processing
    unknown_act,    // [8] unknown component
    unknown_act,    // [9] unknown component
    goblin_torso_act     // [10] Goblin torso subsystem - Master controller with mesh networking, WiFi, and system coordination
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops (120000 Hz)
    1,    // [1] network_monitor - every 1 loops (120000 Hz)
    25,    // [2] goblin_head - every 25 loops (4800 Hz)
    60000,    // [3] goblin_left_eye - every 60000 loops (2.0 Hz)
    60000,    // [4] goblin_right_eye - every 60000 loops (2.0 Hz)
    180000,    // [5] goblin_nose - every 180000 loops (1500.0ms period)
    36000,    // [6] goblin_mouth - every 36000 loops (3.3 Hz)
    84000,    // [7] goblin_speaker - every 84000 loops (1.4 Hz)
    24000,    // [8] unknown - every 24000 loops (5.0 Hz)
    24000,    // [9] unknown - every 24000 loops (5.0 Hz)
    50     // [10] goblin_torso - every 50 loops (2400 Hz)
};
