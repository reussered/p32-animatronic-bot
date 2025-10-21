/*
 * P32 Animatronic Bot - Torso Controller Header
 * ESP32-S3 Master Controller for Torso Subsystem
 */

#ifndef P32_TORSO_CONTROLLER_H
#define P32_TORSO_CONTROLLER_H

#include "esp_err.h"
#include "esp_now.h"
#include "driver/gpio.h"
#include <stdint.h>
#include <stdbool.h>

// Logging tag
static const char* TAG = "P32_TORSO";

// Hardware pin definitions
#define TORSO_I2C_SDA_PIN           21
#define TORSO_I2C_SCL_PIN           22
#define TORSO_I2C_PORT              I2C_NUM_0
#define TORSO_STATUS_LED_PIN        2
#define TORSO_POWER_LED_PIN         18
#define TORSO_ERROR_LED_PIN         19
#define TORSO_ESTOP_PIN             0
#define TORSO_BATTERY_SENSE_PIN     1
#define TORSO_BATTERY_SENSE_CHANNEL ADC1_CHANNEL_1

// Servo configuration
#define TORSO_SERVO_COUNT           6
#define TORSO_SERVO_MIN_PWM         1000
#define TORSO_SERVO_MAX_PWM         2000
#define TORSO_SERVO_NEUTRAL_PWM     1500
#define PCA9685_I2C_ADDRESS         0x40

// Power management thresholds
#define TORSO_LOW_BATTERY_THRESHOLD     45.0f  // Volts
#define TORSO_MAX_CURRENT_THRESHOLD     25.0f  // Amperes
#define TORSO_VOLTAGE_DIVIDER_RATIO     20.0f  // 51.8V -> 3.3V scaling

// Command types
typedef enum {
    TORSO_CMD_SPINE_POSITION = 0,
    TORSO_CMD_BALANCE_LEAN,
    TORSO_CMD_POWER_MANAGEMENT,
    TORSO_CMD_SYSTEM_STATUS,
    TORSO_CMD_EMERGENCY_STOP
} torso_command_type_t;

// Command priority levels
typedef enum {
    TORSO_PRIORITY_LOW = 0,
    TORSO_PRIORITY_NORMAL,
    TORSO_PRIORITY_HIGH,
    TORSO_PRIORITY_EMERGENCY
} torso_priority_t;

// Subsystem IDs
typedef enum {
    SUBSYSTEM_TORSO = 0x01,
    SUBSYSTEM_ARM_LEFT = 0x02,
    SUBSYSTEM_ARM_RIGHT = 0x03,
    SUBSYSTEM_HAND_LEFT = 0x04,
    SUBSYSTEM_HAND_RIGHT = 0x05,
    SUBSYSTEM_LEG_LEFT = 0x06,
    SUBSYSTEM_LEG_RIGHT = 0x07
} subsystem_id_t;

// Torso command structure
typedef struct {
    torso_command_type_t command_type;
    torso_priority_t priority;
    uint32_t timestamp_ms;
    uint32_t transition_time;
    union {
        float spine_curve[6];       // Spine vertebrae angles
        float lean_angle;           // Balance lean angle
        float power_level;          // Power scaling factor (0.0-1.0)
        uint32_t status_flags;      // Status request flags
    };
} p32_torso_command_t;

// Torso state structure
typedef struct {
    float spine_angles[6];          // Current spine vertebrae angles
    float battery_voltage;          // Main battery voltage
    float system_current;           // Total system current draw
    float power_level;              // Current power scaling
    uint64_t uptime_ms;             // System uptime in milliseconds
    uint32_t last_spine_update;     // Last spine movement timestamp
    bool emergency_stop_active;     // Emergency stop status
    uint32_t successful_transmissions;
    uint32_t failed_transmissions;
    struct {
        uint8_t subsystem_id;
        bool online;
        float battery_level;
        uint32_t last_contact_ms;
    } subsystem_status[6];          // Status of all subsystems
} p32_torso_state_t;

// Subsystem status structure
typedef struct {
    uint8_t subsystem_id;
    bool online;
    float battery_level;
    float temperature;
    uint32_t uptime_ms;
    uint16_t error_flags;
} p32_subsystem_status_t;

// System-wide status structure
typedef struct {
    float battery_voltage;
    float system_current;
    float power_level;
    uint64_t uptime_ms;
    bool emergency_stop_active;
} p32_system_status_t;

// Function prototypes - Component interface
esp_err_t p32_comp_torso_controller_init(void);
esp_err_t p32_comp_torso_controller_act(p32_torso_command_t command);

// Internal function prototypes
esp_err_t p32_torso_servo_init(void);
esp_err_t p32_torso_set_spine_curve(float spine_curve[6], uint32_t transition_time);
esp_err_t p32_torso_balance_adjustment(float lean_angle);
esp_err_t p32_torso_power_control(float power_level);

// Task and callback function prototypes
static void p32_torso_control_task(void *pvParameters);
static void p32_torso_process_command(p32_torso_command_t *command);
static void p32_torso_system_monitor(void);
static void p32_torso_emergency_stop(void);
static void p32_torso_espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
static void p32_torso_espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len);

// Utility function prototypes
static uint16_t p32_torso_angle_to_pwm(float angle);
static float p32_torso_read_system_current(void);
static void p32_torso_update_subsystem_status(p32_subsystem_status_t *status);
static void p32_torso_send_status_update(void);

// PCA9685 servo driver interface (external library)
esp_err_t pca9685_init(i2c_port_t i2c_port, uint8_t address);
esp_err_t pca9685_set_pwm_freq(uint8_t freq);
esp_err_t pca9685_set_pwm(uint8_t channel, uint16_t pwm_value);
uint16_t pca9685_get_pwm(uint8_t channel);

#endif // P32_TORSO_CONTROLLER_H