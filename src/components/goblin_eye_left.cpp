// P32 Component: goblin_eye_left
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "p32_component_config.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "p32_eye_display.hpp"
#include "p32_web_client.hpp"
#include "p32_shared_state.hpp"
#include "driver/spi_master.h"
#include "driver/gpio.h"
// #include "testing_framework.hpp"  // Will implement later for Halloween goblin!
// Note: Using FrameProcessor.hpp for direct RGB565 pixel manipulation

static const char *TAG = "GOBLIN_EYE_LEFT";
static eye_display_t left_eye_display;
static bool web_client_initialized = false;

// SPI device handle for GC9A01 display
static spi_device_handle_t spi_display = NULL;
bool spi_bus_initialized = false;  // Shared with right eye (non-static)

// GC9A01 display dimensions
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

// GPIO pin assignments (verified wiring) - Cast to gpio_num_t for C++
#define PIN_MOSI (gpio_num_t)23   // SDA (updated to match physical wiring)
#define PIN_CLK  (gpio_num_t)18   // SCL (updated to match physical wiring)
#define PIN_CS   (gpio_num_t)5    // Chip Select (left eye - updated to match physical wiring)
#define PIN_DC   (gpio_num_t)21   // Data/Command (shared - updated to match physical wiring)
#define PIN_RST  (gpio_num_t)4    // Reset (shared - updated to match physical wiring)

// External shared state
extern p32_shared_state_t g_shared_state;

// Track current animation to avoid redundant switches
static eye_animation_t* current_animation = NULL;

// Helper: Send command to GC9A01
static void gc9a01_send_cmd(uint8_t cmd)
{
    if (spi_display == NULL) {
        ESP_LOGE(TAG, "❌ SPI display handle is NULL! Cannot send command 0x%02X", cmd);
        return;
    }
    
    gpio_set_level(PIN_DC, 0);  // Command mode
    spi_transaction_t trans = {};  // C++ requires proper initialization
    trans.length = 8;
    trans.tx_data[0] = cmd;
    trans.flags = SPI_TRANS_USE_TXDATA;
    esp_err_t ret = spi_device_polling_transmit(spi_display, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ SPI transmit failed: %s", esp_err_to_name(ret));
    }
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
    
    ESP_LOGI(TAG, "  Sending Sleep Out command (0x11)...");
    gc9a01_send_cmd(0x11);  // Sleep out
    ESP_LOGI(TAG, "  Waiting 120ms for display wake...");
    vTaskDelay(pdMS_TO_TICKS(120));
    
    ESP_LOGI(TAG, "  Sending Display On command (0x29)...");
    gc9a01_send_cmd(0x29);  // Display on
    vTaskDelay(pdMS_TO_TICKS(20));
    
    ESP_LOGI(TAG, "✅ GC9A01 basic initialization complete (no clear yet)");
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
// C linkage required for component dispatch system
extern "C" esp_err_t goblin_eye_left_init(void)
{
    esp_err_t ret;
    
    // Testing framework will be implemented later - for now, always do real hardware init
    
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
    dev_cfg.clock_speed_hz = 1 * 1000 * 1000;   // 1 MHz (slower = more reliable)
    dev_cfg.mode = 0;                            // SPI mode 0
    dev_cfg.spics_io_num = PIN_CS;              // CS pin for left eye
    dev_cfg.queue_size = 7;
    dev_cfg.flags = 0;
    dev_cfg.pre_cb = NULL;
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_display);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }
    if (spi_display == NULL) {
        ESP_LOGE(TAG, "❌ SPI device handle is NULL after add_device!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "✅ SPI device added (CS=%d, DC=%d, RST=%d)", PIN_CS, PIN_DC, PIN_RST);
    
    // Hardware reset sequence
    ESP_LOGI(TAG, "🔄 Resetting GC9A01 display...");
    gpio_set_level(PIN_RST, 0);
    ESP_LOGI(TAG, "  RST → LOW (reset asserted)");
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(PIN_RST, 1);
    ESP_LOGI(TAG, "  RST → HIGH (reset released)");
    vTaskDelay(pdMS_TO_TICKS(120));
    ESP_LOGI(TAG, "  Reset sequence complete");
    
    // Initialize GC9A01 display
    ESP_LOGI(TAG, "📡 Initializing GC9A01 display controller...");
    ret = gc9a01_init_display();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ Failed to initialize GC9A01");
        return ret;
    }
    ESP_LOGI(TAG, "✅ GC9A01 display controller initialized");
    
    // TEST: Flash display with colors to verify hardware
    ESP_LOGI(TAG, "🎨 LEFT EYE DISPLAY TEST - Watch for RED...");
    gc9a01_render_test_pattern(0xF800);  // RED
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "🎨 Changing to GREEN...");
    gc9a01_render_test_pattern(0x07E0);  // GREEN
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "🎨 Changing to BLUE...");
    gc9a01_render_test_pattern(0x001F);  // BLUE
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "🎨 Changing to WHITE...");
    gc9a01_render_test_pattern(0xFFFF);  // WHITE
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "🎨 Changing to BLACK...");
    gc9a01_render_test_pattern(0x0000);  // BLACK
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "✅ Display test complete - Did you see colors?");
    
    // Initialize animation system
    ret = eye_display_init(&left_eye_display, "LEFT EYE");
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Left eye display ready!");
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
    }
    
    return ret;
}

// Component action function - NO ARGUMENTS
// C linkage required for component dispatch system
extern "C" void goblin_eye_left_act(void)
{
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
    // Testing framework will be implemented later - for now, always do real hardware action
    
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
    
    // Full hardware version
    eye_display_update(&left_eye_display, current_time);
    
    // Render to actual SPI display
    // For now, show test pattern based on eye openness
    if (spi_display != NULL) {
        float openness = left_eye_display.current_frame.eye_openness;
        
        // Map eye openness to colors for visual feedback
        uint16_t color;
        if (openness > 0.8f) {
            color = 0xFFFF;  // White (wide open)
        } else if (openness > 0.5f) {
            color = 0x07E0;  // Green (open)
        } else if (openness > 0.2f) {
            color = 0xFFE0;  // Yellow (half open)
        } else {
            color = 0x0000;  // Black (closed)
        }
        
        gc9a01_render_test_pattern(color);
        
        ESP_LOGD(TAG, "Left eye rendered - openness: %.2f, color: 0x%04X", openness, color);
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS