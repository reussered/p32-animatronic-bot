/*
 * P32 Animatronic Bot - Torso Controller Implementation
 * ESP32-S3 Master Controller for Torso Subsystem
 * 
 * Features:
 * - 6DOF spine articulation with servo control
 * - Power distribution management
 * - ESP-NOW mesh network master
 * - System coordination and balance control
 * - IMU integration for orientation feedback
 */

#include "p32_torso_controller.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Component configuration flags
#ifdef P32_COMP_TORSO_CONTROLLER

// Torso controller state
static p32_torso_state_t torso_state = {0};
static QueueHandle_t torso_command_queue;
static TaskHandle_t torso_control_task_handle;

// ESP-NOW mesh network configuration
static const uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Component initialization
esp_err_t p32_comp_torso_controller_init(void) {
    ESP_LOGI(TAG, "Initializing P32 Torso Controller");
    
    // Initialize GPIO pins
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << TORSO_STATUS_LED_PIN) | 
                       (1ULL << TORSO_POWER_LED_PIN) |
                       (1ULL << TORSO_ERROR_LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));
    
    // Initialize emergency stop input
    gpio_conf.pin_bit_mask = (1ULL << TORSO_ESTOP_PIN);
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));
    
    // Initialize I2C master for servo control
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = TORSO_I2C_SDA_PIN,
        .scl_io_num = TORSO_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };
    ESP_ERROR_CHECK(i2c_param_config(TORSO_I2C_PORT, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(TORSO_I2C_PORT, i2c_conf.mode, 0, 0, 0));
    
    // Initialize PCA9685 servo driver
    ESP_ERROR_CHECK(p32_torso_servo_init());
    
    // Initialize ADC for power monitoring
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(TORSO_BATTERY_SENSE_CHANNEL, ADC_ATTEN_DB_11));
    
    // Initialize WiFi for ESP-NOW
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(p32_torso_espnow_recv_cb));
    ESP_ERROR_CHECK(esp_now_register_send_cb(p32_torso_espnow_send_cb));
    
    // Create command queue and control task
    torso_command_queue = xQueueCreate(10, sizeof(p32_torso_command_t));
    if (torso_command_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create torso command queue");
        return ESP_FAIL;
    }
    
    // Create torso control task
    BaseType_t task_result = xTaskCreate(
        p32_torso_control_task,
        "torso_control",
        4096,
        NULL,
        5,
        &torso_control_task_handle
    );
    
    if (task_result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create torso control task");
        return ESP_FAIL;
    }
    
    // Set initial LED status
    gpio_set_level(TORSO_POWER_LED_PIN, 1);
    gpio_set_level(TORSO_STATUS_LED_PIN, 1);
    gpio_set_level(TORSO_ERROR_LED_PIN, 0);
    
    ESP_LOGI(TAG, "P32 Torso Controller initialized successfully");
    return ESP_OK;
}

// Servo control initialization
esp_err_t p32_torso_servo_init(void) {
    // Initialize PCA9685 servo driver
    ESP_ERROR_CHECK(pca9685_init(TORSO_I2C_PORT, PCA9685_I2C_ADDRESS));
    ESP_ERROR_CHECK(pca9685_set_pwm_freq(50)); // 50Hz for servos
    
    // Set all servos to neutral position
    for (int i = 0; i < TORSO_SERVO_COUNT; i++) {
        ESP_ERROR_CHECK(pca9685_set_pwm(i, TORSO_SERVO_NEUTRAL_PWM));
        torso_state.spine_angles[i] = 0.0f;
    }
    
    ESP_LOGI(TAG, "Torso servo system initialized");
    return ESP_OK;
}

// Component action function
esp_err_t p32_comp_torso_controller_act(p32_torso_command_t command) {
    // Send command to control task via queue
    BaseType_t result = xQueueSend(torso_command_queue, &command, pdMS_TO_TICKS(100));
    if (result != pdPASS) {
        ESP_LOGW(TAG, "Failed to queue torso command");
        return ESP_FAIL;
    }
    return ESP_OK;
}

// Main torso control task
static void p32_torso_control_task(void *pvParameters) {
    p32_torso_command_t command;
    TickType_t last_update = xTaskGetTickCount();
    
    ESP_LOGI(TAG, "Torso control task started");
    
    while (1) {
        // Process commands from queue
        if (xQueueReceive(torso_command_queue, &command, pdMS_TO_TICKS(20)) == pdPASS) {
            p32_torso_process_command(&command);
        }
        
        // Periodic system monitoring
        TickType_t current_time = xTaskGetTickCount();
        if ((current_time - last_update) >= pdMS_TO_TICKS(100)) {
            p32_torso_system_monitor();
            last_update = current_time;
        }
        
        // Emergency stop check
        if (gpio_get_level(TORSO_ESTOP_PIN) == 0) {
            ESP_LOGW(TAG, "Emergency stop activated!");
            p32_torso_emergency_stop();
        }
    }
}

// Process torso commands
static void p32_torso_process_command(p32_torso_command_t *command) {
    switch (command->command_type) {
        case TORSO_CMD_SPINE_POSITION:
            p32_torso_set_spine_curve(command->spine_curve, command->transition_time);
            break;
            
        case TORSO_CMD_BALANCE_LEAN:
            p32_torso_balance_adjustment(command->lean_angle);
            break;
            
        case TORSO_CMD_POWER_MANAGEMENT:
            p32_torso_power_control(command->power_level);
            break;
            
        case TORSO_CMD_SYSTEM_STATUS:
            p32_torso_send_status_update();
            break;
            
        case TORSO_CMD_EMERGENCY_STOP:
            p32_torso_emergency_stop();
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown torso command: %d", command->command_type);
            break;
    }
}

// Spine articulation control
esp_err_t p32_torso_set_spine_curve(float spine_curve[6], uint32_t transition_time) {
    ESP_LOGI(TAG, "Setting spine curve over %dms", transition_time);
    
    // Calculate servo positions for each vertebra
    for (int i = 0; i < TORSO_SERVO_COUNT; i++) {
        // Clamp angle to safe range (-45 deg to +45 deg)
        float angle = fmaxf(-45.0f, fminf(45.0f, spine_curve[i]));
        
        // Convert angle to PWM value (1000-2000?s)
        uint16_t pwm_value = p32_torso_angle_to_pwm(angle);
        
        // Apply servo command
        ESP_ERROR_CHECK(pca9685_set_pwm(i, pwm_value));
        
        // Update state
        torso_state.spine_angles[i] = angle;
    }
    
    torso_state.last_spine_update = esp_timer_get_time() / 1000;
    return ESP_OK;
}

// Balance adjustment
esp_err_t p32_torso_balance_adjustment(float lean_angle) {
    ESP_LOGI(TAG, "Applying balance lean: %.2f degrees", lean_angle);
    
    // Calculate compensatory spine curve
    float balance_curve[6];
    for (int i = 0; i < 6; i++) {
        // Distribute lean across vertebrae (more at top)
        float vertebra_weight = (float)(i + 1) / 6.0f;
        balance_curve[i] = lean_angle * vertebra_weight;
    }
    
    return p32_torso_set_spine_curve(balance_curve, 500); // 500ms transition
}

// Power management
esp_err_t p32_torso_power_control(float power_level) {
    ESP_LOGI(TAG, "Setting system power level: %.1f%%", power_level * 100.0f);
    
    // Clamp power level to safe range
    power_level = fmaxf(0.1f, fminf(1.0f, power_level));
    
    // Apply power scaling to all servos
    for (int i = 0; i < TORSO_SERVO_COUNT; i++) {
        // Scale current servo positions by power level
        uint16_t current_pwm = pca9685_get_pwm(i);
        uint16_t scaled_pwm = TORSO_SERVO_NEUTRAL_PWM + 
                             (current_pwm - TORSO_SERVO_NEUTRAL_PWM) * power_level;
        ESP_ERROR_CHECK(pca9685_set_pwm(i, scaled_pwm));
    }
    
    torso_state.power_level = power_level;
    return ESP_OK;
}

// System monitoring
static void p32_torso_system_monitor(void) {
    // Monitor battery voltage
    int adc_reading = adc1_get_raw(TORSO_BATTERY_SENSE_CHANNEL);
    float voltage = (adc_reading * 3.3f / 4095.0f) * TORSO_VOLTAGE_DIVIDER_RATIO;
    torso_state.battery_voltage = voltage;
    
    // Check for low battery
    if (voltage < TORSO_LOW_BATTERY_THRESHOLD) {
        ESP_LOGW(TAG, "Low battery warning: %.2fV", voltage);
        gpio_set_level(TORSO_ERROR_LED_PIN, 1);
    }
    
    // Monitor system current
    torso_state.system_current = p32_torso_read_system_current();
    
    // Check for overcurrent
    if (torso_state.system_current > TORSO_MAX_CURRENT_THRESHOLD) {
        ESP_LOGW(TAG, "Overcurrent detected: %.2fA", torso_state.system_current);
        p32_torso_power_control(0.7f); // Reduce to 70% power
    }
    
    // Update uptime
    torso_state.uptime_ms = esp_timer_get_time() / 1000;
    
    // Blink status LED to show activity
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(TORSO_STATUS_LED_PIN, led_state ? 1 : 0);
}

// Emergency stop implementation
static void p32_torso_emergency_stop(void) {
    ESP_LOGW(TAG, "EMERGENCY STOP - Disabling all servos");
    
    // Disable all servo outputs
    for (int i = 0; i < TORSO_SERVO_COUNT; i++) {
        ESP_ERROR_CHECK(pca9685_set_pwm(i, 0)); // 0 = servo off
        torso_state.spine_angles[i] = 0.0f;
    }
    
    // Set error LED
    gpio_set_level(TORSO_ERROR_LED_PIN, 1);
    gpio_set_level(TORSO_STATUS_LED_PIN, 0);
    
    // Broadcast emergency stop to all subsystems
    p32_torso_command_t emergency_cmd = {
        .command_type = TORSO_CMD_EMERGENCY_STOP,
        .priority = TORSO_PRIORITY_EMERGENCY
    };
    
    esp_now_send(broadcast_mac, (uint8_t*)&emergency_cmd, sizeof(emergency_cmd));
    
    torso_state.emergency_stop_active = true;
}

// ESP-NOW communication callbacks
static void p32_torso_espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        torso_state.successful_transmissions++;
    } else {
        torso_state.failed_transmissions++;
        ESP_LOGW(TAG, "ESP-NOW transmission failed");
    }
}

static void p32_torso_espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len == sizeof(p32_subsystem_status_t)) {
        p32_subsystem_status_t *status = (p32_subsystem_status_t*)data;
        p32_torso_update_subsystem_status(status);
    } else if (len == sizeof(p32_torso_command_t)) {
        p32_torso_command_t *command = (p32_torso_command_t*)data;
        p32_comp_torso_controller_act(*command);
    }
}

// Utility functions
static uint16_t p32_torso_angle_to_pwm(float angle) {
    // Convert angle (-45 deg to +45 deg) to PWM value (1000-2000?s)
    float pwm_range = TORSO_SERVO_MAX_PWM - TORSO_SERVO_MIN_PWM;
    float normalized_angle = (angle + 45.0f) / 90.0f; // 0.0 to 1.0
    return TORSO_SERVO_MIN_PWM + (uint16_t)(normalized_angle * pwm_range);
}

static float p32_torso_read_system_current(void) {
    // Read current sensor via I2C (INA219 or similar)
    // This is a placeholder implementation
    return 5.0f; // Simulated current reading
}

static void p32_torso_update_subsystem_status(p32_subsystem_status_t *status) {
    // Update subsystem status in global state
    switch (status->subsystem_id) {
        case SUBSYSTEM_ARM_LEFT:
            torso_state.subsystem_status[0] = *status;
            break;
        case SUBSYSTEM_ARM_RIGHT:
            torso_state.subsystem_status[1] = *status;
            break;
        case SUBSYSTEM_LEG_LEFT:
            torso_state.subsystem_status[2] = *status;
            break;
        case SUBSYSTEM_LEG_RIGHT:
            torso_state.subsystem_status[3] = *status;
            break;
        default:
            ESP_LOGW(TAG, "Unknown subsystem ID: %d", status->subsystem_id);
            break;
    }
}

static void p32_torso_send_status_update(void) {
    p32_system_status_t system_status = {
        .battery_voltage = torso_state.battery_voltage,
        .system_current = torso_state.system_current,
        .power_level = torso_state.power_level,
        .uptime_ms = torso_state.uptime_ms,
        .emergency_stop_active = torso_state.emergency_stop_active
    };
    
    // Broadcast status to all subsystems
    esp_now_send(broadcast_mac, (uint8_t*)&system_status, sizeof(system_status));
}

#endif // P32_COMP_TORSO_CONTROLLER
