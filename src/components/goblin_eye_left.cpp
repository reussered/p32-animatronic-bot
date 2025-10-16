// P32 Component: goblin_eye_left
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "p32_component_config.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "p32_eye_display.hpp"
#include "p32_eye_graphics.hpp"
#include "p32_web_client.hpp"
#include "p32_shared_state.hpp"
#include "driver/spi_master.h"
#include "driver/gpio.h"

static const char *TAG = "GOBLIN_EYE_LEFT";
static eye_display_t left_eye_display;
static eye_graphics_config_t left_eye_config;
static bool web_client_initialized = false;

// Framebuffer for rendering (240x240 RGB565 = 115,200 bytes)
static uint16_t* left_eye_framebuffer = NULL;

// SPI device handle for GC9A01 display
static spi_device_handle_t spi_display = NULL;
bool spi_bus_initialized = false;  // Shared with right eye (non-static)

// GC9A01 display dimensions
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

// GPIO pin assignments (verified wiring) - Cast to gpio_num_t for C++
#define PIN_MOSI (gpio_num_t)13   // SDA
#define PIN_CLK  (gpio_num_t)12   // SCL (user confirmed)
#define PIN_CS   (gpio_num_t)15   // Chip Select (left eye)
#define PIN_DC   (gpio_num_t)2    // Data/Command (shared)
#define PIN_RST  (gpio_num_t)21   // Reset (shared)

// External shared state
extern p32_shared_state_t g_shared_state;

// Track current animation to avoid redundant switches
static eye_animation_t* current_animation = NULL;

// Helper: Send command to GC9A01
static void gc9a01_send_cmd(uint8_t cmd)
{
    if (spi_display == NULL) return;
    
    gpio_set_level(PIN_DC, 0);  // Command mode
    spi_transaction_t trans = {};  // C++ requires proper initialization
    trans.length = 8;
    trans.tx_data[0] = cmd;
    trans.flags = SPI_TRANS_USE_TXDATA;
    spi_device_polling_transmit(spi_display, &trans);
}

// Helper: Send data to GC9A01
static void gc9a01_send_data(uint8_t data)
{
    if (spi_display == NULL) return;
    
    gpio_set_level(PIN_DC, 1);  // Data mode
    spi_transaction_t trans = {};  // C++ requires proper initialization
    trans.length = 8;
    trans.tx_data[0] = data;
    trans.flags = SPI_TRANS_USE_TXDATA;
    spi_device_polling_transmit(spi_display, &trans);
}

// Helper: Initialize GC9A01 display
static esp_err_t gc9a01_init_display(void)
{
    // Basic initialization sequence for GC9A01
    gc9a01_send_cmd(0xEF);  // Inter register enable 2
    gc9a01_send_cmd(0xEB);
    gc9a01_send_data(0x14);
    
    gc9a01_send_cmd(0xFE);  // Inter register enable 1
    gc9a01_send_cmd(0xEF);
    
    gc9a01_send_cmd(0x36);  // Memory access control
    gc9a01_send_data(0x48); // Row/column exchange, BGR
    
    gc9a01_send_cmd(0x3A);  // Pixel format
    gc9a01_send_data(0x05); // 16-bit RGB565
    
    gc9a01_send_cmd(0x11);  // Sleep out
    vTaskDelay(pdMS_TO_TICKS(120));
    
    gc9a01_send_cmd(0x29);  // Display on
    vTaskDelay(pdMS_TO_TICKS(20));
    
    // Clear screen to black
    gc9a01_send_cmd(0x2C);  // Memory write
    gpio_set_level(PIN_DC, 1);  // Data mode
    uint16_t black = 0x0000;
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        spi_transaction_t trans = {};  // C++ requires proper initialization
        trans.length = 16;
        trans.flags = SPI_TRANS_USE_TXDATA;
        trans.tx_data[0] = 0;
        trans.tx_data[1] = 0;
        spi_device_polling_transmit(spi_display, &trans);
    }
    
    ESP_LOGI(TAG, "GC9A01 display initialized and cleared");
    return ESP_OK;
}

// Helper: Render simple test pattern (for debugging)
static void gc9a01_render_test_pattern(uint16_t color)
{
    if (spi_display == NULL) return;
    
    // Set full screen window
    gc9a01_send_cmd(0x2A);  // Column address
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0xEF);  // 239
    
    gc9a01_send_cmd(0x2B);  // Row address
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0xEF);  // 239
    
    gc9a01_send_cmd(0x2C);  // Memory write
    gpio_set_level(PIN_DC, 1);  // Data mode
    
    // Send solid color to entire display
    uint8_t color_bytes[2] = {static_cast<uint8_t>((color >> 8) & 0xFF), static_cast<uint8_t>(color & 0xFF)};
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        spi_transaction_t trans = {};  // C++ requires proper initialization
        trans.length = 16;
        trans.flags = SPI_TRANS_USE_TXDATA;
        trans.tx_data[0] = color_bytes[0];
        trans.tx_data[1] = color_bytes[1];
        spi_device_polling_transmit(spi_display, &trans);
    }
}

// Component: Left eye display animation
esp_err_t goblin_eye_left_init(void)
{
    esp_err_t ret;
    
#ifdef SIMPLE_TEST
    printf("INIT: goblin_eye_left - Left eye display animation\n");
    
    // Initialize left eye display
    ret = eye_display_init(&left_eye_display, "LEFT EYE");
    if (ret == ESP_OK) {
        // Start with a blink animation
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
        printf("       Left eye display initialized with blink animation\n");
        
        // Initialize web client streaming on first eye init
        if (!web_client_initialized) {
            web_client_init();
            web_client_initialized = true;
            printf("       Web client streaming to PC enabled (HTTP JSON)\n");
        }
    }
    return ret;
#endif

    // ===== REAL HARDWARE INITIALIZATION =====
    ESP_LOGI(TAG, "Initializing left eye GC9A01 display...");
    
    // Configure GPIO pins for DC and RST
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_DC) | (1ULL << PIN_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Initialize SPI bus (only once, shared by both eyes)
    if (!spi_bus_initialized) {
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = PIN_MOSI,
            .miso_io_num = -1,  // No MISO needed for display
            .sclk_io_num = PIN_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2  // RGB565
        };
        ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
            return ret;
        }
        spi_bus_initialized = true;
        ESP_LOGI(TAG, "SPI bus initialized (MOSI=%d, CLK=%d)", PIN_MOSI, PIN_CLK);
    }
    
    // Add left eye display device to SPI bus
    spi_device_interface_config_t dev_cfg = {};  // C++ requires proper initialization
    dev_cfg.clock_speed_hz = 10 * 1000 * 1000;  // 10 MHz
    dev_cfg.mode = 0;                            // SPI mode 0
    dev_cfg.spics_io_num = PIN_CS;              // CS pin for left eye
    dev_cfg.queue_size = 7;
    dev_cfg.flags = 0;
    dev_cfg.pre_cb = NULL;
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_display);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "SPI device added (CS=%d, DC=%d, RST=%d)", PIN_CS, PIN_DC, PIN_RST);
    
    // Hardware reset sequence
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // Initialize GC9A01 display
    ret = gc9a01_init_display();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize GC9A01");
        return ret;
    }
    
    // Allocate framebuffer for eye rendering
    left_eye_framebuffer = (uint16_t*)malloc(EYE_DISPLAY_WIDTH * EYE_DISPLAY_HEIGHT * sizeof(uint16_t));
    if (left_eye_framebuffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate framebuffer (%d bytes)",
                 EYE_DISPLAY_WIDTH * EYE_DISPLAY_HEIGHT * 2);
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "Allocated %d KB framebuffer", 
             (EYE_DISPLAY_WIDTH * EYE_DISPLAY_HEIGHT * 2) / 1024);
    
    // Initialize eye graphics configuration
    eye_graphics_init_default(&left_eye_config);
    
    // Initialize animation system
    ret = eye_display_init(&left_eye_display, "LEFT EYE");
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Left eye display ready with pixel graphics!");
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
    }
    
    return ret;
}

// Component action function - NO ARGUMENTS
void goblin_eye_left_act(void)
{
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
    // ===== PROXIMITY-BASED BEHAVIOR =====
    // Read distance from shared state (updated by nose sensor)
    uint8_t distance = g_shared_state.distance_cm;
    
    // Change animation based on face proximity
    if (distance < 30 && current_animation != &goblin_curious_look_animation)
    {
        // Face is CLOSE → switch to CURIOUS
        ESP_LOGI(TAG, "👁️ Face detected at %d cm - CURIOUS MODE", distance);
        eye_display_start_animation(&left_eye_display, &goblin_curious_look_animation);
        current_animation = &goblin_curious_look_animation;
    }
    else if (distance > 50 && current_animation != &goblin_blink_animation)
    {
        // Face is FAR → switch to IDLE BLINK
        ESP_LOGI(TAG, "👁️ Face moved away (%d cm) - IDLE MODE", distance);
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
        current_animation = &goblin_blink_animation;
    }
    
#ifdef SIMPLE_TEST
    // Update and render the eye display animation
    eye_display_update(&left_eye_display, current_time);
    
    // Send animation data to PC display server (non-blocking)
    web_client_send_animation_data_for_component("LEFT_EYE", &left_eye_display);
    
    // Render local display (called at hitCount rate)
    printf("\n=== LEFT EYE ===\n");
    eye_display_render(&left_eye_display);
        
    // Start new animations periodically for demo
    if (!left_eye_display.active) {
        // Cycle through animations
        static int anim_cycle = 0;
        switch (anim_cycle % 3) {
            case 0:
                printf("Starting BLINK animation...\n");
                eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
                break;
            case 1:
                printf("Starting ANGRY STARE animation...\n");
                eye_display_start_animation(&left_eye_display, &goblin_angry_stare_animation);
                break;
            case 2:
                printf("Starting CURIOUS LOOK animation...\n");
                eye_display_start_animation(&left_eye_display, &goblin_curious_look_animation);
                break;
        }
        anim_cycle++;
    }
    return;
#endif

    // Full hardware version
    eye_display_update(&left_eye_display, current_time);
    
    // Render pixel-perfect eye graphics to framebuffer
    if (spi_display != NULL && left_eye_framebuffer != NULL) {
        // Render current animation frame to framebuffer
        eye_graphics_render_frame(left_eye_framebuffer, 
                                   &left_eye_config,
                                   &left_eye_display.current_frame);
        
        // Set full screen window for GC9A01
        gc9a01_send_cmd(0x2A);  // Column address
        gc9a01_send_data(0x00);
        gc9a01_send_data(0x00);
        gc9a01_send_data(0x00);
        gc9a01_send_data(0xEF);  // 239
        
        gc9a01_send_cmd(0x2B);  // Row address
        gc9a01_send_data(0x00);
        gc9a01_send_data(0x00);
        gc9a01_send_data(0x00);
        gc9a01_send_data(0xEF);  // 239
        
        gc9a01_send_cmd(0x2C);  // Memory write
        gpio_set_level(PIN_DC, 1);  // Data mode
        
        // Send framebuffer to display via SPI
        // For better performance, send in chunks
        const int CHUNK_SIZE = 4096;  // 2KB chunks
        int total_pixels = DISPLAY_WIDTH * DISPLAY_HEIGHT;
        int bytes_per_pixel = 2;  // RGB565
        
        for (int offset = 0; offset < total_pixels; offset += CHUNK_SIZE) {
            int pixels_to_send = (offset + CHUNK_SIZE > total_pixels) ? 
                                 (total_pixels - offset) : CHUNK_SIZE;
            
            spi_transaction_t trans = {};
            trans.length = pixels_to_send * 16;  // bits
            trans.tx_buffer = &left_eye_framebuffer[offset];
            trans.flags = 0;  // Use tx_buffer, not tx_data
            
            esp_err_t ret = spi_device_polling_transmit(spi_display, &trans);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "SPI transmit failed at offset %d", offset);
                break;
            }
        }
        
        ESP_LOGD(TAG, "Left eye rendered - openness: %.2f, pupil: %.2f, expr: %d",
                 left_eye_display.current_frame.eye_openness,
                 left_eye_display.current_frame.pupil_size,
                 left_eye_display.current_frame.expression);
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS