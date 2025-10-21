#ifndef SPEAKER_HPP
#define SPEAKER_HPP

/**
 * @file speaker.hpp
 * @brief Audio output processing - goblin variant with guttural sound profile
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize speaker component
 * Called once during system startup
 */
void speaker_init(void);

/**
 * @brief Execute speaker component logic
 * Called every loop iteration based on hitCount
 */
void speaker_act(void);

#endif // SPEAKER_HPP
