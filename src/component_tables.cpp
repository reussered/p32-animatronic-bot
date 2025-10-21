// P32 Component Table Definitions
// Three parallel tables populated from JSON component configurations

#include "p32_component_registry.h"

// Component function declarations - NO ARGUMENTS
extern esp_err_t system_core_init(void);
extern esp_err_t network_monitor_init(void);
extern esp_err_t goblin_eye_left_init(void);
extern esp_err_t goblin_eye_right_init(void);
extern esp_err_t goblin_nose_init(void);

extern void system_core_act(void);
extern void network_monitor_act(void);
extern void goblin_eye_left_act(void);
extern void goblin_eye_right_act(void);
extern void goblin_nose_act(void);

// Table 1: Init function pointers (from JSON component list)
init_func_t initTable[INIT_TABLE_SIZE] = {
    system_core_init,
    network_monitor_init,
    goblin_eye_left_init,
    goblin_eye_right_init,
    goblin_nose_init
};

// Table 2: Act function pointers (from JSON component list)
act_func_t actTable[ACT_TABLE_SIZE] = {
    system_core_act,
    network_monitor_act,
    goblin_eye_left_act,
    goblin_eye_right_act,
    goblin_nose_act
};

// Table 3: hitCount values (from JSON "timing": {"hitCount": N})
uint32_t hitCountTable[ACT_TABLE_SIZE] = {
    1000,  // system_core_act - every 1000 loops
    5000,  // network_monitor_act - every 5000 loops
    50,    // goblin_eye_left_act - every 50 loops
    50,    // goblin_eye_right_act - every 50 loops
    200    // goblin_nose_act - every 200 loops
};