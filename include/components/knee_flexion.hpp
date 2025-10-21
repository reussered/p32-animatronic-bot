#ifndef KNEE_FLEXION_HPP
#define KNEE_FLEXION_HPP

/**
 * @file knee_flexion.hpp
 * @brief Left knee flexion/extension servo - shin bend control for walking
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize knee_flexion component
 * Called once during system startup
 */
void knee_flexion_init(void);

/**
 * @brief Execute knee_flexion component logic
 * Called every loop iteration based on hitCount
 */
void knee_flexion_act(void);

#endif // KNEE_FLEXION_HPP
