/**
 * @file st7796.hpp
 * @brief ST7796 320x480 TFT display hardware driver interface
 * @author P32 Animatronic Bot Project
 */

#ifndef ST7796_HPP
#define ST7796_HPP

#include <cstdint>

// ST7796 Display Specifications
#define ST7796_WIDTH  320
#define ST7796_HEIGHT 480
#define ST7796_PIXELS (ST7796_WIDTH * ST7796_HEIGHT)
#define ST7796_FRAME_SIZE (ST7796_PIXELS * 2)  // 2 bytes per pixel (RGB565)

/**
 * @brief Initialize ST7796 hardware driver
 */
void st7796_init(void);

/**
 * @brief Main action function for ST7796 display processing
 */
void st7796_act(void);

/**
 * @brief Display Buffer Interface - allocate buffer for display components
 * @return Pointer to newly allocated buffer (307,200 bytes for 320x480x2)
 */
uint8_t* getBuffer(void);

/**
 * @brief Display Buffer Interface - get total frame size in pixels
 * @return Total number of pixels in frame (153,600)
 */
uint32_t getFrameSize(void);

/**
 * @brief Display Buffer Interface - get frame row size in pixels  
 * @return Number of pixels per row (320)
 */
uint32_t getFrameRowSize(void);

/**
 * @brief Set display rotation
 * @param rotation 0=0°, 1=90°, 2=180°, 3=270°
 */
void st7796_set_rotation(uint8_t rotation);

/**
 * @brief Control backlight brightness
 * @param brightness 0-255 brightness level
 */
void st7796_set_backlight(uint8_t brightness);

/**
 * @brief Send display buffer to ST7796
 * @param buffer Pointer to RGB565 pixel data
 * @param size Buffer size in bytes
 */
void st7796_send_buffer(const uint8_t* buffer, uint32_t size);

#endif // ST7796_HPP