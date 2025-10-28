/**
 * @file gc9a01.cpp
 * @brief GC9A01 240x240 circular display hardware driver implementation
 * @author P32 Animatronic Bot Project
 */

#include "components/gc9a01.hpp"
#include "components/spi_bus.hpp"
#include "components/goblin_eye.hpp"
#include "core/memory/SharedMemory.hpp"
#include "shared/Environment.hpp"
#include "p32_shared_state.h"
#include "esp_log.h"
#include <cstdlib>
#include <ArduinoJson.h>

static const char *GC9A01_TAG = "GC9A01";

// Display dimensions for GC9A01
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240
#define FRAME_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)

// External GSM instance
// GSM is defined as a macro in SharedMemory.hpp

// SPI handles for the displays - configured by SPI bus component

// Display uses SPI bus pin variables directly - no local pin storage
// spi_bus_current_cs_pin, spi_bus_current_dc_pin, spi_bus_reset_pin

// SPI bus pins are accessed through the current device variables set by positioned components
// No pin mapping or local storage needed - displays use SPI bus variables directly

// Forward declarations removed - no pin mapping function needed

/**
 * @brief Display Buffer Interface - allocate buffer for eye components
 * @return Pointer to newly allocated buffer
 */
uint8_t* getBuffer(void) {
    return (uint8_t*)malloc(FRAME_SIZE);
}

/**
 * @brief Display Buffer Interface - get total frame size in pixels
 * @return Total number of pixels in frame
 */
uint32_t getFrameSize(void) {
    return FRAME_SIZE;
}

/**
 * @brief Display Buffer Interface - get pixels per row
 * @return Number of pixels per row (display width)
 */
uint32_t getFrameRowSize(void) {
    return DISPLAY_WIDTH;
}

/**
 * @brief Initialize GC9A01 hardware driver
 * Sets display sizes for generic_spi_display to use
 */
esp_err_t gc9a01_init(void) {
    ESP_LOGI(GC9A01_TAG, "GC9A01 initialized - sizes set for generic_spi_display");
    return ESP_OK;
}

/**
 * @brief GC9A01 act function
 * Display data handling is done by generic_spi_display
 */
void gc9a01_act(void) {
    // Data movement handled by generic_spi_display component
    // This component only provides size information
}
