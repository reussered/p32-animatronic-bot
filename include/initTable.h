#ifndef INIT_TABLE_H
#define INIT_TABLE_H

#include "esp_err.h"

// Component initialization function type
typedef esp_err_t (*init_func_t)(void);

// Forward declarations for component init functions
// Generated from individual component files
esp_err_t system_init_component(void);
esp_err_t network_monitor_init(void);
esp_err_t goblin_eye_left_init(void);
esp_err_t goblin_eye_right_init(void);
esp_err_t goblin_nose_init(void);

// JSON-generated initialization table - DECLARATION ONLY
#define INIT_TABLE_SIZE 5
extern init_func_t initTable[INIT_TABLE_SIZE];

#endif // INIT_TABLE_H