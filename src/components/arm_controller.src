/*
 * P32 Animatronic Bot - Arm Controller Implementation  
 * ESP32-C3 Controller for Arm Subsystems (Left/Right)
 * 
 * Features:
 * - 7DOF arm articulation (3DOF shoulder, 1DOF elbow, 3DOF wrist)
 * - Inverse kinematics for reach positioning
 * - ESP-NOW mesh networking with torso master
 * - Real-time force feedback and collision detection
 * - Coordinated bilateral arm movement
 */

#include "p32_arm_controller.hpp"
#include "esp_now.h"
#include "esp_wifi.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <math.h>

// Component configuration flags
#ifdef P32_COMP_ARM_CONTROLLER

// Arm controller state
static p32_arm_state_t arm_state = {0};
static QueueHandle_t arm_command_queue;
static TaskHandle_t arm_control_task_handle;
static bool is_left_arm = true; // Set during initialization

// Component initialization
esp_err_t p32_comp_arm_controller_init(bool left_arm) {
    ESP_LOGI(TAG, "Initializing P32 Arm Controller (%s)", left_arm ? "LEFT" : "RIGHT");
    
    is_left_arm = left_arm;
    arm_state.arm_id = left_arm ? ARM_ID_LEFT : ARM_ID_RIGHT;
    
    // Initialize GPIO pins
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << ARM_STATUS_LED_PIN) | 
                       (1ULL << ARM_ERROR_LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));
    
    // Initialize emergency stop input
    gpio_conf.pin_bit_mask = (1ULL << ARM_ESTOP_PIN);
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));
    
    // Initialize I2C master for servo control
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = ARM_I2C_SDA_PIN,
        .scl_io_num = ARM_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };
    ESP_ERROR_CHECK(i2c_param_config(ARM_I2C_PORT, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(ARM_I2C_PORT, i2c_conf.mode, 0, 0, 0));
    
    // Initialize PCA9685 servo driver
    ESP_ERROR_CHECK(p32_arm_servo_init());
    
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
    ESP_ERROR_CHECK(esp_now_register_recv_cb(p32_arm_espnow_recv_cb));
    
    // Create command queue and control task
    arm_command_queue = xQueueCreate(10, sizeof(p32_arm_command_t));
    if (arm_command_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create arm command queue");
        return ESP_FAIL;
    }
    
    // Create arm control task
    BaseType_t task_result = xTaskCreate(
        p32_arm_control_task,
        "arm_control",
        4096,
        NULL,
        4,
        &arm_control_task_handle
    );
    
    if (task_result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create arm control task");
        return ESP_FAIL;
    }
    
    // Set initial LED status
    gpio_set_level(ARM_STATUS_LED_PIN, 1);
    gpio_set_level(ARM_ERROR_LED_PIN, 0);
    
    ESP_LOGI(TAG, "P32 Arm Controller initialized successfully");
    return ESP_OK;
}

// Servo control initialization
esp_err_t p32_arm_servo_init(void) {
    uint8_t pca_address = is_left_arm ? PCA9685_I2C_ADDRESS_LEFT : PCA9685_I2C_ADDRESS_RIGHT;
    
    // Initialize PCA9685 servo driver
    ESP_ERROR_CHECK(pca9685_init(ARM_I2C_PORT, pca_address));
    ESP_ERROR_CHECK(pca9685_set_pwm_freq(50)); // 50Hz for servos
    
    // Set all servos to neutral position
    for (int i = 0; i < ARM_SERVO_COUNT; i++) {
        ESP_ERROR_CHECK(pca9685_set_pwm(i, ARM_SERVO_NEUTRAL_PWM));
        arm_state.joint_angles[i] = 0.0f;
    }
    
    ESP_LOGI(TAG, "Arm servo system initialized");
    return ESP_OK;
}

// Component action function
esp_err_t p32_comp_arm_controller_act(p32_arm_command_t command) {
    // Send command to control task via queue
    BaseType_t result = xQueueSend(arm_command_queue, &command, pdMS_TO_TICKS(100));
    if (result != pdPASS) {
        ESP_LOGW(TAG, "Failed to queue arm command");
        return ESP_FAIL;
    }
    return ESP_OK;
}

// Main arm control task
static void p32_arm_control_task(void *pvParameters) {
    p32_arm_command_t command;
    TickType_t last_update = xTaskGetTickCount();
    
    ESP_LOGI(TAG, "Arm control task started");
    
    while (1) {
        // Process commands from queue
        if (xQueueReceive(arm_command_queue, &command, pdMS_TO_TICKS(20)) == pdPASS) {
            p32_arm_process_command(&command);
        }
        
        // Periodic monitoring and updates
        TickType_t current_time = xTaskGetTickCount();
        if ((current_time - last_update) >= pdMS_TO_TICKS(50)) {
            p32_arm_system_monitor();
            last_update = current_time;
        }
        
        // Emergency stop check
        if (gpio_get_level(ARM_ESTOP_PIN) == 0) {
            ESP_LOGW(TAG, "Emergency stop activated!");
            p32_arm_emergency_stop();
        }
    }
}

// Process arm commands
static void p32_arm_process_command(p32_arm_command_t *command) {
    switch (command->command_type) {
        case ARM_CMD_JOINT_POSITION:
            p32_arm_set_joint_angles(command->joint_angles, command->transition_time);
            break;
            
        case ARM_CMD_REACH_POSITION:
            p32_arm_reach_target(command->target_position, command->transition_time);
            break;
            
        case ARM_CMD_ARM_SWING:
            p32_arm_swing_motion(command->swing_angle, command->swing_frequency);
            break;
            
        case ARM_CMD_GESTURE:
            p32_arm_execute_gesture(command->gesture_id);
            break;
            
        case ARM_CMD_EMERGENCY_STOP:
            p32_arm_emergency_stop();
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown arm command: %d", command->command_type);
            break;
    }
}

// Joint angle control
esp_err_t p32_arm_set_joint_angles(float joint_angles[7], uint32_t transition_time) {
    ESP_LOGI(TAG, "Setting arm joint angles over %dms", transition_time);
    
    // Validate and clamp joint angles to safe ranges
    float safe_angles[7];
    for (int i = 0; i < ARM_SERVO_COUNT; i++) {
        safe_angles[i] = fmaxf(ARM_JOINT_MIN_ANGLES[i], 
                              fminf(ARM_JOINT_MAX_ANGLES[i], joint_angles[i]));
        
        if (safe_angles[i] != joint_angles[i]) {
            ESP_LOGW(TAG, "Joint %d angle clamped from %.2f to %.2f", 
                    i, joint_angles[i], safe_angles[i]);
        }
    }
    
    // Apply servo commands
    for (int i = 0; i < ARM_SERVO_COUNT; i++) {
        uint16_t pwm_value = p32_arm_angle_to_pwm(safe_angles[i], i);
        ESP_ERROR_CHECK(pca9685_set_pwm(i, pwm_value));
        arm_state.joint_angles[i] = safe_angles[i];
    }
    
    arm_state.last_movement_time = esp_timer_get_time() / 1000;
    return ESP_OK;
}

// Inverse kinematics reach positioning
esp_err_t p32_arm_reach_target(p32_vector3_t target, uint32_t transition_time) {
    ESP_LOGI(TAG, "Reaching to [%.2f, %.2f, %.2f]", target.x, target.y, target.z);
    
    // Calculate inverse kinematics
    float joint_angles[7];
    bool ik_success = p32_arm_calculate_inverse_kinematics(target, joint_angles);
    
    if (ik_success) {
        return p32_arm_set_joint_angles(joint_angles, transition_time);
    } else {
        ESP_LOGW(TAG, "Target position unreachable");
        return ESP_FAIL;
    }
}

// Arm swing motion for walking
esp_err_t p32_arm_swing_motion(float swing_angle, float frequency) {
    // Generate natural arm swing based on gait phase
    float shoulder_swing = swing_angle * sinf(arm_state.gait_phase);
    float elbow_compensation = -0.3f * fabs(shoulder_swing); // Natural elbow bend
    
    float swing_joints[7] = {
        shoulder_swing,     // Shoulder flexion
        0.0f,              // Shoulder abduction
        0.0f,              // Shoulder rotation
        elbow_compensation, // Elbow flexion
        0.0f,              // Wrist flexion
        0.0f,              // Wrist rotation
        0.0f               // Wrist abduction
    };
    
    arm_state.gait_phase += frequency * 0.02f; // Increment phase
    if (arm_state.gait_phase >= 2.0f * M_PI) {
        arm_state.gait_phase = 0.0f;
    }
    
    return p32_arm_set_joint_angles(swing_joints, 50); // Fast update for smooth swing
}

// Gesture execution
esp_err_t p32_arm_execute_gesture(arm_gesture_t gesture_id) {
    ESP_LOGI(TAG, "Executing gesture: %d", gesture_id);
    
    float gesture_angles[7];
    uint32_t gesture_duration = 2000; // Default 2 seconds
    
    switch (gesture_id) {
        case ARM_GESTURE_WAVE:
            // Wave gesture - shoulder abduction with wrist rotation
            gesture_angles[0] = 0.0f;    // Shoulder flexion
            gesture_angles[1] = 90.0f;   // Shoulder abduction (raise arm)
            gesture_angles[2] = 0.0f;    // Shoulder rotation
            gesture_angles[3] = -45.0f;  // Elbow flexion
            gesture_angles[4] = 0.0f;    // Wrist flexion
            gesture_angles[5] = 45.0f;   // Wrist rotation (wave motion)
            gesture_angles[6] = 0.0f;    // Wrist abduction
            break;
            
        case ARM_GESTURE_POINT:
            // Pointing gesture - extended arm with index finger
            gesture_angles[0] = 45.0f;   // Shoulder flexion (forward point)
            gesture_angles[1] = 0.0f;    // Shoulder abduction
            gesture_angles[2] = 0.0f;    // Shoulder rotation
            gesture_angles[3] = 0.0f;    // Elbow extension
            gesture_angles[4] = -15.0f;  // Slight wrist extension
            gesture_angles[5] = 0.0f;    // Wrist rotation
            gesture_angles[6] = 0.0f;    // Wrist abduction
            break;
            
        case ARM_GESTURE_REST:
            // Rest position - arms at sides
            for (int i = 0; i < 7; i++) {
                gesture_angles[i] = 0.0f;
            }
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown gesture ID: %d", gesture_id);
            return ESP_FAIL;
    }
    
    return p32_arm_set_joint_angles(gesture_angles, gesture_duration);
}

// Inverse kinematics calculation
static bool p32_arm_calculate_inverse_kinematics(p32_vector3_t target, float joint_angles[7]) {
    // Simplified 7DOF inverse kinematics for humanoid arm
    // This is a basic implementation - could be enhanced with more sophisticated algorithms
    
    // Arm segment lengths (in mm)
    const float upper_arm_length = 250.0f;
    const float forearm_length = 200.0f;
    
    // Calculate distance to target
    float distance = sqrtf(target.x * target.x + target.y * target.y + target.z * target.z);
    
    // Check if target is reachable
    float max_reach = upper_arm_length + forearm_length;
    if (distance > max_reach) {
        ESP_LOGW(TAG, "Target distance %.2f exceeds max reach %.2f", distance, max_reach);
        return false;
    }
    
    // Calculate shoulder angles
    joint_angles[0] = atan2f(target.z, sqrtf(target.x * target.x + target.y * target.y)); // Shoulder flexion
    joint_angles[1] = atan2f(target.y, target.x); // Shoulder abduction
    joint_angles[2] = 0.0f; // Shoulder rotation (can be optimized)
    
    // Calculate elbow angle using cosine rule
    float cos_elbow = (upper_arm_length * upper_arm_length + forearm_length * forearm_length - distance * distance) /
                     (2.0f * upper_arm_length * forearm_length);
    cos_elbow = fmaxf(-1.0f, fminf(1.0f, cos_elbow)); // Clamp to valid range
    joint_angles[3] = M_PI - acosf(cos_elbow); // Elbow flexion
    
    // Set wrist angles (can be adjusted for orientation)
    joint_angles[4] = 0.0f; // Wrist flexion
    joint_angles[5] = 0.0f; // Wrist rotation
    joint_angles[6] = 0.0f; // Wrist abduction
    
    // Convert from radians to degrees
    for (int i = 0; i < 7; i++) {
        joint_angles[i] *= 180.0f / M_PI;
    }
    
    return true;
}

// System monitoring
static void p32_arm_system_monitor(void) {
    // Monitor servo currents and temperatures
    arm_state.total_current = p32_arm_read_servo_currents();
    
    // Check for overcurrent
    if (arm_state.total_current > ARM_MAX_CURRENT_THRESHOLD) {
        ESP_LOGW(TAG, "Arm overcurrent detected: %.2fA", arm_state.total_current);
        // Reduce servo power to prevent damage
        p32_arm_reduce_power(0.8f);
    }
    
    // Update uptime
    arm_state.uptime_ms = esp_timer_get_time() / 1000;
    
    // Send status to torso master periodically
    if ((arm_state.uptime_ms - arm_state.last_status_send) > ARM_STATUS_REPORT_INTERVAL) {
        p32_arm_send_status_update();
        arm_state.last_status_send = arm_state.uptime_ms;
    }
    
    // Blink status LED
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(ARM_STATUS_LED_PIN, led_state ? 1 : 0);
}

// Emergency stop implementation
static void p32_arm_emergency_stop(void) {
    ESP_LOGW(TAG, "ARM EMERGENCY STOP - Disabling all servos");
    
    // Disable all servo outputs
    for (int i = 0; i < ARM_SERVO_COUNT; i++) {
        ESP_ERROR_CHECK(pca9685_set_pwm(i, 0)); // 0 = servo off
        arm_state.joint_angles[i] = 0.0f;
    }
    
    // Set error LED
    gpio_set_level(ARM_ERROR_LED_PIN, 1);
    gpio_set_level(ARM_STATUS_LED_PIN, 0);
    
    arm_state.emergency_stop_active = true;
}

// ESP-NOW communication callback
static void p32_arm_espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len == sizeof(p32_arm_command_t)) {
        p32_arm_command_t *command = (p32_arm_command_t*)data;
        p32_comp_arm_controller_act(*command);
    } else if (len == sizeof(p32_system_status_t)) {
        // Handle system-wide status updates from torso
        p32_system_status_t *status = (p32_system_status_t*)data;
        if (status->emergency_stop_active) {
            p32_arm_emergency_stop();
        }
    }
}

// Utility functions
static uint16_t p32_arm_angle_to_pwm(float angle, uint8_t joint_index) {
    // Convert angle to PWM value with joint-specific calibration
    float pwm_range = ARM_SERVO_MAX_PWM - ARM_SERVO_MIN_PWM;
    
    // Normalize angle to 0-1 range based on joint limits
    float min_angle = ARM_JOINT_MIN_ANGLES[joint_index];
    float max_angle = ARM_JOINT_MAX_ANGLES[joint_index];
    float normalized_angle = (angle - min_angle) / (max_angle - min_angle);
    
    // Apply joint-specific calibration offset if needed
    normalized_angle += ARM_JOINT_CALIBRATION_OFFSETS[joint_index];
    
    // Clamp and convert to PWM
    normalized_angle = fmaxf(0.0f, fminf(1.0f, normalized_angle));
    return ARM_SERVO_MIN_PWM + (uint16_t)(normalized_angle * pwm_range);
}

static float p32_arm_read_servo_currents(void) {
    // Read total servo current consumption
    // This is a placeholder implementation
    return 3.0f; // Simulated current reading
}

static void p32_arm_reduce_power(float power_factor) {
    // Reduce servo power by scaling PWM values
    for (int i = 0; i < ARM_SERVO_COUNT; i++) {
        uint16_t current_pwm = pca9685_get_pwm(i);
        uint16_t reduced_pwm = ARM_SERVO_NEUTRAL_PWM + 
                              (current_pwm - ARM_SERVO_NEUTRAL_PWM) * power_factor;
        ESP_ERROR_CHECK(pca9685_set_pwm(i, reduced_pwm));
    }
}

static void p32_arm_send_status_update(void) {
    p32_subsystem_status_t status = {
        .subsystem_id = arm_state.arm_id,
        .online = true,
        .battery_level = 100.0f, // Placeholder
        .temperature = 25.0f,    // Placeholder
        .uptime_ms = arm_state.uptime_ms,
        .error_flags = arm_state.emergency_stop_active ? 0x0001 : 0x0000
    };
    
    // Send to torso master controller
    uint8_t torso_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01};
    esp_now_send(torso_mac, (uint8_t*)&status, sizeof(status));
}

#endif // P32_COMP_ARM_CONTROLLER
