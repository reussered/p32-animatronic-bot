// Simple GC9A01 Display Test
// Direct implementation for testing single display

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Pin definitions based on your configuration
#define PIN_NUM_MISO   (gpio_num_t)12
#define PIN_NUM_MOSI   (gpio_num_t)13  
#define PIN_NUM_CLK    (gpio_num_t)14
#define PIN_NUM_CS     (gpio_num_t)15
#define PIN_NUM_DC     (gpio_num_t)2   // Data/Command pin
#define PIN_NUM_RST    (gpio_num_t)4   // Reset pin

// Display constants
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

static const char *TAG = "DISPLAY_TEST";
static spi_device_handle_t spi_handle;

// Test colors
static uint16_t test_colors[] = {
    0xF800, // Red
    0x07E0, // Green  
    0x001F, // Blue
    0xFFE0, // Yellow
    0xF81F, // Magenta
    0x07FF, // Cyan
    0xFFFF  // White
};

// Send command to display
static void lcd_cmd(uint8_t cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    gpio_set_level(PIN_NUM_DC, 0); // Command mode
    spi_device_polling_transmit(spi_handle, &t);
}

// Send data to display
static void lcd_data(uint8_t *data, int len)
{
    if (len == 0) return;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    gpio_set_level(PIN_NUM_DC, 1); // Data mode
    spi_device_polling_transmit(spi_handle, &t);
}

// Initialize GC9A01 display
static void init_display(void)
{
    ESP_LOGI(TAG, "Initializing display...");
    
    // Reset display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Basic initialization commands
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
    
    // Pixel Format (RGB565)
    lcd_cmd(0x3A);
    uint8_t pixfmt = 0x05;
    lcd_data(&pixfmt, 1);
    
    // Display settings
    lcd_cmd(0x90);
    uint8_t gamma1[] = {0x08, 0x08, 0x08, 0x08};
    lcd_data(gamma1, 4);
    
    lcd_cmd(0xBD);
    uint8_t gamma2[] = {0x06};
    lcd_data(gamma2, 1);
    
    lcd_cmd(0xBC);
    uint8_t gamma3[] = {0x00};
    lcd_data(gamma3, 1);
    
    lcd_cmd(0xFF);
    uint8_t gamma4[] = {0x60, 0x01, 0x04};
    lcd_data(gamma4, 3);
    
    lcd_cmd(0xC3);
    uint8_t pwr1[] = {0x13};
    lcd_data(pwr1, 1);
    
    lcd_cmd(0xC4);
    uint8_t pwr2[] = {0x13};
    lcd_data(pwr2, 1);
    
    lcd_cmd(0xC9);
    uint8_t pwr3[] = {0x22};
    lcd_data(pwr3, 1);
    
    lcd_cmd(0xBE);
    uint8_t vcom[] = {0x11};
    lcd_data(vcom, 1);
    
    lcd_cmd(0xE1);
    uint8_t setn[] = {0x10, 0x0E};
    lcd_data(setn, 2);
    
    lcd_cmd(0xDF);
    uint8_t set1[] = {0x21, 0x0c, 0x02};
    lcd_data(set1, 3);
    
    // Gamma correction
    lcd_cmd(0xF0);
    uint8_t pgamma[] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
    lcd_data(pgamma, 6);
    
    lcd_cmd(0xF1);
    uint8_t ngamma[] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
    lcd_data(ngamma, 6);
    
    lcd_cmd(0xF2);
    uint8_t dgamma1[] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
    lcd_data(dgamma1, 6);
    
    lcd_cmd(0xF3);
    uint8_t dgamma2[] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
    lcd_data(dgamma2, 6);
    
    lcd_cmd(0xED);
    uint8_t power[] = {0x1B, 0x0B};
    lcd_data(power, 2);
    
    lcd_cmd(0xAE);
    uint8_t ae[] = {0x77};
    lcd_data(ae, 1);
    
    lcd_cmd(0xCD);
    uint8_t cd[] = {0x63};
    lcd_data(cd, 1);
    
    lcd_cmd(0x70);
    uint8_t area1[] = {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03};
    lcd_data(area1, 9);
    
    lcd_cmd(0xE8);
    uint8_t dtca[] = {0x34};
    lcd_data(dtca, 1);
    
    lcd_cmd(0x62);
    uint8_t area2[] = {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70};
    lcd_data(area2, 12);
    
    lcd_cmd(0x63);
    uint8_t area3[] = {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70};
    lcd_data(area3, 12);
    
    lcd_cmd(0x64);
    uint8_t area4[] = {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07};
    lcd_data(area4, 7);
    
    lcd_cmd(0x66);
    uint8_t area5[] = {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00};
    lcd_data(area5, 10);
    
    lcd_cmd(0x67);
    uint8_t area6[] = {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98};
    lcd_data(area6, 10);
    
    lcd_cmd(0x74);
    uint8_t area7[] = {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00};
    lcd_data(area7, 7);
    
    lcd_cmd(0x98);
    uint8_t area8[] = {0x3e, 0x07};
    lcd_data(area8, 2);
    
    lcd_cmd(0x35); // Tearing effect line on
    lcd_cmd(0x21); // Display inversion on
    
    lcd_cmd(0x11); // Sleep out
    vTaskDelay(120 / portTICK_PERIOD_MS);
    
    lcd_cmd(0x29); // Display on
    vTaskDelay(120 / portTICK_PERIOD_MS);
    
    ESP_LOGI(TAG, "Display initialized successfully!");
}

// Fill screen with solid color
static void fill_screen(uint16_t color)
{
    // Set column address (0-239)
    lcd_cmd(0x2A);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};
    lcd_data(col_data, 4);
    
    // Set row address (0-239)
    lcd_cmd(0x2B);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};
    lcd_data(row_data, 4);
    
    // Write to RAM
    lcd_cmd(0x2C);
    
    gpio_set_level(PIN_NUM_DC, 1); // Data mode
    
    // Send color data for entire screen
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
    {
        uint8_t color_bytes[] = {(uint8_t)((color >> 8) & 0xFF), (uint8_t)(color & 0xFF)};
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 16;
        t.tx_buffer = color_bytes;
        spi_device_polling_transmit(spi_handle, &t);
    }
}

extern "C" void simple_display_test_main(void)
{
    ESP_LOGI(TAG, "Starting GC9A01 Display Test");
    
    // Configure GPIO pins
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // Initialize SPI bus
    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = PIN_NUM_MISO;
    buscfg.mosi_io_num = PIN_NUM_MOSI;
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2;
    
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    
    // Configure SPI device
    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 10 * 1000 * 1000; // 10 MHz
    devcfg.mode = 0;
    devcfg.spics_io_num = PIN_NUM_CS;
    devcfg.queue_size = 7;
    
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle));
    
    // Initialize the display
    init_display();
    
    ESP_LOGI(TAG, "Starting color cycle test...");
    
    int color_index = 0;
    while (true)
    {
        // Change color
        fill_screen(test_colors[color_index]);
        ESP_LOGI(TAG, "Display showing color %d", color_index);
        
        // Next color
        color_index = (color_index + 1) % 7;
        
        // Wait 2 seconds
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}