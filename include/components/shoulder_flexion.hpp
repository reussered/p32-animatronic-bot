#ifndef SHOULDER_FLEXION_HPP
#define SHOULDER_FLEXION_HPP

/**
 * @file shoulder_flexion.hpp
 * @brief Left shoulder flexion/extension servo - primary arm lift
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize shoulder_flexion component
 * Called once during system startup
 */
void shoulder_flexion_init(void);

/**
 * @brief Execute shoulder_flexion component logic
 * Called every loop iteration based on hitCount
 */
void shoulder_flexion_act(void);

#endif // SHOULDER_FLEXION_HPP
