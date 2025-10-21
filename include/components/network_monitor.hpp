#ifndef NETWORK_MONITOR_HPP
#define NETWORK_MONITOR_HPP

/**
 * @file network_monitor.hpp
 * @brief Network connectivity monitoring - WiFi signal strength, connection status
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize network_monitor component
 * Called once during system startup
 */
void network_monitor_init(void);

/**
 * @brief Execute network_monitor component logic
 * Called every loop iteration based on hitCount
 */
void network_monitor_act(void);

#endif // NETWORK_MONITOR_HPP
