#ifndef P32_SERVO_SG90_MICRO_HPP
#define P32_SERVO_SG90_MICRO_HPP

#include "esp_err.h"

// SG90 Micro Servo Component
// Lightweight servo for fine control (wrist, fingers, small movements)

// Component lifecycle functions (NO ARGUMENTS pattern)
esp_err_t servo_sg90_micro_init(void);
void servo_sg90_micro_act(void);

// Micro servo control constants  
#define SG90_PWM_FREQ_HZ            50
#define SG90_PULSE_WIDTH_MIN_US     1000
#define SG90_PULSE_WIDTH_MAX_US     2000
#define SG90_PULSE_WIDTH_CENTER_US  1500
#define SG90_TORQUE_KG_CM           1.8
#define SG90_OPERATING_VOLTAGE_V    5.0
#define SG90_WEIGHT_GRAMS           9

#endif // P32_SERVO_SG90_MICRO_HPP