#ifndef POWER_MANAGER_HPP
#define POWER_MANAGER_HPP

/**
 * @file power_manager.hpp
 * @brief Battery monitoring and power distribution control
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize power_manager component
 * Called once during system startup
 */
void power_manager_init(void);

/**
 * @brief Execute power_manager component logic
 * Called every loop iteration based on hitCount
 */
void power_manager_act(void);

#endif // POWER_MANAGER_HPP
