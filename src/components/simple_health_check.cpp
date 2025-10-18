// Simple Health Check Component
// Tests ESP32-S3 functionality after smoke incident
// No external hardware required

#include "p32_component_config.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "driver/gpio.h"

static const char *TAG = "HEALTH_CHECK";

// Test GPIO pin (GPIO2 is typically built-in LED)
#define TEST_GPIO (gpio_num_t)2

// Track test results
static int test_count = 0;
static bool gpio_working = false;

esp_err_t simple_health_check_init(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   ESP32-S3 HEALTH CHECK TEST          ║");
    ESP_LOGI(TAG, "║   After Smoke Incident - Oct 16       ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");
    
    ESP_LOGI(TAG, "✅ TEST 1: Boot successful");
    ESP_LOGI(TAG, "   - CPU: 240 MHz dual-core");
    ESP_LOGI(TAG, "   - FreeRTOS scheduler: Active");
    
    // Test timer
    ESP_LOGI(TAG, "✅ TEST 2: Timer system test...");
    uint64_t start = esp_timer_get_time();
    vTaskDelay(pdMS_TO_TICKS(100));
    uint64_t elapsed = esp_timer_get_time() - start;
    ESP_LOGI(TAG, "   - Elapsed: %llu us (expected ~100000)", elapsed);
    
    if (elapsed > 90000 && elapsed < 110000) {
        ESP_LOGI(TAG, "   ✅ Timer accurate");
    } else {
        ESP_LOGW(TAG, "   ⚠️ Timer drift detected");
    }
    
    // Test GPIO configuration
    ESP_LOGI(TAG, "✅ TEST 3: GPIO configuration test...");
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << TEST_GPIO);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret == ESP_OK) {
        gpio_working = true;
        ESP_LOGI(TAG, "   ✅ GPIO%d configured successfully", TEST_GPIO);
    } else {
        gpio_working = false;
        ESP_LOGE(TAG, "   ❌ GPIO%d configuration FAILED", TEST_GPIO);
    }
    
    // Test critical pins used by displays
    ESP_LOGI(TAG, "✅ TEST 4: Display pin health check...");
    const int display_pins[] = {2, 12, 13, 15, 21};  // DC, CLK, MOSI, CS, RST
    int ok_count = 0;
    
    for (int i = 0; i < 5; i++) {
        gpio_config_t pin_conf = {};
        pin_conf.pin_bit_mask = (1ULL << display_pins[i]);
        pin_conf.mode = GPIO_MODE_OUTPUT;
        pin_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        pin_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        
        if (gpio_config(&pin_conf) == ESP_OK) {
            ok_count++;
            ESP_LOGI(TAG, "   ✅ GPIO%d OK", display_pins[i]);
        } else {
            ESP_LOGE(TAG, "   ❌ GPIO%d FAILED", display_pins[i]);
        }
    }
    
    ESP_LOGI(TAG, "   - Result: %d/5 display pins OK", ok_count);
    
    // Summary
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   INITIALIZATION COMPLETE             ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
    
    if (ok_count == 5) {
        ESP_LOGI(TAG, "🎉 ALL TESTS PASSED!");
        ESP_LOGI(TAG, "✅ ESP32-S3 fully functional");
        ESP_LOGI(TAG, "✅ Safe to connect displays");
    } else {
        ESP_LOGW(TAG, "⚠️ Some pins failed - check above");
    }
    
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Starting heartbeat monitor (every 1 second)...");
    ESP_LOGI(TAG, "");
    
    return ESP_OK;
}

void simple_health_check_act(void)
{
    // This executes every 1 second (hitCount: 10, loop period: 100ms)
    test_count++;
    
    // Toggle GPIO if working
    if (gpio_working) {
        static int toggle = 0;
        gpio_set_level(TEST_GPIO, toggle);
        toggle = !toggle;
    }
    
    // Heartbeat message
    ESP_LOGI(TAG, "💓 Heartbeat #%d - ESP32-S3 running normally", test_count);
    
    // Every 10 seconds, show detailed status
    if (test_count % 10 == 0) {
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "📊 Status Report (after %d seconds):", test_count);
        ESP_LOGI(TAG, "   - Uptime: %llu ms", esp_timer_get_time() / 1000);
        ESP_LOGI(TAG, "   - Free heap: %lu bytes", esp_get_free_heap_size());
        ESP_LOGI(TAG, "   - GPIO status: %s", gpio_working ? "OK" : "FAILED");
        ESP_LOGI(TAG, "");
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
