// P32 Component: goblin_nose
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "p32_component_config.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "p32_web_client.h"
#include "p32_shared_state.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include <math.h>

static const char *TAG = "GOBLIN_NOSE";
static float simulated_distance = 25.0f; // Simulated sensor reading in cm

// External shared state
extern p32_shared_state_t g_shared_state;

// Component: Proximity sensor monitoring
esp_err_t goblin_nose_init(void)
{
#ifdef SIMPLE_TEST
    printf("INIT: goblin_nose - Proximity sensor monitoring\n");
    return ESP_OK;
#endif

    ESP_LOGI(TAG, "Initializing nose sensor (HC-SR04)");
    
    // Configure GPIO32 as output (trigger)
    gpio_config_t trigger_conf = {
        .pin_bit_mask = (1ULL << 32),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&trigger_conf));
    
    // Configure GPIO33 as input (echo)
    gpio_config_t echo_conf = {
        .pin_bit_mask = (1ULL << 33),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&echo_conf));
    
    // Set trigger LOW initially
    gpio_set_level(32, 0);
    
    ESP_LOGI(TAG, "HC-SR04 sensor initialized (GPIO32=trigger, GPIO33=echo)");
    return ESP_OK;
}

// Component action function - NO ARGUMENTS
void goblin_nose_act(void)
{
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
#ifdef SIMPLE_TEST
    // Simulate varying distance readings for PC display
    simulated_distance = 20.0f + 10.0f * sinf((float)current_time / 2000.0f); // 20-30cm range
    
    // Send sensor data to PC display server (called at hitCount rate)
    web_client_send_sensor_data_for_component("NOSE_SENSOR", simulated_distance, 0);
    printf("ACT: goblin_nose - distance %.1fcm\n", simulated_distance);
    return;
#endif

    // Real hardware: read HC-SR04 ultrasonic sensor
    // GPIO4 = trigger, GPIO5 = echo
    
    // Send 10µs trigger pulse
    gpio_set_level(4, 1);
    ets_delay_us(10);
    gpio_set_level(4, 0);
    
    // Wait for echo to go HIGH (with timeout)
    uint32_t timeout_us = 30000; // 30ms max (about 5m range)
    uint32_t start_time = esp_timer_get_time();
    
    while (gpio_get_level(5) == 0)  // GPIO5 = ECHO pin
    {
        if ((esp_timer_get_time() - start_time) > timeout_us)
        {
            ESP_LOGW(TAG, "HC-SR04 timeout waiting for echo start");
            g_shared_state.distance_cm = 255; // Max distance (out of range)
            return;
        }
    }
    
    uint32_t echo_start = esp_timer_get_time();
    
    // Wait for echo to go LOW
    while (gpio_get_level(5) == 1)  // GPIO5 = ECHO pin
    {
        if ((esp_timer_get_time() - start_time) > timeout_us)
        {
            ESP_LOGW(TAG, "HC-SR04 timeout during echo");
            g_shared_state.distance_cm = 255; // Max distance (out of range)
            return;
        }
    }
    
    uint32_t echo_end = esp_timer_get_time();
    uint32_t duration_us = echo_end - echo_start;
    
    // Calculate distance: duration * speed_of_sound / 2
    // Speed of sound = 343 m/s = 0.0343 cm/µs
    float distance_cm = (duration_us * 0.0343f) / 2.0f;
    
    // Clamp to uint8_t range (0-255 cm, about 8 feet max)
    if (distance_cm > 255.0f) distance_cm = 255.0f;
    if (distance_cm < 2.0f) distance_cm = 2.0f; // HC-SR04 minimum range
    
    // Update shared state (will be mesh-synchronized to other subsystems)
    g_shared_state.distance_cm = (uint8_t)distance_cm;
    
    ESP_LOGD(TAG, "Distance: %d cm", g_shared_state.distance_cm);
}

#endif // ENABLE_GOBLIN_COMPONENTS