// P32 Animatronic Bot - Component System Main
// Uses dispatch tables for modular component management

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#ifdef SIMPLE_DISPLAY_TEST
// For simple display test, use direct implementation
extern "C" void simple_display_test_main(void);
#else
#include "p32_component_registry.hpp"
#endif

static const char *TAG = "P32_MAIN";

// Global loop counter for component timing
uint64_t g_loopCount = 0;

#ifndef SIMPLE_DISPLAY_TEST
// Extern declarations for component tables (defined in component_tables.cpp)
extern esp_err_t (*initTable[TABLE_SIZE])(void);
extern void (*actTable[TABLE_SIZE])(void);
extern uint32_t hitCountTable[TABLE_SIZE];
#endif

extern "C" void app_main(void)
{
#ifdef SIMPLE_DISPLAY_TEST
    ESP_LOGI(TAG, "Starting P32 Simple Display Test");
    simple_display_test_main();
#else
    ESP_LOGI(TAG, "Starting P32 Animatronic Bot - Component System");

    // Initialize all components from the dispatch table
    ESP_LOGI(TAG, "Initializing %d components...", TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (initTable[i] != nullptr) {
            esp_err_t err = initTable[i]();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize component %d: %s", i, esp_err_to_name(err));
            } else {
                ESP_LOGI(TAG, "Component %d initialized successfully", i);
            }
        }
    }

    ESP_LOGI(TAG, "All components initialized. Starting main loop...");

    // Main component execution loop
    while (true) {
        g_loopCount++;

        // Execute components based on their timing requirements
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (actTable[i] != nullptr && hitCountTable[i] > 0) {
                if (g_loopCount % hitCountTable[i] == 0) {
                    actTable[i]();
                }
            }
        }

        // Small delay to prevent 100% CPU usage
        vTaskDelay(pdMS_TO_TICKS(1)); // ~1ms delay
    }
#endif
}