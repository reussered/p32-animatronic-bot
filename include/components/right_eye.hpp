#ifndef RIGHT_EYE_HPP
#define RIGHT_EYE_HPP

/**
 * @file right_eye.hpp
 * @brief Right eye display animation - goblin variant using standard GC9A01 hardware
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize right_eye component
 * Called once during system startup
 */
void right_eye_init(void);

/**
 * @brief Execute right_eye component logic
 * Called every loop iteration based on hitCount
 */
void right_eye_act(void);

#endif // RIGHT_EYE_HPP
