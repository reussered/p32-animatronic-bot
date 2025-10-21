#ifndef RIGHT_EAR_MICROPHONE_HPP
#define RIGHT_EAR_MICROPHONE_HPP

/**
 * @file right_ear_microphone.hpp
 * @brief Right ear HW-496 microphone for directional audio input
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize right_ear_microphone component
 * Called once during system startup
 */
void right_ear_microphone_init(void);

/**
 * @brief Execute right_ear_microphone component logic
 * Called every loop iteration based on hitCount
 */
void right_ear_microphone_act(void);

#endif // RIGHT_EAR_MICROPHONE_HPP
