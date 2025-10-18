/**
 * Simple Core Test Component - C++ Implementation
 * 
 * Purpose: Verify ESP32-S3 can execute code and control GPIO
 * Hardware: Uses onboard LED (GPIO48 RGB or GPIO2 fallback)
 * Power: USB UART only - no external power needed
 * 
 * Expected Behavior:
 * - LED blinks every 1 second
 * - Serial output shows "BLINK ON" / "BLINK OFF" messages
 * - Loop count increments normally
 */

#include "components/simple_core_test.hpp"
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>

namespace {
    constexpr const char* TAG = "SIMPLE_TEST";
    
    // LED GPIO configuration
    // ESP32-S3-DevKitC-1 has RGB LED on GPIO48 (WS2812) - not suitable for simple on/off
    // Using GPIO2 instead - commonly available as simple LED on ESP32 boards
    constexpr gpio_num_t LED_GPIO = GPIO_NUM_2;
    
    bool s_led_state = false;
}

// Access global loop counter from shared state
extern uint64_t g_loopCount;

/**
 * Initialize the LED GPIO pin
 * NO ARGUMENTS - follows component function signature pattern
 */
extern "C" esp_err_t p32_comp_simple_core_test_init(void)
{
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "SIMPLE CORE TEST - Initializing...");
    ESP_LOGI(TAG, "========================================");
    
    // Configure GPIO2 as output (simple LED)
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << LED_GPIO);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO%d config failed!", LED_GPIO);
        return ret;
    }
    
    // Set initial state to OFF
    gpio_set_level(LED_GPIO, 0);
    s_led_state = false;
    
    ESP_LOGI(TAG, "✅ LED GPIO%d configured successfully", LED_GPIO);
    ESP_LOGI(TAG, "✅ Blink test will run every 1 second");
    ESP_LOGI(TAG, "✅ Watch for LED blinking and serial messages");
    ESP_LOGI(TAG, "========================================");
    
    return ESP_OK;
}

/**
 * Toggle LED state - called every 1 second (hitCount: 10)
 * NO ARGUMENTS - accesses g_loopCount from global shared state
 */
extern "C" void p32_comp_simple_core_test_act(void)
{
    // hitCount: 120000 → should execute every ~1 second
    
    static uint64_t last_loop = 0;
    static int64_t last_time_us = 0;
    
    uint64_t loops_elapsed = g_loopCount - last_loop;
    int64_t current_time_us = esp_timer_get_time();
    int64_t time_elapsed_us = current_time_us - last_time_us;
    
    last_loop = g_loopCount;
    last_time_us = current_time_us;
    
    // Toggle LED state
    s_led_state = !s_led_state;
    gpio_set_level(LED_GPIO, s_led_state ? 1 : 0);
    
    // Log with actual time measurement
    if (s_led_state) {
        printf("LED ON  - Loop: %llu (delta: %llu) - Time: %lld us (%.3f ms) - GPIO%d\n", 
               g_loopCount, loops_elapsed, time_elapsed_us, time_elapsed_us / 1000.0, LED_GPIO);
    } else {
        printf("LED OFF - Loop: %llu (delta: %llu) - Time: %lld us (%.3f ms) - GPIO%d\n", 
               g_loopCount, loops_elapsed, time_elapsed_us, time_elapsed_us / 1000.0, LED_GPIO);
    }
}
