// P32 Universal Main - JSON-driven component architecture
// Works with any bot configuration via generated component tables

#include <stdbool.h>
#include "p32_component_tables.hpp"

// Loop counter - main.cpp has WRITE access, components have READ-ONLY via p32_shared_state.hpp
extern uint64_t g_loopCount;

// ESP-IDF requires app_main to have C linkage
extern "C" void app_main(void)
{
    for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
    {
        initTable[i]();
    }
    
    while (true)
    {
        for (int i = 0; i < COMPONENT_TABLE_SIZE; i++)
        {
            if (g_loopCount % hitCountTable[i] == 0)
            {
                actTable[i]();
            }
        }
        g_loopCount++;
    }
}