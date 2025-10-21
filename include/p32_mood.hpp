#pragma once

#include "p32_core.h"

// Mood transition system
typedef struct {
    p32_mood_t from_mood;
    p32_mood_t to_mood;
    uint32_t transition_time_ms;
    uint8_t intensity;
} p32_mood_transition_t;

// Mood engine state
typedef struct {
    p32_mood_t current_mood;
    p32_mood_t target_mood;
    uint32_t mood_start_time;
    uint32_t mood_duration_ms;
    bool transitioning;
    TaskHandle_t mood_task;
    QueueHandle_t mood_queue;
} p32_mood_engine_t;

// Mood change request
typedef struct {
    p32_mood_t new_mood;
    uint32_t duration_ms;
    uint8_t priority;
    bool force_change;
} p32_mood_request_t;

// Mood engine functions
esp_err_t p32_mood_engine_init(void);
void p32_mood_engine_act(void);
esp_err_t p32_mood_request_change(p32_mood_t mood, uint32_t duration_ms, uint8_t priority);
esp_err_t p32_mood_force_change(p32_mood_t mood);
bool p32_mood_is_transitioning(void);
uint32_t p32_mood_get_remaining_duration(void);

// Mood task and processing
void p32_mood_task(void* parameters);
esp_err_t p32_mood_process_transitions(void);

// Mood behavior mappings
extern const p32_mood_transition_t p32_goblin_mood_transitions[];

// Mood utilities
const char* p32_mood_to_string(p32_mood_t mood);
p32_mood_t p32_string_to_mood(const char* mood_str);