#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "GC9A01_TEST";

// Pin definitions for ESP32-S3-DevKitC-1
#define PIN_NUM_MISO    -1   // Not used for displays
#define PIN_NUM_MOSI    13
#define PIN_NUM_CLK     14
#define PIN_NUM_CS      15
#define PIN_NUM_DC      2
#define PIN_NUM_RST     4

// Display constants
#define GC9A01_WIDTH    240
#define GC9A01_HEIGHT   240

static spi_device_handle_t spi_device = NULL;

// GC9A01 initialization commands
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes;
} lcd_init_cmd_t;

DRAM_ATTR static const lcd_init_cmd_t gc9a01_init_cmds[] = {
    {0xEF, {0}, 0},
    {0xEB, {0x14}, 1},
    {0xFE, {0}, 0},
    {0xEF, {0}, 0},
    {0xEB, {0x14}, 1},
    {0x84, {0x40}, 1},
    {0x85, {0xFF}, 1},
    {0x86, {0xFF}, 1},
    {0x87, {0xFF}, 1},
    {0x88, {0x0A}, 1},
    {0x89, {0x21}, 1},
    {0x8A, {0x00}, 1},
    {0x8B, {0x80}, 1},
    {0x8C, {0x01}, 1},
    {0x8D, {0x01}, 1},
    {0x8E, {0xFF}, 1},
    {0x8F, {0xFF}, 1},
    {0xB6, {0x00, 0x20}, 2},
    {0x3A, {0x05}, 1},  // 16-bit color
    {0x90, {0x08, 0x08, 0x08, 0x08}, 4},
    {0xBD, {0x06}, 1},
    {0xBC, {0x00}, 1},
    {0xFF, {0x60, 0x01, 0x04}, 3},
    {0xC3, {0x13}, 1},
    {0xC4, {0x13}, 1},
    {0xC9, {0x22}, 1},
    {0xBE, {0x11}, 1},
    {0xE1, {0x10, 0x0E}, 2},
    {0xDF, {0x21, 0x0c, 0x02}, 3},
    {0xF0, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}, 6},
    {0xF1, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}, 6},
    {0xF2, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}, 6},
    {0xF3, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}, 6},
    {0xED, {0x1B, 0x0B}, 2},
    {0xAE, {0x77}, 1},
    {0xCD, {0x63}, 1},
    {0x70, {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03}, 9},
    {0xE8, {0x34}, 1},
    {0x62, {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70}, 12},
    {0x63, {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70}, 12},
    {0x64, {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07}, 7},
    {0x66, {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00}, 10},
    {0x67, {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98}, 10},
    {0x74, {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00}, 7},
    {0x98, {0x3e, 0x07}, 2},
    {0x35, {0}, 0},
    {0x21, {0}, 0},
    {0x11, {0}, 0x80},  // Sleep out
    {0x29, {0}, 0x80},  // Display on
    {0, {0}, 0xff}
};

static void spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level((gpio_num_t)PIN_NUM_DC, dc);
}

static void gc9a01_spi_init(void)
{
    esp_err_t ret;
    
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = GC9A01_WIDTH * GC9A01_HEIGHT * 2 + 8
    };
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10*1000*1000,           // Clock out at 10 MHz
        .mode = 0,                                // SPI mode 0
        .spics_io_num = PIN_NUM_CS,               // CS pin
        .queue_size = 7,                          // We want to be able to queue 7 transactions at a time
        .pre_cb = spi_pre_transfer_callback,      // Specify pre-transfer callback to handle D/C line
    };
    
    // Initialize the SPI bus
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    
    // Attach the LCD to the SPI bus
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_device);
    ESP_ERROR_CHECK(ret);
    
    // Initialize non-SPI GPIOs
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = ((1ULL<<PIN_NUM_DC) | (1ULL<<PIN_NUM_RST));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "SPI initialized successfully");
}

static void gc9a01_send_cmd(uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t = {};
    t.length = 8;                     // Command is 8 bits
    t.tx_buffer = &cmd;               // The data is the cmd itself
    t.user = (void*)0;                // D/C needs to be set to 0
    ret = spi_device_polling_transmit(spi_device, &t);  // Transmit!
    assert(ret == ESP_OK);            // Should have had no issues.
}

static void gc9a01_send_data(uint8_t *data, int len)
{
    esp_err_t ret;
    if (len == 0) return;             // no need to send anything
    
    spi_transaction_t t = {};
    t.length = len * 8;               // Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;               // Data
    t.user = (void*)1;                // D/C needs to be set to 1
    ret = spi_device_polling_transmit(spi_device, &t);  // Transmit!
    assert(ret == ESP_OK);            // Should have had no issues.
}

static void gc9a01_init(void)
{
    // Reset the display
    gpio_set_level((gpio_num_t)PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level((gpio_num_t)PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    
    ESP_LOGI(TAG, "Starting GC9A01 initialization");
    
    // Send all the commands
    int cmd = 0;
    while (gc9a01_init_cmds[cmd].databytes != 0xff) {
        gc9a01_send_cmd(gc9a01_init_cmds[cmd].cmd);
        gc9a01_send_data((uint8_t*)gc9a01_init_cmds[cmd].data, gc9a01_init_cmds[cmd].databytes&0x1F);
        if (gc9a01_init_cmds[cmd].databytes & 0x80) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        cmd++;
    }
    
    ESP_LOGI(TAG, "GC9A01 initialization complete");
}

static void gc9a01_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // Column Address Set
    gc9a01_send_cmd(0x2A);
    uint8_t data[4];
    data[0] = (x0 >> 8) & 0xFF;
    data[1] = x0 & 0xFF;
    data[2] = (x1 >> 8) & 0xFF;
    data[3] = x1 & 0xFF;
    gc9a01_send_data(data, 4);
    
    // Row Address Set
    gc9a01_send_cmd(0x2B);
    data[0] = (y0 >> 8) & 0xFF;
    data[1] = y0 & 0xFF;
    data[2] = (y1 >> 8) & 0xFF;
    data[3] = y1 & 0xFF;
    gc9a01_send_data(data, 4);
    
    // Memory Write
    gc9a01_send_cmd(0x2C);
}

static void gc9a01_fill_screen(uint16_t color)
{
    gc9a01_set_addr_window(0, 0, GC9A01_WIDTH-1, GC9A01_HEIGHT-1);
    
    // Prepare color data (RGB565 format, big-endian)
    uint8_t color_data[2];
    color_data[0] = (color >> 8) & 0xFF;
    color_data[1] = color & 0xFF;
    
    // Send the color data for each pixel
    for (int i = 0; i < GC9A01_WIDTH * GC9A01_HEIGHT; i++) {
        gc9a01_send_data(color_data, 2);
    }
}

// RGB565 color definitions
#define COLOR_BLACK   0x0000
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_YELLOW  0xFFE0
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting GC9A01 Display Test");
    
    // Initialize SPI and display
    gc9a01_spi_init();
    gc9a01_init();
    
    // Color cycling test
    uint16_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE, COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA, COLOR_BLACK};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    
    ESP_LOGI(TAG, "Starting color cycling test");
    
    while (1) {
        for (int i = 0; i < num_colors; i++) {
            ESP_LOGI(TAG, "Filling screen with color %d", i);
            gc9a01_fill_screen(colors[i]);
            vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait 1 second
        }
    }
}