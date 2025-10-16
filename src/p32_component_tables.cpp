#include "p32_component_tables.hpp"
#include "p32_shared_state.hpp"

// ============================================================================
// Global Loop Counter - Written by main.c, read by all components
// ============================================================================

uint64_t g_loopCount = 0;

// ============================================================================
// Global Shared State - Synchronized across all subsystems via ESP-NOW mesh
// ============================================================================

p32_shared_state_t g_shared_state = {
    .version = 0,
    .timestamp_ms = 0,
    .source_node_id = 0,
    .distance_cm = 100,  // Initialize to "far away" (idle mode)
    .touch_detected = false,
    .temperature_c = 20,
    .light_level = 0.5f,
    .is_speaking = false,
    .is_moving = false,
    .current_behavior = 0,
    .attention_target = 0,
    .battery_percent = 100,
    .uptime_seconds = 0,
    .wifi_connected = false,
    .mesh_healthy = false,
    .cpu_usage_percent = 0,
    .checksum = 0
};

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
    p32_comp_left_eye_init,
    p32_comp_right_eye_init,
    p32_comp_mouth_init,
    p32_comp_speaker_init,
    p32_comp_nose_sensor_init,
    p32_comp_left_ear_microphone_init,
    p32_comp_right_ear_microphone_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_heartbeat_act,    // [0] System heartbeat
    p32_comp_network_monitor_act,    // [1] Network monitoring and loop timing
    p32_comp_left_eye_act,    // [2] Left eye display animation - goblin variant using standard GC9A01 hardware
    p32_comp_right_eye_act,    // [3] Right eye display animation - goblin variant using standard GC9A01 hardware
    p32_comp_mouth_act,    // [4] Mouth display animation - goblin variant with wide grin expressions
    p32_comp_speaker_act,    // [5] Audio output processing - goblin variant with guttural sound profile
    p32_comp_nose_sensor_act,    // [6] Proximity sensor monitoring - goblin hard nose variant
    p32_comp_left_ear_microphone_act,    // [7] Left ear HW-496 microphone for directional audio input
    p32_comp_right_ear_microphone_act     // [8] Right ear HW-496 microphone for directional audio input
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    120000,    // [0] heartbeat - every 120000 loops (1 Hz / 1 second)
    60000,     // [1] network_monitor - every 60000 loops (2 Hz / 500ms)
    6000,      // [2] left_eye - every 6000 loops (20 Hz / 50ms) - smooth animation
    6000,      // [3] right_eye - every 6000 loops (20 Hz / 50ms) - smooth animation
    4000,      // [4] mouth - every 4000 loops (30 Hz / 33ms) - smooth speech
    12000,     // [5] speaker - every 12000 loops (10 Hz / 100ms) - audio chunks
    24000,     // [6] nose_sensor - every 24000 loops (5 Hz / 200ms) - sensor polling
    30000,     // [7] left_ear_microphone - every 30000 loops (4 Hz / 250ms)
    30000      // [8] right_ear_microphone - every 30000 loops (4 Hz / 250ms)
};
