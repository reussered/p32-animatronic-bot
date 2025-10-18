#include "p32_component_config.hpp"

#ifdef ENABLE_SIMPLE_CORE
#include "p32_core.hpp"

static const char* TAG = "P32_SIMPLE";

// Simple global system state
typedef struct {
    bool initialized;
    uint32_t loop_count;
    uint32_t last_heartbeat;
} p32_simple_system_t;

static p32_simple_system_t g_simple_system = {0};

// Simple core initialization
esp_err_t p32_core_init(void) {
    ESP_LOGI(TAG, "Initializing P32 simple core system...");
    
    g_simple_system.initialized = true;
    g_simple_system.loop_count = 0;
    g_simple_system.last_heartbeat = 0;
    
    ESP_LOGI(TAG, "P32 simple core initialized successfully");
    return ESP_OK;
}

// Simple display init (stub)
esp_err_t p32_display_init(void) {
    ESP_LOGI(TAG, "Display system initialized (stub)");
    return ESP_OK;
}

// Simple audio init (stub)
esp_err_t p32_audio_init(void) {
    ESP_LOGI(TAG, "Audio system initialized (stub)");
    return ESP_OK;
}

// Simple sensor init (stub)
esp_err_t p32_sensor_init(void) {
    ESP_LOGI(TAG, "Sensor system initialized (stub)");
    return ESP_OK;
}

// Simple mood engine init (stub)
esp_err_t p32_mood_engine_init(void) {
    ESP_LOGI(TAG, "Mood engine initialized (stub)");
    return ESP_OK;
}

// Simple component initialization
void p32_init_all_components(void) {
    ESP_LOGI(TAG, "Initializing all simple components...");
    
    p32_display_init();
    p32_audio_init();
    p32_sensor_init();
    p32_mood_engine_init();
    
    ESP_LOGI(TAG, "All simple components initialized");
}

// Simple main loop
void p32_simple_main_loop(void) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    g_simple_system.loop_count++;
    
    // Heartbeat every 5 seconds
    if (current_time - g_simple_system.last_heartbeat > 5000) {
        ESP_LOGI(TAG, "FreeRTOS System - Loop %lu, Free heap: %lu bytes", 
                 g_simple_system.loop_count, esp_get_free_heap_size());
        g_simple_system.last_heartbeat = current_time;
    }
}

// Simple system status
bool p32_is_initialized(void) {
    return g_simple_system.initialized;
}

#endif // ENABLE_SIMPLE_CORE
