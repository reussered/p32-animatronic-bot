#pragma once

#include "p32_core.h"

// Sensor types supported
typedef enum {
    SENSOR_ULTRASONIC = 0,
    SENSOR_PIR,
    SENSOR_CAPACITIVE,
    SENSOR_MICROPHONE,
    SENSOR_ACCELEROMETER,
    SENSOR_COUNT
} p32_sensor_type_t;

// Ultrasonic sensor (HC-SR04) structure
typedef struct {
    uint8_t trigger_pin;
    uint8_t echo_pin;
    float distance_cm;
    uint32_t last_reading_ms;
    bool initialized;
    char component_id[32];
} p32_ultrasonic_sensor_t;

// PIR motion sensor structure
typedef struct {
    uint8_t data_pin;
    bool motion_detected;
    uint32_t last_motion_ms;
    bool initialized;
    char component_id[32];
} p32_pir_sensor_t;

// Capacitive touch sensor structure
typedef struct {
    uint8_t touch_pin;
    uint16_t touch_threshold;
    bool touch_detected;
    uint32_t last_touch_ms;
    bool initialized;
    char component_id[32];
} p32_touch_sensor_t;

// Sensor system state
typedef struct {
    p32_ultrasonic_sensor_t proximity_nose;
    p32_pir_sensor_t motion_detector;
    p32_touch_sensor_t touch_sensors[4];
    TaskHandle_t sensor_task;
    QueueHandle_t sensor_event_queue;
    bool sensors_enabled;
} p32_sensor_system_t;

// Sensor event structure
typedef struct {
    char sensor_id[32];
    p32_sensor_type_t type;
    float value;
    uint32_t timestamp_ms;
} p32_sensor_event_t;

// Sensor driver functions
esp_err_t p32_sensor_init(void);
void p32_sensor_act(void);
esp_err_t p32_sensor_setup_ultrasonic(p32_ultrasonic_sensor_t* sensor, const p32_component_config_t* config);
esp_err_t p32_sensor_setup_pir(p32_pir_sensor_t* sensor, const p32_component_config_t* config);
esp_err_t p32_sensor_setup_touch(p32_touch_sensor_t* sensor, const p32_component_config_t* config);

// Sensor reading functions
float p32_sensor_read_distance(p32_ultrasonic_sensor_t* sensor);
bool p32_sensor_read_motion(p32_pir_sensor_t* sensor);
bool p32_sensor_read_touch(p32_touch_sensor_t* sensor);

// Sensor event processing
void p32_sensor_task(void* parameters);
esp_err_t p32_sensor_register_event_handler(void (*handler)(p32_sensor_event_t*));

// Behavioral triggers based on sensor input
typedef enum {
    TRIGGER_PROXIMITY_CLOSE = 0,
    TRIGGER_PROXIMITY_FAR,
    TRIGGER_MOTION_DETECTED,
    TRIGGER_MOTION_STOPPED,
    TRIGGER_TOUCH_HEAD,
    TRIGGER_TOUCH_FACE,
    TRIGGER_COUNT
} p32_sensor_trigger_t;

// Sensor-to-mood mapping for behavioral responses
typedef struct {
    p32_sensor_trigger_t trigger;
    p32_mood_t target_mood;
    uint32_t duration_ms;
    uint8_t priority;
} p32_sensor_mood_mapping_t;

extern const p32_sensor_mood_mapping_t p32_goblin_sensor_mappings[];
extern const p32_sensor_mood_mapping_t p32_zombie_sensor_mappings[];
extern const p32_sensor_mood_mapping_t p32_robot_sensor_mappings[];

// Proximity-based behaviors
esp_err_t p32_sensor_check_proximity_behavior(float distance);
esp_err_t p32_sensor_check_motion_behavior(bool motion_active);
esp_err_t p32_sensor_check_touch_behavior(const char* touch_location);