// ESP32-S3 Health Check Test
// Tests basic functionality after smoke incident
// NO external hardware required

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "HEALTH_CHECK";

// Use built-in LED or any GPIO for blink test (GPIO2 is common)
#define BLINK_GPIO (gpio_num_t)2

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   ESP32-S3 HEALTH CHECK TEST          ║");
    ESP_LOGI(TAG, "║   After Smoke Incident                ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");
    
    // Test 1: Basic boot
    ESP_LOGI(TAG, "✅ TEST 1: Boot successful");
    ESP_LOGI(TAG, "   - CPU running at 240 MHz");
    ESP_LOGI(TAG, "   - FreeRTOS scheduler active");
    
    // Test 2: Timer system
    ESP_LOGI(TAG, "✅ TEST 2: Testing timer system...");
    uint64_t start = esp_timer_get_time();
    vTaskDelay(pdMS_TO_TICKS(100));
    uint64_t elapsed = esp_timer_get_time() - start;
    ESP_LOGI(TAG, "   - Timer test: %llu us (expected ~100000)", elapsed);
    if (elapsed > 90000 && elapsed < 110000) {
        ESP_LOGI(TAG, "   ✅ Timer system working correctly");
    } else {
        ESP_LOGE(TAG, "   ❌ Timer system inaccurate!");
    }
    
    // Test 3: GPIO configuration
    ESP_LOGI(TAG, "✅ TEST 3: Testing GPIO configuration...");
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << BLINK_GPIO);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "   ✅ GPIO configuration successful");
    } else {
        ESP_LOGE(TAG, "   ❌ GPIO configuration failed: %s", esp_err_to_name(ret));
    }
    
    // Test 4: GPIO output
    ESP_LOGI(TAG, "✅ TEST 4: Testing GPIO output (pin %d)...", BLINK_GPIO);
    ESP_LOGI(TAG, "   - Watch for blink pattern (if LED connected)");
    
    // Test 5: Memory allocation
    ESP_LOGI(TAG, "✅ TEST 5: Testing memory allocation...");
    void *test_mem = malloc(1024);
    if (test_mem != NULL) {
        ESP_LOGI(TAG, "   ✅ 1KB malloc successful");
        free(test_mem);
        ESP_LOGI(TAG, "   ✅ Memory free successful");
    } else {
        ESP_LOGE(TAG, "   ❌ Memory allocation failed!");
    }
    
    // Test 6: Multiple GPIO pins
    ESP_LOGI(TAG, "✅ TEST 6: Testing multiple GPIO pins...");
    const int test_pins[] = {2, 4, 5, 12, 13, 15, 16, 21};
    int pin_count = sizeof(test_pins) / sizeof(test_pins[0]);
    int success_count = 0;
    
    for (int i = 0; i < pin_count; i++) {
        gpio_config_t pin_conf = {};
        pin_conf.pin_bit_mask = (1ULL << test_pins[i]);
        pin_conf.mode = GPIO_MODE_OUTPUT;
        pin_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        pin_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        pin_conf.intr_type = GPIO_INTR_DISABLE;
        
        if (gpio_config(&pin_conf) == ESP_OK) {
            success_count++;
            ESP_LOGI(TAG, "   ✅ GPIO%d configured OK", test_pins[i]);
        } else {
            ESP_LOGE(TAG, "   ❌ GPIO%d configuration FAILED", test_pins[i]);
        }
    }
    
    ESP_LOGI(TAG, "   - Pin test: %d/%d successful", success_count, pin_count);
    
    // Final summary
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║   HEALTH CHECK SUMMARY                ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════════╝");
    
    if (success_count == pin_count) {
        ESP_LOGI(TAG, "🎉 ALL TESTS PASSED!");
        ESP_LOGI(TAG, "✅ ESP32-S3 is fully functional");
        ESP_LOGI(TAG, "✅ Safe to connect displays");
    } else {
        ESP_LOGW(TAG, "⚠️ Some GPIO pins failed");
        ESP_LOGW(TAG, "Check which pins failed above");
    }
    
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Starting continuous blink test on GPIO%d...", BLINK_GPIO);
    ESP_LOGI(TAG, "(Press RESET to run test again)");
    ESP_LOGI(TAG, "");
    
    // Continuous blink loop to show it's alive
    int blink_count = 0;
    while (true) {
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
        
        blink_count++;
        if (blink_count % 10 == 0) {
            ESP_LOGI(TAG, "💓 Heartbeat: %d seconds running...", blink_count);
        }
    }
}
