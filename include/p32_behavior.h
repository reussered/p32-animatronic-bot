#pragma once

#include "p32_core.h"
#include "p32_mood.h"

// Bot behavior states
typedef enum {
    BOT_STATE_IDLE = 0,
    BOT_STATE_ALERT,
    BOT_STATE_INTERACTING,
    BOT_STATE_SLEEPING,
    BOT_STATE_AGGRESSIVE,
    BOT_STATE_CURIOUS,
    BOT_STATE_COUNT
} p32_bot_state_t;

// Behavior engine
typedef struct {
    p32_bot_state_t current_state;
    char active_bot_type[32];
    uint32_t state_start_time;
    uint32_t last_interaction_time;
    TaskHandle_t behavior_task;
    bool behavior_enabled;
} p32_behavior_engine_t;

// Behavior functions
esp_err_t p32_behavior_init(void);
void p32_behavior_act(void);
esp_err_t p32_behavior_set_state(p32_bot_state_t state);
p32_bot_state_t p32_behavior_get_state(void);
esp_err_t p32_behavior_process_interaction(float proximity_cm);

// Bot-specific behavior patterns
esp_err_t p32_behavior_goblin_idle(void);
esp_err_t p32_behavior_goblin_alert(void);
esp_err_t p32_behavior_goblin_interact(void);

// Behavior task
void p32_behavior_task(void* parameters);

// State utilities
const char* p32_behavior_state_to_string(p32_bot_state_t state);