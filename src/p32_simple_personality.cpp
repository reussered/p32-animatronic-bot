#include "p32_component_config.hpp"

#ifdef ENABLE_SIMPLE_GOBLIN
// SIMPLE GOBLIN PERSONALITY COMPONENT
#include "p32_core.hpp"

static const char* TAG = "P32_SIMPLE_PERSONALITY";

// Simple personality types
typedef enum {
    PERSONALITY_GOBLIN = 0,
    PERSONALITY_ANDROID,
    PERSONALITY_ZOMBIE,
    PERSONALITY_COUNT
} p32_simple_personality_type_t;

// Simple personality state
typedef struct {
    p32_simple_personality_type_t current_personality;
    uint32_t activation_time;
    bool active;
    float mood_level; // 0.0 to 1.0
} p32_simple_personality_t;

static p32_simple_personality_t g_personality = {0};

// Simple personality names
static const char* personality_names[PERSONALITY_COUNT] = {
    "Goblin",
    "Android", 
    "Zombie"
};

// Simple personality initialization
esp_err_t p32_personality_init(void) {
    ESP_LOGI(TAG, "Initializing simple personality system...");
    
    g_personality.current_personality = PERSONALITY_GOBLIN;
    g_personality.activation_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    g_personality.active = true;
    g_personality.mood_level = 0.5f; // neutral mood
    
    ESP_LOGI(TAG, "Active personality: %s", personality_names[g_personality.current_personality]);
    return ESP_OK;
}

// Simple personality update
void p32_personality_update(void) {
    if (!g_personality.active) return;
    
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t time_active = current_time - g_personality.activation_time;
    
    // Simple mood drift towards neutral over time
    if (time_active > 10000) { // 10 seconds
        if (g_personality.mood_level > 0.5f) {
            g_personality.mood_level -= 0.01f;
        } else if (g_personality.mood_level < 0.5f) {
            g_personality.mood_level += 0.01f;
        }
    }
}

// Simple personality reaction to sensor input
void p32_personality_react_to_sensor(float sensor_value) {
    if (!g_personality.active) return;
    
    ESP_LOGI(TAG, "%s personality reacting to sensor: %.2f", 
             personality_names[g_personality.current_personality], sensor_value);
    
    // Simple mood changes based on sensor input
    switch (g_personality.current_personality) {
        case PERSONALITY_GOBLIN:
            // Goblin gets excited by proximity
            if (sensor_value < 30.0f) { // close proximity
                g_personality.mood_level = fminf(1.0f, g_personality.mood_level + 0.2f);
                ESP_LOGI(TAG, "Goblin curious! Mood: %.2f", g_personality.mood_level);
            }
            break;
            
        case PERSONALITY_ANDROID:
            // Android analyzes input logically
            ESP_LOGI(TAG, "Android analyzing sensor data: %.2fcm", sensor_value);
            break;
            
        case PERSONALITY_ZOMBIE:
            // Zombie responds slowly
            if (sensor_value < 50.0f) { // medium proximity
                ESP_LOGI(TAG, "Zombie detecting presence...");
                g_personality.mood_level = fmaxf(0.0f, g_personality.mood_level - 0.1f);
            }
            break;
            
        default:
            break;
    }
}

// Simple personality switching
esp_err_t p32_personality_switch(p32_simple_personality_type_t new_personality) {
    if (new_personality >= PERSONALITY_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Switching personality: %s -> %s", 
             personality_names[g_personality.current_personality],
             personality_names[new_personality]);
    
    g_personality.current_personality = new_personality;
    g_personality.activation_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    g_personality.mood_level = 0.5f; // reset mood
    
    return ESP_OK;
}

// Simple personality status
const char* p32_personality_get_current_name(void) {
    return personality_names[g_personality.current_personality];
}

float p32_personality_get_mood_level(void) {
    return g_personality.mood_level;
}

// Simple personality action (stub for component system)
esp_err_t p32_personality_component_act(void* instance, const char* action, void* params) {
    ESP_LOGI(TAG, "Personality action: %s", action);
    
    if (strcmp(action, "update") == 0) {
        p32_personality_update();
    } else if (strcmp(action, "sensor_input") == 0) {
        float* sensor_val = (float*)params;
        if (sensor_val) {
            p32_personality_react_to_sensor(*sensor_val);
        }
    }
    
    return ESP_OK;
}
#endif // 0 - TEMPORARILY DISABLED
