#ifndef P32_MG996R_SERVO_HPP
#define P32_MG996R_SERVO_HPP

#include "esp_err.h"

// MG996R Servo Motor Component
// High-torque servo for major joints (shoulder, elbow, spine)

#ifdef __cplusplus
extern "C" {
#endif

// Component lifecycle functions (NO ARGUMENTS pattern)
esp_err_t mg996r_servo_init(void);
void mg996r_servo_act(void);

// Servo control constants
#define MG996R_PWM_FREQ_HZ          50
#define MG996R_PULSE_WIDTH_MIN_US   1000
#define MG996R_PULSE_WIDTH_MAX_US   2000
#define MG996R_PULSE_WIDTH_CENTER_US 1500
#define MG996R_TORQUE_KG_CM         11
#define MG996R_OPERATING_VOLTAGE_V  6.0

#ifdef __cplusplus
}
#endif

#endif // P32_MG996R_SERVO_HPP