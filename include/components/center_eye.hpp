#ifndef CENTER_EYE_HPP
#define CENTER_EYE_HPP

/**
 * @file center_eye.hpp
 * @brief Central cyclops eye - large TFT display in portrait orientation
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize center_eye component
 * Called once during system startup
 */
void center_eye_init(void);

/**
 * @brief Execute center_eye component logic
 * Called every loop iteration based on hitCount
 */
void center_eye_act(void);

#endif // CENTER_EYE_HPP
