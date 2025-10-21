#ifndef WRIST_FLEXION_HPP
#define WRIST_FLEXION_HPP

/**
 * @file wrist_flexion.hpp
 * @brief Left wrist flexion/extension servo - hand positioning
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize wrist_flexion component
 * Called once during system startup
 */
void wrist_flexion_init(void);

/**
 * @brief Execute wrist_flexion component logic
 * Called every loop iteration based on hitCount
 */
void wrist_flexion_act(void);

#endif // WRIST_FLEXION_HPP
