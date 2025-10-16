// P32 Shared State - Global State Implementation
// Global instances of loop counter and shared state

#include "p32_shared_state.hpp"
#include <string.h>

// Global loop counter (64-bit to prevent overflow at 120,000 iterations/sec)
// NOTE: Non-const definition here, allows main.c to write via extern declaration
// Components see it as const via p32_shared_state.h
uint64_t g_loopCount = 0;

// Global shared state (synchronized via ESP-NOW mesh across all subsystems)
p32_shared_state_t g_shared_state = {
    .version = 0,
    .timestamp_ms = 0,
    .source_node_id = 0,
    .mood_serialized = {0, 0, 0, 0, 0, 0, 0, 0, 0},
    .personality_serialized = {0},  // Reserved for future Personality class
    .distance_cm = 0,
    .touch_detected = false,
    .temperature_c = 0,
    .light_level = 0.0f,
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

// C++ Global Mood instance (defined in C++ compilation unit)
#ifdef __cplusplus
Mood g_mood;
Mood g_mood_previous;  // For dirty checking (operator!= comparison)
// Personality g_personality;        // Future
// Personality g_personality_previous;  // Future
#endif

// Shadow copy for whole-struct change detection (fast memcmp method)
p32_shared_state_t g_shared_state_previous = {0};
