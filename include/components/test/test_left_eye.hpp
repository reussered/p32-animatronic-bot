#ifndef TEST_LEFT_EYE_HPP
#define TEST_LEFT_EYE_HPP

/**
 * @file test_left_eye.hpp
 * @brief Test Left Eye Component - GC9A01 240x240 Display Test
 * @author P32 Animatronic Bot Project
 */

#include "esp_err.h"

/**
 * @brief Test Left Eye Component - GC9A01 240x240 Display Test
 * Pin configuration:
 * SPI_CLK: GPIO 14 (from spi_bus)
 * SPI_MOSI: GPIO 13 (from spi_bus)  
 * SPI_CS: GPIO 15 (from SPI_DEVICE_1)
 * DC: GPIO 27 (Data/Command for left eye)
 * RST: GPIO 26 (Reset - shared)
 */

/**
 * @brief Initialize test_left_eye component
 * Called once during system startup
 */
void test_left_eye_init(void);

/**
 * @brief Execute test_left_eye component logic
 * Called every loop iteration based on hitCount
 */
void test_left_eye_act(void);

#endif // TEST_LEFT_EYE_HPP
