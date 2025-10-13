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
// Generated from individual component files
void system_act_component(uint32_t loopCount);
void network_monitor_act(uint32_t loopCount);
void goblin_eye_left_act(uint32_t loopCount);
void goblin_eye_right_act(uint32_t loopCount);
void goblin_nose_act(uint32_t loopCount);

// JSON-generated action table - DECLARATION ONLY  
#define ACT_TABLE_SIZE 5
extern act_table_entry_t actTable[ACT_TABLE_SIZE];

#endif // ACT_TABLE_H