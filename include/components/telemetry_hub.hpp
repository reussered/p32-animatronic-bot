#ifndef TELEMETRY_HUB_HPP
#define TELEMETRY_HUB_HPP

/**
 * @file telemetry_hub.hpp
 * @brief Central telemetry aggregation and publishing hub - collects data from all subsystems
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize telemetry_hub component
 * Called once during system startup
 */
void telemetry_hub_init(void);

/**
 * @brief Execute telemetry_hub component logic
 * Called every loop iteration based on hitCount
 */
void telemetry_hub_act(void);

#endif // TELEMETRY_HUB_HPP
