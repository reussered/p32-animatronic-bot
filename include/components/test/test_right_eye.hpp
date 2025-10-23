#ifndef TEST_RIGHT_EYE_HPP
#define TEST_RIGHT_EYE_HPP

/**
 * @file test_right_eye.hpp
 * @brief Test Right Eye Component - GC9A01 240x240 Display Test
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Test Right Eye Component - GC9A01 240x240 Display Test
 * Pin configuration:
 * SPI_CLK: GPIO 14 (from SPI_BUS_VSPI)
 * SPI_MOSI: GPIO 13 (from SPI_BUS_VSPI)  
 * SPI_CS: GPIO 32 (from SPI_DEVICE_2)
 * DC: GPIO 33 (Data/Command for right eye)
 * RST: GPIO 26 (Reset - shared)
 */

/**
 * @brief Initialize test_right_eye component
 * Called once during system startup
 */
void test_right_eye_init(void);

/**
 * @brief Execute test_right_eye component logic
 * Called every loop iteration based on hitCount
 */
void test_right_eye_act(void);

#endif // TEST_RIGHT_EYE_HPP