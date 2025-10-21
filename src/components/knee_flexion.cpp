/**
 * @file knee_flexion.cpp
 * @brief Left knee flexion/extension servo - shin bend control for walking
 * @author Auto-generated from JSON specification
 */

#include "components/knee_flexion.hpp"
#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"
#include "p32_shared_state.h"

// External GSM instance
extern SharedMemory GSM;

/**
 * @brief Initialize knee_flexion component
 * Called once during system startup
 */
void knee_flexion_init(void) {
    // Component initialization logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Read shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    
    // TODO: Implement component-specific initialization
}

/**
 * @brief Execute knee_flexion component logic  
 * Called every loop iteration based on hitCount: 1
 */
void knee_flexion_act(void) {
    // Component execution logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Update shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    // // Modify shared_data...
    // GSM.write<SomeSharedDataType>(shared_data);
    
    // TODO: Implement component-specific behavior
}
