// P32 Component: goblin_nose
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "p32_web_client.h"
#include <math.h>

static const char *TAG = "GOBLIN_NOSE";
static float simulated_distance = 25.0f; // Simulated sensor reading in cm

// Component: Proximity sensor monitoring
esp_err_t goblin_nose_init(void) {
#ifdef SIMPLE_TEST
    printf("INIT: goblin_nose - Proximity sensor monitoring\n");
    return ESP_OK;
#endif

    // Generic component initialization
    ESP_LOGI(TAG, "Component initialized");
    return ESP_OK;
}

// Component action function - executes every 100 loops
void goblin_nose_act(uint32_t loopCount) {
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000); // Convert to ms
    
#ifdef SIMPLE_TEST
    // Simulate varying distance readings for PC display
    simulated_distance = 20.0f + 10.0f * sinf((float)current_time / 2000.0f); // 20-30cm range
    
    // Send sensor data to PC display server every few loops
    web_client_send_sensor_data_for_component("NOSE_SENSOR", simulated_distance, loopCount);
    
    if (loopCount % 1000 == 0) {
        printf("ACT: goblin_nose - distance %.1fcm (loop %lu)\n", simulated_distance, loopCount);
    }
    return;
#endif

    // Real hardware: read HC-SR04 ultrasonic sensor (TODO: implement driver)
    // For now, send simulated data to PC display
    web_client_send_sensor_data_for_component("NOSE_SENSOR", simulated_distance, loopCount);
    
    ESP_LOGD(TAG, "Proximity sensor reading: %.1f cm - loop %lu", simulated_distance, loopCount);
}