#ifndef MOUTH_HPP
#define MOUTH_HPP

/**
 * @file mouth.hpp
 * @brief Mouth display animation - goblin variant with wide grin expressions
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize mouth component
 * Called once during system startup
 */
void mouth_init(void);

/**
 * @brief Execute mouth component logic
 * Called every loop iteration based on hitCount
 */
void mouth_act(void);

#endif // MOUTH_HPP
