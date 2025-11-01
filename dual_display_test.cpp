#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_s3_r8n16_pin_assignments.h"

// Display pin definitions - dynamically assigned at runtime
static int pin_spi_miso = -1;
static int pin_spi_mosi = -1;
static int pin_spi_clk = -1;
static int pin_cs_left = -1;
static int pin_cs_right = -1;

// Display commands
#define GC9A01_SWRESET 0x01
#define GC9A01_SLPIN  0x10
#define GC9A01_SLPOUT 0x11
#define GC9A01_DISPON 0x29

static const char *TAG = "DUAL_DISPLAY_TEST";

spi_device_handle_t spi_left;
spi_device_handle_t spi_right;

// SPI transaction function
void spi_write_command(spi_device_handle_t spi, uint8_t cmd) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0;  // D/C = 0 for command
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI command transmit failed: %s", esp_err_to_name(ret));
    }
}

void spi_write_data(spi_device_handle_t spi, uint8_t *data, size_t len) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1;  // D/C = 1 for data
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI data transmit failed: %s", esp_err_to_name(ret));
    }
}

// Initialize a display
void init_display(spi_device_handle_t spi, const char *name) {
    ESP_LOGI(TAG, "Initializing %s display...", name);

    // Hardware reset would go here if RST pins were connected
    vTaskDelay(pdMS_TO_TICKS(100));

    // Software reset
    spi_write_command(spi, GC9A01_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    // Sleep out
    spi_write_command(spi, GC9A01_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(500));

    // Display on
    spi_write_command(spi, GC9A01_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "%s display initialized successfully", name);
}

// Fill display with color
void fill_display(spi_device_handle_t spi, uint16_t color, const char *name) {
    ESP_LOGI(TAG, "Filling %s display with color 0x%04X", name, color);

    // Set column address (0-239)
    spi_write_command(spi, 0x2A);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};  // Start 0, End 239
    spi_write_data(spi, col_data, 4);

    // Set row address (0-239)
    spi_write_command(spi, 0x2B);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};  // Start 0, End 239
    spi_write_data(spi, row_data, 4);

    // Memory write
    spi_write_command(spi, 0x2C);

    // Send pixel data (240x240 pixels = 57600 pixels)
    uint8_t pixel_data[2];
    pixel_data[0] = color >> 8;    // High byte
    pixel_data[1] = color & 0xFF;  // Low byte

    for (int i = 0; i < 240 * 240; i++) {
        spi_write_data(spi, pixel_data, 2);
    }

    ESP_LOGI(TAG, "%s display filled", name);
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting Dual Display Test with dynamic pin assignment");

    // Assign SPI pins dynamically from assignable arrays
    // Find available MISO pin
    pin_spi_miso = -1;
    for (size_t i = 0; i < spi_assignable_count; i++) {
        int pin = spi_assignable[i];
        bool already_assigned = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == pin) {
                already_assigned = true;
                break;
            }
        }
        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {
            assigned_pins[assigned_pins_count++] = pin;
            pin_spi_miso = pin;
            break;
        }
    }
    if (pin_spi_miso == -1) {
        ESP_LOGE(TAG, "Failed to assign SPI MISO pin!");
        return;
    }

    // Find available MOSI pin
    pin_spi_mosi = -1;
    for (size_t i = 0; i < spi_assignable_count; i++) {
        int pin = spi_assignable[i];
        bool already_assigned = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == pin) {
                already_assigned = true;
                break;
            }
        }
        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {
            assigned_pins[assigned_pins_count++] = pin;
            pin_spi_mosi = pin;
            break;
        }
    }
    if (pin_spi_mosi == -1) {
        ESP_LOGE(TAG, "Failed to assign SPI MOSI pin!");
        return;
    }

    // Find available CLK pin
    pin_spi_clk = -1;
    for (size_t i = 0; i < spi_assignable_count; i++) {
        int pin = spi_assignable[i];
        bool already_assigned = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == pin) {
                already_assigned = true;
                break;
            }
        }
        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {
            assigned_pins[assigned_pins_count++] = pin;
            pin_spi_clk = pin;
            break;
        }
    }
    if (pin_spi_clk == -1) {
        ESP_LOGE(TAG, "Failed to assign SPI CLK pin!");
        return;
    }

    // Find available left CS pin
    pin_cs_left = -1;
    for (size_t i = 0; i < spi_assignable_count; i++) {
        int pin = spi_assignable[i];
        bool already_assigned = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == pin) {
                already_assigned = true;
                break;
            }
        }
        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {
            assigned_pins[assigned_pins_count++] = pin;
            pin_cs_left = pin;
            break;
        }
    }
    if (pin_cs_left == -1) {
        ESP_LOGE(TAG, "Failed to assign left CS pin!");
        return;
    }

    // Find available right CS pin
    pin_cs_right = -1;
    for (size_t i = 0; i < spi_assignable_count; i++) {
        int pin = spi_assignable[i];
        bool already_assigned = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == pin) {
                already_assigned = true;
                break;
            }
        }
        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {
            assigned_pins[assigned_pins_count++] = pin;
            pin_cs_right = pin;
            break;
        }
    }
    if (pin_cs_right == -1) {
        ESP_LOGE(TAG, "Failed to assign right CS pin!");
        return;
    }

    ESP_LOGI(TAG, "Assigned pins - MISO:%d, MOSI:%d, CLK:%d, CS_LEFT:%d, CS_RIGHT:%d",
             pin_spi_miso, pin_spi_mosi, pin_spi_clk, pin_cs_left, pin_cs_right);

    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = pin_spi_mosi,
        .miso_io_num = pin_spi_miso,
        .sclk_io_num = pin_spi_clk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 240 * 240 * 2 + 8,  // Max transfer size
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // Configure left eye SPI device
    spi_device_interface_config_t devcfg_left = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 40 * 1000 * 1000,  // 40 MHz
        .input_delay_ns = 0,
        .spics_io_num = pin_cs_left,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg_left, &spi_left));

    // Configure right eye SPI device
    spi_device_interface_config_t devcfg_right = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 40 * 1000 * 1000,  // 40 MHz
        .input_delay_ns = 0,
        .spics_io_num = pin_cs_right,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg_right, &spi_right));

    ESP_LOGI(TAG, "SPI devices configured successfully");

    // Initialize displays
    init_display(spi_left, "LEFT EYE");
    init_display(spi_right, "RIGHT EYE");

    // Test pattern: Fill left eye with red, right eye with blue
    fill_display(spi_left, 0xF800, "LEFT EYE");   // Red
    fill_display(spi_right, 0x001F, "RIGHT EYE"); // Blue

    ESP_LOGI(TAG, "Dual display test completed successfully!");
    ESP_LOGI(TAG, "You should see: LEFT EYE = RED, RIGHT EYE = BLUE");

    // Keep displays active
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Displays active - test running...");
    }
}