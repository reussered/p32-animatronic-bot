#include "p32_component_config.h"

#ifdef ENABLE_PERSONALITY_COMPONENT
#include "p32_personality_component.h"

static const char* TAG = "P32_PERSONALITY";

// Base personality component implementation
esp_err_t p32_personality_component_init(const p32_component_config_t* config, void** instance) {
    ESP_LOGI(TAG, "Initializing personality component: %s", config->component_id);
    
    p32_personality_instance_t* personality = malloc(sizeof(p32_personality_instance_t));
    if (!personality) {
        ESP_LOGE(TAG, "Failed to allocate personality instance");
        return ESP_ERR_NO_MEM;
    }
    
    memset(personality, 0, sizeof(p32_personality_instance_t));
    
    // Initialize with default personality (will be overridden by specific personality)
    strncpy(personality->personality_id, config->component_id, sizeof(personality->personality_id));
    strncpy(personality->personality_name, "Generic Bot", sizeof(personality->personality_name));
    strncpy(personality->family_type, "GENERIC", sizeof(personality->family_type));
    
    personality->current_mood = MOOD_CONTENTMENT;
    personality->default_mood = MOOD_CONTENTMENT;
    personality->mood_change_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Default behavior parameters
    personality->aggression_level = 0.3f;
    personality->curiosity_level = 0.7f;
    personality->sociability_level = 0.5f;
    personality->energy_level = 0.6f;
    
    // Default thresholds
    personality->proximity_alert_distance = 50.0f;
    personality->proximity_interact_distance = 20.0f;
    personality->mood_decay_time_ms = 10000;
    personality->interaction_timeout_ms = 15000;
    
    personality->initialized = true;
    personality->last_activity_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    *instance = personality;
    
    ESP_LOGI(TAG, "Personality component initialized: %s", personality->personality_name);
    return ESP_OK;
}

esp_err_t p32_personality_component_act(void* instance, const char* action, void* params) {
    p32_personality_instance_t* personality = (p32_personality_instance_t*)instance;
    p32_action_params_t* action_params = (p32_action_params_t*)params;
    
    if (!personality || !personality->initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (strcmp(action, P32_PERSONALITY_ACTION_PROCESS_SENSOR) == 0) {
        // Process sensor input through personality filter
        p32_sensor_event_for_personality_t* sensor_event = (p32_sensor_event_for_personality_t*)action_params->data_ptr;
        
        if (sensor_event) {
            return p32_personality_process_sensor_event(personality, sensor_event);
        }
        
    } else if (strcmp(action, P32_PERSONALITY_ACTION_UPDATE_MOOD) == 0) {
        // Update personality mood state
        p32_mood_t new_mood = (p32_mood_t)(int)action_params->numeric_value;
        personality->current_mood = new_mood;
        personality->mood_change_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        ESP_LOGI(TAG, "Personality %s mood updated to: %d", personality->personality_id, new_mood);
        return ESP_OK;
        
    } else if (strcmp(action, P32_PERSONALITY_ACTION_GET_REACTION) == 0) {
        // Get personality reaction to current state
        p32_personality_reaction_t* reaction = (p32_personality_reaction_t*)action_params->data_ptr;
        
        if (reaction) {
            return p32_personality_generate_reaction(personality, reaction);
        }
        
    } else if (strcmp(action, P32_PERSONALITY_ACTION_SET_PARAMETER) == 0) {
        // Adjust personality parameter
        const char* param_name = action_params->key;
        float param_value = action_params->numeric_value;
        
        return p32_personality_set_parameter(personality, param_name, param_value);
        
    } else if (strcmp(action, P32_PERSONALITY_ACTION_RESET_STATE) == 0) {
        // Reset personality to default state
        personality->current_mood = personality->default_mood;
        personality->mood_change_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        personality->last_activity_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        
        ESP_LOGI(TAG, "Personality %s state reset", personality->personality_id);
        return ESP_OK;
        
    } else if (strcmp(action, P32_ACTION_UPDATE) == 0) {
        // Periodic personality update
        return p32_personality_periodic_update(personality);
    }
    
    ESP_LOGW(TAG, "Unknown personality action: %s", action);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t p32_personality_component_cleanup(void* instance) {
    p32_personality_instance_t* personality = (p32_personality_instance_t*)instance;
    
    if (personality) {
        ESP_LOGI(TAG, "Cleaning up personality: %s", personality->personality_id);
        free(personality);
    }
    
    return ESP_OK;
}

// Internal personality processing functions
esp_err_t p32_personality_process_sensor_event(p32_personality_instance_t* personality, 
                                               p32_sensor_event_for_personality_t* event) {
    
    personality->last_activity_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    if (strcmp(event->sensor_type, "ultrasonic") == 0) {
        float distance = event->value;
        
        // Process proximity based on personality traits
        if (distance < personality->proximity_interact_distance) {
            // Close interaction - personality determines response
            p32_mood_t suggested_mood = MOOD_CURIOSITY;
            
            if (personality->sociability_level > 0.7f) {
                suggested_mood = MOOD_HAPPINESS;
            } else if (personality->aggression_level > 0.6f) {
                suggested_mood = MOOD_ANGER;
            } else if (personality->curiosity_level > 0.5f) {
                suggested_mood = MOOD_CURIOSITY;
            }
            
            // Send mood change to system via component system
            p32_action_params_t mood_params;
            p32_action_params_init(&mood_params);
            mood_params.numeric_value = (float)suggested_mood;
            
            // This would trigger other components to respond
            ESP_LOGI(TAG, "Personality %s suggests mood: %d for close proximity (%.1fcm)", 
                     personality->personality_id, suggested_mood, distance);
            
        } else if (distance < personality->proximity_alert_distance) {
            // Alert distance - more cautious response
            p32_mood_t alert_mood = (personality->curiosity_level > 0.6f) ? MOOD_CURIOSITY : MOOD_IRRITATION;
            
            ESP_LOGI(TAG, "Personality %s suggests alert mood: %d for medium proximity (%.1fcm)", 
                     personality->personality_id, alert_mood, distance);
        }
    }
    
    return ESP_OK;
}

esp_err_t p32_personality_generate_reaction(p32_personality_instance_t* personality, 
                                            p32_personality_reaction_t* reaction) {
    
    if (!reaction) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Generate reaction based on current personality state
    reaction->suggested_mood = personality->current_mood;
    reaction->mood_duration_ms = personality->mood_decay_time_ms;
    reaction->mood_priority = (uint8_t)(personality->energy_level * 10);
    
    // Set animation and sound based on personality type and mood
    switch (personality->current_mood) {
        case MOOD_HAPPINESS:
            strncpy(reaction->suggested_animation, "happy_idle", sizeof(reaction->suggested_animation));
            strncpy(reaction->suggested_sound, personality->happy_sound_file, sizeof(reaction->suggested_sound));
            break;
        case MOOD_ANGER:
            strncpy(reaction->suggested_animation, "angry_stare", sizeof(reaction->suggested_animation));
            strncpy(reaction->suggested_sound, personality->angry_sound_file, sizeof(reaction->suggested_sound));
            break;
        case MOOD_CURIOSITY:
            strncpy(reaction->suggested_animation, "curious_look", sizeof(reaction->suggested_animation));
            strncpy(reaction->suggested_sound, personality->alert_sound_file, sizeof(reaction->suggested_sound));
            break;
        default:
            strncpy(reaction->suggested_animation, "idle_blink", sizeof(reaction->suggested_animation));
            strncpy(reaction->suggested_sound, personality->ambient_sound_file, sizeof(reaction->suggested_sound));
            break;
    }
    
    reaction->should_change_state = false; // Default, can be overridden by specific personalities
    
    return ESP_OK;
}

esp_err_t p32_personality_set_parameter(p32_personality_instance_t* personality, 
                                        const char* param_name, float value) {
    
    if (!param_name) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Clamp value to valid range
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    
    if (strcmp(param_name, "aggression") == 0) {
        personality->aggression_level = value;
    } else if (strcmp(param_name, "curiosity") == 0) {
        personality->curiosity_level = value;
    } else if (strcmp(param_name, "sociability") == 0) {
        personality->sociability_level = value;
    } else if (strcmp(param_name, "energy") == 0) {
        personality->energy_level = value;
    } else {
        ESP_LOGW(TAG, "Unknown personality parameter: %s", param_name);
        return ESP_ERR_NOT_FOUND;
    }
    
    ESP_LOGI(TAG, "Personality %s parameter %s set to: %.2f", 
             personality->personality_id, param_name, value);
    
    return ESP_OK;
}

esp_err_t p32_personality_periodic_update(p32_personality_instance_t* personality) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Check for mood decay
    uint32_t mood_age = current_time - personality->mood_change_time;
    if (mood_age > personality->mood_decay_time_ms && 
        personality->current_mood != personality->default_mood) {
        
        ESP_LOGI(TAG, "Personality %s mood decaying back to default", personality->personality_id);
        personality->current_mood = personality->default_mood;
        personality->mood_change_time = current_time;
    }
    
    // Check for interaction timeout
    uint32_t activity_age = current_time - personality->last_activity_time;
    if (activity_age > personality->interaction_timeout_ms) {
        // Switch to idle/low-energy state
        if (personality->energy_level > 0.2f) {
            personality->energy_level *= 0.95f; // Gradually reduce energy
        }
    }
    
    return ESP_OK;
}

// Hot-swapping capability
esp_err_t p32_personality_hot_swap(const char* personality_type) {
    ESP_LOGI(TAG, "Hot-swapping to personality: %s", personality_type);
    
    // For now, just log the personality change request
    // In a full implementation, this would:
    // 1. Find the current active personality
    // 2. Save its state
    // 3. Deactivate it
    // 4. Load and activate the new personality
    
    ESP_LOGI(TAG, "Personality hot-swap to %s requested", personality_type);
    
    // Basic implementation - just return success
    return ESP_OK;
    
    /* Full implementation would look like:
    p32_software_component_t* new_comp = p32_component_find_by_id(personality_type);
    if (new_comp && new_comp->initialized) {
        p32_action_params_t params;
        p32_action_params_init(&params);
        p32_component_send_action(personality_type, P32_PERSONALITY_ACTION_RESET_STATE, &params);
        
        ESP_LOGI(TAG, "Activated personality: %s", personality_type);
        return ESP_OK;
    }
    
    ESP_LOGE(TAG, "Failed to find target personality: %s", personality_type);
    return ESP_ERR_NOT_FOUND;
    */
}

// Personality system functions
void p32_personality_system_init(void) {
    ESP_LOGI(TAG, "Initializing personality system...");
    
    // Register base personality component
    p32_component_register("personality_base", "Base Personality Component",
                          p32_personality_component_init,
                          p32_personality_component_act,
                          p32_personality_component_cleanup);
    
    ESP_LOGI(TAG, "Personality system initialized");
}

void p32_personality_act(void) {
    extern uint64_t loopCount;
    
    // Process personality behavior every 30 loops (about 1.7Hz if main loop is 50Hz)
    if (loopCount % 30 == 0) {
        // Find active personality and process its behavior
        // This would typically be the currently loaded personality component
        // For now, just process basic personality functions
        
        ESP_LOGD(TAG, "Personality system tick at loop %llu", loopCount);
        
        // Here you could trigger personality-specific behaviors
        // based on current mood, recent interactions, etc.
    }
}

// Missing function implementations
esp_err_t p32_personality_process_sensor_event(p32_personality_instance_t* personality, p32_sensor_event_for_personality_t* event) {
    if (!personality || !event) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Processing sensor event for personality %s", personality->personality_id);
    
    // Basic sensor event processing - can be overridden by specific personalities
    switch (event->sensor_type) {
        case P32_SENSOR_TYPE_DISTANCE:
            ESP_LOGI(TAG, "Distance sensor: %.2f", event->sensor_value);
            break;
        case P32_SENSOR_TYPE_MOTION:
            ESP_LOGI(TAG, "Motion sensor: %s", event->sensor_value > 0.5 ? "detected" : "clear");
            break;
        default:
            ESP_LOGW(TAG, "Unknown sensor type: %d", event->sensor_type);
            break;
    }
    
    return ESP_OK;
}

esp_err_t p32_personality_generate_reaction(p32_personality_instance_t* personality, p32_personality_reaction_t* reaction) {
    if (!personality || !reaction) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Generating reaction for personality %s", personality->personality_id);
    
    // Basic reaction generation
    reaction->mood_change = 0.0f;
    reaction->animation_trigger = false;
    reaction->sound_trigger = false;
    
    return ESP_OK;
}

esp_err_t p32_personality_periodic_update(p32_personality_instance_t* personality) {
    if (!personality) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Periodic personality state updates
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t time_since_activity = current_time - personality->last_activity_time;
    
    // Mood decay over time (optional)
    if (time_since_activity > 30000) { // 30 seconds of inactivity
        ESP_LOGD(TAG, "Personality %s idle decay", personality->personality_id);
    }
    
    return ESP_OK;
}

#endif // ENABLE_PERSONALITY_COMPONENT
