// src/components/system/system_core.c
// System Core Component - FreeRTOS initialization, heap management, error handling
// Component Type: SYSTEM_LEVEL (attached to torso subsystem)
// Timing: hitCount 100 (executes every 10 seconds)

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef ENABLE_SYSTEM_COMPONENTS

static const char *TAG = "system_core";

// NO ARGUMENTS - Init function signature
esp_err_t system_core_init(void) {
    ESP_LOGI(TAG, "Initializing system core...");
    ESP_LOGI(TAG, "FreeRTOS kernel version: %s", tskKERNEL_VERSION_NUMBER);
    ESP_LOGI(TAG, "ESP-IDF version: %s", esp_get_idf_version());
    
    // Log system information
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "Chip: %s with %d CPU cores, WiFi%s%s",
             CONFIG_IDF_TARGET,
             chip_info.cores,
             (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    
    ESP_LOGI(TAG, "Silicon revision: %d", chip_info.revision);
    ESP_LOGI(TAG, "Flash size: %dMB %s", 
             spi_flash_get_chip_size() / (1024 * 1024),
             (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    
    // Log heap information
    ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap: %d bytes", esp_get_minimum_free_heap_size());
    
    ESP_LOGI(TAG, "System core initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
void system_core_act(void) {
    // hitCount: 100 -> executes every 10 seconds
    
    // Update system status in shared state
    g_shared_state.uptime_seconds = esp_timer_get_time() / 1000000;
    
    // Check heap health
    uint32_t free_heap = esp_get_free_heap_size();
    uint32_t min_heap = esp_get_minimum_free_heap_size();
    
    // Log heap status periodically
    ESP_LOGD(TAG, "System health check at loop %u:", g_loopCount);
    ESP_LOGD(TAG, "  Free heap: %u bytes", free_heap);
    ESP_LOGD(TAG, "  Min heap: %u bytes", min_heap);
    ESP_LOGD(TAG, "  Uptime: %u seconds", g_shared_state.uptime_seconds);
    
    // Warn if heap is getting low
    if (free_heap < 20000) {
        ESP_LOGW(TAG, "Low heap warning: %u bytes free", free_heap);
    }
}

#endif // ENABLE_SYSTEM_COMPONENTS
