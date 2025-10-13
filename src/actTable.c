#include "actTable.h"

// JSON-generated action table with timing from bot configuration
// Auto-generated from bot JSON with hitCount values from config
act_table_entry_t actTable[ACT_TABLE_SIZE] = {
    { p32_comp_heartbeat_act,  1 },    // System heartbeat
    { p32_comp_network_monitor_act,  1 },    // Network monitoring and loop timing
    { p32_comp_power_manager_act, 100 },    // Battery monitoring and power distribution control
    { p32_comp_mesh_coordinator_act, 10 },    // ESP-NOW mesh network master controller
    { p32_comp_master_controller_act, 50 },    // Master controller for coordinating all subsystems via mesh network
    { p32_comp_mesh_software_coordinator_act, 20 },    // High-level software coordination logic for distributed robot mesh network
    { p32_comp_spine_lower_act, 20 }     // Lower spine flexion/extension control
};