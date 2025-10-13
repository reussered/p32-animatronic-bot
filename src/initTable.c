#include "initTable.h"

// JSON-generated initialization table
// Auto-generated from bot JSON configuration
init_func_t initTable[INIT_TABLE_SIZE] = {
    p32_comp_heartbeat_init,
    p32_comp_network_monitor_init,
    p32_comp_power_manager_init,
    p32_comp_mesh_coordinator_init,
    p32_comp_master_controller_init,
    p32_comp_mesh_software_coordinator_init,
    p32_comp_spine_lower_init
};