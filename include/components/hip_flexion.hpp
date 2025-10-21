#ifndef HIP_FLEXION_HPP
#define HIP_FLEXION_HPP

/**
 * @file hip_flexion.hpp
 * @brief Left hip flexion/extension servo - primary leg lift for walking
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize hip_flexion component
 * Called once during system startup
 */
void hip_flexion_init(void);

/**
 * @brief Execute hip_flexion component logic
 * Called every loop iteration based on hitCount
 */
void hip_flexion_act(void);

#endif // HIP_FLEXION_HPP
