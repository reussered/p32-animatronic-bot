#ifndef P32_COMPONENT_REGISTRY_H
#define P32_COMPONENT_REGISTRY_H

#include "esp_err.h"
#include <stdint.h>

// Component initialization function type
typedef esp_err_t (*init_func_t)(void);

// Component action function type
typedef void (*act_func_t)(uint64_t loopCount);

// Action table entry with timing
typedef struct {
    act_func_t act_func;
    uint32_t hitCount;  // Execute every N loops
    const char* name;   // Component name for debugging
} act_table_entry_t;

// Forward declarations for all component functions
// Initialization functions
esp_err_t system_core_init(void);
esp_err_t network_monitor_init(void);
esp_err_t unknown_component_init(void);
esp_err_t goblin_left_eye_init(void);
esp_err_t goblin_right_eye_init(void);
esp_err_t goblin_nose_init(void);
esp_err_t goblin_mouth_init(void);
esp_err_t goblin_speaker_init(void);
esp_err_t goblin_ear_left_init(void);
esp_err_t goblin_ear_right_init(void);
esp_err_t unknown_component_init(void);

// Action functions
void system_core_act(uint64_t loopCount);
void network_monitor_act(uint64_t loopCount);
void unknown_component_act(uint64_t loopCount);
void goblin_left_eye_act(uint64_t loopCount);
void goblin_right_eye_act(uint64_t loopCount);
void goblin_nose_act(uint64_t loopCount);
void goblin_mouth_act(uint64_t loopCount);
void goblin_speaker_act(uint64_t loopCount);
void goblin_ear_left_act(uint64_t loopCount);
void goblin_ear_right_act(uint64_t loopCount);
void unknown_component_act(uint64_t loopCount);

// Component tables
#define INIT_TABLE_SIZE 11
#define ACT_TABLE_SIZE 11

extern init_func_t initTable[INIT_TABLE_SIZE];
extern act_table_entry_t actTable[ACT_TABLE_SIZE];

#endif // P32_COMPONENT_REGISTRY_H