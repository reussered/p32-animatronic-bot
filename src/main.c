// P32 Universal Main - JSON-driven component architecture
// Works with any bot configuration via generated component tables

#include <stdio.h>
#include "p32_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_task_wdt.h"

// Generated component registry header
#include "p32_component_registry.h"

static const char *TAG = "P32_MAIN";
static uint64_t loopCount = 0;

void app_main(void)
{
    
    // Initialize all components from JSON-generated initTable
    ESP_LOGI(TAG, "Initializing %d components...", INIT_TABLE_SIZE);
    for (int i = 0; i < INIT_TABLE_SIZE; i++) {
        if (initTable[i] != NULL) {
            esp_err_t ret = initTable[i]();
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize component %d", i);
            }
        }
    }
    
    ESP_LOGI(TAG, "=== P32 Goblin System Starting ===");
    ESP_LOGI(TAG, "Components: %d active | Loop delay: 100ms", ACT_TABLE_SIZE);
    ESP_LOGI(TAG, "Press 'p' + Enter to pause, 'c' + Enter to continue");
    ESP_LOGI(TAG, "==========================================");
    
    bool paused = false;
    
    // Main action loop - components execute based on loopCount % hitCount
    while (true) {
        // Check for user input (simple pause/continue)
        // Note: In real implementation, you'd use proper UART input handling
        
        if (!paused) {
            // Show current loop info every 1000 loops for readability
            if (loopCount % 1000 == 0) {
                ESP_LOGI(TAG, "Loop %llu - Checking components...", loopCount);
            }
            
            // Execute components
            for (int i = 0; i < ACT_TABLE_SIZE; i++) {
                if (actTable[i].act_func != NULL && actTable[i].hitCount > 0) {
                    if (loopCount % actTable[i].hitCount == 0) {
                        ESP_LOGI(TAG, "[%s] Executing (loop %llu)", 
                                actTable[i].name ? actTable[i].name : "unknown", loopCount);
                        actTable[i].act_func(loopCount);
                    }
                }
            }
            
            loopCount++;
        }
        
        // Readable delay - 100ms between loop iterations
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // Keep watchdog happy
        if (loopCount % 100 == 0) {
            esp_task_wdt_reset();
        }
    }
}