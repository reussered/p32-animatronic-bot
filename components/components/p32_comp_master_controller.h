/*
P32 Master Controller Component Header
Master controller component for P32 universal loop architecture
*/

#ifndef P32_COMP_MASTER_CONTROLLER_H
#define P32_COMP_MASTER_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "p32_mesh_master.h"

#ifdef __cplusplus
extern "C" {
#endif

// P32 Component Interface Functions (auto-discovered by JSON build system)
esp_err_t p32_comp_master_controller_init(void);
esp_err_t p32_comp_master_controller_act(uint64_t loop_count);
esp_err_t p32_comp_master_controller_deinit(void);

// Public interface for other components
esp_err_t p32_comp_master_controller_set_mood(p32_mood_t mood, uint8_t intensity);
esp_err_t p32_comp_master_controller_set_behavior(p32_behavior_t behavior);
esp_err_t p32_comp_master_controller_move_servo(p32_node_role_t subsystem, 
                                               uint8_t servo_id, 
                                               int16_t position, 
                                               uint16_t speed);
esp_err_t p32_comp_master_controller_emergency_stop(void);

// Status functions
const p32_master_state_t* p32_comp_master_controller_get_state(void);
bool p32_comp_master_controller_is_subsystem_online(p32_node_role_t subsystem);
uint8_t p32_comp_master_controller_get_online_subsystem_count(void);

#ifdef __cplusplus
}
#endif

#endif // P32_COMP_MASTER_CONTROLLER_H