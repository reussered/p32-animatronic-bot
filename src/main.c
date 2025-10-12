#include "p32_core.h"
#include "p32_component_table.h"
#include "p32_personality_variants.h"

static const char* TAG = "P32_MAIN";

void app_main() {
    ESP_LOGI(TAG, "P32 Animatronic Bot System Starting...");
    ESP_LOGI(TAG, "ESP32-S3 DevKit, ESP-IDF %s", esp_get_idf_version());
    
    // Initialize core P32 system
    ESP_LOGI(TAG, "Initializing P32 core system...");
    ESP_ERROR_CHECK(p32_core_init());
    
    // Initialize all components using component table
    ESP_LOGI(TAG, "Initializing all components...");
    p32_init_all_components();
    
    // Register all personality variants
    ESP_LOGI(TAG, "Registering personality variants...");
    esp_err_t ret = p32_register_all_personalities();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to register personalities, continuing anyway");
    }
    
    // Set initial mood state
    ESP_LOGI(TAG, "Setting initial mood to CONTENTMENT...");
    p32_set_mood(MOOD_CONTENTMENT);
    
    ESP_LOGI(TAG, "P32 System initialization complete!");
    ESP_LOGI(TAG, "Starting main component loop...");
    
    // Main application loop - calls all component act() functions
    while (1) {
        // Process all components in sequence
        p32_loop_all_components();
        
        // Main loop delay (50ms = 20Hz system tick rate)
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}