#include "p32_behavior.h"
#include "p32_display.h"
#include "p32_audio.h"
#include "p32_mood.h"

static const char* TAG = "P32_BEHAVIOR";
static p32_behavior_engine_t g_behavior_engine = {0};

// Behavior state names
static const char* behavior_state_names[] = {
    "IDLE",
    "ALERT", 
    "INTERACTING",
    "SLEEPING",
    "AGGRESSIVE",
    "CURIOUS"
};

esp_err_t p32_behavior_init(void) {
    ESP_LOGI(TAG, "Initializing behavior engine...");
    
    memset(&g_behavior_engine, 0, sizeof(p32_behavior_engine_t));
    
    // Initialize behavior state
    g_behavior_engine.current_state = BOT_STATE_IDLE;
    strncpy(g_behavior_engine.active_bot_type, "GOBLINFULL", sizeof(g_behavior_engine.active_bot_type));
    g_behavior_engine.state_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    g_behavior_engine.last_interaction_time = 0;
    g_behavior_engine.behavior_enabled = true;
    
    // Create behavior processing task
    BaseType_t ret = xTaskCreate(p32_behavior_task, "p32_behavior", 4096, NULL, 2, &g_behavior_engine.behavior_task);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create behavior task");
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Behavior engine initialized - Bot: %s, State: %s",
             g_behavior_engine.active_bot_type,
             p32_behavior_state_to_string(g_behavior_engine.current_state));
    
    return ESP_OK;
}

void p32_behavior_act(void) {
    extern uint32_t loopCount;
    
    // Update behavior state every 25 loops (about 2Hz if main loop is 50Hz)
    if (loopCount % 25 == 0) {
        if (g_behavior_engine.behavior_enabled) {
            uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
            uint32_t time_in_state = current_time - g_behavior_engine.state_start_time;
            uint32_t time_since_interaction = current_time - g_behavior_engine.last_interaction_time;
            
            // Auto-transition to idle if no interaction for 30 seconds
            if (time_since_interaction > 30000 && g_behavior_engine.current_state != BOT_STATE_IDLE) {
                ESP_LOGI(TAG, "No interaction timeout, returning to idle");
                p32_behavior_set_state(BOT_STATE_IDLE);
            }
        }
    }
}

esp_err_t p32_behavior_set_state(p32_bot_state_t state) {
    if (state >= BOT_STATE_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    p32_bot_state_t old_state = g_behavior_engine.current_state;
    g_behavior_engine.current_state = state;
    g_behavior_engine.state_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    ESP_LOGI(TAG, "Behavior state changed: %s -> %s",
             p32_behavior_state_to_string(old_state),
             p32_behavior_state_to_string(state));
    
    return ESP_OK;
}

p32_bot_state_t p32_behavior_get_state(void) {
    return g_behavior_engine.current_state;
}

esp_err_t p32_behavior_process_interaction(float proximity_cm) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    g_behavior_engine.last_interaction_time = current_time;
    
    // Update behavior based on proximity
    if (proximity_cm < 15.0f) {
        // Very close - interactive mode
        if (g_behavior_engine.current_state != BOT_STATE_INTERACTING) {
            p32_behavior_set_state(BOT_STATE_INTERACTING);
            p32_mood_request_change(MOOD_CURIOSITY, 5000, 6);
        }
    } else if (proximity_cm < 50.0f) {
        // Moderate distance - alert mode
        if (g_behavior_engine.current_state == BOT_STATE_IDLE) {
            p32_behavior_set_state(BOT_STATE_ALERT);
            p32_mood_request_change(MOOD_CURIOSITY, 3000, 4);
        }
    } else {
        // Far away or no detection - return to idle
        if (g_behavior_engine.current_state != BOT_STATE_IDLE) {
            uint32_t time_since_interaction = current_time - g_behavior_engine.last_interaction_time;
            if (time_since_interaction > 10000) { // 10 seconds timeout
                p32_behavior_set_state(BOT_STATE_IDLE);
                p32_mood_request_change(MOOD_CONTENTMENT, 0, 2);
            }
        }
    }
    
    return ESP_OK;
}

void p32_behavior_task(void* parameters) {
    ESP_LOGI(TAG, "Behavior task started");
    
    const TickType_t behavior_interval = pdMS_TO_TICKS(1000); // Process every 1 second
    
    while (1) {
        vTaskDelay(behavior_interval);
        
        if (!g_behavior_engine.behavior_enabled) {
            continue;
        }
        
        // Execute behavior based on current state and bot type
        if (strcmp(g_behavior_engine.active_bot_type, "GOBLINFULL") == 0) {
            switch (g_behavior_engine.current_state) {
                case BOT_STATE_IDLE:
                    p32_behavior_goblin_idle();
                    break;
                case BOT_STATE_ALERT:
                    p32_behavior_goblin_alert();
                    break;
                case BOT_STATE_INTERACTING:
                    p32_behavior_goblin_interact();
                    break;
                default:
                    break;
            }
        }
        
        // Check for automatic state timeouts
        uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        uint32_t state_duration = current_time - g_behavior_engine.state_start_time;
        
        // Auto-return to idle after prolonged states
        if (g_behavior_engine.current_state == BOT_STATE_ALERT && state_duration > 15000) {
            p32_behavior_set_state(BOT_STATE_IDLE);
        } else if (g_behavior_engine.current_state == BOT_STATE_INTERACTING && state_duration > 30000) {
            p32_behavior_set_state(BOT_STATE_IDLE);
        }
    }
}

esp_err_t p32_behavior_goblin_idle(void) {
    static uint32_t last_blink = 0;
    static uint32_t last_ambient_sound = 0;
    
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Periodic blinking (every 3-5 seconds)
    if (current_time - last_blink > 4000) {
        p32_display_set_eye_animation(EYE_ANIM_BLINK);
        vTaskDelay(pdMS_TO_TICKS(200));
        p32_display_set_eye_animation(EYE_ANIM_CURIOUS_LOOK);
        last_blink = current_time;
    }
    
    // Ambient breathing sounds (every 8-12 seconds)
    if (current_time - last_ambient_sound > 10000) {
        p32_audio_play_ambient_breathing();
        last_ambient_sound = current_time;
    }
    
    return ESP_OK;
}

esp_err_t p32_behavior_goblin_alert(void) {
    static bool alert_animation_active = false;
    
    if (!alert_animation_active) {
        // Look around alertly
        p32_display_set_eye_animation(EYE_ANIM_LOOK_LEFT);
        vTaskDelay(pdMS_TO_TICKS(500));
        p32_display_set_eye_animation(EYE_ANIM_LOOK_RIGHT);
        vTaskDelay(pdMS_TO_TICKS(500));
        p32_display_set_eye_animation(EYE_ANIM_CURIOUS_LOOK);
        
        // Play curious sound
        p32_audio_play_curious_chirp();
        
        alert_animation_active = true;
    }
    
    return ESP_OK;
}

esp_err_t p32_behavior_goblin_interact(void) {
    static uint32_t last_interaction_response = 0;
    
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Interactive responses every 2 seconds
    if (current_time - last_interaction_response > 2000) {
        // Animated eye contact and expressions
        p32_display_set_eye_animation(EYE_ANIM_CURIOUS_LOOK);
        p32_display_set_mouth_animation(MOUTH_ANIM_SMILE);
        
        // Play interaction sound
        p32_audio_play_curious_chirp();
        
        // Brief happiness mood boost
        p32_mood_request_change(MOOD_HAPPINESS, 3000, 5);
        
        last_interaction_response = current_time;
    }
    
    return ESP_OK;
}

const char* p32_behavior_state_to_string(p32_bot_state_t state) {
    if (state >= BOT_STATE_COUNT) {
        return "UNKNOWN";
    }
    return behavior_state_names[state];
}