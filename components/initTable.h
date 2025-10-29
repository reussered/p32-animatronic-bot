#ifndef INIT_TABLE_H
#define INIT_TABLE_H

#include "esp_err.h"

// Component initialization function type
typedef esp_err_t (*init_func_t)(void);

// Forward declarations for component init functions
// Generated from JSON bot configuration
esp_err_t p32_comp_heartbeat_init(void);
esp_err_t p32_comp_network_monitor_init(void);
esp_err_t p32_comp_left_eye_init(void);
esp_err_t p32_comp_right_eye_init(void);
esp_err_t p32_comp_nose_sensor_init(void);

// JSON-generated initialization table
#define INIT_TABLE_SIZE 5
extern init_func_t initTable[INIT_TABLE_SIZE];

#endif // INIT_TABLE_H