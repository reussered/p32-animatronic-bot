// P32 Universal Main - FreeRTOS Optimized - JSON-driven component architecture
// Runs on FreeRTOS but optimized for maximum cycles with minimal yielding

#include <stdbool.h>
#include "p32_component_tables.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

// Loop counter - main.cpp has WRITE access, components have READ-ONLY via p32_shared_state.hpp
extern uint64_t g_loopCount;

// ESP-IDF requires app_main to have C linkage
extern "C" void app_main(void)
{
    // Initialize hardware watchdog - 2 second timeout for real-time system
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 2000,
        .idle_core_mask = (1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1,
        .trigger_panic = true
    };
    esp_task_wdt_init(&wdt_config);
    esp_task_wdt_add(NULL);
    
    // Initialize all components
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
    {
        initTable[i]();
    }
    
    // High-speed component execution loop with minimal overhead
    uint32_t wdt_counter = 0;
    const uint32_t WDT_FEED_FREQUENCY = 50;  // Feed watchdog every 50 loops (~483ms at 103Hz)
    
    while (true)
    {
        // Execute components at maximum speed
        for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
        {
            if (g_loopCount % hitCountTable[i] == 0)
            {
                actTable[i]();
            }
        }
        g_loopCount++;
        
        // Feed watchdog - minimal overhead (~5μs every 483ms = 0.001% CPU)
        wdt_counter++;
        if (wdt_counter >= WDT_FEED_FREQUENCY) {
            esp_task_wdt_reset();
            wdt_counter = 0;
        }
        
        // NO taskYIELD() - system intentionally runs at maximum load for 60fps performance
    }
}