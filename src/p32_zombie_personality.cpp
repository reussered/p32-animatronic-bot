#include "p32_component_config.h"

#ifdef ENABLE_ZOMBIE_PERSONALITY
#include "p32_personality_component.h"

static const char* TAG = "P32_ZOMBIE_PERSONALITY";

// Zombie-specific personality implementation
esp_err_t p32_zombie_personality_init(const p32_component_config_t* config, void** instance) {
    ESP_LOGI(TAG, "Initializing zombie personality: %s", config->component_id);
    
    // Use base personality init first
    esp_err_t ret = p32_personality_component_init(config, instance);
    if (ret != ESP_OK) {
        return ret;
    }
    
    p32_personality_instance_t* personality = (p32_personality_instance_t*)*instance;
    
    // Override with zombie-specific traits
    strncpy(personality->personality_name, "Hungry Zombie", sizeof(personality->personality_name));
    strncpy(personality->family_type, "ZOMBIE", sizeof(personality->family_type));
    
    personality->default_mood = MOOD_HUNGER;
    personality->current_mood = MOOD_HUNGER;
    
    // Zombie behavior parameters - low intelligence, high aggression
    personality->aggression_level = 0.8f;      // Very aggressive
    personality->curiosity_level = 0.2f;       // Low curiosity
    personality->sociability_level = 0.1f;     // Antisocial
    personality->energy_level = 0.4f;          // Slow, deliberate
    
    // Zombie-specific thresholds
    personality->proximity_alert_distance = 100.0f;  // Notices from far away
    personality->proximity_interact_distance = 40.0f; // Gets aggressive closer
    personality->mood_decay_time_ms = 15000;         // Slow mood changes
    personality->interaction_timeout_ms = 20000;     // Long attention span
    
    // Zombie audio files
    strncpy(personality->ambient_sound_file, "zombie_moan.wav", sizeof(personality->ambient_sound_file));
    strncpy(personality->alert_sound_file, "zombie_growl.wav", sizeof(personality->alert_sound_file));
    strncpy(personality->happy_sound_file, "zombie_hungry.wav", sizeof(personality->happy_sound_file));
    strncpy(personality->angry_sound_file, "zombie_roar.wav", sizeof(personality->angry_sound_file));
    
    ESP_LOGI(TAG, "Zombie personality configured: aggression=%.1f, hunger focus", 
             personality->aggression_level);
    
    return ESP_OK;
}

esp_err_t p32_zombie_personality_act(void* instance, const char* action, void* params) {
    p32_personality_instance_t* personality = (p32_personality_instance_t*)instance;
    
    // Handle zombie-specific actions first
    if (strcmp(action, P32_PERSONALITY_ACTION_PROCESS_SENSOR) == 0) {
        return p32_zombie_process_sensor_event(personality, (p32_sensor_event_for_personality_t*)((p32_action_params_t*)params)->data_ptr);
    }
    
    // Fall back to base personality behavior
    return p32_personality_component_act(instance, action, params);
}

esp_err_t p32_zombie_process_sensor_event(p32_personality_instance_t* personality, 
                                          p32_sensor_event_for_personality_t* event) {
    
    if (strcmp(event->sensor_type, "ultrasonic") == 0) {
        float distance = event->value;
        
        // Zombie-specific proximity reactions
        if (distance < 20.0f) {
            // Very close - zombie gets aggressive and hungry
            ESP_LOGI(TAG, "Zombie personality: VERY CLOSE proximity (%.1fcm) - HUNGRY!", distance);
            personality->current_mood = MOOD_ANGER;
            personality->aggression_level = fminf(1.0f, personality->aggression_level + 0.1f);
            
        } else if (distance < 50.0f) {
            // Medium close - stalking mode
            ESP_LOGI(TAG, "Zombie personality: Medium proximity (%.1fcm) - stalking", distance);
            personality->current_mood = MOOD_HUNGER;
            
        } else if (distance < 120.0f) {
            // Far but detected - slow approach
            ESP_LOGI(TAG, "Zombie personality: Far proximity (%.1fcm) - detected", distance);
            if (personality->current_mood == MOOD_CONTENTMENT) {
                personality->current_mood = MOOD_HUNGER;
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
esp_err_t p32_register_zombie_personality(void) {
    return p32_component_register("personality_zombie", "Hungry Zombie Personality",
                                  p32_zombie_personality_init,
                                  p32_zombie_personality_act,
                                  p32_personality_component_cleanup);
}

#endif // ENABLE_ZOMBIE_PERSONALITY
