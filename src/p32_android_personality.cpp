#include "p32_component_config.hpp"

#ifdef ENABLE_ANDROID_PERSONALITY
#include "p32_personality_component.hpp"

static const char* TAG = "P32_ANDROID_PERSONALITY";

// Android-specific personality implementation
esp_err_t p32_android_personality_init(const p32_component_config_t* config, void** instance) {
    ESP_LOGI(TAG, "Initializing android personality: %s", config->component_id);
    
    // Use base personality init first
    esp_err_t ret = p32_personality_component_init(config, instance);
    if (ret != ESP_OK) {
        return ret;
    }
    
    p32_personality_instance_t* personality = (p32_personality_instance_t*)*instance;
    
    // Override with android-specific traits
    strncpy(personality->personality_name, "Logical Android", sizeof(personality->personality_name));
    strncpy(personality->family_type, "ANDROID", sizeof(personality->family_type));
    
    personality->default_mood = MOOD_CONTENTMENT;
    personality->current_mood = MOOD_CONTENTMENT;
    
    // Android behavior parameters - logical, controlled
    personality->aggression_level = 0.1f;      // Non-aggressive
    personality->curiosity_level = 0.7f;       // Analytical curiosity
    personality->sociability_level = 0.8f;     // Programmed to interact
    personality->energy_level = 0.5f;          // Consistent energy
    
    // Android-specific thresholds - precise measurements
    personality->proximity_alert_distance = 75.0f;
    personality->proximity_interact_distance = 30.0f;
    personality->mood_decay_time_ms = 5000;    // Quick, logical transitions
    personality->interaction_timeout_ms = 10000;
    
    // Android audio files - synthesized sounds
    strncpy(personality->ambient_sound_file, "android_hum.wav", sizeof(personality->ambient_sound_file));
    strncpy(personality->alert_sound_file, "android_beep.wav", sizeof(personality->alert_sound_file));
    strncpy(personality->happy_sound_file, "android_acknowledge.wav", sizeof(personality->happy_sound_file));
    strncpy(personality->angry_sound_file, "android_error.wav", sizeof(personality->angry_sound_file));
    
    ESP_LOGI(TAG, "Android personality configured: logical processing mode");
    
    return ESP_OK;
}

esp_err_t p32_android_personality_act(void* instance, const char* action, void* params) {
    p32_personality_instance_t* personality = (p32_personality_instance_t*)instance;
    
    // Handle android-specific actions first
    if (strcmp(action, P32_PERSONALITY_ACTION_PROCESS_SENSOR) == 0) {
        return p32_android_process_sensor_event(personality, (p32_sensor_event_for_personality_t*)((p32_action_params_t*)params)->data_ptr);
    }
    
    // Fall back to base personality behavior
    return p32_personality_component_act(instance, action, params);
}

esp_err_t p32_android_process_sensor_event(p32_personality_instance_t* personality, 
                                           p32_sensor_event_for_personality_t* event) {
    
    if (strcmp(event->sensor_type, "ultrasonic") == 0) {
        float distance = event->value;
        
        // Android-specific proximity reactions - logical and measured
        if (distance < 25.0f) {
            // Close proximity - engage interaction protocols
            ESP_LOGI(TAG, "Android personality: Close proximity (%.1fcm) - engaging interaction", distance);
            personality->current_mood = MOOD_CURIOSITY;
            
        } else if (distance < 50.0f) {
            // Medium distance - analyze and prepare
            ESP_LOGI(TAG, "Android personality: Medium proximity (%.1fcm) - analyzing", distance);
            personality->current_mood = MOOD_CURIOSITY;
            
        } else if (distance < 100.0f) {
            // Far distance - passive monitoring
            ESP_LOGI(TAG, "Android personality: Far proximity (%.1fcm) - monitoring", distance);
            if (personality->current_mood != MOOD_CURIOSITY) {
                personality->current_mood = MOOD_CONTENTMENT;
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
esp_err_t p32_register_android_personality(void) {
    return p32_component_register("personality_android", "Logical Android Personality",
                                  p32_android_personality_init,
                                  p32_android_personality_act,
                                  p32_personality_component_cleanup);
}

#endif // ENABLE_ANDROID_PERSONALITY
