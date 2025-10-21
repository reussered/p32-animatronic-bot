#ifndef ELBOW_FLEXION_HPP
#define ELBOW_FLEXION_HPP

/**
 * @file elbow_flexion.hpp
 * @brief Left elbow flexion/extension servo - forearm bend control
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize elbow_flexion component
 * Called once during system startup
 */
void elbow_flexion_init(void);

/**
 * @brief Execute elbow_flexion component logic
 * Called every loop iteration based on hitCount
 */
void elbow_flexion_act(void);

#endif // ELBOW_FLEXION_HPP
