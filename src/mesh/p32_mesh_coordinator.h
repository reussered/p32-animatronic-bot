/*
P32 ESP-NOW Mesh Network Coordinator
Handles communication between master torso controller and slave subsystem controllers

Network Topology:
- Master: Torso Controller (ESP32-S3) - coordinates all subsystems
- Slaves: Head, Arms, Legs, Hand controllers (ESP32-C3) - execute commands

Message Types:
- COMMAND: Master -> Slave (servo positions, mood changes, behaviors)
- STATUS: Slave -> Master (health, positions, sensor readings)
- HEARTBEAT: Bidirectional keepalive
- EMERGENCY: Any -> All (safety stops, critical alerts)
*/

#ifndef P32_MESH_COORDINATOR_H
#define P32_MESH_COORDINATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Network Configuration
#define P32_MESH_MAX_NODES 8
#define P32_MESH_CHANNEL 1
#define P32_MESH_KEY_LEN 16
#define P32_MESH_PAYLOAD_SIZE 200
#define P32_MESH_HEARTBEAT_INTERVAL_MS 1000
#define P32_MESH_COMMAND_TIMEOUT_MS 500

// Message Types
typedef enum {
    P32_MSG_HEARTBEAT = 0x01,
    P32_MSG_COMMAND = 0x02,
    P32_MSG_STATUS = 0x03,
    P32_MSG_EMERGENCY = 0xFF
} p32_msg_type_t;

// Node Roles
typedef enum {
    P32_ROLE_MASTER = 0x10,
    P32_ROLE_SLAVE_HEAD = 0x21,
    P32_ROLE_SLAVE_ARM_LEFT = 0x22,
    P32_ROLE_SLAVE_ARM_RIGHT = 0x23,
    P32_ROLE_SLAVE_LEG_LEFT = 0x24,
    P32_ROLE_SLAVE_LEG_RIGHT = 0x25,
    P32_ROLE_SLAVE_HAND_LEFT = 0x26,
    P32_ROLE_SLAVE_HAND_RIGHT = 0x27
} p32_node_role_t;

// Node Status
typedef enum {
    P32_STATUS_OFFLINE = 0,
    P32_STATUS_INITIALIZING = 1,
    P32_STATUS_READY = 2,
    P32_STATUS_ACTIVE = 3,
    P32_STATUS_ERROR = 4,
    P32_STATUS_EMERGENCY = 5
} p32_node_status_t;

// Message Header (16 bytes)
typedef struct __attribute__((packed)) {
    uint32_t magic;           // 0xP32MESH (validation)
    uint8_t msg_type;         // p32_msg_type_t
    uint8_t source_role;      // sender node role
    uint8_t dest_role;        // destination role (0xFF for broadcast)
    uint8_t sequence;         // message sequence number
    uint32_t timestamp;       // sender timestamp
    uint16_t payload_len;     // payload length in bytes
    uint16_t checksum;        // CRC16 of header + payload
} p32_mesh_header_t;

// Command Message Payload
typedef struct __attribute__((packed)) {
    uint8_t command_id;       // Command identifier
    uint8_t priority;         // 0=low, 255=emergency
    uint16_t param_count;     // Number of parameters
    // Variable length parameters follow
} p32_command_payload_t;

// Status Message Payload
typedef struct __attribute__((packed)) {
    uint8_t node_status;      // p32_node_status_t
    uint8_t battery_level;    // 0-100 percent
    uint8_t cpu_load;         // 0-100 percent
    uint8_t temperature;      // Celsius (offset by 40, so 0 = -40 degC)
    uint32_t uptime_sec;      // Uptime in seconds
    uint16_t error_count;     // Total error count since boot
    uint16_t component_mask;  // Bitmask of active components
} p32_status_payload_t;

// Heartbeat Message Payload  
typedef struct __attribute__((packed)) {
    uint32_t node_id;         // Unique node identifier
    uint8_t firmware_version; // Firmware version number
    uint8_t config_version;   // Configuration version
    uint16_t loop_count;      // Main loop iteration count (for timing sync)
} p32_heartbeat_payload_t;

// Complete Message Structure
typedef struct __attribute__((packed)) {
    p32_mesh_header_t header;
    union {
        p32_command_payload_t command;
        p32_status_payload_t status;
        p32_heartbeat_payload_t heartbeat;
        uint8_t raw_payload[P32_MESH_PAYLOAD_SIZE];
    } payload;
} p32_mesh_message_t;

// Node Information
typedef struct {
    uint8_t mac_addr[6];      // ESP32 MAC address
    p32_node_role_t role;     // Node role in mesh
    p32_node_status_t status; // Current status
    uint32_t last_seen;       // Last heartbeat timestamp
    uint16_t message_count;   // Messages received from this node
    bool is_connected;        // Connection status
} p32_node_info_t;

// Mesh Network State
typedef struct {
    p32_node_role_t local_role;                    // This node's role
    p32_node_info_t nodes[P32_MESH_MAX_NODES];     // Known nodes
    uint8_t node_count;                            // Number of active nodes
    uint32_t network_start_time;                   // Network initialization time
    uint16_t message_sequence;                     // Next message sequence number
    bool is_initialized;                           // Network ready flag
} p32_mesh_network_t;

// Callback function types
typedef void (*p32_mesh_receive_cb_t)(const p32_mesh_message_t* message, const uint8_t* sender_mac);
typedef void (*p32_mesh_node_change_cb_t)(const p32_node_info_t* node, bool is_online);

// Core mesh functions
esp_err_t p32_mesh_init(p32_node_role_t local_role, const char* mesh_key);
esp_err_t p32_mesh_deinit(void);
esp_err_t p32_mesh_start(void);
esp_err_t p32_mesh_stop(void);

// Message transmission
esp_err_t p32_mesh_send_command(p32_node_role_t dest_role, uint8_t command_id, 
                               const uint8_t* params, uint16_t param_len);
esp_err_t p32_mesh_send_status(void);
esp_err_t p32_mesh_send_heartbeat(void);
esp_err_t p32_mesh_broadcast_emergency(uint8_t emergency_code);

// Node management
const p32_mesh_network_t* p32_mesh_get_network_info(void);
const p32_node_info_t* p32_mesh_get_node_info(p32_node_role_t role);
bool p32_mesh_is_node_online(p32_node_role_t role);
uint8_t p32_mesh_get_online_node_count(void);

// Callback registration
esp_err_t p32_mesh_register_receive_callback(p32_mesh_receive_cb_t callback);
esp_err_t p32_mesh_register_node_change_callback(p32_mesh_node_change_cb_t callback);

// Utility functions
uint16_t p32_mesh_calculate_checksum(const uint8_t* data, uint16_t len);
bool p32_mesh_validate_message(const p32_mesh_message_t* message);
const char* p32_mesh_role_to_string(p32_node_role_t role);
const char* p32_mesh_status_to_string(p32_node_status_t status);

// Master-specific functions (only available on master node)
#if defined(P32_MESH_MASTER_NODE)
esp_err_t p32_mesh_master_send_mood_update(uint8_t mood_state, uint8_t intensity);
esp_err_t p32_mesh_master_send_behavior_command(p32_node_role_t target, const char* behavior_name);
esp_err_t p32_mesh_master_request_status_all(void);
esp_err_t p32_mesh_master_emergency_stop_all(void);
#endif

// Slave-specific functions (only available on slave nodes)
#if defined(P32_MESH_SLAVE_NODE)
esp_err_t p32_mesh_slave_report_component_status(uint16_t component_mask, uint8_t error_flags);
esp_err_t p32_mesh_slave_request_sync(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // P32_MESH_COORDINATOR_H