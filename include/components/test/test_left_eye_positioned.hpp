#ifndef TEST_LEFT_EYE_POSITIONED_HPP
#define TEST_LEFT_EYE_POSITIONED_HPP

/**
 * @file test_left_eye_positioned.hpp
 * @brief Test Left Eye Positioned Component - Wrapper for test_left_eye with position info
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Initialize test_left_eye_positioned component
 * Called once during system startup
 */
esp_err_t test_left_eye_positioned_init(void);

/**
 * @brief Execute test_left_eye_positioned component logic
 * Called every loop iteration based on hitCount
 */
void test_left_eye_positioned_act(void);

#endif // TEST_LEFT_EYE_POSITIONED_HPP