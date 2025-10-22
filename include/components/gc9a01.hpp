#ifndef GC9A01_HPP
#define GC9A01_HPP

/**
 * @file gc9a01.hpp
 * @brief GC9A01 240x240 circular display hardware driver
 * @author P32 Animatronic Bot Project
 */

#include "core/memory/SharedMemory.hpp"
#include <cstdint>

/**
 * @brief Initialize GC9A01 hardware driver
 * Called once during system startup
 */
void gc9a01_init(void);

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

#endif // GC9A01_HPP
