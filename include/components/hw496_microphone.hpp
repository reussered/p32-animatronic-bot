#ifndef HW496_MICROPHONE_HPP
#define HW496_MICROPHONE_HPP

/**
 * @file hw496_microphone.hpp
 * @brief HW-496 MEMS microphone with MAX4466 pre-amplifier
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize hw496_microphone component
 * Called once during system startup
 */
void hw496_microphone_init(void);

/**
 * @brief Execute hw496_microphone component logic
 * Called every loop iteration based on hitCount
 */
void hw496_microphone_act(void);

#endif // HW496_MICROPHONE_HPP
