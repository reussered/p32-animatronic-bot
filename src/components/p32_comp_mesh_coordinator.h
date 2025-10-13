/*
P32 Mesh Coordinator Component Header
Component wrapper for ESP-NOW mesh networking in the P32 universal loop architecture
*/

#ifndef P32_COMP_MESH_COORDINATOR_H
#define P32_COMP_MESH_COORDINATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "p32_mesh_coordinator.h"

#ifdef __cplusplus
extern "C" {
#endif

// P32 Component Interface Functions
// These integrate with the universal loop architecture via initTable/actTable

/**
 * Initialize mesh coordinator component
 * Called once during system startup from initTable
 */
esp_err_t p32_comp_mesh_coordinator_init(void);

/**
 * Execute mesh coordinator component action
 * Called periodically from actTable based on hitCount timing
 * @param loop_count Current main loop iteration count
 */
esp_err_t p32_comp_mesh_coordinator_act(uint64_t loop_count);

/**
 * Deinitialize mesh coordinator component
 * Called during system shutdown
 */
esp_err_t p32_comp_mesh_coordinator_deinit(void);

// Component Status Functions
bool p32_comp_mesh_coordinator_is_online(void);
p32_node_role_t p32_comp_mesh_coordinator_get_role(void);
bool p32_comp_mesh_coordinator_is_master(void);
uint8_t p32_comp_mesh_coordinator_get_online_nodes(void);

// Inter-component Communication
esp_err_t p32_comp_mesh_coordinator_send_command(p32_node_role_t dest_role, 
                                                 uint8_t command_id, 
                                                 const uint8_t* params, 
                                                 uint16_t param_len);

#ifdef __cplusplus
}
#endif

#endif // P32_COMP_MESH_COORDINATOR_H