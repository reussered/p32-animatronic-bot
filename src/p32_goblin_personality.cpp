#include "p32_component_config.hpp"

#ifdef ENABLE_GOBLIN_PERSONALITY
#include "p32_personality_component.hpp"

static const char* TAG = "P32_GOBLIN_PERSONALITY";

// Goblin-specific personality implementation
esp_err_t p32_goblin_personality_init(const p32_component_config_t* config, void** instance) {
    ESP_LOGI(TAG, "Initializing goblin personality: %s", config->component_id);
    
    // Use base personality init first
    esp_err_t ret = p32_personality_component_init(config, instance);
    if (ret != ESP_OK) {
        return ret;
    }
    
    p32_personality_instance_t* personality = (p32_personality_instance_t*)*instance;
    
    // Override with goblin-specific traits
    strncpy(personality->personality_name, "Curious Goblin", sizeof(personality->personality_name));
    strncpy(personality->family_type, "GOBLIN", sizeof(personality->family_type));
    
    personality->default_mood = MOOD_CURIOSITY;
    personality->current_mood = MOOD_CURIOSITY;
    
    // Goblin behavior parameters
    personality->aggression_level = 0.4f;      // Moderately aggressive
    personality->curiosity_level = 0.9f;       // Very curious
    personality->sociability_level = 0.6f;     // Somewhat social
    personality->energy_level = 0.8f;          // High energy
    
    // Goblin-specific thresholds
    personality->proximity_alert_distance = 60.0f;
    personality->proximity_interact_distance = 25.0f;
    personality->mood_decay_time_ms = 8000;
    personality->interaction_timeout_ms = 12000;
    
    // Goblin audio files
    strncpy(personality->ambient_sound_file, "ambient_breathing.wav", sizeof(personality->ambient_sound_file));
    strncpy(personality->alert_sound_file, "curious_chirp.wav", sizeof(personality->alert_sound_file));
    strncpy(personality->happy_sound_file, "curious_chirp.wav", sizeof(personality->happy_sound_file));
    strncpy(personality->angry_sound_file, "growl_angry.wav", sizeof(personality->angry_sound_file));
    
    ESP_LOGI(TAG, "Goblin personality configured: curiosity=%.1f, aggression=%.1f", 
             personality->curiosity_level, personality->aggression_level);
    
    return ESP_OK;
}

esp_err_t p32_goblin_personality_act(void* instance, const char* action, void* params) {
    p32_personality_instance_t* personality = (p32_personality_instance_t*)instance;
    
    // Handle goblin-specific actions first
    if (strcmp(action, P32_PERSONALITY_ACTION_PROCESS_SENSOR) == 0) {
        return p32_goblin_process_sensor_event(personality, (p32_sensor_event_for_personality_t*)((p32_action_params_t*)params)->data_ptr);
    }
    
    // Fall back to base personality behavior
    return p32_personality_component_act(instance, action, params);
}

esp_err_t p32_goblin_process_sensor_event(p32_personality_instance_t* personality, 
                                          p32_sensor_event_for_personality_t* event) {
    
    if (strcmp(event->sensor_type, "ultrasonic") == 0) {
        float distance = event->value;
        
        // Goblin-specific proximity reactions
        if (distance < 15.0f) {
            // Very close - goblins get excited and curious
            ESP_LOGI(TAG, "Goblin personality: VERY CLOSE proximity (%.1fcm) - excitement!", distance);
            personality->current_mood = MOOD_HAPPINESS;
            personality->energy_level = fminf(1.0f, personality->energy_level + 0.1f);
            
        } else if (distance < 30.0f) {
            // Close - investigate with caution
            ESP_LOGI(TAG, "Goblin personality: Close proximity (%.1fcm) - investigate", distance);
            personality->current_mood = MOOD_CURIOSITY;
            
        } else if (distance < 80.0f) {
            // Medium distance - alert but not threatened
            ESP_LOGI(TAG, "Goblin personality: Medium proximity (%.1fcm) - alert", distance);
            if (personality->current_mood == MOOD_CONTENTMENT) {
                personality->current_mood = MOOD_CURIOSITY;
            }
        }
        
        personality->mood_change_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        personality->last_activity_time = personality->mood_change_time;
        
        return ESP_OK;
    }
    
    // Fall back to base processing
    return p32_personality_process_sensor_event(personality, event);
}

// Registration function
esp_err_t p32_register_goblin_personality(void) {
    return p32_component_register("personality_goblin", "Curious Goblin Personality",
                                  p32_goblin_personality_init,
                                  p32_goblin_personality_act,
                                  p32_personality_component_cleanup);
}

#endif // ENABLE_GOBLIN_PERSONALITY
