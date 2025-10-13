// P32 Component Table Definitions
// Contains the actual table data that headers declare

#include "initTable.h" 
#include "actTable.h"

// Include component function declarations
extern esp_err_t system_init_component(void);
extern esp_err_t network_monitor_init(void);
extern esp_err_t goblin_eye_left_init(void);
extern esp_err_t goblin_eye_right_init(void);
extern esp_err_t goblin_nose_init(void);

extern void system_act_component(uint32_t loopCount);
extern void network_monitor_act(uint32_t loopCount);
extern void goblin_eye_left_act(uint32_t loopCount);
extern void goblin_eye_right_act(uint32_t loopCount);
extern void goblin_nose_act(uint32_t loopCount);

// Component initialization table - ACTUAL DEFINITION
init_func_t initTable[INIT_TABLE_SIZE] = {
    system_init_component,
    network_monitor_init,
    goblin_eye_left_init,
    goblin_eye_right_init,
    goblin_nose_init,
};

// Component action table - ACTUAL DEFINITION
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    { system_act_component, 1000 },      // Every 1000 loops
    { network_monitor_act, 5000 },  // Every 5000 loops  
    { goblin_eye_left_act, 50 },    // Every 50 loops
    { goblin_eye_right_act, 50 },   // Every 50 loops
    { goblin_nose_act, 200 },       // Every 200 loops
};