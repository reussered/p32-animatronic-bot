// P32 Universal Main - JSON-driven component architecture
// Works with any bot configuration via generated component tables

#include <stdbool.h>
#include "p32_component_tables.h"
#include "core/memory/SharedMemory.hpp"

// Loop counter - main.cpp has WRITE access, components have READ-ONLY via p32_shared_state.h
extern uint64_t g_loopCount;

// Global SharedMemory instance - one per ESP32 core module
SharedMemory GSM;

extern "C" void app_main(void)
{
    // Initialize ESP-NOW for mesh communication
    GSM.espnow_init();
    
    // Initialize all components
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
    {
        initTable[i]();
    }
    
    // Main execution loop
    while (true)
    {
        for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
        {
            if (hitCountTable[i] > 0 && g_loopCount % hitCountTable[i] == 0)
            {
                actTable[i]();
            }
        }
        g_loopCount++;
    }
}