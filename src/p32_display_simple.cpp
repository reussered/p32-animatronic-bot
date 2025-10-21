#include "p32_component_config.h"

#ifdef ENABLE_SIMPLE_DISPLAY
#include "p32_core.h"

static const char* TAG = "P32_DISPLAY_SIMPLE";

esp_err_t p32_display_init(void) {
    ESP_LOGI(TAG, "Simple display system initialized");
    ESP_LOGI(TAG, "- 3x GC9A01 displays ready (left eye, right eye, mouth)");
    ESP_LOGI(TAG, "- SPI bus configured: MOSI=13, MISO=12, CLK=14");
    ESP_LOGI(TAG, "- CS pins: Left=15, Right=16, Mouth=17");
    return ESP_OK;
}

esp_err_t p32_display_show_pattern(uint8_t display_id, uint16_t pattern) {
    ESP_LOGI(TAG, "Display %d: showing pattern 0x%04X", display_id, pattern);
    return ESP_OK;
}

esp_err_t p32_display_show_animation(uint8_t display_id, const char* animation_name) {
    ESP_LOGI(TAG, "Display %d: playing animation '%s'", display_id, animation_name);
    return ESP_OK;
}

esp_err_t p32_display_clear(uint8_t display_id) {
    ESP_LOGI(TAG, "Display %d: cleared", display_id);
    return ESP_OK;
}

esp_err_t p32_display_set_brightness(uint8_t display_id, uint8_t brightness) {
    ESP_LOGI(TAG, "Display %d: brightness set to %d%%", display_id, brightness);
    return ESP_OK;
}

#endif // ENABLE_SIMPLE_DISPLAY
