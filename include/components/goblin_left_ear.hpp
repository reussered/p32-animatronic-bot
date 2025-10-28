#ifndef GOBLIN_LEFT_EAR_HPP
#define GOBLIN_LEFT_EAR_HPP

/**
 * @file goblin_left_ear.hpp
 * @brief Goblin left ear component - positioned servo for ear movement
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Initialize goblin left ear component
 * @return ESP_OK on success, esp_err_t error code on failure
 */
esp_err_t goblin_left_ear_init(void);

/**
 * @brief Execute goblin left ear component logic
 */
void goblin_left_ear_act(void);

#endif // GOBLIN_LEFT_EAR_HPP