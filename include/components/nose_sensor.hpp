#ifndef NOSE_SENSOR_HPP
#define NOSE_SENSOR_HPP

/**
 * @file nose_sensor.hpp
 * @brief Proximity sensor monitoring - bear soft nose variant
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize nose_sensor component
 * Called once during system startup
 */
void nose_sensor_init(void);

/**
 * @brief Execute nose_sensor component logic
 * Called every loop iteration based on hitCount
 */
void nose_sensor_act(void);

#endif // NOSE_SENSOR_HPP
