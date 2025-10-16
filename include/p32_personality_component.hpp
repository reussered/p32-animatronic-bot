#pragma once

#include "p32_component_system.h"
#include "p32_core.h"

// Personality module interface - completely software-based
typedef struct {
    char personality_id[32];
    char personality_name[64];
    char family_type[32];
    
    // Personality state
    p32_mood_t current_mood;
    p32_mood_t default_mood;
    uint32_t mood_change_time;
    
    // Behavior parameters (configurable per personality)
    float aggression_level;      // 0.0 - 1.0
    float curiosity_level;       // 0.0 - 1.0  
    float sociability_level;     // 0.0 - 1.0
    float energy_level;          // 0.0 - 1.0
    
    // Reaction thresholds
    float proximity_alert_distance;
    float proximity_interact_distance;
    uint32_t mood_decay_time_ms;
    uint32_t interaction_timeout_ms;
    
    // Audio preferences  
    char ambient_sound_file[64];
    char alert_sound_file[64];
    char happy_sound_file[64];
    char angry_sound_file[64];
    
    // Animation preferences
    uint16_t idle_animation_set;
    uint16_t alert_animation_set;
    uint16_t happy_animation_set;
    uint16_t angry_animation_set;
    
    // Internal state
    bool initialized;
    uint32_t last_activity_time;
} p32_personality_instance_t;

// Personality component interface
esp_err_t p32_personality_component_init(const p32_component_config_t* config, void** instance);
esp_err_t p32_personality_component_act(void* instance, const char* action, void* params);
esp_err_t p32_personality_component_cleanup(void* instance);

// Personality-specific actions
#define P32_PERSONALITY_ACTION_PROCESS_SENSOR    "process_sensor"
#define P32_PERSONALITY_ACTION_UPDATE_MOOD       "update_mood"
#define P32_PERSONALITY_ACTION_GET_REACTION      "get_reaction"
#define P32_PERSONALITY_ACTION_SET_PARAMETER     "set_parameter"
#define P32_PERSONALITY_ACTION_RESET_STATE       "reset_state"

// Sensor event structure for personality processing
typedef struct {
    char sensor_type[32];
    float value;
    uint32_t timestamp;
    char context[64];
} p32_sensor_event_for_personality_t;

// Personality reaction structure
typedef struct {
    p32_mood_t suggested_mood;
    uint32_t mood_duration_ms;
    uint8_t mood_priority;
    
    char suggested_animation[32];
    char suggested_sound[64];
    
    bool should_change_state;
    char new_behavior_state[32];
} p32_personality_reaction_t;

// Specific personality implementations (can be swapped)
esp_err_t p32_register_goblin_personality(void);
esp_err_t p32_register_zombie_personality(void);
esp_err_t p32_register_robot_personality(void);
esp_err_t p32_register_vampire_personality(void);
esp_err_t p32_register_android_personality(void);

// Personality utility functions
esp_err_t p32_personality_load_from_json(const char* json_config, p32_personality_instance_t* personality);
esp_err_t p32_personality_save_state(const p32_personality_instance_t* personality);
esp_err_t p32_personality_switch_active(const char* new_personality_id);

// Personality system functions
void p32_personality_system_init(void);
void p32_personality_act(void);

// Dynamic personality hot-swapping
esp_err_t p32_personality_hot_swap(const char* personality_type);
esp_err_t p32_personality_get_available_list(char personalities[][32], uint8_t max_count);

// Personality parameter adjustment (for learning/adaptation)
esp_err_t p32_personality_adjust_parameter(const char* personality_id, const char* parameter, float adjustment);
esp_err_t p32_personality_get_parameter(const char* personality_id, const char* parameter, float* value);