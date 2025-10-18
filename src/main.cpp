// P32 Universal Main - FreeRTOS Optimized - JSON-driven component architecture
// Runs on FreeRTOS but optimized for maximum cycles with minimal yielding

#include <stdbool.h>
#include "p32_component_tables.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Loop counter - main.cpp has WRITE access, components have READ-ONLY via p32_shared_state.hpp
extern uint64_t g_loopCount;

// ESP-IDF requires app_main to have C linkage
extern "C" void app_main(void)
{
    // Initialize all components
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
    {
        initTable[i]();
    }
    
    // High-speed component execution loop with minimal FreeRTOS yielding
    uint32_t yield_counter = 0;
    const uint32_t YIELD_FREQUENCY = 10000;  // Yield every 10,000 loops
    
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
        
        // Minimal yielding - only every 10,000 loops to keep system stable
        yield_counter++;
        if (yield_counter >= YIELD_FREQUENCY) {
            taskYIELD();  // Give other FreeRTOS tasks a chance
            yield_counter = 0;
        }
    }
}