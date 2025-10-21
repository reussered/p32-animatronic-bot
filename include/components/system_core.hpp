#ifndef SYSTEM_CORE_HPP
#define SYSTEM_CORE_HPP

/**
 * @file system_core.hpp
 * @brief Core system management - initialization, health checks, error handling
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize system_core component
 * Called once during system startup
 */
void system_core_init(void);

/**
 * @brief Execute system_core component logic
 * Called every loop iteration based on hitCount
 */
void system_core_act(void);

#endif // SYSTEM_CORE_HPP
