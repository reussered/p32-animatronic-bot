#ifndef INIT_TABLE_H
#define INIT_TABLE_H

#include "esp_err.h"

// Component initialization function type
typedef esp_err_t (*init_func_t)(void);

// Forward declarations for component init functions
// Generated from JSON bot configuration
esp_err_t p32_comp_heartbeat_init(void);
esp_err_t p32_comp_network_monitor_init(void);
esp_err_t p32_comp_power_manager_init(void);
esp_err_t p32_comp_mesh_coordinator_init(void);
esp_err_t p32_comp_master_controller_init(void);
esp_err_t p32_comp_mesh_software_coordinator_init(void);
esp_err_t p32_comp_spine_lower_init(void);

// JSON-generated initialization table
#define INIT_TABLE_SIZE 7
extern init_func_t initTable[INIT_TABLE_SIZE];

#endif // INIT_TABLE_H