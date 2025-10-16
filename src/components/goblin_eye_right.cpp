// P32 Component: goblin_eye_right
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
// Note: Using FrameProcessor.hpp for direct RGB565 pixel manipulation

static const char *TAG = "GOBLIN_EYE_RIGHT";
static eye_display_t right_eye_display;

// SPI device handle for GC9A01 display
static spi_device_handle_t spi_display = NULL;
extern bool spi_bus_initialized;  // Shared with left eye

// GC9A01 display dimensions
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240

// GPIO pin assignments (verified wiring)
#define PIN_MOSI (gpio_num_t)13   // SDA (shared)
#define PIN_CLK  (gpio_num_t)12   // SCL (shared - user confirmed)
#define PIN_CS   (gpio_num_t)16   // Chip Select (right eye - different from left!)
#define PIN_DC   (gpio_num_t)2    // Data/Command (shared)
#define PIN_RST  (gpio_num_t)21   // Reset (shared)

// External shared state
extern p32_shared_state_t g_shared_state;

// Track current animation to avoid redundant switches
static eye_animation_t* current_animation = NULL;

// Helper functions (same as left eye)
static void gc9a01_send_cmd(uint8_t cmd)
{
    if (spi_display == NULL) return;
    
    gpio_set_level(PIN_DC, 0);  // Command mode
    spi_transaction_t trans = {}; trans.length = 8; trans.flags = SPI_TRANS_USE_TXDATA;
    trans.tx_data[0] = cmd;
    spi_device_polling_transmit(spi_display, &trans);
}

static void gc9a01_send_data(uint8_t data)
{
    if (spi_display == NULL) return;
    
    gpio_set_level(PIN_DC, 1);  // Data mode
    spi_transaction_t trans = {}; trans.length = 8; trans.flags = SPI_TRANS_USE_TXDATA;
    trans.tx_data[0] = data;
    spi_device_polling_transmit(spi_display, &trans);
}

static esp_err_t gc9a01_init_display(void)
{
    // Basic initialization sequence for GC9A01
    gc9a01_send_cmd(0xEF);
    gc9a01_send_cmd(0xEB);
    gc9a01_send_data(0x14);
    
    gc9a01_send_cmd(0xFE);
    gc9a01_send_cmd(0xEF);
    
    gc9a01_send_cmd(0x36);
    gc9a01_send_data(0x48);
    
    gc9a01_send_cmd(0x3A);
    gc9a01_send_data(0x05);
    
    gc9a01_send_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    gc9a01_send_cmd(0x29);
    vTaskDelay(pdMS_TO_TICKS(20));
    
    ESP_LOGI(TAG, "GC9A01 display initialized");
    return ESP_OK;
}

static void gc9a01_render_test_pattern(uint16_t color)
{
    if (spi_display == NULL) return;
    
    gc9a01_send_cmd(0x2A);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0xEF);
    
    gc9a01_send_cmd(0x2B);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0x00);
    gc9a01_send_data(0xEF);
    
    gc9a01_send_cmd(0x2C);
    gpio_set_level(PIN_DC, 1);
    
    uint8_t color_bytes[2] = {static_cast<uint8_t>((color >> 8) & 0xFF), static_cast<uint8_t>(color & 0xFF)};
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        spi_transaction_t trans = {}; 
        trans.length = 16;  // 16 bits for RGB565
        trans.flags = SPI_TRANS_USE_TXDATA;
        trans.tx_data[0] = color_bytes[0];
        trans.tx_data[1] = color_bytes[1];
        spi_device_polling_transmit(spi_display, &trans);
    }
}

// Component: Right eye display animation
esp_err_t goblin_eye_right_init(void)
{
    esp_err_t ret;
    
#ifdef SIMPLE_TEST
    printf("INIT: goblin_eye_right - Right eye display animation\n");
    
    // Initialize right eye display  
    ret = eye_display_init(&right_eye_display, "RIGHT EYE");
    if (ret == ESP_OK) {
        // Start with curious look (different from left eye)
        eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
        printf("       Right eye display initialized with curious look\n");
    }
    return ret;
#endif

    // ===== REAL HARDWARE INITIALIZATION =====
    ESP_LOGI(TAG, "Initializing right eye GC9A01 display...");
    
    // GPIO already configured by left eye (shared DC and RST pins)
    // SPI bus already initialized by left eye
    
    // Add right eye display device to SPI bus
    spi_device_interface_config_t dev_cfg = {};  // C++ requires proper initialization
    dev_cfg.clock_speed_hz = 10 * 1000 * 1000;  // 10 MHz
    dev_cfg.mode = 0;                            // SPI mode 0
    dev_cfg.spics_io_num = PIN_CS;              // CS pin for RIGHT eye (GPIO16)
    dev_cfg.queue_size = 7;
    dev_cfg.flags = 0;
    dev_cfg.pre_cb = NULL;
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_display);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "SPI device added (CS=%d)", PIN_CS);
    
    // Initialize GC9A01 display (no reset needed, already done by left eye)
    ret = gc9a01_init_display();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize GC9A01");
        return ret;
    }
    
    // Initialize animation system
    ret = eye_display_init(&right_eye_display, "RIGHT EYE");
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Right eye display ready!");
        eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
    }
    
    return ret;
}

// Component action function - NO ARGUMENTS
void goblin_eye_right_act(void)
{
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
    // ===== PROXIMITY-BASED BEHAVIOR =====
    // Read distance from shared state (updated by nose sensor)
    uint8_t distance = g_shared_state.distance_cm;
    
    // Change animation based on face proximity (same logic as left eye)
    if (distance < 30 && current_animation != &goblin_curious_look_animation)
    {
        // Face is CLOSE → switch to CURIOUS
        ESP_LOGI(TAG, "👁️ Face detected at %d cm - CURIOUS MODE", distance);
        eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
        current_animation = &goblin_curious_look_animation;
    }
    else if (distance > 50 && current_animation != &goblin_blink_animation)
    {
        // Face is FAR → switch to IDLE BLINK
        ESP_LOGI(TAG, "👁️ Face moved away (%d cm) - IDLE MODE", distance);
        eye_display_start_animation(&right_eye_display, &goblin_blink_animation);
        current_animation = &goblin_blink_animation;
    }
    
#ifdef SIMPLE_TEST
    // Update and render the eye display animation
    eye_display_update(&right_eye_display, current_time);
    
    // Send animation data to PC display server (non-blocking)
    web_client_send_animation_data_for_component("RIGHT_EYE", &right_eye_display);
    
    // Render local display (called at hitCount rate)
    printf("\n=== RIGHT EYE ===\n");
    eye_display_render(&right_eye_display);
        
    // Start new animations when current finishes
    if (!right_eye_display.active) {
        // Different cycle pattern from left eye
        static int anim_cycle = 1; // Start offset
        switch (anim_cycle % 3) {
            case 0:
                printf("Starting CURIOUS LOOK animation...\n");
                eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
                break;
            case 1:
                printf("Starting BLINK animation...\n");
                eye_display_start_animation(&right_eye_display, &goblin_blink_animation);
                break;
            case 2:
                printf("Starting ANGRY STARE animation...\n");
                eye_display_start_animation(&right_eye_display, &goblin_angry_stare_animation);
                break;
        }
        anim_cycle++;
    }
    return;
#endif

    // Full hardware version
    eye_display_update(&right_eye_display, current_time);
    
    // Render to actual SPI display
    // For now, show test pattern based on eye openness
    if (spi_display != NULL) {
        float openness = right_eye_display.current_frame.eye_openness;
        
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
        
        ESP_LOGD(TAG, "Right eye rendered - openness: %.2f, color: 0x%04X", openness, color);
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS

