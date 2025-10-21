#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

/**
 * @file watchdog.hpp
 * @brief Hardware watchdog timer - system health monitoring and automatic recovery
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize watchdog component
 * Called once during system startup
 */
void watchdog_init(void);

/**
 * @brief Execute watchdog component logic
 * Called every loop iteration based on hitCount
 */
void watchdog_act(void);

#endif // WATCHDOG_HPP
