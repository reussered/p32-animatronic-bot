#include "goblin_head_component_functions.hpp"

// Auto-generated component aggregation file

// --- Begin: config\components\hardware\hw496_microphone.src ---
// HW-496 MEMS Microphone Component
// Uses generic microphone driver with HW496-specific configuration

#include "esp_log.h"
#include "hw496_microphone.hdr"
#include "../drivers/generic_mic_driver.hdr"
#include "core/memory/SharedMemory.hpp"

static const char *TAG_hw496_microphone = "hw496_microphone";

// HW496-specific configuration
// Based on HW496 datasheet: adjustable gain (25x-125x), 58dB SNR, 20Hz-20kHz response
static const float HW496_DEFAULT_GAIN = 2.0f;  // 50x gain setting
static const int HW496_NOISE_GATE_THRESHOLD = 50;  // Adjust based on testing
static const bool HW496_NOISE_GATE_ENABLED = true;

esp_err_t hw496_microphone_init(void) {
    ESP_LOGI(TAG_hw496_microphone, "HW-496 microphone init");

    // HW496-specific hardware initialization
    // Note: generic_mic_driver is initialized separately as a dependency
    // HW496 uses default generic_mic_driver settings

    ESP_LOGI(TAG_hw496_microphone, "HW-496 microphone initialized (using generic driver defaults)");
    return ESP_OK;
}

void hw496_microphone_act(void) {
    // Read microphone data from SharedMemory (written by generic_mic_driver)
    MicrophoneData* mic_data = GSM.read<MicrophoneData>();

    if (mic_data && mic_data->driver_initialized) {
        // HW496-specific processing or monitoring
        // For now, log the data for testing
        ESP_LOGD(TAG_hw496_microphone, "HW496 data: raw=%d, processed=%d, voltage=%dmV, gain=%.1f, sound=%s",
                 mic_data->raw_sample, mic_data->processed_sample, mic_data->voltage_mv,
                 mic_data->gain_applied, mic_data->sound_detected ? "detected" : "none");

        // HW496 could add hardware-specific processing here
        // For example, adjusting gain based on audio levels, or triggering hardware responses
    } else {
        ESP_LOGW(TAG_hw496_microphone, "Microphone data not available or driver not initialized");
    }
}
// --- End: config\components\hardware\hw496_microphone.src ---

// --- Begin: config\components\hardware\gc9a01.src ---
// gc9a01_display component implementation
// GC9A01 1.28" round LCD display driver

#include "esp_log.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp32_s3_r8n16_pin_assignments.h"

static const char *TAG_gc9a01 = "gc9a01";

// Display commands
#define GC9A01_SWRESET 0x01
#define GC9A01_SLPIN  0x10
#define GC9A01_SLPOUT 0x11
#define GC9A01_DISPON 0x29
#define GC9A01_CASET  0x2A  // Column address set
#define GC9A01_RASET  0x2B  // Row address set
#define GC9A01_RAMWR  0x2C  // Memory write

// SPI device handles for left and right displays
static spi_device_handle_t spi_left = NULL;
static spi_device_handle_t spi_right = NULL;

// CS pins - assigned dynamically at runtime
static int left_eye_cs_pin = -1;
static int right_eye_cs_pin = -1;

// SPI transaction function
static void spi_write_command(spi_device_handle_t spi, uint8_t cmd) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0;  // D/C = 0 for command
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_gc9a01, "SPI command transmit failed: %s", esp_err_to_name(ret));
    }
}

static void spi_write_data(spi_device_handle_t spi, uint8_t *data, size_t len) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1;  // D/C = 1 for data
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_gc9a01, "SPI data transmit failed: %s", esp_err_to_name(ret));
    }
}

// Initialize a single display
static esp_err_t init_display(spi_device_handle_t spi, const char *name) {
    ESP_LOGI(TAG_gc9a01, "Initializing %s display...", name);

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

    ESP_LOGI(TAG_gc9a01, "%s display initialized successfully", name);
    return ESP_OK;
}

// Fill display with color
static void fill_display(spi_device_handle_t spi, uint16_t color) {
    // Set column address (0-239)
    spi_write_command(spi, GC9A01_CASET);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};  // Start 0, End 239
    spi_write_data(spi, col_data, 4);

    // Set row address (0-239)
    spi_write_command(spi, GC9A01_RASET);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};  // Start 0, End 239
    spi_write_data(spi, row_data, 4);

    // Memory write
    spi_write_command(spi, GC9A01_RAMWR);

    // Send pixel data (240x240 pixels = 57600 pixels)
    uint8_t pixel_data[2];
    pixel_data[0] = color >> 8;    // High byte
    pixel_data[1] = color & 0xFF;  // Low byte

    for (int i = 0; i < 240 * 240; i++) {
        spi_write_data(spi, pixel_data, 2);
    }
}

esp_err_t gc9a01_init(void) {
    ESP_LOGI(TAG_gc9a01, "gc9a01 display driver init with dynamic pin assignment");

    // Assign CS pins dynamically from SPI assignable array
    // Find available left eye CS pin
    left_eye_cs_pin = -1;
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
            left_eye_cs_pin = pin;
            break;
        }
    }
    if (left_eye_cs_pin == -1) {
        ESP_LOGE(TAG_gc9a01, "Failed to assign left eye CS pin!");
        return ESP_FAIL;
    }

    // Find available right eye CS pin
    right_eye_cs_pin = -1;
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
            right_eye_cs_pin = pin;
            break;
        }
    }
    if (right_eye_cs_pin == -1) {
        ESP_LOGE(TAG_gc9a01, "Failed to assign right eye CS pin!");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG_gc9a01, "Assigned display CS pins - Left:%d, Right:%d",
             left_eye_cs_pin, right_eye_cs_pin);

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
        .spics_io_num = left_eye_cs_pin,  // Left eye CS - dynamically assigned
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    esp_err_t ret = spi_bus_add_device(SPI2_HOST, &devcfg_left, &spi_left);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_gc9a01, "Failed to add left eye SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

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
        .spics_io_num = right_eye_cs_pin,  // Right eye CS - dynamically assigned
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };

    ret = spi_bus_add_device(SPI2_HOST, &devcfg_right, &spi_right);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_gc9a01, "Failed to add right eye SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG_gc9a01, "SPI devices configured successfully");

    // Initialize displays
    ret = init_display(spi_left, "LEFT EYE");
    if (ret != ESP_OK) return ret;

    ret = init_display(spi_right, "RIGHT EYE");
    if (ret != ESP_OK) return ret;

    // Test: Fill displays with different colors to verify they're working
    fill_display(spi_left, 0xF800);   // Red for left eye
    fill_display(spi_right, 0x001F);  // Blue for right eye

    ESP_LOGI(TAG_gc9a01, "gc9a01 displays initialized and tested");
    return ESP_OK;
}

void gc9a01_act(void) {
    // Display driver is passive - drawing is handled by higher-level components
    // ESP_LOGD(TAG_gc9a01, "gc9a01 act");
}
// --- End: config\components\hardware\gc9a01.src ---

// --- Begin: config\components\hardware\generic_spi_display.src ---
// generic_spi_display component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_generic_spi_display = "generic_spi_display";

esp_err_t generic_spi_display_init(void) {
    ESP_LOGI(TAG_generic_spi_display, "generic_spi_display init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void generic_spi_display_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_generic_spi_display, "generic_spi_display act");
}
// --- End: config\components\hardware\generic_spi_display.src ---

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

// NOTE: Source for component 'goblin_head' not found.

// --- Begin: config\components\positioned\goblin_left_ear.src ---
// goblin_left_ear component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_left_ear = "goblin_left_ear";

esp_err_t goblin_left_ear_init(void) {
    ESP_LOGI(TAG_goblin_left_ear, "goblin_left_ear init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_left_ear_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_left_ear, "goblin_left_ear act");
}
// --- End: config\components\positioned\goblin_left_ear.src ---

// --- Begin: config\components\positioned\goblin_left_eye.src ---
// goblin_left_eye component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_left_eye = "goblin_left_eye";

esp_err_t goblin_left_eye_init(void) {
    ESP_LOGI(TAG_goblin_left_eye, "goblin_left_eye init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_left_eye_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_left_eye, "goblin_left_eye act");
}
// --- End: config\components\positioned\goblin_left_eye.src ---

// --- Begin: config\components\positioned\goblin_mouth.src ---
// goblin_mouth component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_mouth = "goblin_mouth";

esp_err_t goblin_mouth_init(void) {
    ESP_LOGI(TAG_goblin_mouth, "goblin_mouth init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_mouth_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_mouth, "goblin_mouth act");
}
// --- End: config\components\positioned\goblin_mouth.src ---

// --- Begin: config\components\creature_specific\goblin_mouth.src ---
// goblin_mouth component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_mouth = "goblin_mouth";

esp_err_t goblin_mouth_init(void) {
    ESP_LOGI(TAG_goblin_mouth, "goblin_mouth init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_mouth_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_mouth, "goblin_mouth act");
}
// --- End: config\components\creature_specific\goblin_mouth.src ---

// --- Begin: config\components\positioned\goblin_nose.src ---
// goblin_nose component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_nose = "goblin_nose";

esp_err_t goblin_nose_init(void) {
    ESP_LOGI(TAG_goblin_nose, "goblin_nose init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_nose_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_nose, "goblin_nose act");
}
// --- End: config\components\positioned\goblin_nose.src ---

// --- Begin: config\components\positioned\goblin_right_ear.src ---
// goblin_right_ear component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_right_ear = "goblin_right_ear";

esp_err_t goblin_right_ear_init(void) {
    ESP_LOGI(TAG_goblin_right_ear, "goblin_right_ear init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_right_ear_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_right_ear, "goblin_right_ear act");
}
// --- End: config\components\positioned\goblin_right_ear.src ---

// --- Begin: config\components\positioned\goblin_right_eye.src ---
// goblin_right_eye component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_right_eye = "goblin_right_eye";

esp_err_t goblin_right_eye_init(void) {
    ESP_LOGI(TAG_goblin_right_eye, "goblin_right_eye init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_right_eye_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_right_eye, "goblin_right_eye act");
}
// --- End: config\components\positioned\goblin_right_eye.src ---

// --- Begin: config\components\positioned\goblin_speaker.src ---
// goblin_speaker component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_speaker = "goblin_speaker";

esp_err_t goblin_speaker_init(void) {
    ESP_LOGI(TAG_goblin_speaker, "goblin_speaker init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_speaker_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_speaker, "goblin_speaker act");
}
// --- End: config\components\positioned\goblin_speaker.src ---

// --- Begin: config\components\hardware\hc_sr04_ultrasonic_distance_sensor.src ---
// hc_sr04_ultrasonic_distance_sensor component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_hc_sr04_ultrasonic_distance_sensor = "hc_sr04_ultrasonic_distance_sensor";

esp_err_t hc_sr04_ultrasonic_distance_sensor_init(void) {
    ESP_LOGI(TAG_hc_sr04_ultrasonic_distance_sensor, "hc_sr04_ultrasonic_distance_sensor init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void hc_sr04_ultrasonic_distance_sensor_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_hc_sr04_ultrasonic_distance_sensor, "hc_sr04_ultrasonic_distance_sensor act");
}
// --- End: config\components\hardware\hc_sr04_ultrasonic_distance_sensor.src ---

// --- Begin: config\components\hardware\servo_sg90_micro.src ---
// servo_sg90_micro component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_servo_sg90_micro = "servo_sg90_micro";

esp_err_t servo_sg90_micro_init(void) {
    ESP_LOGI(TAG_servo_sg90_micro, "servo_sg90_micro init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void servo_sg90_micro_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_servo_sg90_micro, "servo_sg90_micro act");
}
// --- End: config\components\hardware\servo_sg90_micro.src ---

// --- Begin: config\components\hardware\speaker.src ---
// speaker component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_speaker = "speaker";

esp_err_t speaker_init(void) {
    ESP_LOGI(TAG_speaker, "speaker init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void speaker_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_speaker, "speaker act");
}
// --- End: config\components\hardware\speaker.src ---

// --- Begin: config\components\interfaces\spi_bus.src ---
// spi_bus component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_spi_bus = "spi_bus";

esp_err_t spi_bus_init(void) {
    ESP_LOGI(TAG_spi_bus, "spi_bus init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void spi_bus_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_spi_bus, "spi_bus act");
}
// --- End: config\components\interfaces\spi_bus.src ---
