/**
 * @file goblin_personality.cpp
 * @brief Goblin family personality traits - mischievous, aggressive, curious
 * @author Auto-generated from JSON specification
 */

#include "components/goblin_personality.hpp"
#include "core/memory/SharedMemory.hpp"

#include "p32_shared_state.h"

// External GSM instance
extern SharedMemory GSM;

/**
 * @brief Initialize goblin_personality component
 * Called once during system startup
 */
void goblin_personality_init(void) {
    // Component initialization logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Read shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    
    // TODO: Implement component-specific initialization
}

/**
 * @brief Execute goblin_personality component logic  
 * Called every loop iteration based on hitCount: 1
 */
void goblin_personality_act(void) {
    // Component execution logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Update shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    // // Modify shared_data...
    // GSM.write<SomeSharedDataType>(shared_data);
    
    // TODO: Implement component-specific behavior
}
