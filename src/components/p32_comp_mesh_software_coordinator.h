/*
P32 Mesh Software Coordinator Component Header
High-level mesh coordination logic for distributed robot control
*/

#ifndef P32_COMP_MESH_SOFTWARE_COORDINATOR_H
#define P32_COMP_MESH_SOFTWARE_COORDINATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "p32_mesh_coordinator.h"

#ifdef __cplusplus
extern "C" {
#endif

// P32 Component Interface Functions
esp_err_t p32_comp_mesh_software_coordinator_init(void);
esp_err_t p32_comp_mesh_software_coordinator_act(uint64_t loop_count);
esp_err_t p32_comp_mesh_software_coordinator_deinit(void);

// Public interface functions
bool p32_comp_mesh_software_coordinator_is_healthy(void);
uint8_t p32_comp_mesh_software_coordinator_get_health_score(void);
esp_err_t p32_comp_mesh_software_coordinator_request_emergency_stop(void);

// Internal coordination functions (used within component)
static void p32_mesh_sw_check_subsystem_health(uint64_t current_loop);
static void p32_mesh_sw_sync_behaviors(uint64_t current_loop);
static void p32_mesh_sw_report_overall_health(uint64_t current_loop);

#ifdef __cplusplus
}
#endif

#endif // P32_COMP_MESH_SOFTWARE_COORDINATOR_H