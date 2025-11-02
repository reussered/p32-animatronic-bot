#include "subsystems/test_head/test_head_component_functions.hpp"

// Auto-generated component aggregation file

// --- Begin: config\components\hardware\gc9a01.src ---
// gc9a01_display component implementation
// This component is a data provider. It defines the physical characteristics
// of the GC9A01 display but contains no active logic.

#include "esp_log.h"
#include "esp_heap_caps.h"
#include <stdlib.h>

static const char *TAG_gc9a01 = "gc9a01";

// Display Buffer Interface - provides buffer allocation and size info
// to positioned components (eyes, mouth, etc.)

/**
 * @brief Get display width in pixels
 * @return Display width (240 for GC9A01)
 */
uint16_t gc9a01_get_width(void) {
    return 240;
}

/**
 * @brief Get display height in pixels
 * @return Display height (240 for GC9A01)
 */
uint16_t gc9a01_get_height(void) {
    return 240;
}

/**
 * @brief Get total frame size in pixels
 * @return Total pixels in one frame (width * height)
 */
uint32_t getFrameSize(void) {
    return 240 * 240;  // 57,600 pixels
}

/**
 * @brief Get pixels per row (display width)
 * @return Number of pixels per row
 */
uint32_t getFrameRowSize(void) {
    return 240;
}

/**
 * @brief Get display buffer size in bytes for RGB565 format
 * @return Total bytes needed for one full framebuffer (width * height * 2)
 */
size_t getDisplayBufferSize(void) {
    return 240 * 240 * 2;  // RGB565 = 2 bytes per pixel = 115,200 bytes
}

esp_err_t gc9a01_init(void) {
    ESP_LOGI(TAG_gc9a01, "gc9a01 component initialized (passive)");
    return ESP_OK;
}

void gc9a01_act(void) {
    // This component is passive and does nothing in its act function.
}
// --- End: config\components\hardware\gc9a01.src ---

// --- Begin: config\components\drivers\generic_spi_display.src ---
// generic_spi_display component implementation
// Generic SPI display interface for basic drawing operations

#include "esp_log.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "components/spi_display_bus.hdr"
#include "driver/gpio.h"

static const char *TAG_generic_spi_display = "generic_spi_display";

// Display commands
#define GC9A01_SWRESET 0x01
#define GC9A01_SLPOUT 0x11
#define GC9A01_DISPON 0x29
#define GC9A01_CASET  0x2A
#define GC9A01_RASET  0x2B
#define GC9A01_RAMWR  0x2C

// Internal helper functions to send commands and data
static void generic_spi_write_data(spi_device_handle_t spi, const uint8_t *data, size_t len) {
    if (!spi) return;
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0) return;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // D/C = 1 for data
    ret = spi_device_polling_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);
}

static void spi_write_command(spi_device_handle_t spi, const uint8_t cmd) {
    if (!spi) return;
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // D/C = 0 for command
    ret = spi_device_polling_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);
}

esp_err_t generic_spi_display_init(void) {
    ESP_LOGI(TAG_generic_spi_display, "Initializing generic SPI display with CS=%d", cur_spi_pin.cs);

    if (cur_spi_pin.handle == NULL) {
        ESP_LOGE(TAG_generic_spi_display, "SPI handle is NULL, cannot initialize display.");
        return ESP_FAIL;
    }

    // Hardware reset
    if (cur_spi_pin.rst != -1) {
        gpio_set_level((gpio_num_t)cur_spi_pin.rst, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level((gpio_num_t)cur_spi_pin.rst, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Software reset
    spi_write_command(cur_spi_pin.handle, GC9A01_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    // Sleep out
    spi_write_command(cur_spi_pin.handle, GC9A01_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(500));

    // Display on
    spi_write_command(cur_spi_pin.handle, GC9A01_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG_generic_spi_display, "Display with CS=%d initialized successfully", cur_spi_pin.cs);
    return ESP_OK;
}

void generic_spi_display_act(void) {
    if (cur_spi_pin.handle == NULL) {
        ESP_LOGE(TAG_generic_spi_display, "act: SPI handle is NULL for CS=%d", cur_spi_pin.cs);
        return;
    }

    // Get the appropriate framebuffer based on which display we're driving
    // CS=3 is left eye, CS=6 is right eye
    uint16_t* framebuffer = nullptr;
    if (cur_spi_pin.cs == 3) {
        framebuffer = goblin_left_eye_get_buffer();
    } else if (cur_spi_pin.cs == 6) {
        framebuffer = goblin_right_eye_get_buffer();
    }

    if (!framebuffer) {
        ESP_LOGE(TAG_generic_spi_display, "No framebuffer available for CS=%d", cur_spi_pin.cs);
        return;
    }

    // Set column address (0-239)
    spi_write_command(cur_spi_pin.handle, GC9A01_CASET);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};
    generic_spi_write_data(cur_spi_pin.handle, col_data, 4);

    // Set row address (0-239)
    spi_write_command(cur_spi_pin.handle, GC9A01_RASET);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};
    generic_spi_write_data(cur_spi_pin.handle, row_data, 4);

    // Memory write
    spi_write_command(cur_spi_pin.handle, GC9A01_RAMWR);

    // Send the actual framebuffer data
    // Convert RGB565 pixels to byte stream (big-endian)
    const size_t total_pixels = 240 * 240;
    const size_t row_size = 240 * 2;  // 240 pixels * 2 bytes per pixel
    uint8_t* row_buffer = (uint8_t*)malloc(row_size);
    
    if (!row_buffer) {
        ESP_LOGE(TAG_generic_spi_display, "Failed to allocate row buffer");
        return;
    }

    // Send framebuffer row by row
    for (uint32_t row = 0; row < 240; row++) {
        uint16_t* row_pixels = framebuffer + (row * 240);
        
        // Convert uint16_t pixels to byte stream
        for (uint32_t col = 0; col < 240; col++) {
            uint16_t pixel = row_pixels[col];
            row_buffer[col * 2] = pixel >> 8;       // High byte
            row_buffer[col * 2 + 1] = pixel & 0xFF; // Low byte
        }
        
        generic_spi_write_data(cur_spi_pin.handle, row_buffer, row_size);
    }
    
    free(row_buffer);
}
// --- End: config\components\drivers\generic_spi_display.src ---

// --- Begin: config\components\creature_specific\goblin_eye.src ---
// goblin_eye component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_eye = "goblin_eye";

esp_err_t goblin_eye_init(void) {
    ESP_LOGI(TAG_goblin_eye, "goblin_eye init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_eye_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_eye, "goblin_eye act");
}
// --- End: config\components\creature_specific\goblin_eye.src ---

// --- Begin: config\components\positioned\goblin_left_eye.src ---
// goblin_left_eye component implementation
// Positioned component that manages the left eye display buffer

#include "esp_log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG_goblin_left_eye = "goblin_left_eye";

// Display buffer for left eye
static uint16_t* left_display_buffer = nullptr;

/**
 * @brief Initialize left eye display buffer
 * Allocates framebuffer using Display Buffer Interface
 */
esp_err_t goblin_left_eye_init(void) {
    ESP_LOGI(TAG_goblin_left_eye, "Initializing left eye display buffer");
    
    // Allocate display buffer using size from gc9a01
    size_t buffer_size = getDisplayBufferSize();
    left_display_buffer = (uint16_t*)malloc(buffer_size);
    
    if (!left_display_buffer) {
        ESP_LOGE(TAG_goblin_left_eye, "Failed to allocate left eye buffer");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with test pattern - red for left eye
    uint16_t test_color = 0xF800;  // Red in RGB565
    uint32_t pixel_count = getFrameSize();
    for (uint32_t i = 0; i < pixel_count; i++) {
        left_display_buffer[i] = test_color;
    }
    
    ESP_LOGI(TAG_goblin_left_eye, "Left eye buffer allocated: %zu bytes (%.1f KB)", 
             buffer_size, buffer_size / 1024.0f);
    
    return ESP_OK;
}

/**
 * @brief Get pointer to left eye display buffer
 * @return Pointer to RGB565 framebuffer, or nullptr if not initialized
 */
uint16_t* goblin_left_eye_get_buffer(void) {
    return left_display_buffer;
}

/**
 * @brief Update left eye animation
 * Will eventually render eye animations into the buffer
 */
void goblin_left_eye_act(void) {
    // Future: Render eye animations here
    // For now, buffer is static (initialized in init())
}
// --- End: config\components\positioned\goblin_left_eye.src ---

// --- Begin: config\components\positioned\goblin_right_eye.src ---
// goblin_right_eye component implementation
// Positioned component that manages the right eye display buffer

#include "esp_log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG_goblin_right_eye = "goblin_right_eye";

// Display buffer for right eye
static uint16_t* right_display_buffer = nullptr;

/**
 * @brief Initialize right eye display buffer
 * Allocates framebuffer using Display Buffer Interface
 */
esp_err_t goblin_right_eye_init(void) {
    ESP_LOGI(TAG_goblin_right_eye, "Initializing right eye display buffer");
    
    // Allocate display buffer using size from gc9a01
    size_t buffer_size = getDisplayBufferSize();
    right_display_buffer = (uint16_t*)malloc(buffer_size);
    
    if (!right_display_buffer) {
        ESP_LOGE(TAG_goblin_right_eye, "Failed to allocate right eye buffer");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with test pattern - green for right eye
    uint16_t test_color = 0x07E0;  // Green in RGB565
    uint32_t pixel_count = getFrameSize();
    for (uint32_t i = 0; i < pixel_count; i++) {
        right_display_buffer[i] = test_color;
    }
    
    ESP_LOGI(TAG_goblin_right_eye, "Right eye buffer allocated: %zu bytes (%.1f KB)", 
             buffer_size, buffer_size / 1024.0f);
    
    return ESP_OK;
}

/**
 * @brief Get pointer to right eye display buffer
 * @return Pointer to RGB565 framebuffer, or nullptr if not initialized
 */
uint16_t* goblin_right_eye_get_buffer(void) {
    return right_display_buffer;
}

/**
 * @brief Update right eye animation
 * Will eventually render eye animations into the buffer
 */
void goblin_right_eye_act(void) {
    // Future: Render eye animations here
    // For now, buffer is static (initialized in init())
}
// --- End: config\components\positioned\goblin_right_eye.src ---

// --- Begin: config\components\drivers\spi_display_bus.src ---
// spi_display_bus driver implementation
// Provides dedicated SPI bus allocation for display peripherals

#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_s3_r8n16_pin_assignments.h"
#include "components/spi_display_bus.hdr"

#include <stddef.h>
#include <stdio.h>

static constexpr size_t SPI_DISPLAY_SLOT_COUNT = 32U;
static spi_display_pinset_t spi_display_slots[SPI_DISPLAY_SLOT_COUNT];
spi_display_pinset_t cur_spi_pin;

static const char *TAG_spi_display_bus = "spi_display_bus";

static spi_display_pinset_t shared_display_pins;

static int get_next_assignable(const int *assignable, size_t assignable_count) {
    for (size_t i = 0; i < assignable_count; ++i) {
        const int candidate = assignable[i];
        bool already_claimed = false;
        for (size_t j = 0; j < assigned_pins_count; ++j) {
            if (assigned_pins[j] == candidate) {
                already_claimed = true;
                break;
            }
        }

        if (!already_claimed) {
            if (assigned_pins_count >= (sizeof(assigned_pins) / sizeof(assigned_pins[0]))) {
                ESP_LOGE(TAG_spi_display_bus, "Assigned pin buffer exhausted");
                esp_system_abort("Assigned pin buffer exhausted");
            }

            assigned_pins[assigned_pins_count++] = candidate;
            return candidate;
        }
    }

    ESP_LOGE(TAG_spi_display_bus, "No assignable pins remain for SPI display bus");
    esp_system_abort("SPI display bus ran out of assignable pins");
    return -1;
}

esp_err_t spi_display_bus_init(void) {
    static size_t device_count = 0;

    if (device_count >= SPI_DISPLAY_SLOT_COUNT) 
    {
        ESP_LOGE(TAG_spi_display_bus, "No remaining SPI display device slots available");
        return ESP_FAIL;
    }

    // On the very first call, allocate shared pins and initialize the bus
    if (device_count == 0) 
    {
        spi_display_slots[0].mosi = get_next_assignable(spi_assignable, spi_assignable_count);
        spi_display_slots[0].clk = get_next_assignable(spi_assignable, spi_assignable_count);
        spi_display_slots[0].rst = get_next_assignable(spi_assignable, spi_assignable_count);

        spi_bus_config_t bus_cfg = {
            .mosi_io_num = spi_display_slots[0].mosi,
            .miso_io_num = -1,
            .sclk_io_num = spi_display_slots[0].clk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 240 * 240 * 2 + 8,
            .flags = SPICOMMON_BUSFLAG_MASTER,
            .intr_flags = 0
        };

        const esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) 
        {
            ESP_LOGE(TAG_spi_display_bus, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI(TAG_spi_display_bus,
                 "Shared SPI display pins assigned MOSI:%d CLK:%d RST:%d",
                 spi_display_slots[0].mosi,
                 spi_display_slots[0].clk,
                 spi_display_slots[0].rst);
    }

    // For subsequent devices, copy the shared pin configuration from the first device
    if (device_count > 0)
    {
        spi_display_slots[device_count].mosi = spi_display_slots[0].mosi;
        spi_display_slots[device_count].clk = spi_display_slots[0].clk;
        spi_display_slots[device_count].rst = spi_display_slots[0].rst;
    }

    // For every device (including the first), allocate its unique pins and add to bus
    spi_display_slots[device_count].cs = get_next_assignable(spi_assignable, spi_assignable_count);
    spi_display_slots[device_count].dc = get_next_assignable(spi_assignable, spi_assignable_count);
    spi_display_slots[device_count].bl = get_next_assignable(spi_assignable, spi_assignable_count);
    
    // Configure RST, DC, and BL as outputs
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    if (spi_display_slots[device_count].rst != -1) {
        io_conf.pin_bit_mask = (1ULL << spi_display_slots[device_count].rst);
        gpio_config(&io_conf);
        gpio_set_level((gpio_num_t)spi_display_slots[device_count].rst, 1);  // Set high initially
    }
    
    if (spi_display_slots[device_count].dc != -1) {
        io_conf.pin_bit_mask = (1ULL << spi_display_slots[device_count].dc);
        gpio_config(&io_conf);
    }
    
    if (spi_display_slots[device_count].bl != -1) {
        io_conf.pin_bit_mask = (1ULL << spi_display_slots[device_count].bl);
        gpio_config(&io_conf);
        gpio_set_level((gpio_num_t)spi_display_slots[device_count].bl, 1);  // Turn on backlight
    }
    
    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 40 * 1000 * 1000, // 40 MHz
        .input_delay_ns = 0,
        .spics_io_num = spi_display_slots[device_count].cs,
        .queue_size = 7,
        .pre_cb = NULL, // Set D/C line high for data
        .post_cb = NULL
    };

    esp_err_t ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_display_slots[device_count].handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_spi_display_bus, "spi_bus_add_device failed for CS=%d: %s", spi_display_slots[device_count].cs, esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG_spi_display_bus,
             "Display slot %u assigned unique pins CS:%d DC:%d BL:%d and handle %p",
             static_cast<unsigned>(device_count),
             spi_display_slots[device_count].cs,
             spi_display_slots[device_count].dc,
             spi_display_slots[device_count].bl,
             (void*)spi_display_slots[device_count].handle);

    // Set cur_spi_pin to the device we just created so the next component can use it immediately
    cur_spi_pin = spi_display_slots[device_count];

    device_count++;
    return ESP_OK;
}

void spi_display_bus_act(void) 
{
    static int device = 0;

    // If the current device slot is uninitialized, reset to the first device.
    // This handles both the end-of-list and the empty-list cases.
    if (device >= SPI_DISPLAY_SLOT_COUNT || spi_display_slots[device].cs == -1) 
    {
        device = 0;
    }

    // If the first slot is also uninitialized, there's nothing to do.
    if (spi_display_slots[device].cs == -1)
    {
        return;
    }

    // Set the current global pinset for other components to use
    cur_spi_pin = spi_display_slots[device];

    // Move to the next device for the next call
    device++;
}
// --- End: config\components\drivers\spi_display_bus.src ---
