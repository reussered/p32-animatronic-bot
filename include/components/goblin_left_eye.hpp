#ifndef GOBLIN_LEFT_EYE_HPP
#define GOBLIN_LEFT_EYE_HPP

#include "esp_err.h"

// Goblin Left Eye Component - GC9A01 240x240 Display Test
// Pin configuration:
// SPI_CLK: GPIO 14 (from SPI_BUS_VSPI)
// SPI_MOSI: GPIO 13 (from SPI_BUS_VSPI)  
// SPI_CS: GPIO 15 (from SPI_DEVICE_1)
// DC: GPIO 2 (Data/Command)
// RST: GPIO 4 (Reset)

esp_err_t goblin_left_eye_init(void);
void goblin_left_eye_act(void);

#endif // GOBLIN_LEFT_EYE_HPP
