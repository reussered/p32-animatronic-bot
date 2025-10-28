#ifndef P32_COMPONENT_REGISTRY_HPP
#define P32_COMPONENT_REGISTRY_HPP

#include "esp_err.h"
#include <stdint.h>

// ============================================================================
// P32 Component Registry - Function Declarations
// Auto-generated from JSON bot configuration
// ============================================================================

// Forward Declarations - Init Functions (from individual component files)

esp_err_t heartbeat_init(void);
esp_err_t network_monitor_init(void);
esp_err_t goblin_head_init(void);
esp_err_t spi_bus_init(void);
esp_err_t goblin_left_eye_init(void);
esp_err_t goblin_eye_init(void);
esp_err_t gc9a01_init(void);
esp_err_t generic_spi_display_init(void);
esp_err_t goblin_right_eye_init(void);
esp_err_t goblin_eye_init(void);
esp_err_t gc9a01_init(void);
esp_err_t generic_spi_display_init(void);
esp_err_t goblin_nose_init(void);
esp_err_t hc_sr04_ultrasonic_distance_sensor_init(void);
esp_err_t goblin_mouth_init(void);
esp_err_t goblin_mouth_init(void);
esp_err_t gc9a01_init(void);
esp_err_t generic_spi_display_init(void);
esp_err_t goblin_speaker_init(void);
esp_err_t speaker_init(void);
esp_err_t goblin_left_ear_init(void);
esp_err_t servo_sg90_micro_init(void);
esp_err_t goblin_right_ear_init(void);
esp_err_t servo_sg90_micro_init(void);
esp_err_t goblin_torso_init(void);
esp_err_t spine_flexion_servo_init(void);
esp_err_t servo_sg90_micro_init(void);
esp_err_t spine_extension_servo_init(void);
esp_err_t servo_sg90_micro_init(void);
esp_err_t waist_rotation_servo_init(void);
esp_err_t servo_sg90_micro_init(void);
esp_err_t torso_status_led_init(void);
esp_err_t servo_sg90_micro_init(void);
esp_err_t torso_speaker_init(void);
esp_err_t servo_sg90_micro_init(void);

// Forward Declarations - Act Functions (from individual component files)

void heartbeat_act(void);
void network_monitor_act(void);
void goblin_head_act(void);
void spi_bus_act(void);
void goblin_left_eye_act(void);
void goblin_eye_act(void);
void gc9a01_act(void);
void generic_spi_display_act(void);
void goblin_right_eye_act(void);
void goblin_eye_act(void);
void gc9a01_act(void);
void generic_spi_display_act(void);
void goblin_nose_act(void);
void hc_sr04_ultrasonic_distance_sensor_act(void);
void goblin_mouth_act(void);
void goblin_mouth_act(void);
void gc9a01_act(void);
void generic_spi_display_act(void);
void goblin_speaker_act(void);
void speaker_act(void);
void goblin_left_ear_act(void);
void servo_sg90_micro_act(void);
void goblin_right_ear_act(void);
void servo_sg90_micro_act(void);
void goblin_torso_act(void);
void spine_flexion_servo_act(void);
void servo_sg90_micro_act(void);
void spine_extension_servo_act(void);
void servo_sg90_micro_act(void);
void waist_rotation_servo_act(void);
void servo_sg90_micro_act(void);
void torso_status_led_act(void);
void servo_sg90_micro_act(void);
void torso_speaker_act(void);
void servo_sg90_micro_act(void);

// Table size constant - ALL TABLES MUST HAVE SAME SIZE
#define TABLE_SIZE 35

#endif // P32_COMPONENT_REGISTRY_HPP