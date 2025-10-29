#ifndef ACT_TABLE_H
#define ACT_TABLE_H

#include <stdint.h>

// Component action function type
typedef void (*act_func_t)(uint32_t loopCount);

// Action table entry with timing
typedef struct {
    act_func_t act_func;
    uint32_t hitCount;  // Execute every N loops (from JSON config)
} act_table_entry_t;

// Forward declarations for component action functions
// Generated from JSON bot configuration
void p32_comp_heartbeat_act(uint32_t loopCount);
void p32_comp_network_monitor_act(uint32_t loopCount);
void p32_comp_left_eye_act(uint32_t loopCount);
void p32_comp_right_eye_act(uint32_t loopCount);
void p32_comp_nose_sensor_act(uint32_t loopCount);

// JSON-generated action table with timing from bot configuration
#define ACT_TABLE_SIZE 5
extern act_table_entry_t actTable[ACT_TABLE_SIZE];

#endif // ACT_TABLE_H