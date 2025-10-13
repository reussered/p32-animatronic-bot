#include "actTable.h"

// JSON-generated action table with timing from bot configuration
// Auto-generated from bot JSON with hitCount values from config
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    { p32_comp_heartbeat_act,  1 },    // System heartbeat
    { p32_comp_network_monitor_act,  1 },    // Network monitoring and loop timing
    { p32_comp_left_eye_act,  5 },    // Left eye display animation
    { p32_comp_right_eye_act,  5 },    // Right eye display animation
    { p32_comp_nose_sensor_act, 15 }     // Proximity sensor monitoring
};