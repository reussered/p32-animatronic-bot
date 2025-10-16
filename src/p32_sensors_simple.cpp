#include "p32_component_config.hpp"

#ifdef ENABLE_SIMPLE_SENSOR
#include "p32_core.hpp"
#include "esp_random.h"

static const char* TAG = "P32_SENSORS_SIMPLE";
static float last_distance = 25.0f; // Simulated distance reading

esp_err_t p32_sensor_init(void) {
    ESP_LOGI(TAG, "Simple sensor system initialized");
    ESP_LOGI(TAG, "- HC-SR04 ultrasonic sensor ready: TRIG=9, ECHO=10");
    ESP_LOGI(TAG, "- PIR motion detector ready");
    ESP_LOGI(TAG, "- Touch sensors ready");
    return ESP_OK;
}

float p32_sensor_read_distance(void* sensor) {
    // Simulate varying distance readings
    last_distance += (float)(esp_random() % 20 - 10) / 10.0f;
    if (last_distance < 5.0f) last_distance = 5.0f;
    if (last_distance > 100.0f) last_distance = 100.0f;
    
    ESP_LOGI(TAG, "Distance sensor reading: %.1f cm", last_distance);
    return last_distance;
}

bool p32_sensor_read_motion(void* sensor) {
    bool motion = (esp_random() % 100) < 10; // 10% chance of motion
    if (motion) {
        ESP_LOGI(TAG, "Motion detected!");
    }
    return motion;
}

bool p32_sensor_read_touch(uint8_t sensor_id) {
    bool touched = (esp_random() % 100) < 5; // 5% chance of touch
    if (touched) {
        ESP_LOGI(TAG, "Touch sensor %d activated!", sensor_id);
    }
    return touched;
}

esp_err_t p32_sensor_calibrate(uint8_t sensor_id) {
    ESP_LOGI(TAG, "Sensor %d calibrated", sensor_id);
    return ESP_OK;
}

#endif // ENABLE_SIMPLE_SENSOR
