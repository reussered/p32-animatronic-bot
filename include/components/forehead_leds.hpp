#ifndef FOREHEAD_LEDS_HPP
#define FOREHEAD_LEDS_HPP

/**
 * @file forehead_leds.hpp
 * @brief Status LED strip using adhesive mounting
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize forehead_leds component
 * Called once during system startup
 */
void forehead_leds_init(void);

/**
 * @brief Execute forehead_leds component logic
 * Called every loop iteration based on hitCount
 */
void forehead_leds_act(void);

#endif // FOREHEAD_LEDS_HPP
