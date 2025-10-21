/**
 * @file hand_controller.c
 * @brief P32 Hand Subsystem Controller Implementation
 * 
 * ESP32-C3 hand controller for 5-finger articulated hands with tactile feedback.
 * Provides gesture recognition, object manipulation, and force sensing.
 * 
 * Features:
 * - 5DOF finger control (thumb, index, middle, ring, pinky)
 * - Force-sensitive resistor (FSR) pressure sensing per finger
 * - PCA9685 servo expansion with 16-channel PWM control
 * - Gesture library with 20+ predefined gestures
 * - Object grip detection and adaptive force control
 * - ESP-NOW mesh communication with torso master controller
 * 
 * Hardware Configuration:
 * - ESP32-C3-WROOM-02 microcontroller
 * - PCA9685 16-channel PWM servo driver (I2C)
 * - 5x SG90 micro servos (finger joints)
 * - 5x Force-sensitive resistors (finger tips)
 * - 1x MCP3008 ADC for analog sensor reading
 * 
 * @author P32 Animatronic Bot Project
 * @date 2024
 */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <driver/spi_master.h>
#include <esp_now.h>
#include <math.h>

#include "p32_hand_controller.h"
#include "p32_core.h"

#ifdef P32_COMP_HAND_CONTROLLER

static const char* TAG = "P32_HAND_CTRL";

// Hand controller state
static hand_controller_state_t g_hand_state = {0};
static bool g_initialized = false;
static TaskHandle_t g_hand_task_handle = NULL;
static QueueHandle_t g_gesture_queue = NULL;

// I2C configuration for PCA9685
static i2c_config_t g_i2c_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = HAND_I2C_SDA_GPIO,
    .scl_io_num = HAND_I2C_SCL_GPIO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 400000,  // 400kHz
};

// SPI configuration for MCP3008 ADC
static spi_device_handle_t g_adc_spi_handle = NULL;

// Gesture library - 20 predefined gestures
static const finger_position_t g_gesture_library[MAX_GESTURES] = {
    // Basic gestures
    {.thumb = 0, .index = 0, .middle = 0, .ring = 0, .pinky = 0, .name = "open_hand"},
    {.thumb = 180, .index = 180, .middle = 180, .ring = 180, .pinky = 180, .name = "closed_fist"},
    {.thumb = 90, .index = 0, .middle = 180, .ring = 180, .pinky = 180, .name = "point_index"},
    {.thumb = 0, .index = 0, .middle = 0, .ring = 180, .ring = 180, .name = "peace_sign"},
    {.thumb = 0, .index = 180, .middle = 180, .ring = 180, .pinky = 0, .name = "rock_horns"},
    
    // Functional grips
    {.thumb = 45, .index = 45, .middle = 90, .ring = 135, .pinky = 135, .name = "precision_grip"},
    {.thumb = 90, .index = 90, .middle = 90, .ring = 90, .pinky = 90, .name = "power_grip"},
    {.thumb = 30, .index = 30, .middle = 30, .ring = 30, .pinky = 30, .name = "light_grip"},
    {.thumb = 135, .index = 135, .middle = 135, .ring = 135, .pinky = 135, .name = "strong_grip"},
    
    // Expressive gestures
    {.thumb = 0, .index = 0, .middle = 180, .ring = 0, .pinky = 0, .name = "middle_finger"},
    {.thumb = 180, .index = 0, .middle = 0, .ring = 0, .pinky = 0, .name = "thumbs_up"},
    {.thumb = 0, .index = 0, .middle = 0, .ring = 0, .pinky = 0, .name = "thumbs_down"},
    {.thumb = 90, .index = 90, .middle = 0, .ring = 0, .pinky = 180, .name = "call_me"},
    
    // Counting gestures
    {.thumb = 180, .index = 0, .middle = 180, .ring = 180, .pinky = 180, .name = "one"},
    {.thumb = 180, .index = 0, .middle = 0, .ring = 180, .pinky = 180, .name = "two"},
    {.thumb = 180, .index = 0, .middle = 0, .ring = 0, .ring = 180, .name = "three"},
    {.thumb = 0, .index = 0, .middle = 0, .ring = 0, .pinky = 180, .name = "four"},
    {.thumb = 0, .index = 0, .middle = 0, .ring = 0, .pinky = 0, .name = "five"},
    
    // Special positions
    {.thumb = 45, .index = 135, .middle = 45, .ring = 135, .pinky = 45, .name = "wave_position"},
    {.thumb = 90, .index = 45, .middle = 0, .ring = 0, .pinky = 0, .name = "okay_sign"}
};

/**
 * @brief Initialize PCA9685 PWM servo driver
 */
static esp_err_t init_pca9685(void) {
    ESP_LOGI(TAG, "Initializing PCA9685 servo driver");
    
    // Install I2C driver
    esp_err_t ret = i2c_param_config(I2C_NUM_0, &g_i2c_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C config failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Reset PCA9685
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCA9685_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, PCA9685_MODE1, true);
    i2c_master_write_byte(cmd, 0x80, true);  // Reset
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PCA9685 reset failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));  // Wait for reset
    
    // Configure PCA9685 for servo operation
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCA9685_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, PCA9685_MODE1, true);
    i2c_master_write_byte(cmd, 0x20, true);  // Auto-increment enabled
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PCA9685 mode config failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Set PWM frequency for servos (50Hz)
    uint8_t prescale = (uint8_t)(25000000 / (4096 * 50) - 1);  // ~122 for 50Hz
    
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCA9685_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, PCA9685_PRESCALE, true);
    i2c_master_write_byte(cmd, prescale, true);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    ESP_LOGI(TAG, "PCA9685 initialized successfully");
    return ret;
}

/**
 * @brief Initialize MCP3008 ADC for force sensors
 */
static esp_err_t init_mcp3008_adc(void) {
    ESP_LOGI(TAG, "Initializing MCP3008 ADC for force sensors");
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = HAND_SPI_MOSI_GPIO,
        .miso_io_num = HAND_SPI_MISO_GPIO,
        .sclk_io_num = HAND_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1000000,  // 1MHz
        .mode = 0,
        .spics_io_num = HAND_SPI_CS_GPIO,
        .queue_size = 7,
    };
    
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &g_adc_spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI device add failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "MCP3008 ADC initialized successfully");
    return ESP_OK;
}

/**
 * @brief Set servo position on PCA9685
 */
static esp_err_t set_servo_position(uint8_t channel, uint16_t angle_degrees) {
    // Convert angle to PWM value
    // SG90 servos: 500us (0°) to 2400us (180°) pulse width
    // At 50Hz: 4096 counts per 20ms period
    // 500us = 102 counts, 2400us = 491 counts
    uint16_t pwm_value = 102 + (angle_degrees * (491 - 102)) / 180;
    
    // Clamp to valid range
    if (pwm_value > 491) pwm_value = 491;
    if (pwm_value < 102) pwm_value = 102;
    
    // Write to PCA9685 registers
    uint8_t reg_base = PCA9685_LED0_ON_L + (channel * 4);
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCA9685_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_base, true);
    i2c_master_write_byte(cmd, 0, true);        // ON_L
    i2c_master_write_byte(cmd, 0, true);        // ON_H
    i2c_master_write_byte(cmd, pwm_value & 0xFF, true);  // OFF_L
    i2c_master_write_byte(cmd, (pwm_value >> 8) & 0xFF, true);  // OFF_H
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}

/**
 * @brief Read force sensor value from MCP3008 ADC
 */
static uint16_t read_force_sensor(uint8_t channel) {
    if (channel >= 8) return 0;  // Invalid channel
    
    uint8_t tx_data[3] = {
        0x01,  // Start bit
        (0x08 | channel) << 4,  // Single-ended mode + channel
        0x00
    };
    uint8_t rx_data[3] = {0};
    
    spi_transaction_t trans = {
        .length = 24,  // 3 bytes * 8 bits
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    
    esp_err_t ret = spi_device_transmit(g_adc_spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "ADC read failed: %s", esp_err_to_name(ret));
        return 0;
    }
    
    // Extract 10-bit result
    uint16_t result = ((rx_data[1] & 0x03) << 8) | rx_data[2];
    return result;
}

/**
 * @brief Execute gesture command with smooth interpolation
 */
static esp_err_t execute_gesture(const gesture_command_t* gesture_cmd) {
    if (!gesture_cmd || gesture_cmd->gesture_id >= MAX_GESTURES) {
        ESP_LOGE(TAG, "Invalid gesture command");
        return ESP_ERR_INVALID_ARG;
    }
    
    const finger_position_t* target = &g_gesture_library[gesture_cmd->gesture_id];
    finger_position_t current = g_hand_state.current_position;
    
    ESP_LOGI(TAG, "Executing gesture: %s (duration: %dms)", target->name, gesture_cmd->duration_ms);
    
    // Calculate interpolation steps
    uint32_t steps = gesture_cmd->duration_ms / HAND_UPDATE_INTERVAL_MS;
    if (steps == 0) steps = 1;
    
    // Perform smooth interpolation
    for (uint32_t step = 0; step <= steps; step++) {
        float progress = (float)step / steps;
        
        // Calculate intermediate positions
        uint16_t thumb_pos = current.thumb + (target->thumb - current.thumb) * progress;
        uint16_t index_pos = current.index + (target->index - current.index) * progress;
        uint16_t middle_pos = current.middle + (target->middle - current.middle) * progress;
        uint16_t ring_pos = current.ring + (target->ring - current.ring) * progress;
        uint16_t pinky_pos = current.pinky + (target->pinky - current.pinky) * progress;
        
        // Set servo positions
        set_servo_position(SERVO_THUMB, thumb_pos);
        set_servo_position(SERVO_INDEX, index_pos);
        set_servo_position(SERVO_MIDDLE, middle_pos);
        set_servo_position(SERVO_RING, ring_pos);
        set_servo_position(SERVO_PINKY, pinky_pos);
        
        // Update current position
        g_hand_state.current_position.thumb = thumb_pos;
        g_hand_state.current_position.index = index_pos;
        g_hand_state.current_position.middle = middle_pos;
        g_hand_state.current_position.ring = ring_pos;
        g_hand_state.current_position.pinky = pinky_pos;
        
        vTaskDelay(pdMS_TO_TICKS(HAND_UPDATE_INTERVAL_MS));
    }
    
    // Update state
    g_hand_state.current_gesture_id = gesture_cmd->gesture_id;
    g_hand_state.last_gesture_time = esp_timer_get_time() / 1000;
    
    ESP_LOGI(TAG, "Gesture completed successfully");
    return ESP_OK;
}

/**
 * @brief Update force sensor readings
 */
static void update_force_sensors(void) {
    for (int i = 0; i < 5; i++) {
        uint16_t raw_value = read_force_sensor(i);
        
        // Convert to force (0-100% scale)
        // Assuming FSR range: 0-1023 ADC counts
        float force_percent = (float)raw_value / 1023.0f * 100.0f;
        
        // Apply exponential smoothing filter
        float alpha = 0.3f;  // Smoothing factor
        g_hand_state.force_sensors[i] = alpha * force_percent + 
                                       (1.0f - alpha) * g_hand_state.force_sensors[i];
        
        // Detect grip events
        if (g_hand_state.force_sensors[i] > FORCE_THRESHOLD_HIGH && 
            !g_hand_state.finger_contact[i]) {
            g_hand_state.finger_contact[i] = true;
            g_hand_state.grip_detected = true;
            ESP_LOGD(TAG, "Finger %d contact detected (%.1f%%)", i, g_hand_state.force_sensors[i]);
        } else if (g_hand_state.force_sensors[i] < FORCE_THRESHOLD_LOW && 
                   g_hand_state.finger_contact[i]) {
            g_hand_state.finger_contact[i] = false;
            ESP_LOGD(TAG, "Finger %d contact released", i);
        }
    }
    
    // Update grip state
    bool any_contact = false;
    for (int i = 0; i < 5; i++) {
        if (g_hand_state.finger_contact[i]) {
            any_contact = true;
            break;
        }
    }
    g_hand_state.grip_detected = any_contact;
}

/**
 * @brief Calculate grip strength and center of pressure
 */
static void analyze_grip(void) {
    float total_force = 0.0f;
    float weighted_x = 0.0f;
    float weighted_y = 0.0f;
    
    // Finger positions (relative coordinates)
    static const float finger_x[] = {-20, -10, 0, 10, 20};  // mm from center
    static const float finger_y[] = {30, 40, 40, 35, 25};   // mm from palm
    
    for (int i = 0; i < 5; i++) {
        float force = g_hand_state.force_sensors[i];
        total_force += force;
        weighted_x += force * finger_x[i];
        weighted_y += force * finger_y[i];
    }
    
    g_hand_state.grip_strength = total_force / 5.0f;  // Average force
    
    if (total_force > 0.1f) {  // Avoid division by zero
        g_hand_state.grip_center_x = weighted_x / total_force;
        g_hand_state.grip_center_y = weighted_y / total_force;
    } else {
        g_hand_state.grip_center_x = 0.0f;
        g_hand_state.grip_center_y = 0.0f;
    }
    
    // Log grip analysis periodically
    static uint32_t last_log_time = 0;
    uint32_t current_time = esp_timer_get_time() / 1000;
    if (current_time - last_log_time > 1000 && g_hand_state.grip_detected) {  // Every second
        ESP_LOGI(TAG, "Grip analysis - Strength: %.1f%%, Center: (%.1f, %.1f)mm", 
                 g_hand_state.grip_strength, g_hand_state.grip_center_x, g_hand_state.grip_center_y);
        last_log_time = current_time;
    }
}

/**
 * @brief Hand control task - Main control loop
 */
static void hand_control_task(void* pvParameters) {
    ESP_LOGI(TAG, "Hand control task started");
    
    gesture_command_t gesture_cmd;
    TickType_t last_update = xTaskGetTickCount();
    
    while (1) {
        // Process gesture commands from queue
        if (xQueueReceive(g_gesture_queue, &gesture_cmd, 0) == pdTRUE) {
            execute_gesture(&gesture_cmd);
        }
        
        // Update sensor readings every cycle
        update_force_sensors();
        analyze_grip();
        
        // Update state timestamp
        g_hand_state.last_update_time = esp_timer_get_time() / 1000;
        
        // Maintain control loop frequency
        vTaskDelayUntil(&last_update, pdMS_TO_TICKS(HAND_UPDATE_INTERVAL_MS));
    }
}

/**
 * @brief ESP-NOW data received callback
 */
static void espnow_recv_cb(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
    if (len == sizeof(hand_command_t)) {
        hand_command_t* cmd = (hand_command_t*)data;
        
        if (cmd->command_type == HAND_CMD_GESTURE) {
            gesture_command_t gesture_cmd = {
                .gesture_id = cmd->gesture_id,
                .duration_ms = cmd->duration_ms
            };
            
            if (xQueueSend(g_gesture_queue, &gesture_cmd, 0) != pdTRUE) {
                ESP_LOGW(TAG, "Gesture queue full, command dropped");
            }
        } else if (cmd->command_type == HAND_CMD_DIRECT_POSITION) {
            // Direct position control (immediate)
            set_servo_position(SERVO_THUMB, cmd->finger_positions.thumb);
            set_servo_position(SERVO_INDEX, cmd->finger_positions.index);
            set_servo_position(SERVO_MIDDLE, cmd->finger_positions.middle);
            set_servo_position(SERVO_RING, cmd->finger_positions.ring);
            set_servo_position(SERVO_PINKY, cmd->finger_positions.pinky);
            
            g_hand_state.current_position = cmd->finger_positions;
        }
    }
}

/**
 * @brief Initialize ESP-NOW communication
 */
static esp_err_t init_espnow(void) {
    ESP_LOGI(TAG, "Initializing ESP-NOW communication");
    
    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_now_register_recv_cb(espnow_recv_cb);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW register recv callback failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Add torso master controller as peer
    esp_now_peer_info_t peer_info = {0};
    memcpy(peer_info.peer_addr, TORSO_MASTER_MAC, 6);
    peer_info.channel = 0;
    peer_info.encrypt = false;
    
    ret = esp_now_add_peer(&peer_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW add peer failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "ESP-NOW communication initialized");
    return ESP_OK;
}

/**
 * @brief Component initialization function
 */
esp_err_t p32_comp_hand_controller_init(void) {
    if (g_initialized) {
        ESP_LOGW(TAG, "Hand controller already initialized");
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Initializing P32 Hand Controller");
    
    // Initialize state
    memset(&g_hand_state, 0, sizeof(hand_controller_state_t));
    g_hand_state.subsystem_id = HAND_SUBSYSTEM_ID;
    
    // Initialize hardware
    esp_err_t ret = init_pca9685();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PCA9685 initialization failed");
        return ret;
    }
    
    ret = init_mcp3008_adc();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "MCP3008 initialization failed");
        return ret;
    }
    
    ret = init_espnow();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW initialization failed");
        return ret;
    }
    
    // Create gesture command queue
    g_gesture_queue = xQueueCreate(GESTURE_QUEUE_SIZE, sizeof(gesture_command_t));
    if (!g_gesture_queue) {
        ESP_LOGE(TAG, "Failed to create gesture queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Set initial position (open hand)
    gesture_command_t init_gesture = {
        .gesture_id = 0,  // open_hand
        .duration_ms = 2000
    };
    execute_gesture(&init_gesture);
    
    // Create hand control task
    BaseType_t task_ret = xTaskCreate(
        hand_control_task,
        "hand_ctrl",
        HAND_TASK_STACK_SIZE,
        NULL,
        HAND_TASK_PRIORITY,
        &g_hand_task_handle
    );
    
    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create hand control task");
        return ESP_ERR_NO_MEM;
    }
    
    g_initialized = true;
    ESP_LOGI(TAG, "P32 Hand Controller initialized successfully");
    
    return ESP_OK;
}

/**
 * @brief Component action function
 */
esp_err_t p32_comp_hand_controller_act(p32_component_command_t command) {
    if (!g_initialized) {
        ESP_LOGE(TAG, "Hand controller not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    switch (command.type) {
        case P32_CMD_GESTURE:
            if (command.data.gesture.gesture_id < MAX_GESTURES) {
                gesture_command_t gesture_cmd = {
                    .gesture_id = command.data.gesture.gesture_id,
                    .duration_ms = command.data.gesture.duration_ms
                };
                
                if (xQueueSend(g_gesture_queue, &gesture_cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
                    return ESP_OK;
                } else {
                    ESP_LOGW(TAG, "Gesture queue full");
                    return ESP_ERR_TIMEOUT;
                }
            } else {
                ESP_LOGE(TAG, "Invalid gesture ID: %d", command.data.gesture.gesture_id);
                return ESP_ERR_INVALID_ARG;
            }
            
        case P32_CMD_DIRECT_CONTROL:
            // Direct servo control
            for (int i = 0; i < 5; i++) {
                set_servo_position(i, command.data.direct.servo_positions[i]);
            }
            return ESP_OK;
            
        case P32_CMD_GET_STATUS:
            // Status is continuously updated by the control task
            return ESP_OK;
            
        case P32_CMD_CALIBRATE:
            // Perform calibration sequence
            ESP_LOGI(TAG, "Starting hand calibration sequence");
            
            // Move through full range of motion
            for (uint8_t gesture = 0; gesture < 5; gesture++) {  // First 5 basic gestures
                gesture_command_t cal_gesture = {
                    .gesture_id = gesture,
                    .duration_ms = 1000
                };
                execute_gesture(&cal_gesture);
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            
            // Return to neutral position
            gesture_command_t neutral = {.gesture_id = 0, .duration_ms = 1000};
            execute_gesture(&neutral);
            
            ESP_LOGI(TAG, "Hand calibration completed");
            return ESP_OK;
            
        case P32_CMD_EMERGENCY_STOP:
            ESP_LOGW(TAG, "Emergency stop - opening hand immediately");
            
            // Immediate open position
            set_servo_position(SERVO_THUMB, 0);
            set_servo_position(SERVO_INDEX, 0);
            set_servo_position(SERVO_MIDDLE, 0);
            set_servo_position(SERVO_RING, 0);
            set_servo_position(SERVO_PINKY, 0);
            
            // Clear gesture queue
            xQueueReset(g_gesture_queue);
            
            return ESP_OK;
            
        default:
            ESP_LOGW(TAG, "Unknown command type: %d", command.type);
            return ESP_ERR_NOT_SUPPORTED;
    }
}

/**
 * @brief Get current hand state
 */
esp_err_t p32_hand_get_state(hand_controller_state_t* state) {
    if (!state || !g_initialized) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *state = g_hand_state;
    return ESP_OK;
}

/**
 * @brief Get gesture name by ID
 */
const char* p32_hand_get_gesture_name(uint8_t gesture_id) {
    if (gesture_id >= MAX_GESTURES) {
        return "unknown";
    }
    return g_gesture_library[gesture_id].name;
}

/**
 * @brief Component cleanup function
 */
esp_err_t p32_comp_hand_controller_cleanup(void) {
    if (!g_initialized) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Cleaning up hand controller");
    
    // Stop control task
    if (g_hand_task_handle) {
        vTaskDelete(g_hand_task_handle);
        g_hand_task_handle = NULL;
    }
    
    // Cleanup queue
    if (g_gesture_queue) {
        vQueueDelete(g_gesture_queue);
        g_gesture_queue = NULL;
    }
    
    // Cleanup ESP-NOW
    esp_now_deinit();
    
    // Cleanup SPI and I2C
    if (g_adc_spi_handle) {
        spi_bus_remove_device(g_adc_spi_handle);
        spi_bus_free(SPI2_HOST);
    }
    i2c_driver_delete(I2C_NUM_0);
    
    g_initialized = false;
    ESP_LOGI(TAG, "Hand controller cleanup completed");
    
    return ESP_OK;
}

#endif // P32_COMP_HAND_CONTROLLER
