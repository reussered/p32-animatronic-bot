#ifndef THUMB_HPP
#define THUMB_HPP

/**
 * @file thumb.hpp
 * @brief Left thumb servo - opposition and grip control
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize thumb component
 * Called once during system startup
 */
void thumb_init(void);

/**
 * @brief Execute thumb component logic
 * Called every loop iteration based on hitCount
 */
void thumb_act(void);

#endif // THUMB_HPP
