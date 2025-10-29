// src/components/system/watchdog.c
// Watchdog Component - Hardware watchdog timer for system health monitoring
// Component Type: SYSTEM_LEVEL (attached to torso subsystem)
// Timing: hitCount 500 (executes every 50 seconds)

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

#ifdef ENABLE_SYSTEM_COMPONENTS

static const char *TAG = "watchdog";
static esp_task_wdt_user_handle_t s_wdt_handle = NULL;

// NO ARGUMENTS - Init function signature
esp_err_t watchdog_init(void) {
    ESP_LOGI(TAG, "Initializing hardware watchdog...");
    ESP_LOGI(TAG, "Timeout: 60 seconds, Auto recovery enabled");
    
    // Initialize task watchdog (60 second timeout)
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 60000,
        .idle_core_mask = 0,
        .trigger_panic = false  // Don't panic, just reset
    };
    
    esp_err_t ret = esp_task_wdt_init(&wdt_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize watchdog: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Subscribe main task to watchdog
    ret = esp_task_wdt_add_user(TAG, &s_wdt_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add watchdog user: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Watchdog initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
void watchdog_act(void) {
    // hitCount: 500 -> executes every 50 seconds (before 60 sec timeout)
    
    // Reset watchdog timer
    if (s_wdt_handle != NULL) {
        esp_err_t ret = esp_task_wdt_reset_user(s_wdt_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to reset watchdog: %s", esp_err_to_name(ret));
        } else {
            ESP_LOGD(TAG, "Watchdog reset at loop %u", g_loopCount);
        }
    }
}

#endif // ENABLE_SYSTEM_COMPONENTS
