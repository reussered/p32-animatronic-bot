/*
P32 ESP-NOW Mesh Network Coordinator Implementation
Handles communication between master torso controller and slave subsystem controllers
*/

#include "p32_mesh_coordinator.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_crc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "string.h"

static const char* TAG = "P32_MESH";

// Magic number for message validation
#define P32_MESH_MAGIC 0x50333220  // "P32 " in hex

// Global mesh state
static p32_mesh_network_t g_mesh_network = {0};
static bool g_mesh_initialized = false;

// Callbacks
static p32_mesh_receive_cb_t g_receive_callback = NULL;
static p32_mesh_node_change_cb_t g_node_change_callback = NULL;

// Internal timers
static TimerHandle_t g_heartbeat_timer = NULL;
static TimerHandle_t g_cleanup_timer = NULL;

// Forward declarations
static void p32_mesh_esp_now_recv_cb(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len);
static void p32_mesh_esp_now_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status);
static void p32_mesh_heartbeat_timer_cb(TimerHandle_t timer);
static void p32_mesh_cleanup_timer_cb(TimerHandle_t timer);
static esp_err_t p32_mesh_add_node(const uint8_t* mac_addr, p32_node_role_t role);
static p32_node_info_t* p32_mesh_find_node_by_mac(const uint8_t* mac_addr);
static p32_node_info_t* p32_mesh_find_node_by_role(p32_node_role_t role);
static uint32_t p32_mesh_get_timestamp_ms(void);

esp_err_t p32_mesh_init(p32_node_role_t local_role, const char* mesh_key)
{
    if (g_mesh_initialized) {
        ESP_LOGW(TAG, "Mesh already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing P32 mesh network as %s", p32_mesh_role_to_string(local_role));

    // Initialize WiFi in STA mode for ESP-NOW
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    
    // Register ESP-NOW callbacks
    ESP_ERROR_CHECK(esp_now_register_recv_cb(p32_mesh_esp_now_recv_cb));
    ESP_ERROR_CHECK(esp_now_register_send_cb(p32_mesh_esp_now_send_cb));

    // Set PMK for encrypted communication
    if (mesh_key && strlen(mesh_key) >= P32_MESH_KEY_LEN) {
        ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t*)mesh_key));
    }

    // Initialize network state
    memset(&g_mesh_network, 0, sizeof(g_mesh_network));
    g_mesh_network.local_role = local_role;
    g_mesh_network.network_start_time = p32_mesh_get_timestamp_ms();
    g_mesh_network.message_sequence = esp_random() & 0xFF;
    g_mesh_network.is_initialized = true;

    // Create heartbeat timer
    g_heartbeat_timer = xTimerCreate(
        "mesh_heartbeat",
        pdMS_TO_TICKS(P32_MESH_HEARTBEAT_INTERVAL_MS),
        pdTRUE,  // Auto-reload
        NULL,
        p32_mesh_heartbeat_timer_cb
    );

    // Create cleanup timer (runs every 5 seconds to remove stale nodes)
    g_cleanup_timer = xTimerCreate(
        "mesh_cleanup",
        pdMS_TO_TICKS(5000),
        pdTRUE,  // Auto-reload
        NULL,
        p32_mesh_cleanup_timer_cb
    );

    g_mesh_initialized = true;
    ESP_LOGI(TAG, "P32 mesh network initialized successfully");

    return ESP_OK;
}

esp_err_t p32_mesh_start(void)
{
    if (!g_mesh_initialized) {
        ESP_LOGE(TAG, "Mesh not initialized");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Starting P32 mesh network");

    // Start heartbeat timer
    if (g_heartbeat_timer) {
        xTimerStart(g_heartbeat_timer, 0);
    }

    // Start cleanup timer
    if (g_cleanup_timer) {
        xTimerStart(g_cleanup_timer, 0);
    }

    // Send initial heartbeat to announce presence
    p32_mesh_send_heartbeat();

    ESP_LOGI(TAG, "P32 mesh network started");
    return ESP_OK;
}

esp_err_t p32_mesh_stop(void)
{
    if (!g_mesh_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Stopping P32 mesh network");

    // Stop timers
    if (g_heartbeat_timer) {
        xTimerStop(g_heartbeat_timer, 0);
    }
    if (g_cleanup_timer) {
        xTimerStop(g_cleanup_timer, 0);
    }

    return ESP_OK;
}

esp_err_t p32_mesh_deinit(void)
{
    if (!g_mesh_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Deinitializing P32 mesh network");

    p32_mesh_stop();

    // Clean up timers
    if (g_heartbeat_timer) {
        xTimerDelete(g_heartbeat_timer, 0);
        g_heartbeat_timer = NULL;
    }
    if (g_cleanup_timer) {
        xTimerDelete(g_cleanup_timer, 0);
        g_cleanup_timer = NULL;
    }

    // Deinitialize ESP-NOW
    esp_now_deinit();

    g_mesh_initialized = false;
    ESP_LOGI(TAG, "P32 mesh network deinitialized");

    return ESP_OK;
}

esp_err_t p32_mesh_send_heartbeat(void)
{
    if (!g_mesh_initialized) {
        return ESP_FAIL;
    }

    p32_mesh_message_t message = {0};
    
    // Fill header
    message.header.magic = P32_MESH_MAGIC;
    message.header.msg_type = P32_MSG_HEARTBEAT;
    message.header.source_role = g_mesh_network.local_role;
    message.header.dest_role = 0xFF; // Broadcast
    message.header.sequence = g_mesh_network.message_sequence++;
    message.header.timestamp = p32_mesh_get_timestamp_ms();
    message.header.payload_len = sizeof(p32_heartbeat_payload_t);

    // Fill heartbeat payload
    message.payload.heartbeat.node_id = (uint32_t)g_mesh_network.local_role;
    message.payload.heartbeat.firmware_version = 1; // TODO: Get from config
    message.payload.heartbeat.config_version = 1;   // TODO: Get from config
    message.payload.heartbeat.loop_count = (uint16_t)(xTaskGetTickCount() / 100);

    // Calculate checksum
    message.header.checksum = p32_mesh_calculate_checksum(
        (uint8_t*)&message, 
        sizeof(p32_mesh_header_t) + message.header.payload_len
    );

    // Broadcast to all peers
    esp_err_t result = esp_now_send(NULL, (uint8_t*)&message, 
                                   sizeof(p32_mesh_header_t) + message.header.payload_len);

    if (result == ESP_OK) {
        ESP_LOGD(TAG, "Heartbeat sent (seq: %d)", message.header.sequence);
    } else {
        ESP_LOGW(TAG, "Failed to send heartbeat: %s", esp_err_to_name(result));
    }

    return result;
}

esp_err_t p32_mesh_send_command(p32_node_role_t dest_role, uint8_t command_id, 
                               const uint8_t* params, uint16_t param_len)
{
    if (!g_mesh_initialized) {
        return ESP_FAIL;
    }

    // Find destination node
    p32_node_info_t* dest_node = p32_mesh_find_node_by_role(dest_role);
    if (!dest_node || !dest_node->is_connected) {
        ESP_LOGW(TAG, "Destination node %s not available", p32_mesh_role_to_string(dest_role));
        return ESP_FAIL;
    }

    p32_mesh_message_t message = {0};
    
    // Fill header
    message.header.magic = P32_MESH_MAGIC;
    message.header.msg_type = P32_MSG_COMMAND;
    message.header.source_role = g_mesh_network.local_role;
    message.header.dest_role = dest_role;
    message.header.sequence = g_mesh_network.message_sequence++;
    message.header.timestamp = p32_mesh_get_timestamp_ms();
    message.header.payload_len = sizeof(p32_command_payload_t) + param_len;

    // Check payload size
    if (message.header.payload_len > P32_MESH_PAYLOAD_SIZE) {
        ESP_LOGE(TAG, "Command payload too large: %d bytes", message.header.payload_len);
        return ESP_ERR_INVALID_SIZE;
    }

    // Fill command payload
    message.payload.command.command_id = command_id;
    message.payload.command.priority = 128; // Normal priority
    message.payload.command.param_count = param_len;

    // Copy parameters
    if (params && param_len > 0) {
        memcpy(message.payload.raw_payload + sizeof(p32_command_payload_t), params, param_len);
    }

    // Calculate checksum
    message.header.checksum = p32_mesh_calculate_checksum(
        (uint8_t*)&message, 
        sizeof(p32_mesh_header_t) + message.header.payload_len
    );

    // Send to specific node
    esp_err_t result = esp_now_send(dest_node->mac_addr, (uint8_t*)&message, 
                                   sizeof(p32_mesh_header_t) + message.header.payload_len);

    if (result == ESP_OK) {
        ESP_LOGI(TAG, "Command %d sent to %s (seq: %d)", 
                command_id, p32_mesh_role_to_string(dest_role), message.header.sequence);
    } else {
        ESP_LOGW(TAG, "Failed to send command to %s: %s", 
                p32_mesh_role_to_string(dest_role), esp_err_to_name(result));
    }

    return result;
}

esp_err_t p32_mesh_send_status(void)
{
    if (!g_mesh_initialized) {
        return ESP_FAIL;
    }

    p32_mesh_message_t message = {0};
    
    // Fill header
    message.header.magic = P32_MESH_MAGIC;
    message.header.msg_type = P32_MSG_STATUS;
    message.header.source_role = g_mesh_network.local_role;
    message.header.dest_role = P32_ROLE_MASTER; // Send status to master
    message.header.sequence = g_mesh_network.message_sequence++;
    message.header.timestamp = p32_mesh_get_timestamp_ms();
    message.header.payload_len = sizeof(p32_status_payload_t);

    // Fill status payload (TODO: Get real values from system)
    message.payload.status.node_status = P32_STATUS_ACTIVE;
    message.payload.status.battery_level = 85; // Mock battery level
    message.payload.status.cpu_load = 45;      // Mock CPU load
    message.payload.status.temperature = 65;   // 25 degC (65-40)
    message.payload.status.uptime_sec = (p32_mesh_get_timestamp_ms() - g_mesh_network.network_start_time) / 1000;
    message.payload.status.error_count = 0;    // Mock error count
    message.payload.status.component_mask = 0xFF; // All components active

    // Calculate checksum
    message.header.checksum = p32_mesh_calculate_checksum(
        (uint8_t*)&message, 
        sizeof(p32_mesh_header_t) + message.header.payload_len
    );

    // Find master node
    p32_node_info_t* master_node = p32_mesh_find_node_by_role(P32_ROLE_MASTER);
    
    esp_err_t result;
    if (master_node && master_node->is_connected) {
        // Send to master
        result = esp_now_send(master_node->mac_addr, (uint8_t*)&message, 
                             sizeof(p32_mesh_header_t) + message.header.payload_len);
    } else {
        // Broadcast if master not known
        result = esp_now_send(NULL, (uint8_t*)&message, 
                             sizeof(p32_mesh_header_t) + message.header.payload_len);
    }

    if (result == ESP_OK) {
        ESP_LOGD(TAG, "Status sent (seq: %d)", message.header.sequence);
    } else {
        ESP_LOGW(TAG, "Failed to send status: %s", esp_err_to_name(result));
    }

    return result;
}

// ESP-NOW receive callback
static void p32_mesh_esp_now_recv_cb(const esp_now_recv_info_t* recv_info, const uint8_t* data, int len)
{
    if (!recv_info || !data || len < sizeof(p32_mesh_header_t)) {
        ESP_LOGW(TAG, "Invalid message received");
        return;
    }

    p32_mesh_message_t* message = (p32_mesh_message_t*)data;

    // Validate message
    if (!p32_mesh_validate_message(message)) {
        ESP_LOGW(TAG, "Message validation failed");
        return;
    }

    // Update or add node information
    p32_node_info_t* node = p32_mesh_find_node_by_mac(recv_info->src_addr);
    if (!node) {
        // New node - add it
        if (p32_mesh_add_node(recv_info->src_addr, message->header.source_role) != ESP_OK) {
            ESP_LOGW(TAG, "Failed to add new node");
            return;
        }
        node = p32_mesh_find_node_by_mac(recv_info->src_addr);
    }

    if (node) {
        node->last_seen = p32_mesh_get_timestamp_ms();
        node->message_count++;
        if (!node->is_connected) {
            node->is_connected = true;
            ESP_LOGI(TAG, "Node %s came online", p32_mesh_role_to_string(node->role));
            
            // Notify callback
            if (g_node_change_callback) {
                g_node_change_callback(node, true);
            }
        }
    }

    // Handle message based on type
    switch (message->header.msg_type) {
        case P32_MSG_HEARTBEAT:
            ESP_LOGD(TAG, "Heartbeat from %s", p32_mesh_role_to_string(message->header.source_role));
            break;
            
        case P32_MSG_COMMAND:
            ESP_LOGI(TAG, "Command %d from %s", 
                    message->payload.command.command_id,
                    p32_mesh_role_to_string(message->header.source_role));
            break;
            
        case P32_MSG_STATUS:
            ESP_LOGD(TAG, "Status from %s: battery=%d%%, load=%d%%, uptime=%ds",
                    p32_mesh_role_to_string(message->header.source_role),
                    message->payload.status.battery_level,
                    message->payload.status.cpu_load,
                    message->payload.status.uptime_sec);
            break;
            
        case P32_MSG_EMERGENCY:
            ESP_LOGW(TAG, "EMERGENCY message from %s", 
                    p32_mesh_role_to_string(message->header.source_role));
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown message type: 0x%02X", message->header.msg_type);
            break;
    }

    // Call user callback
    if (g_receive_callback) {
        g_receive_callback(message, recv_info->src_addr);
    }
}

// ESP-NOW send callback
static void p32_mesh_esp_now_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    ESP_LOGD(TAG, "Message send %s", (status == ESP_NOW_SEND_SUCCESS) ? "success" : "failed");
}

// Heartbeat timer callback
static void p32_mesh_heartbeat_timer_cb(TimerHandle_t timer)
{
    p32_mesh_send_heartbeat();
}

// Cleanup timer callback - removes stale nodes
static void p32_mesh_cleanup_timer_cb(TimerHandle_t timer)
{
    uint32_t current_time = p32_mesh_get_timestamp_ms();
    const uint32_t timeout_ms = P32_MESH_HEARTBEAT_INTERVAL_MS * 5; // 5 missed heartbeats

    for (int i = 0; i < g_mesh_network.node_count; i++) {
        p32_node_info_t* node = &g_mesh_network.nodes[i];
        
        if (node->is_connected && 
            (current_time - node->last_seen) > timeout_ms) {
            
            ESP_LOGW(TAG, "Node %s went offline (timeout)", p32_mesh_role_to_string(node->role));
            node->is_connected = false;
            
            // Notify callback
            if (g_node_change_callback) {
                g_node_change_callback(node, false);
            }
        }
    }
}

// Utility functions
uint16_t p32_mesh_calculate_checksum(const uint8_t* data, uint16_t len)
{
    return esp_crc16_le(0, data, len);
}

bool p32_mesh_validate_message(const p32_mesh_message_t* message)
{
    if (!message) return false;
    
    // Check magic number
    if (message->header.magic != P32_MESH_MAGIC) {
        return false;
    }
    
    // Check payload length
    if (message->header.payload_len > P32_MESH_PAYLOAD_SIZE) {
        return false;
    }
    
    // Validate checksum
    uint16_t calculated_checksum = p32_mesh_calculate_checksum(
        (uint8_t*)message, 
        sizeof(p32_mesh_header_t) + message->header.payload_len
    );
    
    return calculated_checksum == message->header.checksum;
}

const char* p32_mesh_role_to_string(p32_node_role_t role)
{
    switch (role) {
        case P32_ROLE_MASTER: return "MASTER";
        case P32_ROLE_SLAVE_HEAD: return "HEAD";
        case P32_ROLE_SLAVE_ARM_LEFT: return "ARM_LEFT";
        case P32_ROLE_SLAVE_ARM_RIGHT: return "ARM_RIGHT";
        case P32_ROLE_SLAVE_LEG_LEFT: return "LEG_LEFT";
        case P32_ROLE_SLAVE_LEG_RIGHT: return "LEG_RIGHT";
        case P32_ROLE_SLAVE_HAND_LEFT: return "HAND_LEFT";
        case P32_ROLE_SLAVE_HAND_RIGHT: return "HAND_RIGHT";
        default: return "UNKNOWN";
    }
}

const char* p32_mesh_status_to_string(p32_node_status_t status)
{
    switch (status) {
        case P32_STATUS_OFFLINE: return "OFFLINE";
        case P32_STATUS_INITIALIZING: return "INITIALIZING";
        case P32_STATUS_READY: return "READY";
        case P32_STATUS_ACTIVE: return "ACTIVE";
        case P32_STATUS_ERROR: return "ERROR";
        case P32_STATUS_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

// Internal helper functions
static esp_err_t p32_mesh_add_node(const uint8_t* mac_addr, p32_node_role_t role)
{
    if (g_mesh_network.node_count >= P32_MESH_MAX_NODES) {
        ESP_LOGE(TAG, "Maximum nodes reached");
        return ESP_ERR_NO_MEM;
    }

    p32_node_info_t* node = &g_mesh_network.nodes[g_mesh_network.node_count];
    memcpy(node->mac_addr, mac_addr, 6);
    node->role = role;
    node->status = P32_STATUS_READY;
    node->last_seen = p32_mesh_get_timestamp_ms();
    node->message_count = 1;
    node->is_connected = true;

    g_mesh_network.node_count++;

    ESP_LOGI(TAG, "Added node %s (%02X:%02X:%02X:%02X:%02X:%02X)", 
            p32_mesh_role_to_string(role),
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    return ESP_OK;
}

static p32_node_info_t* p32_mesh_find_node_by_mac(const uint8_t* mac_addr)
{
    for (int i = 0; i < g_mesh_network.node_count; i++) {
        if (memcmp(g_mesh_network.nodes[i].mac_addr, mac_addr, 6) == 0) {
            return &g_mesh_network.nodes[i];
        }
    }
    return NULL;
}

static p32_node_info_t* p32_mesh_find_node_by_role(p32_node_role_t role)
{
    for (int i = 0; i < g_mesh_network.node_count; i++) {
        if (g_mesh_network.nodes[i].role == role) {
            return &g_mesh_network.nodes[i];
        }
    }
    return NULL;
}

static uint32_t p32_mesh_get_timestamp_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

// Public getter functions
const p32_mesh_network_t* p32_mesh_get_network_info(void)
{
    return &g_mesh_network;
}

const p32_node_info_t* p32_mesh_get_node_info(p32_node_role_t role)
{
    return p32_mesh_find_node_by_role(role);
}

bool p32_mesh_is_node_online(p32_node_role_t role)
{
    const p32_node_info_t* node = p32_mesh_find_node_by_role(role);
    return node && node->is_connected;
}

uint8_t p32_mesh_get_online_node_count(void)
{
    uint8_t count = 0;
    for (int i = 0; i < g_mesh_network.node_count; i++) {
        if (g_mesh_network.nodes[i].is_connected) {
            count++;
        }
    }
    return count;
}

// Callback registration
esp_err_t p32_mesh_register_receive_callback(p32_mesh_receive_cb_t callback)
{
    g_receive_callback = callback;
    return ESP_OK;
}

esp_err_t p32_mesh_register_node_change_callback(p32_mesh_node_change_cb_t callback)
{
    g_node_change_callback = callback;
    return ESP_OK;
}

// Master-specific functions
#if defined(P32_MESH_MASTER_NODE)
esp_err_t p32_mesh_master_send_mood_update(uint8_t mood_state, uint8_t intensity)
{
    // Send mood update to all nodes
    uint8_t params[2] = {mood_state, intensity};
    
    for (int i = 0; i < g_mesh_network.node_count; i++) {
        if (g_mesh_network.nodes[i].is_connected && 
            g_mesh_network.nodes[i].role != P32_ROLE_MASTER) {
            
            p32_mesh_send_command(g_mesh_network.nodes[i].role, 0x10, params, 2);
        }
    }
    
    return ESP_OK;
}

esp_err_t p32_mesh_master_emergency_stop_all(void)
{
    ESP_LOGW(TAG, "EMERGENCY STOP - Broadcasting to all nodes");
    
    p32_mesh_message_t message = {0};
    message.header.magic = P32_MESH_MAGIC;
    message.header.msg_type = P32_MSG_EMERGENCY;
    message.header.source_role = g_mesh_network.local_role;
    message.header.dest_role = 0xFF; // Broadcast
    message.header.sequence = g_mesh_network.message_sequence++;
    message.header.timestamp = p32_mesh_get_timestamp_ms();
    message.header.payload_len = 1;
    message.payload.raw_payload[0] = 0xFF; // Emergency stop code
    
    message.header.checksum = p32_mesh_calculate_checksum(
        (uint8_t*)&message, 
        sizeof(p32_mesh_header_t) + message.header.payload_len
    );
    
    return esp_now_send(NULL, (uint8_t*)&message, 
                       sizeof(p32_mesh_header_t) + message.header.payload_len);
}
#endif

// Slave-specific functions  
#if defined(P32_MESH_SLAVE_NODE)
esp_err_t p32_mesh_slave_report_component_status(uint16_t component_mask, uint8_t error_flags)
{
    // Send component status to master
    uint8_t params[3];
    params[0] = (component_mask >> 8) & 0xFF;
    params[1] = component_mask & 0xFF;
    params[2] = error_flags;
    
    return p32_mesh_send_command(P32_ROLE_MASTER, 0x20, params, 3);
}
#endif