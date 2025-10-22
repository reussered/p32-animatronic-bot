#ifndef P32_TORSO_INTEGRATED_POWER_SYSTEM_HPP
#define P32_TORSO_INTEGRATED_POWER_SYSTEM_HPP

#include "esp_err.h"

// Torso Integrated Power System Component
// Battery management and power distribution for all subsystems

#ifdef __cplusplus
extern "C" {
#endif

// Component lifecycle functions (NO ARGUMENTS pattern)
esp_err_t torso_integrated_power_system_init(void);
void torso_integrated_power_system_act(void);

// Power system constants
#define BATTERY_NOMINAL_VOLTAGE_V   51.8    // 14S configuration
#define BATTERY_CAPACITY_MAH        13600   // 4P parallel 3400mAh cells
#define BATTERY_LOW_VOLTAGE_V       44.8    // 3.2V per cell * 14
#define BATTERY_CRITICAL_VOLTAGE_V  42.0    // 3.0V per cell * 14
#define MAX_CONTINUOUS_CURRENT_A    20.0    // BMS protection limit
#define CHARGING_VOLTAGE_V          58.8    // 4.2V per cell * 14

// Power monitoring intervals
#define POWER_MONITOR_INTERVAL_MS   1000
#define BATTERY_SAMPLE_INTERVAL_MS  100
#define THERMAL_CHECK_INTERVAL_MS   5000

#ifdef __cplusplus
}
#endif

#endif // P32_TORSO_INTEGRATED_POWER_SYSTEM_HPP