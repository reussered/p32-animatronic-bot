/**
 * @file gc9a01.hpp
 * @brief GC9A01 240x240 circular display hardware driver interface
 * @author P32 Animatronic Bot Project
 */

#ifndef GC9A01_HPP
#define GC9A01_HPP

#include <cstdint>

/**
 * @brief Initialize GC9A01 hardware driver
 */
void gc9a01_init(void);

/**
 * @brief Main action function for GC9A01 display processing
 */
void gc9a01_act(void);

/**
 * @brief Display Buffer Interface - allocate buffer for eye components
 * @return Pointer to newly allocated buffer
 */
uint8_t* getBuffer(void);

/**
 * @brief Display Buffer Interface - get total frame size in pixels
 * @return Total number of pixels in frame
 */
uint32_t getFrameSize(void);

/**
 * @brief Display Buffer Interface - get pixels per row
 * @return Number of pixels per row (display width)
 */
uint32_t getFrameRowSize(void);

#endif // GC9A01_HPP