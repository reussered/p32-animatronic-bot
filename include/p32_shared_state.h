// P32 Shared State - Global State Synchronized Across All Subsystems via ESP-NOW Mesh
// This file defines the global state structure that is replicated on every ESP32 subsystem
// and kept in sync via the ESP-NOW mesh network component.

#ifndef P32_SHARED_STATE_H
#define P32_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Global loop counter (64-bit to prevent overflow)
// At 120,000 iterations/second: 32-bit overflows in ~10 hours, 64-bit in 4.8 million years
// Components can READ this directly, but ONLY main.c should write it
extern uint64_t g_loopCount;

// Shared state structure (synchronized via ESP-NOW mesh across all subsystems)
// This is a PLAIN C STRUCT - no C++ objects, only POD types for ESP-NOW transmission
typedef struct {
    // ===== Version and sync metadata =====
    uint32_t version;           // Incremented on every state change
    uint32_t timestamp_ms;      // When this state was last updated
    uint8_t source_node_id;     // Which subsystem originated the change
    
    // ===== Mood state (serialized from C++ Mood class - see include/Mood.hpp) =====
    // Mood class has 9 components (int8_t, -128 to +127 each):
    // [0] ANGER, [1] FEAR, [2] HAPPINESS, [3] SADNESS, [4] CURIOSITY, 
    // [5] AFFECTION, [6] IRRITATION, [7] CONTENTMENT, [8] EXCITEMENT
    int8_t mood_serialized[9];
    
    // ===== Personality state (serialized from C++ Personality class) =====
    // Personality traits affect behavior patterns and mood responses
    // Serialized representation for mesh synchronization
    uint8_t personality_serialized[32];  // Reserved for future Personality class
    
    // ===== Sensor aggregation =====
    uint8_t distance_cm;        // Nose sensor distance (HC-SR04)
    bool touch_detected;        // Touch sensor state
    int16_t temperature_c;      // Temperature reading
    float light_level;          // Ambient light level (0.0 - 1.0)
    
    // ===== Behavioral flags =====
    bool is_speaking;           // Audio output active
    bool is_moving;             // Motion in progress
    uint8_t current_behavior;   // Active behavior ID
    uint8_t attention_target;   // What the bot is focused on
    
    // ===== System status =====
    uint8_t battery_percent;    // Battery level (0-100)
    uint16_t uptime_seconds;    // System uptime
    bool wifi_connected;        // WiFi status
    bool mesh_healthy;          // All mesh nodes responding
    uint8_t cpu_usage_percent;  // CPU utilization
    
    // ===== Checksum for validation =====
    uint32_t checksum;          // CRC32 of entire structure
} p32_shared_state_t;

// Global shared state instance (one copy per ESP32 subsystem)
extern p32_shared_state_t g_shared_state;

#ifdef __cplusplus
}
#endif

// ===== C++ Integration with Class-Based Components =====
#ifdef __cplusplus
#include "Mood.hpp"
// #include "Personality.hpp"  // Future: Add when Personality class is implemented

// ===== Global C++ Class Instances =====
// These are the "real" objects that components work with
// They get serialized into g_shared_state for ESP-NOW mesh transmission

extern Mood g_mood;                    // C++ Mood class instance
// extern Personality g_personality;   // Future: Personality class instance

// ===== Synchronization Helpers: Class ↔ Serialized State =====

// Mood serialization: Copy C++ class to POD struct (fast memcpy)
inline void mood_serialize() {
    memcpy(g_shared_state.mood_serialized, g_mood.components, sizeof(g_mood.components));
}

// Mood deserialization: Copy POD struct to C++ class (fast memcpy)
inline void mood_deserialize() {
    memcpy(g_mood.components, g_shared_state.mood_serialized, sizeof(g_mood.components));
}

// Future: Personality serialization/deserialization
// inline void personality_serialize() {
//     memcpy(g_shared_state.personality_serialized, &g_personality, sizeof(g_personality));
// }
// inline void personality_deserialize() {
//     memcpy(&g_personality, g_shared_state.personality_serialized, sizeof(g_personality));
// }

// Master synchronization: Call before ESP-NOW broadcast
inline void sync_all_classes_to_shared_state() {
    mood_serialize();
    // personality_serialize();  // Future
}

// Slave synchronization: Call after ESP-NOW receive
inline void sync_all_classes_from_shared_state() {
    mood_deserialize();
    // personality_deserialize();  // Future
}

// ===== Dirty Checking: Only Broadcast When State Actually Changes =====
// This prevents unnecessary ESP-NOW transmissions

// METHOD 1: Per-Class Change Detection (uses operator!=)
// ──────────────────────────────────────────────────────────────────────
// Previous state snapshots for comparison
extern Mood g_mood_previous;
// extern Personality g_personality_previous;  // Future

// Check if ANY C++ class has changed since last broadcast
// Uses operator!= implemented in each class for efficient comparison
inline bool have_classes_changed() {
    // Each class implements operator!= for efficient comparison
    if (g_mood != g_mood_previous) return true;
    // if (g_personality != g_personality_previous) return true;  // Future
    
    return false;
}

// Save current state as "previous" after successful broadcast
inline void save_classes_as_previous() {
    g_mood_previous = g_mood;  // Uses Mood::operator= (memcpy internally)
    // g_personality_previous = g_personality;  // Future
}

// METHOD 2: Whole-Struct Change Detection (uses memcmp on POD)
// ──────────────────────────────────────────────────────────────────────
// Previous POD state for raw memory comparison
extern p32_shared_state_t g_shared_state_previous;

// SIMPLEST METHOD: Treat entire struct as raw memory block
// Compare entire block, send entire block, copy entire block
// Metadata (version, timestamp) changes handled AFTER change detection
inline bool has_shared_state_changed() {
    // Compare ENTIRE memory block as raw bytes
    return (memcmp(&g_shared_state, &g_shared_state_previous, 
                   sizeof(p32_shared_state_t)) != 0);
}

// Save entire memory block for next comparison
inline void save_shared_state_as_previous() {
    memcpy(&g_shared_state_previous, &g_shared_state, 
           sizeof(p32_shared_state_t));
}

// USAGE PATTERN (in mesh component):
//   1. Modify C++ classes (g_mood.addHappiness(10), etc.)
//   2. sync_all_classes_to_shared_state()  // Serialize C++ → POD
//   3. if (has_shared_state_changed()) {    // Compare raw memory
//          g_shared_state.version++;        // Update metadata
//          esp_now_send(&g_shared_state);   // Send raw block
//          save_shared_state_as_previous(); // Copy raw block
//      }

#endif // __cplusplus

#endif // P32_SHARED_STATE_H
