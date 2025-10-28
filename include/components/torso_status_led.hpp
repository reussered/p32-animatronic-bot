#ifndef TORSO_STATUS_LED_HPP
#define TORSO_STATUS_LED_HPP

/**
 * @file torso_status_led.hpp
 * @brief Torso status LED component for system status indication
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Initialize torso status LED component
 * @return ESP_OK on success, esp_err_t error code on failure
 */
esp_err_t torso_status_led_init(void);

/**
 * @brief Execute torso status LED component logic
 */
void torso_status_led_act(void);

#endif // TORSO_STATUS_LED_HPP