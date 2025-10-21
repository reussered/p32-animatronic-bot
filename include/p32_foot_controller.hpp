/**
 * @file p32_foot_controller.h
 * @brief P32 Foot Subsystem Controller Header
 * 
 * Defines structures, constants, and function prototypes for the ESP32-C3
 * foot controller optimized for ankle placement with minimal physical wiring.
 * 
 * Physical Placement Strategy:
 * - Controller PCB mounted directly in ankle joint housing
 * - Minimizes sensor wire lengths (8cm average vs 30cm+ if leg-integrated)
 * - Reduces construction complexity and wire management
 * - Enables modular foot replacement and maintenance
 * 
 * Wiring Optimization Analysis:
 * - FSR Sensors: 8 x 8cm = 64cm total sensor wiring
 * - Toe Servos: 2 x 5cm = 10cm servo control wiring  
 * - Power Feed: 2 x 15cm = 30cm from leg controller
 * - ESP-NOW: 0cm (wireless mesh to leg controller)
 * - Total Physical Wiring: 104cm vs 300cm+ if integrated
 * 
 * @author P32 Animatronic Bot Project
 * @date 2024
 */

#ifndef P32_FOOT_CONTROLLER_H
#define P32_FOOT_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include "p32_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// Foot controller configuration
#define P32_COMP_FOOT_CONTROLLER    1

// GPIO pin assignments (ESP32-C3 optimized for ankle mounting)
#define FOOT_SPI_MOSI_GPIO          2   // Short traces to ADC on ankle PCB
#define FOOT_SPI_MISO_GPIO          3
#define FOOT_SPI_SCLK_GPIO          4
#define FOOT_SPI_CS_GPIO            5
#define FOOT_TOE_BIG_SERVO_GPIO     6   // PWM for big toe servo
#define FOOT_TOE_OTHER_SERVO_GPIO   7   // PWM for other toes servo
#define FOOT_STATUS_LED_GPIO        18  // System status indicator
#define FOOT_ERROR_LED_GPIO         19  // Error status indicator

// System configuration
#define FOOT_SENSOR_COUNT           8
#define TOE_SERVO_COUNT             2
#define FOOT_COMMAND_QUEUE_SIZE     10
#define FOOT_UPDATE_INTERVAL_MS     20
#define FOOT_SENSOR_UPDATE_MS       10
#define FOOT_STATUS_SEND_INTERVAL_MS 100
#define FOOT_TASK_STACK_SIZE        4096
#define FOOT_TASK_PRIORITY          5
#define FOOT_SUBSYSTEM_ID           0x08

// Physical wiring optimization metrics
#define FOOT_MAX_SENSOR_WIRE_LENGTH_CM  10  // Maximum sensor wire length
#define FOOT_TOTAL_WIRING_LENGTH_CM     104 // Total physical wiring
#define FOOT_WIRING_SAVINGS_PERCENT     65  // Savings vs leg integration

// Leg controller MAC address for ESP-NOW communication
#define LEG_CONTROLLER_MAC          {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0x10}

/**
 * @brief Foot side enumeration
 */
typedef enum {
    FOOT_SIDE_LEFT = 0,
    FOOT_SIDE_RIGHT = 1
} foot_side_t;

/**
 * @brief Toe servo identifiers
 */
typedef enum {
    TOE_BIG = 0,        ///< Big toe servo
    TOE_OTHER = 1       ///< Other toes servo (grouped)
} toe_servo_id_t;

/**
 * @brief Foot loading patterns
 */
typedef enum {
    LOADING_NORMAL = 0,     ///< Normal distributed loading
    LOADING_HEEL_STRIKE,    ///< Heel-heavy loading (landing)
    LOADING_TOE_OFF,        ///< Toe-heavy loading (push-off)
    LOADING_FLAT_FOOT,      ///< Flat foot loading (standing)
    LOADING_UNBALANCED      ///< Asymmetric loading (instability)
} loading_pattern_t;

/**
 * @brief Pressure sensor position structure
 */
typedef struct {
    float x;                ///< X position relative to foot center (mm)
    float y;                ///< Y position relative to foot center (mm)
    const char* name;       ///< Sensor location name
} sensor_position_t;

/**
 * @brief Foot command types
 */
typedef enum {
    FOOT_CMD_SET_TOE_ANGLES = 1,    ///< Set toe articulation angles
    FOOT_CMD_CALIBRATE_SENSORS,     ///< Calibrate pressure sensors
    FOOT_CMD_GET_STATUS,            ///< Request status update
    FOOT_CMD_EMERGENCY_STOP         ///< Emergency stop and safe position
} foot_command_type_t;

/**
 * @brief Foot command structure
 */
typedef struct {
    foot_command_type_t command_type;   ///< Command type
    uint32_t timestamp;                 ///< Command timestamp (ms)
    float toe_angles[TOE_SERVO_COUNT];  ///< Toe servo angles (degrees)
    uint32_t sequence_number;           ///< Command sequence for reliability
} foot_command_t;

/**
 * @brief Foot status message types (ESP-NOW communication)
 */
typedef enum {
    FOOT_MSG_STATUS_UPDATE = 1,     ///< Regular status update
    FOOT_MSG_CONTACT_EVENT,         ///< Ground contact event
    FOOT_MSG_STABILITY_ALERT,       ///< Balance/stability alert
    FOOT_MSG_SENSOR_ERROR           ///< Sensor malfunction report
} foot_message_type_t;

/**
 * @brief Foot status message structure (sent to leg controller)
 */
typedef struct {
    foot_message_type_t message_type;       ///< Message type
    uint32_t timestamp;                     ///< Message timestamp (ms)
    
    // Ground contact and pressure data
    bool ground_contact;                    ///< Overall ground contact status
    float total_pressure;                   ///< Average pressure percentage
    float pressure_sensors[FOOT_SENSOR_COUNT]; ///< Individual sensor readings
    
    // Balance and stability analysis
    float cop_x, cop_y;                     ///< Center of pressure (mm)
    loading_pattern_t loading_pattern;      ///< Current loading pattern
    bool stability_detected;                ///< Foot stability status
    float balance_left_right;               ///< Left/right balance (%)
    float balance_fore_aft;                 ///< Fore/aft balance (%)
    
    // System health
    bool sensor_errors[FOOT_SENSOR_COUNT];  ///< Per-sensor error flags
    uint32_t message_sequence;              ///< Message sequence number
} foot_status_message_t;

/**
 * @brief Foot controller state structure
 */
typedef struct {
    // Basic state
    uint8_t subsystem_id;                   ///< Subsystem identifier
    foot_side_t foot_side;                  ///< Left or right foot
    uint32_t last_update_time;              ///< Last update timestamp (ms)
    uint32_t last_sensor_update;            ///< Last sensor reading time (ms)
    
    // Pressure sensing
    float pressure_sensors[FOOT_SENSOR_COUNT]; ///< Sensor readings (0-100%)
    uint16_t sensor_baseline[FOOT_SENSOR_COUNT]; ///< Baseline ADC values
    bool sensor_contact[FOOT_SENSOR_COUNT]; ///< Per-sensor contact detection
    bool ground_contact;                    ///< Overall ground contact
    float total_pressure;                   ///< Average pressure
    
    // Center of pressure and balance
    float cop_x, cop_y;                     ///< Center of pressure (mm)
    loading_pattern_t loading_pattern;      ///< Current loading pattern
    bool stability_detected;                ///< Stability status
    float balance_left_right;               ///< Left/right balance (%)
    float balance_fore_aft;                 ///< Fore/aft balance (%)
    
    // Toe articulation
    float toe_angles[TOE_SERVO_COUNT];      ///< Current toe angles (degrees)
    float target_toe_angles[TOE_SERVO_COUNT]; ///< Target toe angles
    
    // System health and diagnostics
    bool hardware_initialized;              ///< Hardware initialization status
    bool calibration_completed;             ///< Sensor calibration status
    uint32_t loop_count;                    ///< Control loop iterations
    uint32_t commands_received;             ///< ESP-NOW commands received
    uint32_t commands_dropped;              ///< Commands dropped (queue full)
    uint32_t messages_sent;                 ///< Status messages sent
    uint32_t communication_errors;          ///< Communication error count
    
    // Physical wiring metrics (for optimization tracking)
    uint16_t sensor_wire_length_cm;         ///< Total sensor wiring length
    uint16_t servo_wire_length_cm;          ///< Total servo wiring length
    uint16_t power_wire_length_cm;          ///< Power supply wiring length
    uint16_t total_physical_wiring_cm;      ///< Total physical wiring
} foot_controller_state_t;

/**
 * @brief Foot calibration data
 */
typedef struct {
    uint16_t sensor_min_values[FOOT_SENSOR_COUNT];  ///< Minimum ADC readings
    uint16_t sensor_max_values[FOOT_SENSOR_COUNT];  ///< Maximum ADC readings
    float sensor_sensitivity[FOOT_SENSOR_COUNT];    ///< Calibrated sensitivity
    bool calibration_valid[FOOT_SENSOR_COUNT];      ///< Per-sensor calibration status
    uint32_t calibration_timestamp;                 ///< Last calibration time
} foot_calibration_data_t;

/**
 * @brief Physical wiring analysis structure
 */
typedef struct {
    // Current configuration metrics
    uint16_t current_wiring_length_cm;      ///< Actual wiring length measured
    uint16_t estimated_leg_integration_cm;  ///< Estimated if leg-integrated
    float wiring_savings_percent;           ///< Percentage savings achieved
    
    // Construction complexity metrics
    uint8_t wire_bundle_count;              ///< Number of wire bundles
    uint8_t connection_points;              ///< Number of connection points
    float assembly_time_minutes;            ///< Estimated assembly time
    float maintenance_difficulty_score;     ///< Maintenance complexity (1-10)
    
    // Cost analysis
    float wire_cost_usd;                    ///< Wire and connector costs
    float labor_cost_savings_usd;          ///< Labor savings vs integration
    float total_cost_benefit_usd;           ///< Total cost benefit
} wiring_analysis_t;

// Function prototypes

/**
 * @brief Initialize foot controller subsystem
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_foot_controller_init(void);

/**
 * @brief Execute foot controller action
 * @param command Command to execute
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_foot_controller_act(p32_component_command_t command);

/**
 * @brief Get current foot controller state
 * @param state Pointer to state structure to fill
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if state is NULL
 */
esp_err_t p32_foot_get_state(foot_controller_state_t* state);

/**
 * @brief Set toe articulation angles
 * @param big_toe_angle Big toe angle in degrees (-90 to +90)
 * @param other_toes_angle Other toes angle in degrees (-90 to +90)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_set_toe_angles(float big_toe_angle, float other_toes_angle);

/**
 * @brief Get pressure sensor reading
 * @param sensor_id Sensor identifier (0-7)
 * @param pressure_percent Pointer to store pressure percentage
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_get_pressure(uint8_t sensor_id, float* pressure_percent);

/**
 * @brief Get all pressure sensor readings
 * @param pressures Array to store 8 pressure values (0-100%)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_get_all_pressures(float pressures[FOOT_SENSOR_COUNT]);

/**
 * @brief Check ground contact status
 * @return true if foot is in contact with ground, false otherwise
 */
bool p32_foot_is_ground_contact(void);

/**
 * @brief Get center of pressure
 * @param cop_x Pointer to store X coordinate (mm from foot center)
 * @param cop_y Pointer to store Y coordinate (mm from foot center)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_get_center_of_pressure(float* cop_x, float* cop_y);

/**
 * @brief Get balance analysis
 * @param left_right_percent Pointer to store left/right balance (%)
 * @param fore_aft_percent Pointer to store fore/aft balance (%)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_get_balance(float* left_right_percent, float* fore_aft_percent);

/**
 * @brief Get current loading pattern
 * @return Loading pattern enumeration value
 */
loading_pattern_t p32_foot_get_loading_pattern(void);

/**
 * @brief Check stability status
 * @return true if foot is stable, false if unstable
 */
bool p32_foot_is_stable(void);

/**
 * @brief Calibrate pressure sensors
 * @param calibration_data Pointer to calibration data structure
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_calibrate_sensors(foot_calibration_data_t* calibration_data);

/**
 * @brief Get sensor position information
 * @param sensor_id Sensor identifier (0-7)
 * @param position Pointer to position structure to fill
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_get_sensor_position(uint8_t sensor_id, sensor_position_t* position);

/**
 * @brief Analyze physical wiring configuration
 * @param analysis Pointer to wiring analysis structure to fill
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_analyze_wiring(wiring_analysis_t* analysis);

/**
 * @brief Get physical placement metrics
 * @param total_wiring_cm Pointer to store total wiring length
 * @param assembly_complexity Pointer to store complexity score (1-10)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_get_placement_metrics(uint16_t* total_wiring_cm, float* assembly_complexity);

/**
 * @brief Emergency stop - move to safe position
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_emergency_stop(void);

/**
 * @brief Reset controller to factory defaults
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_foot_factory_reset(void);

/**
 * @brief Clean up and deinitialize foot controller
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_foot_controller_cleanup(void);

// Convenience macros for common operations
#define FOOT_CONTACT()              p32_foot_is_ground_contact()
#define FOOT_STABLE()               p32_foot_is_stable()
#define FOOT_TOES_UP()              p32_foot_set_toe_angles(15.0f, 10.0f)
#define FOOT_TOES_DOWN()            p32_foot_set_toe_angles(-10.0f, -5.0f)
#define FOOT_TOES_NEUTRAL()         p32_foot_set_toe_angles(0.0f, 0.0f)

// Physical placement optimization constants
#define ANKLE_MOUNT_WIRING_CM       104     // Total wiring for ankle mount
#define LEG_INTEGRATION_WIRING_CM   300     // Estimated for leg integration
#define WIRING_SAVINGS_PERCENT      65      // Savings with ankle mount
#define ASSEMBLY_TIME_SAVINGS_MIN   45      // Assembly time savings (minutes)

// Pressure sensing zones (for analysis)
#define HEEL_SENSORS                {0, 1}          // Heel sensor indices
#define MIDFOOT_SENSORS             {2, 3}          // Midfoot sensor indices  
#define FOREFOOT_SENSORS            {4, 5, 6, 7}   // Forefoot sensor indices

#ifdef __cplusplus
}
#endif

#endif // P32_FOOT_CONTROLLER_H