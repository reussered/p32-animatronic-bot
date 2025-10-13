#include "p32_component_config.h"

#ifdef ENABLE_COMPONENT_TABLE
#include "p32_component_table.h"
#include "nvs_flash.h"
#include "esp_system.h"

static const char* TAG = "P32_COMPONENT_TABLE";

// Global loop counter
uint32_t loopCount = 0;

void p32_init_all_components(void) {
    ESP_LOGI(TAG, "Initializing all components...");
    ESP_LOGI(TAG, "Component init count: %d", componentInitCount);
    
    // Initialize loop counter
    loopCount = 0;
    
    // Initialize serial/logging system first
    ESP_LOGI(TAG, "Serial monitor and logging already initialized by ESP-IDF");
    
    // Initialize NVS for configuration storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS flash initialized");
    
    // Call all component init functions
    for(int i = 0; i < componentInitCount; ++i) {
        ESP_LOGI(TAG, "Initializing component %d/%d", i+1, componentInitCount);
        componentInitTable[i]();
    }
    
    ESP_LOGI(TAG, "All components initialized successfully");
}

void p32_loop_all_components(void) {
    // Call all component act/loop functions
    for(int i = 0; i < componentCount; ++i) {
        componentTable[i]();
    }
    
    // Increment loop counter after full pass
    loopCount++;
    
    // Log system status periodically
    if (loopCount % 1000 == 0) {
        ESP_LOGI(TAG, "System running - Loop: %lu", loopCount);
    }
}

#endif // ENABLE_COMPONENT_TABLE
