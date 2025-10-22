#include "p32_component_tables.h"

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
    p32_comp_goblin_head_left_eye_init,
    p32_comp_goblin_head_right_eye_init,
    p32_comp_goblin_head_mouth_init,
    p32_comp_goblin_head_speaker_init,
    p32_comp_goblin_head_nose_init,
    p32_comp_left_ear_microphone_init,
    p32_comp_right_ear_microphone_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_act,    // [0] System heartbeat
    p32_comp_network_monitor_act,    // [1] Network monitoring and loop timing
    p32_comp_goblin_head_left_eye_act,    // [2] Left eye display animation - goblin variant using standard GC9A01 hardware
    p32_comp_goblin_head_right_eye_act,    // [3] Right eye display animation - goblin variant using standard GC9A01 hardware
    p32_comp_goblin_head_mouth_act,    // [4] Mouth display animation - goblin variant with wide grin expressions
    p32_comp_goblin_head_speaker_act,    // [5] Audio output processing - goblin variant with guttural sound profile
    p32_comp_goblin_head_nose_act,    // [6] Proximity sensor monitoring - goblin hard nose variant
    p32_comp_left_ear_microphone_act,    // [7] Left ear HW-496 microphone for directional audio input
    p32_comp_right_ear_microphone_act     // [8] Right ear HW-496 microphone for directional audio input
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops (120000 Hz)
    1,    // [1] network_monitor - every 1 loops (120000 Hz)
    5,    // [2] goblin_head_left_eye - every 5 loops (24000 Hz)
    5,    // [3] goblin_head_right_eye - every 5 loops (24000 Hz)
    3,    // [4] goblin_head_mouth - every 3 loops (40000 Hz)
    7,    // [5] goblin_head_speaker - every 7 loops (17143 Hz)
    15,    // [6] goblin_head_nose - every 15 loops (8000 Hz)
    20,    // [7] left_ear_microphone - every 20 loops (6000 Hz)
    20     // [8] right_ear_microphone - every 20 loops (6000 Hz)
};
