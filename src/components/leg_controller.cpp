/**
 * @file leg_controller.c
 * @brief P32 Leg Subsystem Controller Implementation
 * 
 * ESP32-C3 leg controller for 6DOF bipedal locomotion with integrated foot control.
 * Optimized GPIO allocation - consolidates foot controller if capacity allows.
 * 
 * Features:
 * - 6DOF leg control (3DOF hip + 1DOF knee + 2DOF ankle)
 * - Integrated 8-sensor foot pressure monitoring (if GPIO available)
 * - Real-time gait generation and balance control
 * - ZMP (Zero Moment Point) calculation for stability
 * - Dynamic load balancing with opposite leg
 * - ESP-NOW mesh communication with torso master
 * 
 * GPIO Optimization Strategy:
 * - Base leg control: 12 GPIO pins (6 servos via PCA9685 I2C + 4 encoders + 2 IMU)
 * - Foot integration: +10 GPIO pins (8 FSR sensors + 2 toe servos)
 * - ESP32-C3 has 22 GPIO pins available -> can handle full leg+foot system
 * 
 * Hardware Configuration:
 * - ESP32-C3-WROOM-02 microcontroller (22 GPIO pins available)
 * - PCA9685 16-channel PWM servo driver (I2C) for all servos
 * - 6x High-torque servos (MG996R/MG958) for leg joints
 * - 2x SG90 micro servos for toe articulation (if foot integrated)
 * - 4x AS5600 magnetic encoders for joint feedback
 * - 1x MPU6050 IMU for leg segment orientation
 * - 8x FSR sensors for ground pressure (if foot integrated)
 * - 1x MCP3008 ADC for analog sensor reading
 * 
 * @author P32 Animatronic Bot Project
 * @date 2024
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
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

#include "p32_leg_controller.h"
#include "p32_core.h"

#ifdef P32_COMP_LEG_CONTROLLER

static const char* TAG = "P32_LEG_CTRL";

// Leg controller state
static leg_controller_state_t g_leg_state = {0};
static bool g_initialized = false;
static TaskHandle_t g_leg_task_handle = NULL;
static TaskHandle_t g_gait_task_handle = NULL;
static QueueHandle_t g_command_queue = NULL;

// GPIO Pin Allocation Analysis
// ESP32-C3 GPIO Usage (22 pins available):
// Base Leg Control (12 pins):
// - I2C (SDA=4, SCL=5) for PCA9685 servo driver -> 2 pins
// - Encoder feedback (6,7,8,9) -> 4 pins  
// - IMU I2C (10,20) -> 2 pins
// - Status LEDs (18,19) -> 2 pins
// - Emergency stop (0) -> 1 pin
// - SPI CS for ADC (1) -> 1 pin
// Foot Integration (10 pins if capacity allows):
// - SPI bus (MOSI=2, MISO=3, SCLK=21) -> 3 pins (shared)
// - FSR direct GPIO (11,12,13,14,15,16,17) -> 7 pins
// Total: 22 pins (exactly at ESP32-C3 limit, foot integration possible)

// I2C configuration for PCA9685 and IMU
static i2c_config_t g_i2c_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = LEG_I2C_SDA_GPIO,
    .scl_io_num = LEG_I2C_SCL_GPIO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 400000,
};

// SPI configuration for MCP3008 ADC (foot integration)
static spi_device_handle_t g_adc_spi_handle = NULL;

// Gait generation state
static gait_controller_t g_gait_controller = {0};
static walking_parameters_t g_walking_params = {
    .step_length = 0.15f,      // 15cm default step
    .step_height = 0.08f,      // 8cm step clearance
    .step_frequency = 1.0f,    // 1 step per second
    .stance_ratio = 0.6f,      // 60% stance, 40% swing
    .body_height = 0.85f,      // 85cm body height
    .foot_separation = 0.25f   // 25cm between feet
};

// Joint limits and safety ranges (degrees)
static const joint_limits_t g_joint_limits[LEG_DOF_COUNT] = {
    // Hip joints
    {.min_angle = -30.0f, .max_angle = 120.0f, .max_velocity = 90.0f},  // Hip flexion/extension
    {.min_angle = -45.0f, .max_angle = 45.0f,  .max_velocity = 60.0f},  // Hip abduction/adduction  
    {.min_angle = -30.0f, .max_angle = 30.0f,  .max_velocity = 45.0f},  // Hip rotation
    // Knee joint
    {.min_angle = 0.0f,   .max_angle = 150.0f, .max_velocity = 120.0f}, // Knee flexion
    // Ankle joints
    {.min_angle = -30.0f, .max_angle = 45.0f,  .max_velocity = 90.0f},  // Ankle dorsi/plantarflexion
    {.min_angle = -20.0f, .max_angle = 20.0f,  .max_velocity = 60.0f}   // Ankle inversion/eversion
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
    
    // Reset and configure PCA9685
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
    
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Set PWM frequency for servos (50Hz)
    uint8_t prescale = (uint8_t)(25000000 / (4096 * 50) - 1);
    
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
 * @brief Initialize MPU6050 IMU for leg orientation
 */
static esp_err_t init_mpu6050_imu(void) {
    ESP_LOGI(TAG, "Initializing MPU6050 IMU");
    
    // Wake up MPU6050
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, true);
    i2c_master_write_byte(cmd, 0x00, true);  // Wake up
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050 wakeup failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure accelerometer and gyroscope ranges
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, MPU6050_ACCEL_CONFIG, true);
    i2c_master_write_byte(cmd, 0x00, true);  // ±2g range
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    ESP_LOGI(TAG, "MPU6050 IMU initialized successfully");
    return ret;
}

/**
 * @brief Initialize foot pressure sensors (if integrated)
 */
static esp_err_t init_foot_sensors(void) {
    #ifdef LEG_FOOT_INTEGRATION_ENABLED
    ESP_LOGI(TAG, "Initializing integrated foot pressure sensors");
    
    // Initialize SPI bus for MCP3008 ADC
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = LEG_SPI_MOSI_GPIO,
        .miso_io_num = LEG_SPI_MISO_GPIO,
        .sclk_io_num = LEG_SPI_SCLK_GPIO,
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
        .clock_speed_hz = 1000000,
        .mode = 0,
        .spics_io_num = LEG_SPI_CS_GPIO,
        .queue_size = 7,
    };
    
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &g_adc_spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI device add failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Initialize FSR sensor GPIO pins
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << 11) | (1ULL << 12) | (1ULL << 13) | (1ULL << 14) |
                       (1ULL << 15) | (1ULL << 16) | (1ULL << 17),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));
    
    ESP_LOGI(TAG, "Foot sensors integrated successfully");
    #else
    ESP_LOGI(TAG, "Foot integration disabled - using separate foot controller");
    #endif
    
    return ESP_OK;
}

/**
 * @brief Set servo position with safety limits
 */
static esp_err_t set_joint_position(uint8_t joint_id, float angle_degrees) {
    if (joint_id >= LEG_DOF_COUNT) {
        ESP_LOGE(TAG, "Invalid joint ID: %d", joint_id);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Apply safety limits
    const joint_limits_t* limits = &g_joint_limits[joint_id];
    if (angle_degrees < limits->min_angle) {
        ESP_LOGW(TAG, "Joint %d angle %.1f° below limit %.1f°", 
                 joint_id, angle_degrees, limits->min_angle);
        angle_degrees = limits->min_angle;
    } else if (angle_degrees > limits->max_angle) {
        ESP_LOGW(TAG, "Joint %d angle %.1f° above limit %.1f°", 
                 joint_id, angle_degrees, limits->max_angle);
        angle_degrees = limits->max_angle;
    }
    
    // Convert angle to PWM value
    uint16_t pwm_value = LEG_SERVO_MIN_PWM + 
                        (angle_degrees - limits->min_angle) * 
                        (LEG_SERVO_MAX_PWM - LEG_SERVO_MIN_PWM) / 
                        (limits->max_angle - limits->min_angle);
    
    // Write to PCA9685 register
    uint8_t reg_base = PCA9685_LED0_ON_L + (joint_id * 4);
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCA9685_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_base, true);
    i2c_master_write_byte(cmd, 0, true);
    i2c_master_write_byte(cmd, 0, true);
    i2c_master_write_byte(cmd, pwm_value & 0xFF, true);
    i2c_master_write_byte(cmd, (pwm_value >> 8) & 0xFF, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    // Update state
    g_leg_state.joint_angles[joint_id] = angle_degrees;
    g_leg_state.last_update_time = esp_timer_get_time() / 1000;
    
    return ret;
}

/**
 * @brief Read foot pressure sensors (if integrated)
 */
static void update_foot_pressure(void) {
    #ifdef LEG_FOOT_INTEGRATION_ENABLED
    for (int i = 0; i < FOOT_SENSOR_COUNT; i++) {
        uint8_t tx_data[3] = {0x01, (0x08 | i) << 4, 0x00};
        uint8_t rx_data[3] = {0};
        
        spi_transaction_t trans = {
            .length = 24,
            .tx_buffer = tx_data,
            .rx_buffer = rx_data
        };
        
        esp_err_t ret = spi_device_transmit(g_adc_spi_handle, &trans);
        if (ret == ESP_OK) {
            uint16_t adc_value = ((rx_data[1] & 0x03) << 8) | rx_data[2];
            g_leg_state.foot_pressure[i] = (float)adc_value / 1023.0f * 100.0f;
        }
    }
    
    // Calculate center of pressure
    float total_pressure = 0.0f;
    float weighted_x = 0.0f, weighted_y = 0.0f;
    
    static const float sensor_positions[][2] = {
        {-50, -100}, {50, -100},   // Heel sensors
        {-25, -50},  {25, -50},    // Mid-foot sensors
        {-30, 0},    {30, 0},      // Ball sensors
        {-15, 50},   {15, 50}      // Toe sensors
    };
    
    for (int i = 0; i < FOOT_SENSOR_COUNT; i++) {
        float pressure = g_leg_state.foot_pressure[i];
        total_pressure += pressure;
        weighted_x += pressure * sensor_positions[i][0];
        weighted_y += pressure * sensor_positions[i][1];
    }
    
    if (total_pressure > 1.0f) {
        g_leg_state.cop_x = weighted_x / total_pressure;
        g_leg_state.cop_y = weighted_y / total_pressure;
        g_leg_state.ground_contact = true;
    } else {
        g_leg_state.cop_x = 0.0f;
        g_leg_state.cop_y = 0.0f;
        g_leg_state.ground_contact = false;
    }
    
    g_leg_state.total_ground_force = total_pressure;
    #endif
}

/**
 * @brief Forward kinematics calculation
 */
static void calculate_forward_kinematics(const float joint_angles[LEG_DOF_COUNT], 
                                       leg_pose_t* end_effector_pose) {
    // Hip position (relative to pelvis)
    float hip_x = LEG_SEGMENT_LENGTHS[0] * cosf(joint_angles[1]) * cosf(joint_angles[0]);
    float hip_y = LEG_SEGMENT_LENGTHS[0] * sinf(joint_angles[1]);
    float hip_z = -LEG_SEGMENT_LENGTHS[0] * cosf(joint_angles[1]) * sinf(joint_angles[0]);
    
    // Knee position
    float knee_angle = joint_angles[0] + joint_angles[3];
    float knee_x = hip_x + LEG_SEGMENT_LENGTHS[1] * cosf(knee_angle);
    float knee_y = hip_y;
    float knee_z = hip_z - LEG_SEGMENT_LENGTHS[1] * sinf(knee_angle);
    
    // Ankle position
    float ankle_angle = knee_angle + joint_angles[4];
    end_effector_pose->x = knee_x + LEG_SEGMENT_LENGTHS[2] * cosf(ankle_angle);
    end_effector_pose->y = knee_y;
    end_effector_pose->z = knee_z - LEG_SEGMENT_LENGTHS[2] * sinf(ankle_angle);
    
    // Foot orientation
    end_effector_pose->roll = joint_angles[5];    // Ankle roll
    end_effector_pose->pitch = ankle_angle;       // Accumulated pitch
    end_effector_pose->yaw = joint_angles[2];     // Hip yaw
}

/**
 * @brief Inverse kinematics calculation
 */
static esp_err_t calculate_inverse_kinematics(const leg_pose_t* target_pose, 
                                            float joint_angles[LEG_DOF_COUNT]) {
    float target_x = target_pose->x;
    float target_y = target_pose->y; 
    float target_z = target_pose->z;
    
    // Calculate leg length in XZ plane
    float leg_length_xz = sqrtf(target_x * target_x + target_z * target_z);
    float total_leg_length = sqrtf(leg_length_xz * leg_length_xz + target_y * target_y);
    
    // Check if target is reachable
    float max_reach = LEG_SEGMENT_LENGTHS[0] + LEG_SEGMENT_LENGTHS[1] + LEG_SEGMENT_LENGTHS[2];
    if (total_leg_length > max_reach * 0.95f) {  // 95% of max reach for safety
        ESP_LOGW(TAG, "Target pose unreachable: distance %.3f > max %.3f", 
                 total_leg_length, max_reach * 0.95f);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Hip yaw (rotation around vertical axis)
    joint_angles[2] = atan2f(target_x, -target_z);
    
    // Hip abduction (Y-axis movement)
    joint_angles[1] = atan2f(target_y, leg_length_xz);
    
    // Knee angle using law of cosines
    float thigh_len = LEG_SEGMENT_LENGTHS[0];
    float shin_len = LEG_SEGMENT_LENGTHS[1];
    float foot_len = LEG_SEGMENT_LENGTHS[2];
    
    float c2 = (thigh_len * thigh_len + shin_len * shin_len - leg_length_xz * leg_length_xz) /
               (2.0f * thigh_len * shin_len);
    
    if (c2 < -1.0f) c2 = -1.0f;
    if (c2 > 1.0f) c2 = 1.0f;
    
    joint_angles[3] = acosf(c2) - M_PI/2;  // Knee flexion
    
    // Hip flexion
    float alpha = atan2f(-target_z, fabsf(target_y));
    float beta = acosf((thigh_len * thigh_len + leg_length_xz * leg_length_xz - shin_len * shin_len) /
                      (2.0f * thigh_len * leg_length_xz));
    joint_angles[0] = alpha - beta;
    
    // Ankle angles to maintain foot orientation
    joint_angles[4] = target_pose->pitch - joint_angles[0] - joint_angles[3];
    joint_angles[5] = target_pose->roll;
    
    return ESP_OK;
}

/**
 * @brief Generate walking gait trajectory
 */
static void generate_gait_trajectory(float gait_phase, leg_pose_t* target_pose) {
    walking_parameters_t* params = &g_walking_params;
    
    if (gait_phase < params->stance_ratio) {
        // Stance phase - foot on ground
        float stance_progress = gait_phase / params->stance_ratio;
        
        target_pose->x = params->step_length * (0.5f - stance_progress);
        target_pose->y = 0.0f;  // On ground
        target_pose->z = -params->body_height;
        target_pose->pitch = 0.0f;
        target_pose->roll = 0.0f;
        target_pose->yaw = 0.0f;
    } else {
        // Swing phase - foot in air
        float swing_progress = (gait_phase - params->stance_ratio) / (1.0f - params->stance_ratio);
        
        target_pose->x = params->step_length * (swing_progress - 0.5f);
        target_pose->y = params->step_height * sinf(M_PI * swing_progress);  // Arc trajectory
        target_pose->z = -params->body_height;
        target_pose->pitch = 0.0f;
        target_pose->roll = 0.0f;
        target_pose->yaw = 0.0f;
    }
}

/**
 * @brief Gait generation task
 */
static void gait_control_task(void* pvParameters) {
    ESP_LOGI(TAG, "Gait control task started");
    
    TickType_t last_wake_time = xTaskGetTickCount();
    float gait_time = 0.0f;
    
    while (1) {
        if (g_gait_controller.gait_enabled) {
            // Update gait phase
            float dt = GAIT_UPDATE_INTERVAL_MS / 1000.0f;
            gait_time += dt * g_walking_params.step_frequency;
            
            float gait_phase = fmodf(gait_time, 1.0f);
            
            // Generate target pose
            leg_pose_t target_pose;
            generate_gait_trajectory(gait_phase, &target_pose);
            
            // Apply leg offset for left/right leg
            if (g_leg_state.leg_side == LEG_SIDE_LEFT) {
                target_pose.x += g_walking_params.foot_separation / 2.0f;
            } else {
                target_pose.x -= g_walking_params.foot_separation / 2.0f;
                gait_phase = fmodf(gait_phase + 0.5f, 1.0f);  // 180° phase offset
            }
            
            // Calculate inverse kinematics
            float target_joints[LEG_DOF_COUNT];
            esp_err_t ik_result = calculate_inverse_kinematics(&target_pose, target_joints);
            
            if (ik_result == ESP_OK) {
                // Set joint positions
                for (int i = 0; i < LEG_DOF_COUNT; i++) {
                    set_joint_position(i, target_joints[i] * 180.0f / M_PI);
                }
                
                // Update gait state
                g_gait_controller.current_phase = gait_phase;
                g_gait_controller.target_pose = target_pose;
            } else {
                ESP_LOGW(TAG, "IK solution failed, stopping gait");
                g_gait_controller.gait_enabled = false;
            }
        }
        
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(GAIT_UPDATE_INTERVAL_MS));
    }
}

/**
 * @brief Main leg control task
 */
static void leg_control_task(void* pvParameters) {
    ESP_LOGI(TAG, "Leg control task started");
    
    leg_command_t command;
    TickType_t last_update = xTaskGetTickCount();
    
    while (1) {
        // Process commands from queue
        if (xQueueReceive(g_command_queue, &command, 0) == pdTRUE) {
            switch (command.command_type) {
                case LEG_CMD_SET_POSITION:
                    // Direct joint position control
                    for (int i = 0; i < LEG_DOF_COUNT; i++) {
                        set_joint_position(i, command.joint_angles[i]);
                    }
                    break;
                    
                case LEG_CMD_SET_POSE:
                    // End-effector pose control
                    float joint_angles[LEG_DOF_COUNT];
                    if (calculate_inverse_kinematics(&command.target_pose, joint_angles) == ESP_OK) {
                        for (int i = 0; i < LEG_DOF_COUNT; i++) {
                            set_joint_position(i, joint_angles[i] * 180.0f / M_PI);
                        }
                    }
                    break;
                    
                case LEG_CMD_START_WALKING:
                    g_walking_params = command.walking_params;
                    g_gait_controller.gait_enabled = true;
                    ESP_LOGI(TAG, "Walking started");
                    break;
                    
                case LEG_CMD_STOP_WALKING:
                    g_gait_controller.gait_enabled = false;
                    ESP_LOGI(TAG, "Walking stopped");
                    break;
                    
                case LEG_CMD_EMERGENCY_STOP:
                    g_gait_controller.gait_enabled = false;
                    // Move to safe position
                    for (int i = 0; i < LEG_DOF_COUNT; i++) {
                        set_joint_position(i, 0.0f);  // Neutral positions
                    }
                    ESP_LOGW(TAG, "Emergency stop executed");
                    break;
            }
        }
        
        // Update sensors
        update_foot_pressure();
        
        // Update forward kinematics
        calculate_forward_kinematics(g_leg_state.joint_angles, &g_leg_state.current_pose);
        
        vTaskDelayUntil(&last_update, pdMS_TO_TICKS(LEG_UPDATE_INTERVAL_MS));
    }
}

/**
 * @brief ESP-NOW data received callback
 */
static void espnow_recv_cb(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
    if (len == sizeof(leg_command_t)) {
        leg_command_t* cmd = (leg_command_t*)data;
        
        if (xQueueSend(g_command_queue, cmd, 0) != pdTRUE) {
            ESP_LOGW(TAG, "Command queue full, command dropped");
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
esp_err_t p32_comp_leg_controller_init(void) {
    if (g_initialized) {
        ESP_LOGW(TAG, "Leg controller already initialized");
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Initializing P32 Leg Controller");
    
    // GPIO capacity analysis and optimization decision
    #ifdef LEG_FOOT_INTEGRATION_ENABLED
    ESP_LOGI(TAG, "GPIO Analysis: 22 pins available, 22 pins required (with foot integration)");
    ESP_LOGI(TAG, "Foot integration ENABLED - consolidating foot controller");
    #else
    ESP_LOGI(TAG, "GPIO Analysis: 22 pins available, 12 pins required (leg only)");
    ESP_LOGI(TAG, "Foot integration DISABLED - using separate foot controller");
    #endif
    
    // Initialize state
    memset(&g_leg_state, 0, sizeof(leg_controller_state_t));
    g_leg_state.subsystem_id = LEG_SUBSYSTEM_ID;
    g_leg_state.leg_side = LEG_SIDE_LEFT;  // Set during configuration
    
    // Initialize hardware
    esp_err_t ret = init_pca9685();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PCA9685 initialization failed");
        return ret;
    }
    
    ret = init_mpu6050_imu();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050 initialization failed");
        return ret;
    }
    
    ret = init_foot_sensors();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Foot sensors initialization failed");
        return ret;
    }
    
    ret = init_espnow();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW initialization failed");
        return ret;
    }
    
    // Create command queue
    g_command_queue = xQueueCreate(LEG_COMMAND_QUEUE_SIZE, sizeof(leg_command_t));
    if (!g_command_queue) {
        ESP_LOGE(TAG, "Failed to create command queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Create control tasks
    BaseType_t task_ret = xTaskCreate(
        leg_control_task,
        "leg_ctrl",
        LEG_TASK_STACK_SIZE,
        NULL,
        LEG_TASK_PRIORITY,
        &g_leg_task_handle
    );
    
    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create leg control task");
        return ESP_ERR_NO_MEM;
    }
    
    task_ret = xTaskCreate(
        gait_control_task,
        "gait_ctrl",
        GAIT_TASK_STACK_SIZE,
        NULL,
        GAIT_TASK_PRIORITY,
        &g_gait_task_handle
    );
    
    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create gait control task");
        return ESP_ERR_NO_MEM;
    }
    
    // Move to neutral position
    for (int i = 0; i < LEG_DOF_COUNT; i++) {
        set_joint_position(i, 0.0f);
    }
    
    g_initialized = true;
    ESP_LOGI(TAG, "P32 Leg Controller initialized successfully");
    
    return ESP_OK;
}

/**
 * @brief Component action function
 */
esp_err_t p32_comp_leg_controller_act(p32_component_command_t command) {
    if (!g_initialized) {
        ESP_LOGE(TAG, "Leg controller not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    leg_command_t leg_cmd = {0};
    
    switch (command.type) {
        case P32_CMD_SET_POSITION:
            leg_cmd.command_type = LEG_CMD_SET_POSITION;
            memcpy(leg_cmd.joint_angles, command.data.joint_positions, 
                   sizeof(float) * LEG_DOF_COUNT);
            break;
            
        case P32_CMD_SET_POSE:
            leg_cmd.command_type = LEG_CMD_SET_POSE;
            leg_cmd.target_pose = command.data.target_pose;
            break;
            
        case P32_CMD_START_WALKING:
            leg_cmd.command_type = LEG_CMD_START_WALKING;
            leg_cmd.walking_params = command.data.walking_params;
            break;
            
        case P32_CMD_STOP_WALKING:
            leg_cmd.command_type = LEG_CMD_STOP_WALKING;
            break;
            
        case P32_CMD_EMERGENCY_STOP:
            leg_cmd.command_type = LEG_CMD_EMERGENCY_STOP;
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown command type: %d", command.type);
            return ESP_ERR_NOT_SUPPORTED;
    }
    
    if (xQueueSend(g_command_queue, &leg_cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
        return ESP_OK;
    } else {
        ESP_LOGW(TAG, "Command queue full");
        return ESP_ERR_TIMEOUT;
    }
}

/**
 * @brief Get current leg state
 */
esp_err_t p32_leg_get_state(leg_controller_state_t* state) {
    if (!state || !g_initialized) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *state = g_leg_state;
    return ESP_OK;
}

/**
 * @brief Component cleanup function
 */
esp_err_t p32_comp_leg_controller_cleanup(void) {
    if (!g_initialized) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Cleaning up leg controller");
    
    // Stop tasks
    if (g_leg_task_handle) {
        vTaskDelete(g_leg_task_handle);
        g_leg_task_handle = NULL;
    }
    
    if (g_gait_task_handle) {
        vTaskDelete(g_gait_task_handle);
        g_gait_task_handle = NULL;
    }
    
    // Cleanup queue
    if (g_command_queue) {
        vQueueDelete(g_command_queue);
        g_command_queue = NULL;
    }
    
    // Cleanup hardware
    esp_now_deinit();
    if (g_adc_spi_handle) {
        spi_bus_remove_device(g_adc_spi_handle);
        spi_bus_free(SPI2_HOST);
    }
    i2c_driver_delete(I2C_NUM_0);
    
    g_initialized = false;
    ESP_LOGI(TAG, "Leg controller cleanup completed");
    
    return ESP_OK;
}

#endif // P32_COMP_LEG_CONTROLLER
