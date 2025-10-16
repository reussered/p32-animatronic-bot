// P32 Universal Main - JSON-driven component architecture
// Works with any bot configuration via generated component tables

#include <stdbool.h>
#include "p32_component_tables.h"

// Loop counter - main.c has WRITE access, components have READ-ONLY via p32_shared_state.h
extern uint64_t g_loopCount;

void app_main(void)
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