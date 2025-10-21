#ifndef POWER_MONITOR_HPP
#define POWER_MONITOR_HPP

/**
 * @file power_monitor.hpp
 * @brief Power monitoring - battery voltage, current consumption, remaining capacity
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize power_monitor component
 * Called once during system startup
 */
void power_monitor_init(void);

/**
 * @brief Execute power_monitor component logic
 * Called every loop iteration based on hitCount
 */
void power_monitor_act(void);

#endif // POWER_MONITOR_HPP
