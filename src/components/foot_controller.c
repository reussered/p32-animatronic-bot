/**
 * @file foot_controller.c
 * @brief P32 Foot Subsystem Controller Implementation
 * 
 * ESP32-C3 foot controller optimized for ankle placement with minimal wiring.
 * Physical placement strategy: Mount controller directly in ankle joint to minimize
 * sensor wire lengths and reduce overall system complexity.
 * 
 * Features:
 * - 8-channel pressure sensor monitoring (FSR array in foot sole)
 * - 2DOF toe articulation (big toe + other toes)
 * - Ground contact detection and center-of-pressure calculation
 * - Real-time balance feedback for leg controller
 * - Short-wire sensor connections (<10cm average)
 * - ESP-NOW mesh communication with leg controller
 * 
 * Physical Wiring Optimization:
 * - Controller mounted in ankle joint housing
 * - FSR sensors: 8 wires × 8cm average = 64cm total sensor wiring
 * - Toe servos: 2 wires × 5cm = 10cm servo wiring
 * - Power: Single 5V feed from leg controller (2 wires × 15cm = 30cm)
 * - Total physical wiring: ~104cm vs ~300cm if integrated with leg controller
 * 
 * Hardware Configuration:
 * - ESP32-C3-WROOM-02 microcontroller (ankle-mounted)
 * - 8x Force-sensitive resistors (foot sole pressure points)
 * - 2x SG90 micro servos (toe articulation)
 * - 1x MCP3008 8-channel ADC (analog sensor interface)
 * - Compact PCB design for ankle integration
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
#include <driver/spi_master.h>
#include <driver/ledc.h>
#include <esp_now.h>

#include "p32_foot_controller.h"
#include "p32_core.h"

#ifdef P32_COMP_FOOT_CONTROLLER

static const char* TAG = "P32_FOOT_CTRL";

// Foot controller state
static foot_controller_state_t g_foot_state = {0};
static bool g_initialized = false;
static TaskHandle_t g_foot_task_handle = NULL;
static QueueHandle_t g_command_queue = NULL;

// SPI configuration for MCP3008 ADC
static spi_device_handle_t g_adc_spi_handle = NULL;

// Pressure sensor positions relative to foot center (mm)
// Optimized for humanoid foot pressure distribution
static const sensor_position_t g_sensor_positions[FOOT_SENSOR_COUNT] = {
    {-25, -80, "heel_left"},      // Left heel
    {25,  -80, "heel_right"},     // Right heel
    {-15, -40, "midfoot_left"},   // Left midfoot
    {15,  -40, "midfoot_right"},  // Right midfoot
    {-20, 0,   "ball_left"},      // Left ball of foot
    {20,  0,   "ball_right"},     // Right ball of foot
    {-10, 40,  "big_toe"},        // Big toe area
    {10,  40,  "other_toes"}      // Other toes area
};

// Ground contact detection parameters
static const float CONTACT_THRESHOLD_HIGH = 15.0f;  // % pressure to register contact
static const float CONTACT_THRESHOLD_LOW = 5.0f;    // % pressure to release contact
static const float COP_SMOOTHING_FACTOR = 0.3f;     // Center of pressure filtering

/**
 * @brief Initialize SPI interface for MCP3008 ADC
 * 
 * Physical placement: ADC chip mounted directly on ankle PCB,
 * minimal trace lengths to pressure sensors in foot sole.
 */
static esp_err_t init_mcp3008_adc(void) {
    ESP_LOGI(TAG, "Initializing MCP3008 ADC for pressure sensors");
    
    // SPI bus configuration - optimized for short traces
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = FOOT_SPI_MOSI_GPIO,
        .miso_io_num = FOOT_SPI_MISO_GPIO,
        .sclk_io_num = FOOT_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32
    };
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // SPI device configuration for MCP3008
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1000000,  // 1MHz - adequate for 8 channels at 1kHz
        .mode = 0,                  // CPOL=0, CPHA=0
        .spics_io_num = FOOT_SPI_CS_GPIO,
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
 * @brief Initialize toe servo PWM control
 * 
 * Physical placement: Servos mounted in toe joints with short control wires
 * to ankle-mounted controller PCB.
 */
static esp_err_t init_toe_servos(void) {
    ESP_LOGI(TAG, "Initializing toe servo control");
    
    // Configure LED Controller for servo PWM (50Hz)
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 50,  // 50Hz for servos
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    
    // Configure channels for toe servos
    ledc_channel_config_t ledc_channel[TOE_SERVO_COUNT] = {
        {
            .channel    = LEDC_CHANNEL_0,
            .duty       = 0,
            .gpio_num   = FOOT_TOE_BIG_SERVO_GPIO,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        },
        {
            .channel    = LEDC_CHANNEL_1,
            .duty       = 0,
            .gpio_num   = FOOT_TOE_OTHER_SERVO_GPIO,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
        }
    };
    
    for (int i = 0; i < TOE_SERVO_COUNT; i++) {
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[i]));
    }
    
    // Set initial positions (neutral)
    set_toe_servo_position(TOE_BIG, 0.0f);
    set_toe_servo_position(TOE_OTHER, 0.0f);
    
    ESP_LOGI(TAG, "Toe servos initialized successfully");
    return ESP_OK;
}

/**
 * @brief Set toe servo position
 * 
 * @param toe_id Toe identifier (TOE_BIG or TOE_OTHER)
 * @param angle_degrees Servo angle (-90 to +90 degrees)
 */
static esp_err_t set_toe_servo_position(toe_servo_id_t toe_id, float angle_degrees) {
    if (toe_id >= TOE_SERVO_COUNT) {
        ESP_LOGE(TAG, "Invalid toe servo ID: %d", toe_id);
        return ESP_ERR_INVALID_ARG;
    }
    
    // Clamp angle to valid range
    if (angle_degrees < -90.0f) angle_degrees = -90.0f;
    if (angle_degrees > 90.0f) angle_degrees = 90.0f;
    
    // Convert angle to duty cycle
    // SG90 servo: 1ms (0°) to 2ms (180°) pulse width at 50Hz
    // 13-bit resolution at 50Hz: 8192 counts per 20ms period
    // 1ms = 409 counts, 2ms = 819 counts
    uint32_t duty = 409 + (uint32_t)((angle_degrees + 90.0f) * 410.0f / 180.0f);
    
    ledc_channel_t channel = (toe_id == TOE_BIG) ? LEDC_CHANNEL_0 : LEDC_CHANNEL_1;
    esp_err_t ret = ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    if (ret == ESP_OK) {
        ret = ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
    }
    
    // Update state
    g_foot_state.toe_angles[toe_id] = angle_degrees;
    
    return ret;
}

/**
 * @brief Read pressure sensor value from MCP3008 ADC
 * 
 * Physical optimization: All sensors connected via short wires (<10cm)
 * to minimize noise and wire management complexity.
 */
static uint16_t read_pressure_sensor(uint8_t channel) {
    if (channel >= FOOT_SENSOR_COUNT) {
        ESP_LOGW(TAG, "Invalid sensor channel: %d", channel);
        return 0;
    }
    
    // MCP3008 SPI command format
    uint8_t tx_data[3] = {
        0x01,                           // Start bit
        (0x08 | channel) << 4,          // Single-ended mode + channel
        0x00
    };
    uint8_t rx_data[3] = {0};
    
    spi_transaction_t trans = {
        .length = 24,  // 3 bytes × 8 bits
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    
    esp_err_t ret = spi_device_transmit(g_adc_spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "ADC read failed for channel %d: %s", channel, esp_err_to_name(ret));
        return 0;
    }
    
    // Extract 10-bit ADC result
    uint16_t adc_value = ((rx_data[1] & 0x03) << 8) | rx_data[2];
    
    return adc_value;
}

/**
 * @brief Update all pressure sensor readings with smoothing
 */
static void update_pressure_sensors(void) {
    static uint32_t last_update = 0;
    uint32_t current_time = esp_timer_get_time() / 1000;
    
    // Limit update frequency to reduce noise
    if (current_time - last_update < FOOT_SENSOR_UPDATE_MS) {
        return;
    }
    last_update = current_time;
    
    bool any_contact = false;
    float total_pressure = 0.0f;
    float weighted_x = 0.0f, weighted_y = 0.0f;
    
    for (int i = 0; i < FOOT_SENSOR_COUNT; i++) {
        // Read raw ADC value
        uint16_t raw_adc = read_pressure_sensor(i);
        
        // Convert to pressure percentage (0-100%)
        // FSR calibration: 0-1023 ADC counts mapped to 0-100% pressure
        float pressure_percent = (float)raw_adc / 1023.0f * 100.0f;
        
        // Apply exponential smoothing to reduce noise
        float alpha = 0.2f;  // Smoothing factor
        g_foot_state.pressure_sensors[i] = alpha * pressure_percent + 
                                          (1.0f - alpha) * g_foot_state.pressure_sensors[i];
        
        // Ground contact detection with hysteresis
        bool prev_contact = g_foot_state.sensor_contact[i];
        if (!prev_contact && g_foot_state.pressure_sensors[i] > CONTACT_THRESHOLD_HIGH) {
            g_foot_state.sensor_contact[i] = true;
            ESP_LOGD(TAG, "Sensor %d (%s) contact detected (%.1f%%)", 
                     i, g_sensor_positions[i].name, g_foot_state.pressure_sensors[i]);
        } else if (prev_contact && g_foot_state.pressure_sensors[i] < CONTACT_THRESHOLD_LOW) {
            g_foot_state.sensor_contact[i] = false;
            ESP_LOGD(TAG, "Sensor %d (%s) contact released", i, g_sensor_positions[i].name);
        }
        
        if (g_foot_state.sensor_contact[i]) {
            any_contact = true;
        }
        
        // Accumulate for center of pressure calculation
        float pressure = g_foot_state.pressure_sensors[i];
        total_pressure += pressure;
        weighted_x += pressure * g_sensor_positions[i].x;
        weighted_y += pressure * g_sensor_positions[i].y;
    }
    
    // Update ground contact state
    g_foot_state.ground_contact = any_contact;
    g_foot_state.total_pressure = total_pressure / FOOT_SENSOR_COUNT;
    
    // Calculate center of pressure with smoothing
    if (total_pressure > 1.0f) {  // Avoid division by zero
        float new_cop_x = weighted_x / total_pressure;
        float new_cop_y = weighted_y / total_pressure;
        
        // Apply smoothing to center of pressure
        g_foot_state.cop_x = COP_SMOOTHING_FACTOR * new_cop_x + 
                            (1.0f - COP_SMOOTHING_FACTOR) * g_foot_state.cop_x;
        g_foot_state.cop_y = COP_SMOOTHING_FACTOR * new_cop_y + 
                            (1.0f - COP_SMOOTHING_FACTOR) * g_foot_state.cop_y;
    }
    
    // Update timestamp
    g_foot_state.last_sensor_update = current_time;
}

/**
 * @brief Analyze foot stability and balance
 */
static void analyze_foot_stability(void) {
    // Calculate pressure distribution patterns
    float heel_pressure = (g_foot_state.pressure_sensors[0] + g_foot_state.pressure_sensors[1]) / 2.0f;
    float midfoot_pressure = (g_foot_state.pressure_sensors[2] + g_foot_state.pressure_sensors[3]) / 2.0f;
    float forefoot_pressure = (g_foot_state.pressure_sensors[4] + g_foot_state.pressure_sensors[5] + 
                              g_foot_state.pressure_sensors[6] + g_foot_state.pressure_sensors[7]) / 4.0f;
    
    // Determine foot loading pattern
    if (heel_pressure > forefoot_pressure * 1.5f) {
        g_foot_state.loading_pattern = LOADING_HEEL_STRIKE;
    } else if (forefoot_pressure > heel_pressure * 1.5f) {
        g_foot_state.loading_pattern = LOADING_TOE_OFF;
    } else if (midfoot_pressure > 20.0f) {
        g_foot_state.loading_pattern = LOADING_FLAT_FOOT;
    } else {
        g_foot_state.loading_pattern = LOADING_NORMAL;
    }
    
    // Calculate balance metrics
    float left_pressure = (g_foot_state.pressure_sensors[0] + g_foot_state.pressure_sensors[2] + 
                          g_foot_state.pressure_sensors[4] + g_foot_state.pressure_sensors[6]) / 4.0f;
    float right_pressure = (g_foot_state.pressure_sensors[1] + g_foot_state.pressure_sensors[3] + 
                           g_foot_state.pressure_sensors[5] + g_foot_state.pressure_sensors[7]) / 4.0f;
    
    g_foot_state.balance_left_right = (left_pressure - right_pressure) / 
                                     (left_pressure + right_pressure + 1.0f) * 100.0f;
    
    g_foot_state.balance_fore_aft = (forefoot_pressure - heel_pressure) / 
                                   (forefoot_pressure + heel_pressure + 1.0f) * 100.0f;
    
    // Detect stability issues
    bool prev_stable = g_foot_state.stability_detected;
    float balance_threshold = 30.0f;  // % imbalance threshold
    
    g_foot_state.stability_detected = (fabsf(g_foot_state.balance_left_right) < balance_threshold) &&
                                     (fabsf(g_foot_state.balance_fore_aft) < balance_threshold) &&
                                     (g_foot_state.total_pressure > 10.0f);
    
    if (prev_stable != g_foot_state.stability_detected) {
        ESP_LOGI(TAG, "Stability changed: %s (L/R: %.1f%%, F/A: %.1f%%)",
                 g_foot_state.stability_detected ? "STABLE" : "UNSTABLE",
                 g_foot_state.balance_left_right, g_foot_state.balance_fore_aft);
    }
}

/**
 * @brief Send status update to leg controller via ESP-NOW
 */
static void send_status_to_leg_controller(void) {
    foot_status_message_t status_msg = {
        .message_type = FOOT_MSG_STATUS_UPDATE,
        .timestamp = esp_timer_get_time() / 1000,
        .ground_contact = g_foot_state.ground_contact,
        .total_pressure = g_foot_state.total_pressure,
        .cop_x = g_foot_state.cop_x,
        .cop_y = g_foot_state.cop_y,
        .loading_pattern = g_foot_state.loading_pattern,
        .stability_detected = g_foot_state.stability_detected,
        .balance_left_right = g_foot_state.balance_left_right,
        .balance_fore_aft = g_foot_state.balance_fore_aft
    };
    
    // Copy individual sensor readings
    memcpy(status_msg.pressure_sensors, g_foot_state.pressure_sensors, 
           sizeof(float) * FOOT_SENSOR_COUNT);
    
    // Send via ESP-NOW to leg controller
    esp_err_t ret = esp_now_send(LEG_CONTROLLER_MAC, (uint8_t*)&status_msg, sizeof(status_msg));
    if (ret != ESP_OK) {
        g_foot_state.communication_errors++;
        ESP_LOGD(TAG, "Failed to send status to leg controller: %s", esp_err_to_name(ret));
    } else {
        g_foot_state.messages_sent++;
    }
}

/**
 * @brief Main foot control task
 */
static void foot_control_task(void* pvParameters) {
    ESP_LOGI(TAG, "Foot control task started");
    
    foot_command_t command;
    TickType_t last_wake_time = xTaskGetTickCount();
    uint32_t status_send_counter = 0;
    
    while (1) {
        // Process commands from queue
        if (xQueueReceive(g_command_queue, &command, 0) == pdTRUE) {
            switch (command.command_type) {
                case FOOT_CMD_SET_TOE_ANGLES:
                    for (int i = 0; i < TOE_SERVO_COUNT; i++) {
                        set_toe_servo_position(i, command.toe_angles[i]);
                    }
                    ESP_LOGD(TAG, "Set toe angles: big=%.1f°, other=%.1f°", 
                             command.toe_angles[0], command.toe_angles[1]);
                    break;
                    
                case FOOT_CMD_CALIBRATE_SENSORS:
                    ESP_LOGI(TAG, "Starting sensor calibration");
                    // Baseline calibration - assume no load
                    for (int i = 0; i < FOOT_SENSOR_COUNT; i++) {
                        g_foot_state.sensor_baseline[i] = read_pressure_sensor(i);
                    }
                    g_foot_state.calibration_completed = true;
                    ESP_LOGI(TAG, "Sensor calibration completed");
                    break;
                    
                case FOOT_CMD_EMERGENCY_STOP:
                    ESP_LOGW(TAG, "Emergency stop - neutral toe positions");
                    set_toe_servo_position(TOE_BIG, 0.0f);
                    set_toe_servo_position(TOE_OTHER, 0.0f);
                    break;
                    
                default:
                    ESP_LOGW(TAG, "Unknown command type: %d", command.command_type);
                    break;
            }
        }
        
        // Update sensors and analysis
        update_pressure_sensors();
        analyze_foot_stability();
        
        // Send status updates to leg controller periodically
        status_send_counter++;
        if (status_send_counter >= (FOOT_STATUS_SEND_INTERVAL_MS / FOOT_UPDATE_INTERVAL_MS)) {
            send_status_to_leg_controller();
            status_send_counter = 0;
        }
        
        // Update system metrics
        g_foot_state.loop_count++;
        g_foot_state.last_update_time = esp_timer_get_time() / 1000;
        
        // Maintain precise timing
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(FOOT_UPDATE_INTERVAL_MS));
    }
}

/**
 * @brief ESP-NOW data received callback
 */
static void espnow_recv_cb(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
    if (len == sizeof(foot_command_t)) {
        foot_command_t* cmd = (foot_command_t*)data;
        
        if (xQueueSend(g_command_queue, cmd, 0) != pdTRUE) {
            ESP_LOGW(TAG, "Command queue full, dropping command");
            g_foot_state.commands_dropped++;
        } else {
            g_foot_state.commands_received++;
        }
    } else {
        ESP_LOGW(TAG, "Invalid message length: %d", len);
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
    
    // Add leg controller as peer
    esp_now_peer_info_t peer_info = {0};
    memcpy(peer_info.peer_addr, LEG_CONTROLLER_MAC, 6);
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
 * @brief Initialize status GPIO pins
 */
static esp_err_t init_status_gpio(void) {
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << FOOT_STATUS_LED_GPIO) | (1ULL << FOOT_ERROR_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    esp_err_t ret = gpio_config(&gpio_conf);
    if (ret == ESP_OK) {
        gpio_set_level(FOOT_STATUS_LED_GPIO, 1);  // Status LED on
        gpio_set_level(FOOT_ERROR_LED_GPIO, 0);   // Error LED off
    }
    
    return ret;
}

/**
 * @brief Component initialization function
 */
esp_err_t p32_comp_foot_controller_init(void) {
    if (g_initialized) {
        ESP_LOGW(TAG, "Foot controller already initialized");
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Initializing P32 Foot Controller (ankle-mounted, minimal wiring)");
    
    // Initialize state
    memset(&g_foot_state, 0, sizeof(foot_controller_state_t));
    g_foot_state.subsystem_id = FOOT_SUBSYSTEM_ID;
    g_foot_state.foot_side = FOOT_SIDE_LEFT;  // Set during configuration
    
    // Initialize hardware components
    esp_err_t ret = init_status_gpio();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GPIO initialization failed");
        return ret;
    }
    
    ret = init_mcp3008_adc();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ADC initialization failed");
        return ret;
    }
    
    ret = init_toe_servos();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Servo initialization failed");
        return ret;
    }
    
    ret = init_espnow();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW initialization failed");
        return ret;
    }
    
    // Create command queue
    g_command_queue = xQueueCreate(FOOT_COMMAND_QUEUE_SIZE, sizeof(foot_command_t));
    if (!g_command_queue) {
        ESP_LOGE(TAG, "Failed to create command queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Create main control task
    BaseType_t task_ret = xTaskCreate(
        foot_control_task,
        "foot_ctrl",
        FOOT_TASK_STACK_SIZE,
        NULL,
        FOOT_TASK_PRIORITY,
        &g_foot_task_handle
    );
    
    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create foot control task");
        return ESP_ERR_NO_MEM;
    }
    
    // Initial sensor baseline reading
    vTaskDelay(pdMS_TO_TICKS(100));  // Allow sensors to settle
    for (int i = 0; i < FOOT_SENSOR_COUNT; i++) {
        g_foot_state.sensor_baseline[i] = read_pressure_sensor(i);
    }
    
    g_foot_state.hardware_initialized = true;
    g_initialized = true;
    
    ESP_LOGI(TAG, "P32 Foot Controller initialized successfully");
    ESP_LOGI(TAG, "Physical placement: Ankle-mounted for minimal sensor wiring");
    ESP_LOGI(TAG, "Wiring optimization: ~104cm total vs ~300cm if leg-integrated");
    
    return ESP_OK;
}

/**
 * @brief Component action function
 */
esp_err_t p32_comp_foot_controller_act(p32_component_command_t command) {
    if (!g_initialized) {
        ESP_LOGE(TAG, "Foot controller not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    foot_command_t foot_cmd = {0};
    foot_cmd.timestamp = esp_timer_get_time() / 1000;
    
    switch (command.type) {
        case P32_CMD_SET_TOE_ANGLES:
            foot_cmd.command_type = FOOT_CMD_SET_TOE_ANGLES;
            foot_cmd.toe_angles[TOE_BIG] = command.data.toe_angles.big_toe_angle;
            foot_cmd.toe_angles[TOE_OTHER] = command.data.toe_angles.other_toes_angle;
            break;
            
        case P32_CMD_CALIBRATE:
            foot_cmd.command_type = FOOT_CMD_CALIBRATE_SENSORS;
            break;
            
        case P32_CMD_EMERGENCY_STOP:
            foot_cmd.command_type = FOOT_CMD_EMERGENCY_STOP;
            break;
            
        case P32_CMD_GET_STATUS:
            // Status is continuously sent via ESP-NOW
            return ESP_OK;
            
        default:
            ESP_LOGW(TAG, "Unknown command type: %d", command.type);
            return ESP_ERR_NOT_SUPPORTED;
    }
    
    if (xQueueSend(g_command_queue, &foot_cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
        return ESP_OK;
    } else {
        ESP_LOGW(TAG, "Command queue full");
        return ESP_ERR_TIMEOUT;
    }
}

/**
 * @brief Get current foot controller state
 */
esp_err_t p32_foot_get_state(foot_controller_state_t* state) {
    if (!state || !g_initialized) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *state = g_foot_state;
    return ESP_OK;
}

/**
 * @brief Component cleanup function
 */
esp_err_t p32_comp_foot_controller_cleanup(void) {
    if (!g_initialized) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Cleaning up foot controller");
    
    // Stop control task
    if (g_foot_task_handle) {
        vTaskDelete(g_foot_task_handle);
        g_foot_task_handle = NULL;
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
    
    // Turn off LEDs
    gpio_set_level(FOOT_STATUS_LED_GPIO, 0);
    gpio_set_level(FOOT_ERROR_LED_GPIO, 0);
    
    g_initialized = false;
    ESP_LOGI(TAG, "Foot controller cleanup completed");
    
    return ESP_OK;
}

#endif // P32_COMP_FOOT_CONTROLLER