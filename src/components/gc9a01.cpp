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
#include "driver/spi_master.h"
#include "driver/gpio.h"
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
static spi_device_handle_t current_spi_device_handle = NULL;
static spi_device_handle_t spi_device_1 = NULL;
static spi_device_handle_t spi_device_2 = NULL;

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
 */
esp_err_t gc9a01_init(void) {
    ESP_LOGI(GC9A01_TAG, "Initializing GC9A01 hardware driver");
    
    // Initialize SPI interface
    gc9a01_spi_init();
    
    ESP_LOGI(GC9A01_TAG, "GC9A01 driver initialized");
    return ESP_OK;
}

/**
 * @brief Send current frame buffer to GC9A01 display
 * Uses pins configured for this instance by the positioned component
 */
void gc9a01_act(void) {
    // Process if we have a valid frame (pins are assumed to be configured correctly)
    if (currentFrame == nullptr) {
        return;
    }
    
    // Send the RGB565 frame buffer to this display
    gc9a01_send_frame(0, (uint16_t*)currentFrame, current_frame_size);  // Device parameter ignored, uses pre-configured handle
    
    // Conditional verbose logging based on Environment flags
    Environment* env = GSM.read<Environment>();
    if (env && env->verbose_logging && env->display_diagnostics) {
        if (g_loopCount % 1000 == 0) { // Log every 1000 loops when verbose
            ESP_LOGV(GC9A01_TAG, "Frame sent to SPI device %u at loop %u, frame_size=%u", 
                    current_spi_device, g_loopCount, current_frame_size);
        }
    }
}

/**
 * @brief Initialize SPI interface for GC9A01 display
 * Note: SPI bus and pins are initialized by the SPI bus component.
 * This function only configures GPIO pins for this display instance.
 */
void gc9a01_spi_init(void) {
    ESP_LOGI(GC9A01_TAG, "Configuring GPIO pins for gc9a01 instance");
    
    // Use SPI bus pin variables directly - no pin mapping needed
    uint64_t pin_mask = 0;
    if (spi_bus_current_dc_pin != GPIO_NUM_NC) pin_mask |= (1ULL << spi_bus_current_dc_pin);
    if (spi_bus_reset_pin != GPIO_NUM_NC) pin_mask |= (1ULL << spi_bus_reset_pin);
    
    gpio_config_t io_conf = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    ESP_LOGI(GC9A01_TAG, "Configuring GPIO pins - DC: %d, RST: %d", spi_bus_current_dc_pin, spi_bus_reset_pin);
    
    esp_err_t config_ret = gpio_config(&io_conf);
    if (config_ret != ESP_OK) {
        ESP_LOGE(GC9A01_TAG, "Failed to configure GPIO pins: %s", esp_err_to_name(config_ret));
        return;
    }
    
    // Reset display using shared RST pin
    if (spi_bus_reset_pin != GPIO_NUM_NC) {
        ESP_LOGI(GC9A01_TAG, "Resetting display using shared RST pin %d", spi_bus_reset_pin);
        gpio_set_level(spi_bus_reset_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(spi_bus_reset_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    
    ESP_LOGI(GC9A01_TAG, "GPIO configuration complete for gc9a01 instance");
}

/**
 * @brief Map manufacturer-specific pin names to generic pin names
 * @param manufacturer_name The manufacturer-specific pin name from JSON
 * @return Generic pin name string, or original name if no mapping found
 */
static const char* gc9a01_map_pin_name(const char* manufacturer_name) {
    // SPI display pin name mappings
    if (strcmp(manufacturer_name, "chip_select") == 0 || 
        strcmp(manufacturer_name, "cs") == 0 ||
        strcmp(manufacturer_name, "ss") == 0) {
        return "device_select";
    }
    if (strcmp(manufacturer_name, "data_command") == 0 ||
        strcmp(manufacturer_name, "dc") == 0 ||
        strcmp(manufacturer_name, "a0") == 0) {
        return "data_control";
    }
    if (strcmp(manufacturer_name, "reset") == 0 ||
        strcmp(manufacturer_name, "rst") == 0) {
        return "reset";
    }
    if (strcmp(manufacturer_name, "data") == 0 ||
        strcmp(manufacturer_name, "dout") == 0) {
        return "data";
    }
    
    // Return original name if no mapping found
    return manufacturer_name;
}

/**
 * @brief Send RGB565 frame buffer to specific SPI device
 * Sends data in chunks to avoid ESP32 SPI hardware limits
 */
void gc9a01_send_frame(uint32_t spi_device, uint16_t* frame_buffer, uint32_t pixel_count) {
    // Use the current device handle configured by the positioned component
    spi_device_handle_t device = spi_bus_current_device_handle;
    
    if (device == NULL) {
        ESP_LOGE(GC9A01_TAG, "No SPI device handle configured");
        return;
    }
    
    ESP_LOGI(GC9A01_TAG, "Sending frame to configured SPI device, DC pin %d", spi_bus_current_dc_pin);
    
    if (spi_bus_current_dc_pin == GPIO_NUM_NC) {
        ESP_LOGE(GC9A01_TAG, "Invalid DC pin configured");
        return;
    }
    
    // Set DC pin high for data transmission
    gpio_set_level(spi_bus_current_dc_pin, 1);
    
    // ESP32 SPI hardware limit is around 64KB per transaction
    // Send data in chunks of 2KB (1024 pixels) to be very safe
    const uint32_t CHUNK_SIZE = 1024; // pixels per chunk
    uint32_t remaining_pixels = pixel_count;
    uint16_t* current_buffer = frame_buffer;
    
    while (remaining_pixels > 0) {
        uint32_t chunk_pixels = (remaining_pixels > CHUNK_SIZE) ? CHUNK_SIZE : remaining_pixels;
        uint32_t chunk_bytes = chunk_pixels * 2; // 2 bytes per pixel
        
        // Prepare SPI transaction for this chunk
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = chunk_bytes * 8,  // Length in bits
            .rxlength = 0,
            .user = NULL,
            .tx_buffer = current_buffer,
            .rx_buffer = NULL
        };
        
        // Send chunk to display
        esp_err_t ret = spi_device_transmit(device, &trans);
        if (ret != ESP_OK) {
            ESP_LOGE(GC9A01_TAG, "Failed to transmit chunk to device %u: %s", spi_device, esp_err_to_name(ret));
            return;
        }
        
        // Move to next chunk
        remaining_pixels -= chunk_pixels;
        current_buffer += chunk_pixels;
    }
    
    ESP_LOGV(GC9A01_TAG, "Sent %u pixels to SPI device %u in chunks", pixel_count, spi_device);
}
