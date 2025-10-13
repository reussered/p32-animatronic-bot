/*
P32 Master Controller Header
Defines the master controller interface for torso mesh coordination
*/

#ifndef P32_MESH_MASTER_H
#define P32_MESH_MASTER_H

#include <stdint.h>
#include <stdbool.h>
#include "p32_mesh_coordinator.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Maximum subsystems in robot
#define P32_MAX_SUBSYSTEMS 8

// Command IDs for subsystem communication
#define P32_CMD_SET_BEHAVIOR    0x10
#define P32_CMD_SERVO_POSITION  0x20
#define P32_CMD_REQUEST_STATUS  0x30
#define P32_CMD_RESUME          0x40

// Mood states (matches existing mood system)
typedef enum {
    P32_MOOD_FEAR = 0,
    P32_MOOD_ANGER = 1,
    P32_MOOD_IRRITATION = 2,
    P32_MOOD_HAPPINESS = 3,
    P32_MOOD_CONTENTMENT = 4,
    P32_MOOD_HUNGER = 5,
    P32_MOOD_CURIOSITY = 6,
    P32_MOOD_AFFECTION = 7,
    P32_MOOD_NEUTRAL = 8
} p32_mood_t;

// Behavior states
typedef enum {
    P32_BEHAVIOR_IDLE = 0,
    P32_BEHAVIOR_ALERT = 1,
    P32_BEHAVIOR_AGGRESSIVE = 2,
    P32_BEHAVIOR_PLAYFUL = 3,
    P32_BEHAVIOR_SLEEP = 4,
    P32_BEHAVIOR_HUNT = 5,
    P32_BEHAVIOR_SOCIAL = 6,
    P32_BEHAVIOR_DEFENSIVE = 7
} p32_behavior_t;

// Subsystem status
typedef enum {
    P32_SUBSYS_OFFLINE = 0,
    P32_SUBSYS_INITIALIZING = 1,
    P32_SUBSYS_READY = 2,
    P32_SUBSYS_ACTIVE = 3,
    P32_SUBSYS_ERROR = 4
} p32_subsystem_status_t;

// Subsystem state information
typedef struct {
    p32_node_role_t role;
    p32_subsystem_status_t status;
    uint8_t battery_level;
    uint8_t cpu_load;
    int8_t temperature;
    uint32_t uptime_sec;
    uint16_t error_count;
    uint16_t component_mask;
    uint32_t last_status_time;
    uint32_t last_command_time;
    uint16_t command_sequence;
} p32_subsystem_state_t;

// Global robot state
typedef struct {
    p32_mood_t current_mood;
    uint8_t mood_intensity;
    p32_behavior_t global_behavior;
    bool is_emergency_stop;
    uint8_t online_subsystems;
    uint8_t active_subsystems;
    bool has_subsystem_errors;
} p32_robot_state_t;

// Master controller configuration
typedef struct {
    const char* mesh_key;
    uint8_t expected_subsystems;
    uint32_t status_request_interval_ms;
    uint32_t subsystem_timeout_ms;
} p32_master_config_t;

// Master controller state
typedef struct {
    p32_master_config_t config;
    p32_subsystem_state_t subsystems[P32_MAX_SUBSYSTEMS];
    p32_robot_state_t robot_state;
    bool is_active;
} p32_master_state_t;

// Master controller functions
esp_err_t p32_master_init(const p32_master_config_t* config);
esp_err_t p32_master_start(void);
esp_err_t p32_master_set_global_mood(p32_mood_t mood, uint8_t intensity);
esp_err_t p32_master_set_global_behavior(p32_behavior_t behavior);
esp_err_t p32_master_send_servo_command(p32_node_role_t target_subsystem, 
                                       uint8_t servo_id, int16_t position, uint16_t speed);
esp_err_t p32_master_emergency_stop(void);
esp_err_t p32_master_clear_emergency_stop(void);
esp_err_t p32_master_request_subsystem_status(p32_node_role_t target);

// State getters
const p32_master_state_t* p32_master_get_state(void);
const p32_subsystem_state_t* p32_master_get_subsystem_state(p32_node_role_t role);

// Utility functions
const char* p32_master_mood_to_string(p32_mood_t mood);
const char* p32_master_behavior_to_string(p32_behavior_t behavior);
void p32_master_update_robot_status(void);

#ifdef __cplusplus
}
#endif

#endif // P32_MESH_MASTER_H