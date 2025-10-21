#ifndef LEFT_EAR_HPP
#define LEFT_EAR_HPP

/**
 * @file left_ear.hpp
 * @brief Left ear speaker with integrated goblin ear aesthetics
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize left_ear component
 * Called once during system startup
 */
void left_ear_init(void);

/**
 * @brief Execute left_ear component logic
 * Called every loop iteration based on hitCount
 */
void left_ear_act(void);

#endif // LEFT_EAR_HPP
