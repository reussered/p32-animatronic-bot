#include "p32_component_config.hpp"

#ifdef ENABLE_COMPLEX_SENSORS
#include "p32_sensors.hpp"
#include "p32_audio.hpp"
#include "p32_display.hpp"

static const char* TAG = "P32_SENSORS";
static p32_sensor_system_t g_sensor_system = {0};

// Goblin sensor-to-mood mappings
const p32_sensor_mood_mapping_t p32_goblin_sensor_mappings[] = {
    {TRIGGER_PROXIMITY_CLOSE, MOOD_CURIOSITY, 3000, 7},
    {TRIGGER_PROXIMITY_FAR, MOOD_CONTENTMENT, 0, 3},
    {TRIGGER_MOTION_DETECTED, MOOD_ANGER, 2000, 6},
    {TRIGGER_TOUCH_HEAD, MOOD_AFFECTION, 5000, 5},
    {TRIGGER_TOUCH_FACE, MOOD_HAPPINESS, 3000, 4},
    {0, 0, 0, 0} // Terminator
};

esp_err_t p32_sensor_init(void) {
    ESP_LOGI(TAG, "Initializing sensor system...");
    
    memset(&g_sensor_system, 0, sizeof(p32_sensor_system_t));
    
    // Create sensor event queue
    g_sensor_system.sensor_event_queue = xQueueCreate(20, sizeof(p32_sensor_event_t));
    if (g_sensor_system.sensor_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create sensor event queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Configure proximity sensor (nose) from system config
    extern p32_system_t g_p32_system;
    
    for (int i = 0; i < g_p32_system.current_bot.component_count; i++) {
        p32_component_config_t* comp = &g_p32_system.current_bot.components[i];
        
        if (strcmp(comp->component_id, "goblin_nose") == 0) {
            ESP_ERROR_CHECK(p32_sensor_setup_ultrasonic(&g_sensor_system.proximity_nose, comp));
            break;
        }
    }
    
    g_sensor_system.sensors_enabled = true;
    
    // Create sensor reading task
    BaseType_t ret = xTaskCreate(p32_sensor_task, "p32_sensors", 4096, NULL, 4, &g_sensor_system.sensor_task);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create sensor task");
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Sensor system initialized successfully");
    return ESP_OK;
}

void p32_sensor_act(void) {
    extern uint64_t loopCount;
    
    // Read sensors every 10 loops (about 5Hz if main loop is 50Hz)
    if (loopCount % 10 == 0) {
        if (g_sensor_system.sensors_enabled) {
            // Read ultrasonic distance
            float distance = p32_sensor_read_distance(&g_sensor_system.proximity_nose);
            if (distance > 0) {
                // Check for proximity behavior triggers
                p32_sensor_check_proximity_behavior(distance);
            }
            
            // Read motion sensors
            bool motion = p32_sensor_read_motion(&g_sensor_system.motion_detector);
            if (motion) {
                p32_sensor_check_motion_behavior(motion);
            }
        }
    }
}

esp_err_t p32_sensor_setup_ultrasonic(p32_ultrasonic_sensor_t* sensor, const p32_component_config_t* config) {
    ESP_LOGI(TAG, "Setting up ultrasonic sensor: %s", config->component_id);
    
    strncpy(sensor->component_id, config->component_id, sizeof(sensor->component_id));
    sensor->trigger_pin = config->gpio_pins[0];  // GPIO 32
    sensor->echo_pin = config->gpio_pins[1];     // GPIO 33
    sensor->distance_cm = 0.0f;
    sensor->last_reading_ms = 0;
    
    // Configure GPIO pins
    gpio_config_t trigger_conf = {
        .pin_bit_mask = (1ULL << sensor->trigger_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&trigger_conf));
    
    gpio_config_t echo_conf = {
        .pin_bit_mask = (1ULL << sensor->echo_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&echo_conf));
    
    // Set trigger pin low initially
    gpio_set_level(sensor->trigger_pin, 0);
    
    sensor->initialized = true;
    ESP_LOGI(TAG, "Ultrasonic sensor %s setup complete", config->component_id);
    
    return ESP_OK;
}

float p32_sensor_read_distance(p32_ultrasonic_sensor_t* sensor) {
    if (!sensor->initialized) {
        return -1.0f;
    }
    
    // Send 10us trigger pulse
    gpio_set_level(sensor->trigger_pin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(sensor->trigger_pin, 0);
    
    // Wait for echo pin to go high (start of echo)
    uint32_t timeout_us = 30000; // 30ms timeout
    uint32_t start_time = esp_timer_get_time();
    
    while (gpio_get_level(sensor->echo_pin) == 0) {
        if ((esp_timer_get_time() - start_time) > timeout_us) {
            ESP_LOGW(TAG, "Ultrasonic sensor timeout waiting for echo start");
            return -1.0f;
        }
    }
    
    // Measure echo pulse duration
    uint32_t echo_start = esp_timer_get_time();
    
    while (gpio_get_level(sensor->echo_pin) == 1) {
        if ((esp_timer_get_time() - start_time) > timeout_us) {
            ESP_LOGW(TAG, "Ultrasonic sensor timeout during echo");
            return -1.0f;
        }
    }
    
    uint32_t echo_end = esp_timer_get_time();
    uint32_t echo_duration = echo_end - echo_start;
    
    // Calculate distance (speed of sound = 343 m/s = 0.0343 cm/us)
    // Distance = (echo_duration * 0.0343) / 2
    float distance = (echo_duration * 0.01715f);
    
    sensor->distance_cm = distance;
    sensor->last_reading_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    return distance;
}

void p32_sensor_task(void* parameters) {
    ESP_LOGI(TAG, "Sensor task started");
    
    const TickType_t reading_interval = pdMS_TO_TICKS(200); // Read sensors every 200ms
    TickType_t last_wake_time = xTaskGetTickCount();
    
    while (1) {
        vTaskDelayUntil(&last_wake_time, reading_interval);
        
        if (!g_sensor_system.sensors_enabled) {
            continue;
        }
        
        // Read proximity sensor
        if (g_sensor_system.proximity_nose.initialized) {
            float distance = p32_sensor_read_distance(&g_sensor_system.proximity_nose);
            
            if (distance > 0) {
                // Check for behavioral triggers
                p32_sensor_check_proximity_behavior(distance);
                
                // Create sensor event
                p32_sensor_event_t event = {
                    .type = SENSOR_ULTRASONIC,
                    .value = distance,
                    .timestamp_ms = xTaskGetTickCount() * portTICK_PERIOD_MS
                };
                strncpy(event.sensor_id, g_sensor_system.proximity_nose.component_id, 
                        sizeof(event.sensor_id));
                
                // Queue event for processing
                if (xQueueSend(g_sensor_system.sensor_event_queue, &event, 0) != pdTRUE) {
                    ESP_LOGW(TAG, "Sensor event queue full");
                }
                
                ESP_LOGD(TAG, "Proximity: %.2f cm", distance);
            }
        }
    }
}

esp_err_t p32_sensor_check_proximity_behavior(float distance) {
    static float last_distance = 0.0f;
    static p32_mood_t last_triggered_mood = MOOD_CONTENTMENT;
    
    // Proximity zones for goblin behavior
    const float close_threshold = 20.0f;   // Within 20cm
    const float far_threshold = 100.0f;    // Beyond 100cm
    
    p32_sensor_trigger_t trigger = 0;
    p32_mood_t target_mood = MOOD_CONTENTMENT;
    
    if (distance < close_threshold && last_distance >= close_threshold) {
        // Something moved close
        trigger = TRIGGER_PROXIMITY_CLOSE;
        ESP_LOGI(TAG, "Proximity trigger: CLOSE (%.1fcm)", distance);
    } else if (distance > far_threshold && last_distance <= far_threshold) {
        // Something moved away
        trigger = TRIGGER_PROXIMITY_FAR;
        ESP_LOGI(TAG, "Proximity trigger: FAR (%.1fcm)", distance);
    }
    
    if (trigger != 0) {
        // Find corresponding mood mapping
        for (int i = 0; p32_goblin_sensor_mappings[i].trigger != 0; i++) {
            if (p32_goblin_sensor_mappings[i].trigger == trigger) {
                target_mood = p32_goblin_sensor_mappings[i].target_mood;
                
                // Only change mood if it's different or higher priority
                if (target_mood != last_triggered_mood) {
                    ESP_LOGI(TAG, "Triggering mood change: %d -> %d", p32_get_mood(), target_mood);
                    
                    // Set new mood
                    p32_set_mood(target_mood);
                    
                    // Update display animations
                    p32_display_update_mood_animations(target_mood);
                    
                    // Play appropriate sound
                    p32_audio_play_mood_sound(target_mood);
                    
                    last_triggered_mood = target_mood;
                }
                break;
            }
        }
    }
    
    last_distance = distance;
    return ESP_OK;
}

esp_err_t p32_sensor_check_motion_behavior(bool motion_active) {
    // Placeholder for PIR motion sensor behavior
    ESP_LOGD(TAG, "Motion behavior check: %s", motion_active ? "ACTIVE" : "INACTIVE");
    return ESP_OK;
}

esp_err_t p32_sensor_check_touch_behavior(const char* touch_location) {
    // Placeholder for touch sensor behavior
    ESP_LOGD(TAG, "Touch behavior check: %s", touch_location);
    return ESP_OK;
}

#endif // ENABLE_COMPLEX_SENSORS
