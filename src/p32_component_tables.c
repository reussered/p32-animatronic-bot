// P32 Component Tables - Auto-generated
// Links individual component files into unified tables

#include "p32_component_registry.h"

// Initialization table - all component init functions
init_func_t initTable[INIT_TABLE_SIZE] = {
    system_core_init,
    network_monitor_init,
    goblin_eye_left_init,
    goblin_eye_right_init,
    goblin_nose_init,
};

// Action table - all component action functions with timing
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    { system_core_act, 1000, "system_core" },
    { network_monitor_act, 5000, "network_monitor" },
    { goblin_eye_left_act, 50, "goblin_eye_left" },
    { goblin_eye_right_act, 50, "goblin_eye_right" },
    { goblin_nose_act, 100, "goblin_nose" },
};