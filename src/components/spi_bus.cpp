#include "spi_bus.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include <ArduinoJson.h>
#include <fstream>
#include <string>

// ============================================================================
// SPI Bus Component Implementation - VSPI Bus Management
// ============================================================================

static const char *TAG = "spi_bus";

// SPI Bus Device Registration Array
int spi_bus_device[7] = {0, 0, 0, 0, 0, 0, 0};  // Array to track registered SPI devices
int spi_dev_ptr = 0;        // Pointer for device registration
int spi_act_ptr = 0;        // Pointer for act sequencing

// Current SPI device for act function
int spi_device = 0;

// SPI Bus Pin Configuration - Shared across all SPI devices
int spi_bus_mosi_pin;
int spi_bus_miso_pin;
int spi_bus_sck_pin;
int spi_bus_reset_pin;

// SPI Device Pins - Unique per device
int spi_device_cs_pins[4];  // chip_select_1 to chip_select_4
int spi_device_dc_pins[4];  // data_command_1 to data_command_4

// Current device pins (set by positioned components for their GC9A01 instances)
int spi_bus_current_cs_pin = GPIO_NUM_NC;
int spi_bus_current_dc_pin = GPIO_NUM_NC;

// Current device handle (set by positioned components for their GC9A01 instances)
spi_device_handle_t spi_bus_current_device_handle = NULL;

// SPI Bus Handle
spi_host_device_t spi_bus_host = SPI2_HOST;  // VSPI
spi_bus_config_t spi_bus_config;
spi_device_handle_t spi_device_handles[4] = {NULL, NULL, NULL, NULL};

esp_err_t spi_bus_init(void) {
    ESP_LOGI(TAG, "Initializing SPI bus pins and devices from configuration");

    // Read configuration from SPIFFS
    std::ifstream config_file("/spiffs/config/components/interfaces/spi_bus_vspi.json");
    if (!config_file.is_open()) {
        ESP_LOGE(TAG, "Failed to open SPI bus configuration file");
        return ESP_FAIL;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, config_file);
    if (error) {
        ESP_LOGE(TAG, "Failed to parse SPI bus configuration: %s", error.c_str());
        return ESP_FAIL;
    }

    // Extract pin configurations
    spi_bus_mosi_pin = doc["pins"]["data_output"];
    spi_bus_miso_pin = doc["pins"]["data_input"];
    spi_bus_sck_pin = doc["pins"]["clock"];
    spi_bus_reset_pin = doc["shared_control_pins"]["reset"];

    // Extract device pins
    spi_device_cs_pins[0] = doc["unique_device_pins"]["chip_select_1"];
    spi_device_dc_pins[0] = doc["unique_device_pins"]["data_command_1"];
    spi_device_cs_pins[1] = doc["unique_device_pins"]["chip_select_2"];
    spi_device_dc_pins[1] = doc["unique_device_pins"]["data_command_2"];
    spi_device_cs_pins[2] = doc["unique_device_pins"]["chip_select_3"];
    spi_device_dc_pins[2] = doc["unique_device_pins"]["data_command_3"];
    spi_device_cs_pins[3] = doc["unique_device_pins"]["chip_select_4"];
    spi_device_dc_pins[3] = doc["unique_device_pins"]["data_command_4"];

    ESP_LOGI(TAG, "Loaded SPI pins - MOSI:%d, MISO:%d, SCK:%d, RST:%d",
             spi_bus_mosi_pin, spi_bus_miso_pin, spi_bus_sck_pin, spi_bus_reset_pin);

    // Configure shared pins
    gpio_config_t shared_conf = {
        .pin_bit_mask = (1ULL << spi_bus_mosi_pin) | (1ULL << spi_bus_sck_pin) | (1ULL << spi_bus_reset_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&shared_conf));

    // MISO is input
    gpio_config_t miso_conf = {
        .pin_bit_mask = (1ULL << spi_bus_miso_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&miso_conf));

    // Configure device pins
    for (int i = 0; i < 4; i++) {
        gpio_config_t device_conf = {
            .pin_bit_mask = (1ULL << spi_device_cs_pins[i]) | (1ULL << spi_device_dc_pins[i]),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        ESP_ERROR_CHECK(gpio_config(&device_conf));
    }

    // Set initial states
    gpio_set_level((gpio_num_t)spi_bus_reset_pin, 1);  // Reset high (inactive)

    // Set CS pins high (inactive)
    for (int i = 0; i < 4; i++) {
        gpio_set_level((gpio_num_t)spi_device_cs_pins[i], 1);
    }

    // Initialize SPI bus
    spi_bus_config.mosi_io_num = spi_bus_mosi_pin;
    spi_bus_config.miso_io_num = spi_bus_miso_pin;
    spi_bus_config.sclk_io_num = spi_bus_sck_pin;
    spi_bus_config.quadwp_io_num = -1;
    spi_bus_config.quadhd_io_num = -1;
    spi_bus_config.max_transfer_sz = 4096;

    ESP_ERROR_CHECK(spi_bus_initialize(spi_bus_host, &spi_bus_config, SPI_DMA_CH_AUTO));

    // Initialize SPI devices
    for (int i = 0; i < 4; i++) {
        spi_device_interface_config_t devcfg = {
            .command_bits = 0,
            .address_bits = 0,
            .dummy_bits = 0,
            .mode = 0,
            .duty_cycle_pos = 0,
            .cs_ena_pretrans = 0,
            .cs_ena_posttrans = 0,
            .clock_speed_hz = 10000000,  // 10 MHz
            .input_delay_ns = 0,
            .spics_io_num = spi_device_cs_pins[i],
            .flags = 0,
            .queue_size = 1,
            .pre_cb = NULL,
            .post_cb = NULL
        };

        ESP_ERROR_CHECK(spi_bus_add_device(spi_bus_host, &devcfg, &spi_device_handles[i]));
        ESP_LOGI(TAG, "Added SPI device %d with CS pin %d", i, spi_device_cs_pins[i]);
    }

    // Initialize registration system
    spi_dev_ptr = 0;
    spi_act_ptr = 0;
    for (int i = 0; i < 7; i++) {
        spi_bus_device[i] = 0;
    }

    ESP_LOGI(TAG, "SPI bus and devices initialized from configuration");
    return ESP_OK;
}

void spi_bus_act(void) {
    if (spi_bus_device[spi_act_ptr] == 0) {
        spi_act_ptr = 0;
    }
    spi_device = spi_bus_device[spi_act_ptr++];
}

// Device registration function
int spi_bus_register_device(void) {
    if (spi_dev_ptr >= 7) {
        ESP_LOGE(TAG, "Maximum SPI devices reached");
        return -1;
    }
    
    int device_id = ++spi_dev_ptr;  // Return 1-based device ID
    spi_bus_device[spi_dev_ptr - 1] = device_id;
    
    ESP_LOGI(TAG, "Registered SPI device %d", device_id);
    return device_id;
}