#ifndef ACT_TABLE_H
#define ACT_TABLE_H

#include <stdint.h>

// Component action function type
typedef void (*act_func_t)(uint64_t loopCount);

// Action table entry with timing
typedef struct {
    act_func_t act_func;
    uint32_t hitCount;  // Execute every N loops (from JSON config)
} act_table_entry_t;

// Forward declarations for component action functions
// Generated from JSON bot configuration
void p32_comp_heartbeat_act(uint64_t loopCount);
void p32_comp_network_monitor_act(uint64_t loopCount);
void p32_comp_power_manager_act(uint64_t loopCount);
void p32_comp_mesh_coordinator_act(uint64_t loopCount);
void p32_comp_master_controller_act(uint64_t loopCount);
void p32_comp_mesh_software_coordinator_act(uint64_t loopCount);
void p32_comp_spine_lower_act(uint64_t loopCount);

// JSON-generated action table with timing from bot configuration
#define ACT_TABLE_SIZE 7
extern act_table_entry_t actTable[ACT_TABLE_SIZE];

#endif // ACT_TABLE_H