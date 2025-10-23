/**
 * @file test_left_eye.cpp
 * @brief Test Left Eye Component - GC9A01 240x240 Display Test
 * @author P32 Animatronic Bot Project
 */

#include "components/test/test_left_eye.hpp"
#include "components/goblin_eye.hpp"
#include "p32_shared_state.h"
#include "core/memory/SharedMemory.hpp"

#include "esp_log.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// External GSM instance
extern SharedMemory GSM;

// SPI pins for left eye display
#define PIN_NUM_MISO   12
#define PIN_NUM_MOSI   13  
#define PIN_NUM_CLK    14
#define PIN_NUM_CS     15   // SPI_DEVICE_1
#define PIN_NUM_DC     27   // Data/Command pin for left eye
#define PIN_NUM_RST    26   // Reset pin (shared)

// Display constants for GC9A01 240x240 circular display
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

static const char *TAG = "TEST_LEFT_EYE";
static spi_device_handle_t spi_handle = NULL;

// Test colors in RGB565 format
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_BLACK   0x0000

/**
 * @brief Send command to GC9A01 display
 */
static void lcd_cmd(uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    gpio_set_level((gpio_num_t)PIN_NUM_DC, 0);  // Command mode
    ret = spi_device_polling_transmit(spi_handle, &t);
    assert(ret == ESP_OK);
}

/**
 * @brief Send data to GC9A01 display
 */
static void lcd_data(const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0) return;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    gpio_set_level((gpio_num_t)PIN_NUM_DC, 1);  // Data mode
    ret = spi_device_polling_transmit(spi_handle, &t);
    assert(ret == ESP_OK);
}

/**
 * @brief Fill display with solid color
 */
static void fill_screen(uint16_t color)
{
    // Set column address (0 to 239)
    lcd_cmd(0x2A);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};
    lcd_data(col_data, 4);
    
    // Set row address (0 to 239)
    lcd_cmd(0x2B);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};
    lcd_data(row_data, 4);
    
    // Start memory write
    lcd_cmd(0x2C);
    
    // Send color data for entire screen
    uint8_t color_bytes[2] = {(color >> 8) & 0xFF, color & 0xFF};
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        lcd_data(color_bytes, 2);
    }
}

/**
 * @brief Initialize GC9A01 display with basic configuration
 */
static void gc9a01_init_sequence(void)
{
    // Reset the display
    gpio_set_level((gpio_num_t)PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level((gpio_num_t)PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Basic GC9A01 initialization sequence
    lcd_cmd(0xEF);
    lcd_cmd(0xEB);
    uint8_t data1 = 0x14;
    lcd_data(&data1, 1);
    
    lcd_cmd(0xFE);
    lcd_cmd(0xEF);
    
    lcd_cmd(0xEB);
    uint8_t data2 = 0x14;
    lcd_data(&data2, 1);
    
    lcd_cmd(0x84);
    uint8_t data3 = 0x40;
    lcd_data(&data3, 1);
    
    lcd_cmd(0x85);
    uint8_t data4 = 0xFF;
    lcd_data(&data4, 1);
    
    lcd_cmd(0x86);
    uint8_t data5 = 0xFF;
    lcd_data(&data5, 1);
    
    lcd_cmd(0x87);
    uint8_t data6 = 0xFF;
    lcd_data(&data6, 1);
    
    lcd_cmd(0x88);
    uint8_t data7 = 0x0A;
    lcd_data(&data7, 1);
    
    lcd_cmd(0x89);
    uint8_t data8 = 0x21;
    lcd_data(&data8, 1);
    
    lcd_cmd(0x8A);
    uint8_t data9 = 0x00;
    lcd_data(&data9, 1);
    
    lcd_cmd(0x8B);
    uint8_t data10 = 0x80;
    lcd_data(&data10, 1);
    
    lcd_cmd(0x8C);
    uint8_t data11 = 0x01;
    lcd_data(&data11, 1);
    
    lcd_cmd(0x8D);
    uint8_t data12 = 0x01;
    lcd_data(&data12, 1);
    
    lcd_cmd(0x8E);
    uint8_t data13 = 0xFF;
    lcd_data(&data13, 1);
    
    lcd_cmd(0x8F);
    uint8_t data14 = 0xFF;
    lcd_data(&data14, 1);
    
    // Memory Access Control
    lcd_cmd(0x36);
    uint8_t madctl = 0x48;
    lcd_data(&madctl, 1);
    
    // Pixel Format - 16-bit RGB565
    lcd_cmd(0x3A);
    uint8_t pixfmt = 0x05;
    lcd_data(&pixfmt, 1);
    
    // Power control settings
    lcd_cmd(0xC3);
    uint8_t pwr1[] = {0x13};
    lcd_data(pwr1, 1);
    
    lcd_cmd(0xC4);
    uint8_t pwr2[] = {0x13};
    lcd_data(pwr2, 1);
    
    lcd_cmd(0xC9);
    uint8_t pwr3[] = {0x22};
    lcd_data(pwr3, 1);
    
    // Sleep out
    lcd_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // Display on
    lcd_cmd(0x29);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    ESP_LOGI(TAG, "GC9A01 initialization complete");
}

/**
 * @brief Initialize test_left_eye component
 * Called once during system startup
 */
void test_left_eye_init(void)
{
    ESP_LOGI(TAG, "Initializing test left eye component");
    
    // Configure GPIO pins
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2
    };
    
    // Initialize SPI bus (only once)
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return;
    }
    
    // Configure SPI device
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 10 * 1000 * 1000,  // 10 MHz
        .input_delay_ns = 0,
        .spics_io_num = PIN_NUM_CS,
        .flags = 0,
        .queue_size = 7,
        .pre_cb = NULL,
        .post_cb = NULL,
    };
    
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return;
    }
    
    // Initialize the display
    gc9a01_init_sequence();
    
    // Fill with black initially
    fill_screen(COLOR_BLACK);
    
    ESP_LOGI(TAG, "Test left eye initialization complete");
}

/**
 * @brief Execute test_left_eye component logic
 * Called every loop iteration based on hitCount
 */
void test_left_eye_act(void)
{
    if (spi_handle == NULL) {
        return;  // Not initialized
    }
    
    // Allocate frame buffer for this eye
    static uint16_t left_eye_frame_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    
    // Set up frame buffer context for goblin_eye to use
    currentFrame = (uint8_t*)left_eye_frame_buffer;
    current_frame_size = DISPLAY_WIDTH * DISPLAY_HEIGHT;
    current_spi_device = 1; // SPI_DEVICE_1 for left eye
    
    // Let goblin_eye render the eyeball
    goblin_eye_act();
    
    // Send the rendered frame to the display
    send_frame_to_display(left_eye_frame_buffer);
    
    ESP_LOGV(TAG, "Left eye frame rendered and sent to display");
}

/**
 * @brief Send RGB565 frame buffer to GC9A01 display
 */
static void send_frame_to_display(uint16_t* frame_buffer)
{
    // Set full screen area
    lcd_cmd(0x2A);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};
    lcd_data(col_data, 4);
    
    lcd_cmd(0x2B);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};
    lcd_data(row_data, 4);
    
    // Start memory write
    lcd_cmd(0x2C);
    
    // Send entire frame buffer
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        uint8_t color_bytes[2] = {
            (frame_buffer[i] >> 8) & 0xFF,
            frame_buffer[i] & 0xFF
        };
        lcd_data(color_bytes, 2);
    }
}