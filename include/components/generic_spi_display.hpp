#ifndef GENERIC_SPI_DISPLAY_HPP
#define GENERIC_SPI_DISPLAY_HPP

#include "esp_err.h"
#include "core/memory/SharedMemory.hpp"
#include <cstdint>

/**
 * @file generic_spi_display.hpp
 * @brief Generic SPI Display Driver - reusable for any SPI-based display
 * @author P32 Animatronic Bot Project
 */

// Display configuration structure - loaded from JSON
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t color_depth;  // bits per pixel
    uint32_t frame_size;  // total bytes per frame
    uint8_t rotation;     // 0, 90, 180, 270
    bool invert_colors;
    bool backlight_pwm;
    // Add more config options as needed
} generic_display_config_t;

/**
 * @brief Initialize generic SPI display with configuration
 * @param config Display configuration from JSON
 * @return ESP_OK on success, esp_err_t error code on failure
 */
esp_err_t generic_spi_display_init(const generic_display_config_t* config);

/**
 * @brief Send current frame buffer to the display
 * Uses SPI bus variables configured by positioned component
 */
void generic_spi_display_act(void);

/**
 * @brief Send RGB565 frame buffer to display
 * @param frame_buffer Pointer to frame data
 * @param pixel_count Number of pixels in frame
 */
void generic_spi_display_send_frame(uint16_t* frame_buffer, uint32_t pixel_count);

/**
 * @brief Display Buffer Interface - allocate buffer for display components
 * @return Pointer to newly allocated buffer
 */
uint8_t* generic_display_get_buffer(void);

/**
 * @brief Display Buffer Interface - get total frame size in pixels
 * @return Total number of pixels in frame
 */
uint32_t generic_display_get_frame_size(void);

/**
 * @brief Display Buffer Interface - get frame width in pixels
 * @return Frame width
 */
uint32_t generic_display_get_frame_width(void);

/**
 * @brief Display Buffer Interface - get frame height in pixels
 * @return Frame height
 */
uint32_t generic_display_get_frame_height(void);

#endif // GENERIC_SPI_DISPLAY_HPP