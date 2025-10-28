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
 * @brief Initialize GC9A01 hardware driver using generic SPI display
 * Called once during system startup
 */
esp_err_t gc9a01_init(void);

/**
 * @brief Send current frame buffer to GC9A01 display using generic SPI display
 * Reads currentFrame and current_spi_device set by higher-level components
 */
void gc9a01_act(void);

/**
 * @brief Display Buffer Interface - allocate buffer for display components
 * @return Pointer to display buffer (managed by generic component)
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
