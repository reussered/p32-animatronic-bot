#include "p32_component_config.h"

#ifdef ENABLE_SIMPLE_DISPLAY
// SIMPLE DISPLAY COMPONENT
#include "p32_core.h"

static const char* TAG = "P32_SIMPLE_DISPLAY";

// Simple display state
typedef struct {
    bool initialized;
    uint8_t brightness;
    uint32_t last_update;
    bool animation_active;
} p32_simple_display_t;

static p32_simple_display_t g_displays[3] = {0}; // Left eye, right eye, mouth

// Simple display names
static const char* display_names[] = {"Left Eye", "Right Eye", "Mouth"};

// Simple display initialization
esp_err_t p32_simple_display_init(void) {
    ESP_LOGI(TAG, "Initializing simple display system...");
    
    for (int i = 0; i < 3; i++) {
        g_displays[i].initialized = true;
        g_displays[i].brightness = 128; // 50% brightness
        g_displays[i].last_update = 0;
        g_displays[i].animation_active = false;
        
        ESP_LOGI(TAG, "Display %d (%s) initialized", i, display_names[i]);
    }
    
    return ESP_OK;
}

// Simple display update
void p32_simple_display_update(void) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    for (int i = 0; i < 3; i++) {
        if (!g_displays[i].initialized) continue;
        
        // Simple animation: "blink" every 3 seconds
        if (current_time - g_displays[i].last_update > 3000) {
            g_displays[i].animation_active = !g_displays[i].animation_active;
            g_displays[i].last_update = current_time;
            
            ESP_LOGI(TAG, "%s %s", display_names[i], 
                     g_displays[i].animation_active ? "ON" : "OFF");
        }
    }
}

// Simple display set brightness
esp_err_t p32_simple_display_set_brightness(uint8_t display_id, uint8_t brightness) {
    if (display_id >= 3) return ESP_ERR_INVALID_ARG;
    
    g_displays[display_id].brightness = brightness;
    ESP_LOGI(TAG, "%s brightness set to %d", display_names[display_id], brightness);
    
    return ESP_OK;
}

// Simple display show emotion
esp_err_t p32_simple_display_show_emotion(const char* emotion) {
    ESP_LOGI(TAG, "Displaying emotion: %s", emotion);
    
    // Simple emotion mapping
    if (strcmp(emotion, "happy") == 0) {
        for (int i = 0; i < 3; i++) {
            g_displays[i].brightness = 255; // full bright
        }
    } else if (strcmp(emotion, "sad") == 0) {
        for (int i = 0; i < 3; i++) {
            g_displays[i].brightness = 64; // dim
        }
    } else if (strcmp(emotion, "angry") == 0) {
        g_displays[0].brightness = 255; // eyes bright
        g_displays[1].brightness = 255;
        g_displays[2].brightness = 128; // mouth medium
    }
    
    return ESP_OK;
}

// Simple display status
bool p32_simple_display_is_ready(void) {
    return g_displays[0].initialized && g_displays[1].initialized && g_displays[2].initialized;
}

// Simple display action (stub for component system)
esp_err_t p32_display_component_act(void* instance, const char* action, void* params) {
    ESP_LOGI(TAG, "Display action: %s", action);
    
    if (strcmp(action, "update") == 0) {
        p32_simple_display_update();
    } else if (strcmp(action, "show_emotion") == 0) {
        char* emotion = (char*)params;
        if (emotion) {
            p32_simple_display_show_emotion(emotion);
        }
    } else if (strcmp(action, "set_brightness") == 0) {
        uint8_t* brightness = (uint8_t*)params;
        if (brightness) {
            for (int i = 0; i < 3; i++) {
                p32_simple_display_set_brightness(i, *brightness);
            }
        }
    }
    
    return ESP_OK;
}
#endif // 0 - TEMPORARILY DISABLED
