#ifndef SPINE_FLEXION_SERVO_HPP
#define SPINE_FLEXION_SERVO_HPP

/**
 * @file spine_flexion_servo.hpp
 * @brief Spine flexion servo component for torso bending
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Initialize spine flexion servo component
 * @return ESP_OK on success, esp_err_t error code on failure
 */
esp_err_t spine_flexion_servo_init(void);

/**
 * @brief Execute spine flexion servo component logic
 */
void spine_flexion_servo_act(void);

#endif // SPINE_FLEXION_SERVO_HPP