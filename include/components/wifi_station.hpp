#ifndef WIFI_STATION_HPP
#define WIFI_STATION_HPP

/**
 * @file wifi_station.hpp
 * @brief WiFi station connectivity for cloud integration and remote control
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize wifi_station component
 * Called once during system startup
 */
void wifi_station_init(void);

/**
 * @brief Execute wifi_station component logic
 * Called every loop iteration based on hitCount
 */
void wifi_station_act(void);

#endif // WIFI_STATION_HPP
