#ifndef GC9A01_HPP
#define GC9A01_HPP

/**
 * @file gc9a01.hpp
 * @brief GC9A01 240x240 circular display hardware driver
 * @author P32 Animatronic Bot Project
 */

#include "core/memory/SharedMemory.hpp"
#include <cstdint>

// GC9A01 Display Specifications
#define GC9A01_WIDTH  240
#define GC9A01_HEIGHT 240
#define GC9A01_PIXELS (GC9A01_WIDTH * GC9A01_HEIGHT)
#define GC9A01_FRAME_SIZE (GC9A01_PIXELS * 2)  // 2 bytes per pixel (RGB565)

/**
 * @brief Initialize GC9A01 hardware driver
 * Called once during system startup
 * @return ESP_OK on success, esp_err_t error code on failure
 */
esp_err_t gc9a01_init(void);

/**
 * @brief Send current frame buffer to GC9A01 display
 * Reads currentFrame and current_spi_device set by higher-level components
 */
void gc9a01_act(void);

/**
 * @brief Initialize SPI interface for GC9A01 displays
 */
void gc9a01_spi_init(void);

/**
 * @brief Send RGB565 frame buffer to specific SPI device
 */
void gc9a01_send_frame(uint32_t spi_device, uint16_t* frame_buffer, uint32_t pixel_count);

/**
 * @brief Display Buffer Interface - allocate buffer for display components
 * @return Pointer to newly allocated buffer (115,200 bytes for 240x240x2)
 */
uint8_t* getBuffer(void);

/**
 * @brief Display Buffer Interface - get total frame size in pixels
 * @return Total number of pixels in frame (57,600)
 */
uint32_t getFrameSize(void);

/**
 * @brief Display Buffer Interface - get frame row size in pixels
 * @return Number of pixels per row (240)
 */
uint32_t getFrameRowSize(void);

#endif // GC9A01_HPP
