#ifndef HC_SR04_ULTRASONIC_DISTANCE_SENSOR_HPP
#define HC_SR04_ULTRASONIC_DISTANCE_SENSOR_HPP

/**
 * @file hc_sr04_ultrasonic_distance_sensor.hpp
 * @brief HC-SR04 Ultrasonic Distance Sensor component
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize hc_sr04_ultrasonic_distance_sensor component
 * Called once during system startup
 */
esp_err_t hc_sr04_ultrasonic_distance_sensor_init(void);

/**
 * @brief Execute hc_sr04_ultrasonic_distance_sensor component logic
 * Called every loop iteration based on hitCount
 */
void hc_sr04_ultrasonic_distance_sensor_act(void);

#endif // HC_SR04_ULTRASONIC_DISTANCE_SENSOR_HPP
