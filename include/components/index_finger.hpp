#ifndef INDEX_FINGER_HPP
#define INDEX_FINGER_HPP

/**
 * @file index_finger.hpp
 * @brief Left index finger servo - primary pointing and manipulation
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize index_finger component
 * Called once during system startup
 */
void index_finger_init(void);

/**
 * @brief Execute index_finger component logic
 * Called every loop iteration based on hitCount
 */
void index_finger_act(void);

#endif // INDEX_FINGER_HPP
