/**
 * @file p32_hand_controller.h
 * @brief P32 Hand Subsystem Controller Header
 * 
 * Defines structures, constants, and function prototypes for the ESP32-C3
 * hand controller subsystem with 5-finger articulated control.
 * 
 * Features:
 * - 5DOF finger control with gesture library
 * - Force-sensitive pressure sensing per finger
 * - Object grip detection and adaptive control
 * - ESP-NOW mesh communication
 * 
 * @author P32 Animatronic Bot Project
 * @date 2024
 */

#ifndef P32_HAND_CONTROLLER_H
#define P32_HAND_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include "p32_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// Hand controller configuration
#define P32_COMP_HAND_CONTROLLER    1

// Hardware pin definitions
#define HAND_I2C_SDA_GPIO           4
#define HAND_I2C_SCL_GPIO           5
#define HAND_SPI_MOSI_GPIO          6
#define HAND_SPI_MISO_GPIO          7
#define HAND_SPI_SCLK_GPIO          8
#define HAND_SPI_CS_GPIO            9

// PCA9685 servo driver configuration
#define PCA9685_ADDRESS             0x40
#define PCA9685_MODE1               0x00
#define PCA9685_PRESCALE            0xFE
#define PCA9685_LED0_ON_L           0x06

// Servo channel assignments
#define SERVO_THUMB                 0
#define SERVO_INDEX                 1
#define SERVO_MIDDLE                2
#define SERVO_RING                  3
#define SERVO_PINKY                 4

// Force sensor thresholds (percentage)
#define FORCE_THRESHOLD_LOW         5.0f
#define FORCE_THRESHOLD_HIGH        15.0f

// System configuration
#define MAX_GESTURES                20
#define GESTURE_QUEUE_SIZE          10
#define HAND_UPDATE_INTERVAL_MS     20
#define HAND_TASK_STACK_SIZE        4096
#define HAND_TASK_PRIORITY          5
#define HAND_SUBSYSTEM_ID           0x03

// Torso master controller MAC address for ESP-NOW
#define TORSO_MASTER_MAC            {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}

/**
 * @brief Finger position structure
 */
typedef struct {
    uint16_t thumb;         ///< Thumb servo position (0-180 degrees)
    uint16_t index;         ///< Index finger servo position (0-180 degrees)  
    uint16_t middle;        ///< Middle finger servo position (0-180 degrees)
    uint16_t ring;          ///< Ring finger servo position (0-180 degrees)
    uint16_t pinky;         ///< Pinky finger servo position (0-180 degrees)
    const char* name;       ///< Gesture name (for gesture library entries)
} finger_position_t;

/**
 * @brief Hand command types
 */
typedef enum {
    HAND_CMD_GESTURE = 1,           ///< Execute predefined gesture
    HAND_CMD_DIRECT_POSITION,       ///< Direct finger position control
    HAND_CMD_CALIBRATE,             ///< Perform calibration sequence
    HAND_CMD_GET_STATUS             ///< Request status update
} hand_command_type_t;

/**
 * @brief Hand command structure for ESP-NOW communication
 */
typedef struct {
    hand_command_type_t command_type;   ///< Command type
    uint8_t gesture_id;                 ///< Gesture ID (for gesture commands)
    uint16_t duration_ms;               ///< Duration for smooth transitions
    finger_position_t finger_positions; ///< Direct position data
    uint32_t sequence_number;           ///< Command sequence for reliability
} hand_command_t;

/**
 * @brief Gesture command structure for internal queue
 */
typedef struct {
    uint8_t gesture_id;                 ///< Target gesture ID
    uint16_t duration_ms;               ///< Transition duration in milliseconds
} gesture_command_t;

/**
 * @brief Hand controller state structure
 */
typedef struct {
    // Current state
    uint8_t subsystem_id;               ///< Subsystem identifier
    finger_position_t current_position; ///< Current finger positions
    uint8_t current_gesture_id;         ///< Currently active gesture
    uint32_t last_gesture_time;         ///< Time of last gesture (ms)
    uint32_t last_update_time;          ///< Last sensor update time (ms)
    
    // Force sensing
    float force_sensors[5];             ///< Force sensor readings (0-100%)
    bool finger_contact[5];             ///< Contact detection per finger
    bool grip_detected;                 ///< Overall grip detection state
    float grip_strength;                ///< Calculated grip strength (0-100%)
    float grip_center_x;                ///< Center of pressure X (mm)
    float grip_center_y;                ///< Center of pressure Y (mm)
    
    // Calibration data
    uint16_t servo_min_positions[5];    ///< Minimum servo positions
    uint16_t servo_max_positions[5];    ///< Maximum servo positions
    uint16_t servo_neutral_positions[5]; ///< Neutral servo positions
    
    // Communication
    uint32_t last_command_sequence;     ///< Last processed command sequence
    uint32_t communication_errors;      ///< Count of communication errors
    uint32_t gestures_executed;         ///< Total gestures executed
    
    // System health
    bool hardware_initialized;          ///< Hardware initialization status
    bool calibration_completed;         ///< Calibration completion status
    float system_voltage;               ///< System voltage monitoring
    float servo_current[5];             ///< Per-servo current monitoring
} hand_controller_state_t;

/**
 * @brief Hand performance metrics
 */
typedef struct {
    uint32_t total_gestures;            ///< Total gestures executed
    uint32_t average_gesture_time_ms;   ///< Average gesture execution time
    uint32_t grip_events;               ///< Number of grip detection events
    float max_grip_strength;            ///< Maximum recorded grip strength
    uint32_t communication_packets;     ///< Total ESP-NOW packets received
    uint32_t packet_errors;             ///< Communication packet errors
    float uptime_hours;                 ///< System uptime in hours
} hand_performance_metrics_t;

/**
 * @brief Predefined gesture IDs
 */
typedef enum {
    GESTURE_OPEN_HAND = 0,              ///< Open flat hand
    GESTURE_CLOSED_FIST = 1,            ///< Closed fist
    GESTURE_POINT_INDEX = 2,            ///< Point with index finger
    GESTURE_PEACE_SIGN = 3,             ///< V-sign with index and middle
    GESTURE_ROCK_HORNS = 4,             ///< Rock horns gesture
    GESTURE_PRECISION_GRIP = 5,         ///< Precision grip (thumb + index)
    GESTURE_POWER_GRIP = 6,             ///< Full power grip
    GESTURE_LIGHT_GRIP = 7,             ///< Light object grip
    GESTURE_STRONG_GRIP = 8,            ///< Strong grip for heavy objects
    GESTURE_MIDDLE_FINGER = 9,          ///< Middle finger gesture
    GESTURE_THUMBS_UP = 10,             ///< Thumbs up gesture
    GESTURE_THUMBS_DOWN = 11,           ///< Thumbs down gesture
    GESTURE_CALL_ME = 12,               ///< Phone gesture
    GESTURE_ONE = 13,                   ///< Number one
    GESTURE_TWO = 14,                   ///< Number two
    GESTURE_THREE = 15,                 ///< Number three
    GESTURE_FOUR = 16,                  ///< Number four
    GESTURE_FIVE = 17,                  ///< Number five (open hand)
    GESTURE_WAVE_POSITION = 18,         ///< Wave hand position
    GESTURE_OKAY_SIGN = 19              ///< OK sign gesture
} predefined_gesture_id_t;

/**
 * @brief Force sensor calibration data
 */
typedef struct {
    uint16_t baseline[5];               ///< Baseline ADC values (no force)
    uint16_t max_force[5];              ///< Maximum recorded ADC values
    float sensitivity[5];               ///< Sensitivity calibration factors
    bool calibration_valid[5];          ///< Per-sensor calibration validity
} force_sensor_calibration_t;

// Function prototypes

/**
 * @brief Initialize hand controller subsystem
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_hand_controller_init(void);

/**
 * @brief Execute hand controller action
 * @param command Command to execute
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_hand_controller_act(p32_component_command_t command);

/**
 * @brief Get current hand controller state
 * @param state Pointer to state structure to fill
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if state is NULL
 */
esp_err_t p32_hand_get_state(hand_controller_state_t* state);

/**
 * @brief Get gesture name by ID
 * @param gesture_id Gesture identifier
 * @return Gesture name string or "unknown" if invalid ID
 */
const char* p32_hand_get_gesture_name(uint8_t gesture_id);

/**
 * @brief Get performance metrics
 * @param metrics Pointer to metrics structure to fill
 * @return ESP_OK on success, ESP_ERR_INVALID_ARG if metrics is NULL
 */
esp_err_t p32_hand_get_performance_metrics(hand_performance_metrics_t* metrics);

/**
 * @brief Execute specific gesture by ID
 * @param gesture_id Target gesture identifier
 * @param duration_ms Transition duration in milliseconds
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_execute_gesture(uint8_t gesture_id, uint16_t duration_ms);

/**
 * @brief Set individual finger position
 * @param finger_id Finger identifier (0-4)
 * @param position_degrees Position in degrees (0-180)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_set_finger_position(uint8_t finger_id, uint16_t position_degrees);

/**
 * @brief Get force sensor reading for specific finger
 * @param finger_id Finger identifier (0-4)
 * @param force_percent Pointer to store force percentage
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_get_finger_force(uint8_t finger_id, float* force_percent);

/**
 * @brief Check if grip is currently detected
 * @return true if grip is detected, false otherwise
 */
bool p32_hand_is_grip_detected(void);

/**
 * @brief Get grip strength and center of pressure
 * @param strength Pointer to store grip strength (0-100%)
 * @param center_x Pointer to store center X coordinate (mm)
 * @param center_y Pointer to store center Y coordinate (mm)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_get_grip_analysis(float* strength, float* center_x, float* center_y);

/**
 * @brief Perform force sensor calibration
 * @param calibration_data Pointer to calibration data structure
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_calibrate_force_sensors(force_sensor_calibration_t* calibration_data);

/**
 * @brief Set force sensor thresholds
 * @param low_threshold Low threshold percentage (0-100)
 * @param high_threshold High threshold percentage (0-100)
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_set_force_thresholds(float low_threshold, float high_threshold);

/**
 * @brief Enable or disable continuous force monitoring
 * @param enabled true to enable, false to disable
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_set_force_monitoring(bool enabled);

/**
 * @brief Get servo current consumption
 * @param servo_id Servo identifier (0-4)
 * @param current_ma Pointer to store current in mA
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_get_servo_current(uint8_t servo_id, float* current_ma);

/**
 * @brief Perform emergency stop - immediately open hand
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_emergency_stop(void);

/**
 * @brief Reset hand controller to factory defaults
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_hand_factory_reset(void);

/**
 * @brief Clean up and deinitialize hand controller
 * @return ESP_OK on success, error code on failure
 */
esp_err_t p32_comp_hand_controller_cleanup(void);

// Gesture execution macros for convenience
#define HAND_OPEN()         p32_hand_execute_gesture(GESTURE_OPEN_HAND, 1000)
#define HAND_CLOSE()        p32_hand_execute_gesture(GESTURE_CLOSED_FIST, 1000)
#define HAND_POINT()        p32_hand_execute_gesture(GESTURE_POINT_INDEX, 800)
#define HAND_THUMBS_UP()    p32_hand_execute_gesture(GESTURE_THUMBS_UP, 800)
#define HAND_WAVE()         p32_hand_execute_gesture(GESTURE_WAVE_POSITION, 500)
#define HAND_GRIP()         p32_hand_execute_gesture(GESTURE_PRECISION_GRIP, 1200)

// Force sensing macros
#define HAND_HAS_GRIP()     p32_hand_is_grip_detected()
#define HAND_FORCE_HIGH()   (p32_hand_is_grip_detected())

#ifdef __cplusplus
}
#endif

#endif // P32_HAND_CONTROLLER_H