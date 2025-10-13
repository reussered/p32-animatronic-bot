// P32 Delta-Based Mood Transition System
// Real-time mood effects without frame buffering - ESP32 memory friendly

#ifndef P32_MOOD_TRANSITIONS_H
#define P32_MOOD_TRANSITIONS_H

#include "p32_mood_palette.h"
#include <stdint.h>
#include <stdbool.h>

// Mood transition delta tracking
typedef struct {
    mood_type_t mood_type;          // ANGER, FEAR, HAPPINESS, etc.
    int16_t old_intensity;          // Previous intensity (0-999) 
    int16_t new_intensity;          // Current intensity (0-999)
    int16_t delta;                  // Change amount (can be negative)
    float overshoot_multiplier;     // 0.5-2.0 for mood effects (nervousness, etc.)
    bool reverse_on_decrease;       // True = calculate reverse transform
    uint32_t transition_start_ms;   // When transition began
    uint32_t transition_duration_ms; // How long transition takes
} mood_delta_t;

// Real-time frame processing (no buffering)
typedef struct {
    const uint8_t* indexed_pixels;  // Pointer to source palette indices (ROM/Flash)
    uint16_t width;                 // Frame dimensions
    uint16_t height;                // Frame dimensions
    uint16_t pixel_count;           // Total pixels
    mood_delta_t current_delta;     // Active mood transition
    bool has_active_transition;     // Transition in progress
} realtime_frame_t;

// Mood transition manager - no frame storage!
typedef struct {
    mood_state_t previous_mood;     // Last known mood state
    mood_state_t current_mood;      // Current target mood
    mood_delta_t active_deltas[8];  // One per mood type (ANGER, FEAR, etc.)
    uint8_t active_delta_count;     // Number of ongoing transitions
    uint32_t total_transitions;     // Statistics
    uint32_t overshoot_events;      // Nervousness/internal anger events
    float global_overshoot;         // Global overshoot multiplier (1.0 = normal)
    bool nervousness_mode;          // Enable overshoot effects
} mood_transition_manager_t;

// Function declarations - Delta-based mood system
esp_err_t mood_transitions_init(void);

// Mood change detection and delta calculation
esp_err_t mood_transitions_set_new_mood(const mood_state_t* new_mood);
void mood_transitions_calculate_deltas(const mood_state_t* old_mood, const mood_state_t* new_mood);
bool mood_transitions_has_active_deltas(void);

// Real-time pixel processing (no buffering - calculate on demand)
uint16_t mood_transitions_apply_to_pixel(uint8_t palette_index, mood_type_t primary_mood);
esp_err_t mood_transitions_render_frame_realtime(const realtime_frame_t* frame, uint16_t* output_buffer);

// Mood overshoot effects for emotional complexity
void mood_transitions_set_global_overshoot(float multiplier);  // 0.5 = dampened, 2.0 = exaggerated
void mood_transitions_enable_nervousness(bool enable);         // Random overshoot variations
void mood_transitions_add_internal_tension(mood_type_t mood, float tension_level);

// Delta reversal - no need to store previous frames!
uint16_t mood_transitions_reverse_pixel_change(uint8_t palette_index, const mood_delta_t* delta);
void mood_transitions_reverse_mood_change(mood_type_t mood_type, int16_t reverse_amount);

// Performance monitoring
void mood_transitions_print_stats(void);
uint32_t mood_transitions_get_memory_usage(void);  // Should be < 1KB total!

#endif // P32_MOOD_TRANSITIONS_H