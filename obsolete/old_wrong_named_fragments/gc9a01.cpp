/**
 * @file gc9a01.cpp
 * @brief GC9A01 240x240 circular display hardware driver implementation
 * @author P32 Animatronic Bot Project
 */

#include "components/gc9a01.hpp"
#include "components/goblin_eye.hpp"
#include "core/memory/SharedMemory.hpp"
#include "shared/Environment.hpp"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <cstdlib>

static const char *TAG = "GC9A01";

// Display dimensions for GC9A01
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240
#define FRAME_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)

// External GSM instance
extern SharedMemory GSM;

// SPI handles for the displays
static spi_device_handle_t spi_device_1 = NULL; // Left eye
static spi_device_handle_t spi_device_2 = NULL; // Right eye
static spi_device_handle_t spi_device_3 = NULL; // Mouth (future)

// GPIO pin definitions (from wiring guide)
#define SPI_SCK_PIN     (gpio_num_t)14
#define SPI_MOSI_PIN    (gpio_num_t)13
#define SPI_MISO_PIN    (gpio_num_t)12
#define LEFT_EYE_CS     (gpio_num_t)15
#define LEFT_EYE_DC     (gpio_num_t)18
#define LEFT_EYE_RST    (gpio_num_t)21
#define RIGHT_EYE_CS    (gpio_num_t)5
#define RIGHT_EYE_DC    (gpio_num_t)19
#define RIGHT_EYE_RST   (gpio_num_t)22

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
void gc9a01_init(void) {
    ESP_LOGI(TAG, "Initializing GC9A01 hardware driver");
    
    // Initialize SPI interface
    gc9a01_spi_init();
    
    ESP_LOGI(TAG, "GC9A01 driver initialized");
}

/**
 * @brief Send current frame buffer to GC9A01 display
 * Reads currentFrame and current_spi_device set by higher-level components
 */
void gc9a01_act(void) {
    // Only process if we have a valid frame and device context
    if (currentFrame == nullptr || current_spi_device == 0) {
        return;
    }
    
    // Send the RGB565 frame buffer to the appropriate display
    gc9a01_send_frame(current_spi_device, (uint16_t*)currentFrame, current_frame_size);
    
    // Conditional verbose logging based on Environment flags
    Environment* env = GSM.read<Environment>();
    if (env && env->verbose_logging && env->display_diagnostics) {
        if (g_loopCount % 1000 == 0) { // Log every 1000 loops when verbose
            ESP_LOGV(TAG, "Frame sent to SPI device %u at loop %u, frame_size=%u", 
                    current_spi_device, g_loopCount, current_frame_size);
        }
    }
}

/**
 * @brief Initialize SPI interface for GC9A01 displays
 */
void gc9a01_spi_init(void) {
    esp_err_t ret;
    
    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 240 * 240 * 2, // Max frame size in bytes
        .flags = 0,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = 0
    };
    
    // Initialize SPI bus
    ret = spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }
    
    // Configure left eye display (SPI_DEVICE_1)
    spi_device_interface_config_t devcfg_left = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,                         // SPI mode 0
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 20000000,        // 20 MHz
        .input_delay_ns = 0,
        .spics_io_num = LEFT_EYE_CS,      // CS pin
        .flags = 0,
        .queue_size = 1,                   // Queue only one transaction at a time
        .pre_cb = NULL,
        .post_cb = NULL
    };
    
    ret = spi_bus_add_device(SPI3_HOST, &devcfg_left, &spi_device_1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add left eye SPI device: %s", esp_err_to_name(ret));
        return;
    }
    
    // Configure right eye display (SPI_DEVICE_2)
    spi_device_interface_config_t devcfg_right = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,                         // SPI mode 0
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 20000000,        // 20 MHz
        .input_delay_ns = 0,
        .spics_io_num = RIGHT_EYE_CS,     // CS pin
        .flags = 0,
        .queue_size = 1,                   // Queue only one transaction at a time
        .pre_cb = NULL,
        .post_cb = NULL
    };
    
    ret = spi_bus_add_device(SPI3_HOST, &devcfg_right, &spi_device_2);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add right eye SPI device: %s", esp_err_to_name(ret));
        return;
    }
    
    // Configure GPIO pins for DC and RST
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LEFT_EYE_DC) | (1ULL << LEFT_EYE_RST) | 
                       (1ULL << RIGHT_EYE_DC) | (1ULL << RIGHT_EYE_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    // Reset displays
    gpio_set_level(LEFT_EYE_RST, 0);
    gpio_set_level(RIGHT_EYE_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(LEFT_EYE_RST, 1);
    gpio_set_level(RIGHT_EYE_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    ESP_LOGI(TAG, "SPI interface initialized for GC9A01 displays");
}

/**
 * @brief Send RGB565 frame buffer to specific SPI device
 */
void gc9a01_send_frame(uint32_t spi_device, uint16_t* frame_buffer, uint32_t pixel_count) {
    spi_device_handle_t device = NULL;
    gpio_num_t dc_pin;
    
    // Select the appropriate SPI device and DC pin
    switch (spi_device) {
        case 1: // Left eye
            device = spi_device_1;
            dc_pin = (gpio_num_t)LEFT_EYE_DC;
            break;
        case 2: // Right eye
            device = spi_device_2;
            dc_pin = (gpio_num_t)RIGHT_EYE_DC;
            break;
        default:
            ESP_LOGE(TAG, "Invalid SPI device: %u", spi_device);
            return;
    }
    
    if (device == NULL) {
        ESP_LOGE(TAG, "SPI device %u not initialized", spi_device);
        return;
    }
    
    // Set DC pin high for data transmission
    gpio_set_level(dc_pin, 1);
    
    // Prepare SPI transaction
    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = pixel_count * 16,  // Length in bits
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = frame_buffer,
        .rx_buffer = NULL
    };
    
    // Send frame buffer to display
    esp_err_t ret = spi_device_transmit(device, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to transmit frame to device %u: %s", spi_device, esp_err_to_name(ret));
    }
    
    ESP_LOGV(TAG, "Sent %u pixels to SPI device %u", pixel_count, spi_device);
}
