/**
 * @file p32_leg_controller.h
 * @brief P32 Leg Subsystem Controller Header
 * 
 * Defines structures, constants, and function prototypes for the ESP32-C3
 * leg controller subsystem with 6DOF bipedal locomotion and optional foot integration.
 * 
 * Memory & GPIO Optimization Strategy:
 * - ESP32-C3: 400KB SRAM, 22 GPIO pins -> Leg only OR leg+foot if memory allows
 * - ESP32-S3: 512KB SRAM + 8MB PSRAM, 45 GPIO pins -> Can handle multiple subsystems
 * - ESP32: 520KB SRAM, 34 GPIO pins -> Good middle ground option
 * 
 * @author P32 Animatronic Bot Project
 * @date 2024
 */

#ifndef P32_LEG_CONTROLLER_H
#define P32_LEG_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include "p32_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// Leg controller configuration
#define P32_COMP_LEG_CONTROLLER     1

// ESP32 Chip Selection and Memory Analysis
#ifdef CONFIG_IDF_TARGET_ESP32C3
    #define ESP32_MEMORY_CAPACITY       "400KB SRAM"
    #define ESP32_GPIO_COUNT            22
    #define LEG_FOOT_INTEGRATION_ENABLED 1  // Try integration if GPIO allows
    #define LEG_MEMORY_CONSERVATIVE     1   // Use memory-optimized algorithms
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define ESP32_MEMORY_CAPACITY       "512KB SRAM + 8MB PSRAM"
    #define ESP32_GPIO_COUNT            45
    #define LEG_FOOT_INTEGRATION_ENABLED 1  // Definitely can integrate
    #define LEG_ARM_INTEGRATION_ENABLED  1  // Could also handle arm if needed
    #define LEG_MEMORY_CONSERVATIVE     0   // Can use full-featured algorithms
#elif defined(CONFIG_IDF_TARGET_ESP32)
    #define ESP32_MEMORY_CAPACITY       "520KB SRAM"
    #define ESP32_GPIO_COUNT            34
    #define LEG_FOOT_INTEGRATION_ENABLED 1  // Good capacity for integration
    #define LEG_MEMORY_CONSERVATIVE     0   // Standard memory usage
#else
    #error "Unsupported ESP32 target - please use ESP32, ESP32-C3, or ESP32-S3"
#endif

// GPIO Pin Allocation (optimized per chip)
#ifdef CONFIG_IDF_TARGET_ESP32C3
    // ESP32-C3 GPIO allocation (22 pins total)
    #define LEG_I2C_SDA_GPIO            4   // Shared I2C for PCA9685 + IMU
    #define LEG_I2C_SCL_GPIO            5
    #define LEG_ENCODER_1_GPIO          6   // Hip flexion encoder
    #define LEG_ENCODER_2_GPIO          7   // Hip abduction encoder
    #define LEG_ENCODER_3_GPIO          8   // Hip rotation encoder
    #define LEG_ENCODER_4_GPIO          9   // Knee encoder
    #define LEG_STATUS_LED_GPIO         18
    #define LEG_ERROR_LED_GPIO          19
    #define LEG_ESTOP_GPIO              0   // Boot button as emergency stop
    
    // Foot integration pins (if enabled)
    #ifdef LEG_FOOT_INTEGRATION_ENABLED
        #define LEG_SPI_MOSI_GPIO       2   // SPI for ADC
        #define LEG_SPI_MISO_GPIO       3
        #define LEG_SPI_SCLK_GPIO       21
        #define LEG_SPI_CS_GPIO         1
        #define LEG_FSR_1_GPIO          10  // Direct FSR readings
        #define LEG_FSR_2_GPIO          20
        #define LEG_TOE_SERVO_1_GPIO    11  // Additional toe servos via PCA9685
        #define LEG_TOE_SERVO_2_GPIO    12
        // Total: 22 pins (at capacity limit)
    #endif
    
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    // ESP32-S3 GPIO allocation (45 pins available - can handle multiple subsystems)
    #define LEG_I2C_SDA_GPIO            21
    #define LEG_I2C_SCL_GPIO            22
    #define LEG_ENCODER_1_GPIO          4
    #define LEG_ENCODER_2_GPIO          5
    #define LEG_ENCODER_3_GPIO          6
    #define LEG_ENCODER_4_GPIO          7
    #define LEG_STATUS_LED_GPIO         18
    #define LEG_ERROR_LED_GPIO          19
    #define LEG_ESTOP_GPIO              0
    
    // Full foot integration
    #define LEG_SPI_MOSI_GPIO           11
    #define LEG_SPI_MISO_GPIO           12
    #define LEG_SPI_SCLK_GPIO           13
    #define LEG_SPI_CS_GPIO             14
    #define LEG_FSR_1_GPIO              15  // All 8 FSR sensors
    #define LEG_FSR_2_GPIO              16
    #define LEG_FSR_3_GPIO              17
    #define LEG_FSR_4_GPIO              8
    #define LEG_FSR_5_GPIO              9
    #define LEG_FSR_6_GPIO              10
    #define LEG_FSR_7_GPIO              1
    #define LEG_FSR_8_GPIO              2
    #define LEG_TOE_SERVO_1_GPIO        3   // Toe articulation servos
    #define LEG_TOE_SERVO_2_GPIO        46
    
    // Could potentially handle arm integration too
    #ifdef LEG_ARM_INTEGRATION_ENABLED
        #define LEG_ARM_I2C_SDA_GPIO    35  // Separate I2C for arm servos
        #define LEG_ARM_I2C_SCL_GPIO    36
        // Additional 15+ pins available for arm control
    #endif
    
#elif defined(CONFIG_IDF_TARGET_ESP32)
    // ESP32 GPIO allocation (34 pins available)
    #define LEG_I2C_SDA_GPIO            21
    #define LEG_I2C_SCL_GPIO            22
    #define LEG_ENCODER_1_GPIO          4
    #define LEG_ENCODER_2_GPIO          5
    #define LEG_ENCODER_3_GPIO          18
    #define LEG_ENCODER_4_GPIO          19
    #define LEG_STATUS_LED_GPIO         2
    #define LEG_ERROR_LED_GPIO          15
    #define LEG_ESTOP_GPIO              0
    
    // Foot integration
    #define LEG_SPI_MOSI_GPIO           23
    #define LEG_SPI_MISO_GPIO           25
    #define LEG_SPI_SCLK_GPIO           26
    #define LEG_SPI_CS_GPIO             27
    #define LEG_FSR_1_GPIO              32
    #define LEG_FSR_2_GPIO              33
    #define LEG_FSR_3_GPIO              34
    #define LEG_FSR_4_GPIO              35
    #define LEG_TOE_SERVO_1_GPIO        12
    #define LEG_TOE_SERVO_2_GPIO        13
    // Total: 20 pins used, 14 available for expansion
#endif

// Hardware component addresses
#define PCA9685_ADDRESS             0x40
#define MPU6050_ADDRESS             0x68
#define PCA9685_MODE1               0x00
#define PCA9685_PRESCALE            0xFE
#define PCA9685_LED0_ON_L           0x06
#define MPU6050_PWR_MGMT_1          0x6B
#define MPU6050_ACCEL_CONFIG        0x1C

// Servo configuration
#define LEG_SERVO_MIN_PWM           1000
#define LEG_SERVO_MAX_PWM           2000
#define LEG_SERVO_NEUTRAL_PWM       1500

// System configuration
#define LEG_DOF_COUNT               6
#define LEG_COMMAND_QUEUE_SIZE      20
#define LEG_UPDATE_INTERVAL_MS      20
#define GAIT_UPDATE_INTERVAL_MS     10
#define LEG_TASK_STACK_SIZE         8192
#define GAIT_TASK_STACK_SIZE        4096
#define LEG_TASK_PRIORITY           6
#define GAIT_TASK_PRIORITY          7
#define LEG_SUBSYSTEM_ID            0x06

// Memory usage analysis (varies by chip)
#ifdef LEG_MEMORY_CONSERVATIVE
    #define LEG_TRAJECTORY_BUFFER_SIZE  50   // Smaller buffers for C3
    #define LEG_MAX_GAIT_POINTS         100
    #define LEG_SENSOR_HISTORY_SIZE     20
#else
    #define LEG_TRAJECTORY_BUFFER_SIZE  200  // Larger buffers for S3/ESP32
    #define LEG_MAX_GAIT_POINTS         500
    #define LEG_SENSOR_HISTORY_SIZE     100
#endif

// Foot sensor configuration (if integrated)
#ifdef LEG_FOOT_INTEGRATION_ENABLED
    #define FOOT_SENSOR_COUNT           8
    #define FOOT_FORCE_THRESHOLD        10.0f   // Percentage
    #define FOOT_TOE_COUNT              2       // Big toe + other toes
#endif

// Leg segment lengths (in meters)
#define LEG_SEGMENT_LENGTHS         {0.45f, 0.40f, 0.15f}  // Thigh, shin, foot

// Torso master controller MAC for ESP-NOW
#define TORSO_MASTER_MAC            {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}

/**
 * @brief Leg side enumeration
 */
typedef enum {
    LEG_SIDE_LEFT = 0,
    LEG_SIDE_RIGHT = 1
} leg_side_t;

/**
 * @brief Joint limits structure
 */
typedef struct {
    float min_angle;        ///< Minimum joint angle (degrees)
    float max_angle;        ///< Maximum joint angle (degrees)
    float max_velocity;     ///< Maximum joint velocity (degrees/sec)
} joint_limits_t;

/**
 * @brief 3D pose structure for end-effector
 */
typedef struct {
    float x, y, z;          ///< Position in meters
    float roll, pitch, yaw; ///< Orientation in radians
} leg_pose_t;

/**
 * @brief Walking parameters
 */
typedef struct {
    float step_length;      ///< Step length in meters
    float step_height;      ///< Step height in meters
    float step_frequency;   ///< Steps per second
    float stance_ratio;     ///< Stance phase ratio (0.0-1.0)
    float body_height;      ///< Body height above ground
    float foot_separation;  ///< Distance between feet
} walking_parameters_t;

/**
 * @brief Gait controller state
 */
typedef struct {
    bool gait_enabled;          ///< Gait generation enabled
    float current_phase;        ///< Current gait phase (0.0-1.0)
    leg_pose_t target_pose;     ///< Current target pose
    uint32_t step_count;        ///< Total steps taken
    float average_speed;        ///< Average walking speed (m/s)
} gait_controller_t;

/**
 * @brief Leg command types
 */
typedef enum {
    LEG_CMD_SET_POSITION = 1,   ///< Set joint positions directly
    LEG_CMD_SET_POSE,           ///< Set end-effector pose
    LEG_CMD_START_WALKING,      ///< Start walking with parameters
    LEG_CMD_STOP_WALKING,       ///< Stop walking
    LEG_CMD_CALIBRATE,          ///< Perform calibration
    LEG_CMD_EMERGENCY_STOP      ///< Emergency stop
} leg_command_type_t;

/**
 * @brief Leg command structure
 */
typedef struct {
    leg_command_type_t command_type;        ///< Command type
    float joint_angles[LEG_DOF_COUNT];      ///< Joint angles (degrees)
    leg_pose_t target_pose;                 ///< Target pose
    walking_parameters_t walking_params;    ///< Walking parameters
    uint32_t duration_ms;                   ///< Command duration
    uint32_t sequence_number;               ///< Command sequence
} leg_command_t;

/**
 * @brief Leg controller state
 */
typedef struct {
    // Basic state
    uint8_t subsystem_id;                   ///< Subsystem identifier
    leg_side_t leg_side;                    ///< Left or right leg
    uint32_t last_update_time;              ///< Last update timestamp (ms)
    
    // Joint state
    float joint_angles[LEG_DOF_COUNT];      ///< Current joint angles (degrees)
    float joint_velocities[LEG_DOF_COUNT];  ///< Joint velocities (deg/s)
    float joint_torques[LEG_DOF_COUNT];     ///< Joint torques (estimated)
    
    // End-effector state
    leg_pose_t current_pose;                ///< Current foot pose
    leg_pose_t target_pose;                 ///< Target foot pose
    
    // Foot sensors (if integrated)
    #ifdef LEG_FOOT_INTEGRATION_ENABLED
    float foot_pressure[FOOT_SENSOR_COUNT]; ///< Pressure sensor readings (%)
    bool ground_contact;                    ///< Ground contact detection
    float total_ground_force;               ///< Total ground reaction force
    float cop_x, cop_y;                     ///< Center of pressure (mm)
    #endif
    
    // IMU data
    float imu_accel[3];                     ///< Accelerometer readings (g)
    float imu_gyro[3];                      ///< Gyroscope readings (deg/s)
    float leg_angle;                        ///< Leg segment angle
    
    // System health
    bool hardware_initialized;              ///< Hardware status
    bool calibration_completed;             ///< Calibration status
    float system_voltage;                   ///< System voltage (V)
    float servo_currents[LEG_DOF_COUNT];    ///< Servo current draw (mA)
    uint32_t communication_errors;          ///< ESP-NOW error count
    
    // Performance metrics
    uint32_t total_commands;                ///< Commands processed
    uint32_t successful_steps;              ///< Successful walking steps
    float max_speed_achieved;               ///< Maximum walking speed
    uint32_t uptime_seconds;                ///< System uptime
} leg_controller_state_t;

/**
 * @brief Memory usage statistics (for optimization)
 */
typedef struct {
    uint32_t heap_free_bytes;               ///< Available heap memory
    uint32_t heap_min_free_bytes;           ///< Minimum free heap recorded
    uint32_t stack_high_water_mark;         ///< Task stack usage
    uint16_t trajectory_buffer_usage;       ///< Trajectory buffer utilization (%)
    uint16_t command_queue_usage;           ///< Command queue utilization (%)
    bool memory_pressure_detected;          ///< Memory pressure flag
} leg_memory_stats_t;

/**
 * @brief ESP32 chip optimization profile
 */
typedef struct {
    const char* chip_model;                 ///< ESP32 chip model
    uint32_t sram_size_kb;                  ///< SRAM capacity
    uint32_t psram_size_mb;                 ///< PSRAM capacity (if available)
    uint8_t gpio_count;                     ///< Available GPIO pins
    bool foot_integration_recommended;      ///< Foot integration feasible
    bool arm_integration_possible;          ///< Could handle arm too
    uint16_t max_trajectory_points;         ///< Recommended trajectory buffer
    uint8_t recommended_task_priority;      ///< Optimal task priority
} esp32_optimization_profile_t;

// Function prototypes

/**
 * @brief Initialize leg controller subsystem
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_leg_controller_init(void);

/**
 * @brief Execute leg controller action
 * @param command Command to execute
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_leg_controller_act(p32_component_command_t command);

/**
 * @brief Get current leg controller state
 * @param state Pointer to state structure to fill
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if state is NULL
 */
esp_err_t p32_leg_get_state(leg_controller_state_t* state);

/**
 * @brief Set walking parameters
 * @param params Walking parameters structure
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_set_walking_params(const walking_parameters_t* params);

/**
 * @brief Start walking with current parameters
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_start_walking(void);

/**
 * @brief Stop walking gracefully
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_stop_walking(void);

/**
 * @brief Set target end-effector pose
 * @param pose Target pose structure
 * @param duration_ms Transition duration
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_set_target_pose(const leg_pose_t* pose, uint32_t duration_ms);

/**
 * @brief Set individual joint angle
 * @param joint_id Joint identifier (0-5)
 * @param angle_degrees Target angle in degrees
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_set_joint_angle(uint8_t joint_id, float angle_degrees);

/**
 * @brief Get current end-effector pose
 * @param pose Pointer to pose structure to fill
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_get_current_pose(leg_pose_t* pose);

/**
 * @brief Perform inverse kinematics calculation
 * @param target_pose Target end-effector pose
 * @param joint_angles Output joint angles array
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if unreachable
 */
esp_err_t p32_leg_inverse_kinematics(const leg_pose_t* target_pose, 
                                    float joint_angles[LEG_DOF_COUNT]);

/**
 * @brief Perform forward kinematics calculation  
 * @param joint_angles Input joint angles array
 * @param end_effector_pose Output end-effector pose
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_forward_kinematics(const float joint_angles[LEG_DOF_COUNT],
                                     leg_pose_t* end_effector_pose);

/**
 * @brief Check if target pose is reachable
 * @param pose Pose to check
 * @return true if reachable, false otherwise
 */
bool p32_leg_is_pose_reachable(const leg_pose_t* pose);

// Foot integration functions (if enabled)
#ifdef LEG_FOOT_INTEGRATION_ENABLED

/**
 * @brief Get foot pressure sensor reading
 * @param sensor_id Sensor identifier (0-7)
 * @param pressure_percent Pointer to store pressure percentage
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_get_foot_pressure(uint8_t sensor_id, float* pressure_percent);

/**
 * @brief Check ground contact status
 * @return true if foot is in contact with ground
 */
bool p32_leg_is_ground_contact(void);

/**
 * @brief Get center of pressure
 * @param cop_x Pointer to store X coordinate (mm)
 * @param cop_y Pointer to store Y coordinate (mm)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_get_center_of_pressure(float* cop_x, float* cop_y);

/**
 * @brief Set toe articulation
 * @param toe_id Toe identifier (0-1)
 * @param angle_degrees Toe angle in degrees
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_set_toe_angle(uint8_t toe_id, float angle_degrees);

#endif // LEG_FOOT_INTEGRATION_ENABLED

/**
 * @brief Get memory usage statistics
 * @param stats Pointer to statistics structure
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_get_memory_stats(leg_memory_stats_t* stats);

/**
 * @brief Get ESP32 optimization profile
 * @param profile Pointer to profile structure
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_get_chip_profile(esp32_optimization_profile_t* profile);

/**
 * @brief Optimize controller for current ESP32 chip
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_optimize_for_chip(void);

/**
 * @brief Perform emergency stop
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_leg_emergency_stop(void);

/**
 * @brief Clean up and deinitialize leg controller
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_leg_controller_cleanup(void);

// Convenience macros for common operations
#define LEG_STAND()             p32_leg_set_target_pose(&(leg_pose_t){0, 0, -0.85f, 0, 0, 0}, 2000)
#define LEG_LIFT()              p32_leg_set_target_pose(&(leg_pose_t){0, 0.1f, -0.75f, 0, 0, 0}, 1000)  
#define LEG_STEP_FORWARD(dist)  p32_leg_set_target_pose(&(leg_pose_t){dist, 0, -0.85f, 0, 0, 0}, 800)
#define LEG_IS_WALKING()        (g_gait_controller.gait_enabled)

// Memory optimization macros
#ifdef LEG_MEMORY_CONSERVATIVE
    #define LEG_USE_FLOAT_PRECISION     0   // Use fixed point for C3
    #define LEG_ENABLE_TRAJECTORY_CACHE 0   // Disable caching
    #define LEG_MAX_CONCURRENT_TASKS    2   // Limit task count
#else
    #define LEG_USE_FLOAT_PRECISION     1   // Full floating point
    #define LEG_ENABLE_TRAJECTORY_CACHE 1   // Enable caching
    #define LEG_MAX_CONCURRENT_TASKS    4   // More tasks allowed
#endif

// GPIO allocation validation
#if defined(CONFIG_IDF_TARGET_ESP32C3) && defined(LEG_FOOT_INTEGRATION_ENABLED)
    #if (ESP32_GPIO_COUNT < 22)
        #warning "ESP32-C3 GPIO capacity may be insufficient for full leg+foot integration"
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif // P32_LEG_CONTROLLER_H