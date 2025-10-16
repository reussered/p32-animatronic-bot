#include "p32_component_config.hpp"

#ifdef ENABLE_SIMPLE_SENSOR
// SIMPLE SENSOR COMPONENT
#include "p32_core.hpp"
#include "esp_random.h"

static const char* TAG = "P32_SIMPLE_SENSOR";

// Simple sensor state
typedef struct {
    bool initialized;
    float last_distance;
    uint32_t last_reading;
    bool motion_detected;
    uint32_t motion_time;
} p32_simple_sensor_t;

static p32_simple_sensor_t g_sensor = {0};

// Simple sensor initialization
esp_err_t p32_simple_sensor_init(void) {
    ESP_LOGI(TAG, "Initializing simple sensor system...");
    
    g_sensor.initialized = true;
    g_sensor.last_distance = 100.0f; // default 1 meter
    g_sensor.last_reading = 0;
    g_sensor.motion_detected = false;
    g_sensor.motion_time = 0;
    
    ESP_LOGI(TAG, "Sensor system initialized");
    return ESP_OK;
}

// Simple sensor update (simulate readings)
void p32_simple_sensor_update(void) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Simulate sensor readings every 500ms
    if (current_time - g_sensor.last_reading > 500) {
        // Simple distance simulation (random walk)
        float distance_change = (float)(esp_random() % 201 - 100) / 10.0f; // -10 to +10 cm
        g_sensor.last_distance += distance_change;
        
        // Clamp to reasonable range
        if (g_sensor.last_distance < 5.0f) g_sensor.last_distance = 5.0f;
        if (g_sensor.last_distance > 200.0f) g_sensor.last_distance = 200.0f;
        
        g_sensor.last_reading = current_time;
        
        // Log significant distance changes
        if (g_sensor.last_distance < 30.0f) {
            ESP_LOGI(TAG, "Close proximity detected: %.1fcm", g_sensor.last_distance);
        }
    }
    
    // Simple motion detection (random events)
    if (current_time - g_sensor.motion_time > 5000) { // check every 5 seconds
        g_sensor.motion_detected = (esp_random() % 100) < 20; // 20% chance
        if (g_sensor.motion_detected) {
            ESP_LOGI(TAG, "Motion detected!");
        }
        g_sensor.motion_time = current_time;
    }
}

// Simple get distance
float p32_simple_sensor_get_distance(void) {
    return g_sensor.last_distance;
}

// Simple get motion
bool p32_simple_sensor_get_motion(void) {
    return g_sensor.motion_detected;
}

// Simple sensor trigger personality reaction
void p32_simple_sensor_trigger_reactions(void) {
    // Trigger personality reaction to distance
    extern esp_err_t p32_personality_component_act(void* instance, const char* action, void* params);
    
    float distance = g_sensor.last_distance;
    p32_personality_component_act(NULL, "sensor_input", &distance);
    
    // Trigger display reaction for close proximity
    if (distance < 30.0f) {
        extern esp_err_t p32_display_component_act(void* instance, const char* action, void* params);
        p32_display_component_act(NULL, "show_emotion", "curious");
    }
    
    // Trigger audio reaction for motion
    if (g_sensor.motion_detected) {
        extern esp_err_t p32_audio_component_act(void* instance, const char* action, void* params);
        p32_audio_component_act(NULL, "play_sound", "motion_detected");
    }
}

// Simple sensor status
bool p32_simple_sensor_is_ready(void) {
    return g_sensor.initialized;
}

// Simple sensor action (stub for component system)
esp_err_t p32_sensor_component_act(void* instance, const char* action, void* params) {
    ESP_LOGI(TAG, "Sensor action: %s", action);
    
    if (strcmp(action, "update") == 0) {
        p32_simple_sensor_update();
        p32_simple_sensor_trigger_reactions();
    } else if (strcmp(action, "get_distance") == 0) {
        float* distance_ptr = (float*)params;
        if (distance_ptr) {
            *distance_ptr = g_sensor.last_distance;
        }
    } else if (strcmp(action, "get_motion") == 0) {
        bool* motion_ptr = (bool*)params;
        if (motion_ptr) {
            *motion_ptr = g_sensor.motion_detected;
        }
    }
    
    return ESP_OK;
}
#endif // 0 - TEMPORARILY DISABLED
