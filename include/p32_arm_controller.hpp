/*
 * P32 Animatronic Bot - Arm Controller Header
 * ESP32-C3 Controller for Arm Subsystems
 */

#ifndef P32_ARM_CONTROLLER_H
#define P32_ARM_CONTROLLER_H

#include "esp_err.h"
#include "esp_now.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Logging tag
static const char* TAG = "P32_ARM";

// Hardware pin definitions
#define ARM_I2C_SDA_PIN             4
#define ARM_I2C_SCL_PIN             5
#define ARM_I2C_PORT                I2C_NUM_0
#define ARM_STATUS_LED_PIN          2
#define ARM_ERROR_LED_PIN           19
#define ARM_ESTOP_PIN               0

// Servo configuration
#define ARM_SERVO_COUNT             7
#define ARM_SERVO_MIN_PWM           1000
#define ARM_SERVO_MAX_PWM           2000
#define ARM_SERVO_NEUTRAL_PWM       1500
#define PCA9685_I2C_ADDRESS_LEFT    0x41
#define PCA9685_I2C_ADDRESS_RIGHT   0x42

// Timing and monitoring
#define ARM_MAX_CURRENT_THRESHOLD   8.0f   // Amperes per arm
#define ARM_STATUS_REPORT_INTERVAL  1000   // Milliseconds

// Arm identification
typedef enum {
    ARM_ID_LEFT = 0x02,
    ARM_ID_RIGHT = 0x03
} arm_id_t;

// Command types
typedef enum {
    ARM_CMD_JOINT_POSITION = 0,
    ARM_CMD_REACH_POSITION,
    ARM_CMD_ARM_SWING,
    ARM_CMD_GESTURE,
    ARM_CMD_EMERGENCY_STOP
} arm_command_type_t;

// Gesture types
typedef enum {
    ARM_GESTURE_REST = 0,
    ARM_GESTURE_WAVE,
    ARM_GESTURE_POINT,
    ARM_GESTURE_GREETING,
    ARM_GESTURE_THUMBS_UP
} arm_gesture_t;

// 3D vector structure
typedef struct {
    float x;
    float y;
    float z;
} p32_vector3_t;

// Arm command structure
typedef struct {
    arm_command_type_t command_type;
    uint32_t timestamp_ms;
    uint32_t transition_time;
    union {
        float joint_angles[7];          // Joint angle commands
        p32_vector3_t target_position;  // Reach target position
        struct {
            float swing_angle;          // Swing amplitude
            float swing_frequency;      // Swing frequency
        };
        arm_gesture_t gesture_id;       // Gesture identifier
    };
} p32_arm_command_t;

// Arm state structure
typedef struct {
    arm_id_t arm_id;                    // Left or right arm
    float joint_angles[7];              // Current joint angles
    float target_angles[7];             // Target joint angles
    p32_vector3_t end_effector_pos;     // Current end effector position
    float gait_phase;                   // Walking gait phase (0-2PI)
    float total_current;                // Total servo current
    uint64_t uptime_ms;                 // Controller uptime
    uint64_t last_movement_time;        // Last movement timestamp
    uint64_t last_status_send;          // Last status report time
    bool emergency_stop_active;         // Emergency stop status
    bool is_moving;                     // Movement in progress flag
} p32_arm_state_t;

// Joint angle limits (degrees)
static const float ARM_JOINT_MIN_ANGLES[7] = {
    -90.0f,  // Shoulder flexion
    -180.0f, // Shoulder abduction
    -90.0f,  // Shoulder rotation
    0.0f,    // Elbow flexion
    -90.0f,  // Wrist flexion
    -180.0f, // Wrist rotation
    -45.0f   // Wrist abduction
};

static const float ARM_JOINT_MAX_ANGLES[7] = {
    180.0f,  // Shoulder flexion
    90.0f,   // Shoulder abduction
    90.0f,   // Shoulder rotation
    150.0f,  // Elbow flexion
    90.0f,   // Wrist flexion
    180.0f,  // Wrist rotation
    45.0f    // Wrist abduction
};

// Joint calibration offsets (normalized -1.0 to 1.0)
static const float ARM_JOINT_CALIBRATION_OFFSETS[7] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

// Function prototypes - Component interface
esp_err_t p32_comp_arm_controller_init(bool left_arm);
esp_err_t p32_comp_arm_controller_act(p32_arm_command_t command);

// Internal function prototypes
esp_err_t p32_arm_servo_init(void);
esp_err_t p32_arm_set_joint_angles(float joint_angles[7], uint32_t transition_time);
esp_err_t p32_arm_reach_target(p32_vector3_t target, uint32_t transition_time);
esp_err_t p32_arm_swing_motion(float swing_angle, float frequency);
esp_err_t p32_arm_execute_gesture(arm_gesture_t gesture_id);

// Task and callback function prototypes
static void p32_arm_control_task(void *pvParameters);
static void p32_arm_process_command(p32_arm_command_t *command);
static void p32_arm_system_monitor(void);
static void p32_arm_emergency_stop(void);
static void p32_arm_espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len);

// Algorithm function prototypes
static bool p32_arm_calculate_inverse_kinematics(p32_vector3_t target, float joint_angles[7]);
static p32_vector3_t p32_arm_calculate_forward_kinematics(float joint_angles[7]);

// Utility function prototypes
static uint16_t p32_arm_angle_to_pwm(float angle, uint8_t joint_index);
static float p32_arm_read_servo_currents(void);
static void p32_arm_reduce_power(float power_factor);
static void p32_arm_send_status_update(void);

// Shared structures (defined in torso_controller.h)
typedef struct {
    uint8_t subsystem_id;
    bool online;
    float battery_level;
    float temperature;
    uint32_t uptime_ms;
    uint16_t error_flags;
} p32_subsystem_status_t;

typedef struct {
    float battery_voltage;
    float system_current;
    float power_level;
    uint64_t uptime_ms;
    bool emergency_stop_active;
} p32_system_status_t;

// PCA9685 servo driver interface (external library)
esp_err_t pca9685_init(i2c_port_t i2c_port, uint8_t address);
esp_err_t pca9685_set_pwm_freq(uint8_t freq);
esp_err_t pca9685_set_pwm(uint8_t channel, uint16_t pwm_value);
uint16_t pca9685_get_pwm(uint8_t channel);

#endif // P32_ARM_CONTROLLER_H