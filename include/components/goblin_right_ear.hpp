#ifndef GOBLIN_RIGHT_EAR_HPP
#define GOBLIN_RIGHT_EAR_HPP

/**
 * @file goblin_right_ear.hpp
 * @brief Goblin right ear component - positioned servo for ear movement
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Initialize goblin right ear component
 * @return ESP_OK on success, esp_err_t error code on failure
 */
esp_err_t goblin_right_ear_init(void);

/**
 * @brief Execute goblin right ear component logic
 */
void goblin_right_ear_act(void);

#endif // GOBLIN_RIGHT_EAR_HPP