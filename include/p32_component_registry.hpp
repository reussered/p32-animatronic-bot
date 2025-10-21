#ifndef P32_COMPONENT_REGISTRY_H
#define P32_COMPONENT_REGISTRY_H

#include "esp_err.h"
#include <stdint.h>

// Component initialization function type - NO ARGUMENTS
typedef esp_err_t (*init_func_t)(void);

// Component action function type - NO ARGUMENTS
typedef void (*act_func_t)(void);

// Forward declarations for all component functions
// Initialization functions - NO ARGUMENTS
esp_err_t system_core_init(void);
esp_err_t network_monitor_init(void);
esp_err_t goblin_left_eye_init(void);
esp_err_t goblin_right_eye_init(void);
esp_err_t goblin_nose_init(void);

// Action functions - NO ARGUMENTS
void system_core_act(void);
void network_monitor_act(void);
void goblin_left_eye_act(void);
void goblin_right_eye_act(void);
void goblin_nose_act(void);

// Three parallel tables (populated from JSON)
#define INIT_TABLE_SIZE 5
#define ACT_TABLE_SIZE 5

extern init_func_t initTable[INIT_TABLE_SIZE];
extern act_func_t actTable[ACT_TABLE_SIZE];
extern uint32_t hitCountTable[ACT_TABLE_SIZE];

#endif // P32_COMPONENT_REGISTRY_H