#ifndef LEFT_EAR_MICROPHONE_HPP
#define LEFT_EAR_MICROPHONE_HPP

/**
 * @file left_ear_microphone.hpp
 * @brief Left ear HW-496 microphone for directional audio input
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize left_ear_microphone component
 * Called once during system startup
 */
void left_ear_microphone_init(void);

/**
 * @brief Execute left_ear_microphone component logic
 * Called every loop iteration based on hitCount
 */
void left_ear_microphone_act(void);

#endif // LEFT_EAR_MICROPHONE_HPP
