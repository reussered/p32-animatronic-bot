/*
P32 Master Controller - Torso Mesh Coordinator
Integrates mesh networking with the main P32 component system for torso master controller
*/

#include "p32_mesh_master.h"
#include "p32_mesh_coordinator.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "P32_MASTER";

// Master controller state
static p32_master_state_t g_master_state = {0};
static bool g_master_initialized = false;

// Callbacks for mesh events
static void p32_master_on_message_received(const p32_mesh_message_t* message, const uint8_t* sender_mac);
static void p32_master_on_node_change(const p32_node_info_t* node, bool is_online);

esp_err_t p32_master_init(const p32_master_config_t* config)
{
    if (g_master_initialized) {
        ESP_LOGW(TAG, "Master already initialized");
        return ESP_OK;
    }

    if (!config) {
        ESP_LOGE(TAG, "Invalid config");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Initializing P32 master controller");

    // Store configuration
    memcpy(&g_master_state.config, config, sizeof(p32_master_config_t));
    
    // Initialize mesh network as master
    esp_err_t ret = p32_mesh_init(P32_ROLE_MASTER, config->mesh_key);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize mesh: %s", esp_err_to_name(ret));
        return ret;
    }

    // Register mesh callbacks
    p32_mesh_register_receive_callback(p32_master_on_message_received);
    p32_mesh_register_node_change_callback(p32_master_on_node_change);

    // Initialize subsystem states
    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        g_master_state.subsystems[i].role = P32_ROLE_MASTER + i + 1; // Assign roles
        g_master_state.subsystems[i].status = P32_SUBSYS_OFFLINE;
        g_master_state.subsystems[i].last_command_time = 0;
        g_master_state.subsystems[i].command_sequence = 0;
    }

    // Initialize global robot state
    g_master_state.robot_state.current_mood = P32_MOOD_NEUTRAL;
    g_master_state.robot_state.mood_intensity = 50;
    g_master_state.robot_state.is_emergency_stop = false;
    g_master_state.robot_state.global_behavior = P32_BEHAVIOR_IDLE;

    g_master_initialized = true;
    ESP_LOGI(TAG, "P32 master controller initialized");

    return ESP_OK;
}

esp_err_t p32_master_start(void)
{
    if (!g_master_initialized) {
        ESP_LOGE(TAG, "Master not initialized");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Starting P32 master controller");

    // Start mesh network
    esp_err_t ret = p32_mesh_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start mesh: %s", esp_err_to_name(ret));
        return ret;
    }

    g_master_state.is_active = true;
    ESP_LOGI(TAG, "P32 master controller started");

    return ESP_OK;
}

esp_err_t p32_master_set_global_mood(p32_mood_t mood, uint8_t intensity)
{
    if (!g_master_initialized || !g_master_state.is_active) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting global mood: %s (intensity: %d)", 
            p32_master_mood_to_string(mood), intensity);

    g_master_state.robot_state.current_mood = mood;
    g_master_state.robot_state.mood_intensity = intensity;

    // Broadcast mood update to all subsystems
    return p32_mesh_master_send_mood_update((uint8_t)mood, intensity);
}

esp_err_t p32_master_set_global_behavior(p32_behavior_t behavior)
{
    if (!g_master_initialized || !g_master_state.is_active) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting global behavior: %s", p32_master_behavior_to_string(behavior));

    g_master_state.robot_state.global_behavior = behavior;

    // Send behavior command to relevant subsystems
    uint8_t behavior_param = (uint8_t)behavior;

    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        if (g_master_state.subsystems[i].status == P32_SUBSYS_ACTIVE) {
            p32_mesh_send_command(g_master_state.subsystems[i].role, 
                                P32_CMD_SET_BEHAVIOR, &behavior_param, 1);
        }
    }

    return ESP_OK;
}

esp_err_t p32_master_send_servo_command(p32_node_role_t target_subsystem, 
                                       uint8_t servo_id, int16_t position, uint16_t speed)
{
    if (!g_master_initialized || !g_master_state.is_active) {
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Servo command to %s: servo %d -> %d @ speed %d",
            p32_mesh_role_to_string(target_subsystem), servo_id, position, speed);

    // Pack servo command parameters
    uint8_t params[5];
    params[0] = servo_id;
    params[1] = (position >> 8) & 0xFF;  // Position high byte
    params[2] = position & 0xFF;         // Position low byte  
    params[3] = (speed >> 8) & 0xFF;     // Speed high byte
    params[4] = speed & 0xFF;            // Speed low byte

    return p32_mesh_send_command(target_subsystem, P32_CMD_SERVO_POSITION, params, 5);
}

esp_err_t p32_master_emergency_stop(void)
{
    ESP_LOGW(TAG, "EMERGENCY STOP ACTIVATED");

    g_master_state.robot_state.is_emergency_stop = true;
    
    // Broadcast emergency stop to all nodes
    return p32_mesh_master_emergency_stop_all();
}

esp_err_t p32_master_clear_emergency_stop(void)
{
    ESP_LOGI(TAG, "Emergency stop cleared");

    g_master_state.robot_state.is_emergency_stop = false;

    // Send resume command to all subsystems
    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        if (g_master_state.subsystems[i].status != P32_SUBSYS_OFFLINE) {
            p32_mesh_send_command(g_master_state.subsystems[i].role, P32_CMD_RESUME, NULL, 0);
        }
    }

    return ESP_OK;
}

const p32_master_state_t* p32_master_get_state(void)
{
    return &g_master_state;
}

const p32_subsystem_state_t* p32_master_get_subsystem_state(p32_node_role_t role)
{
    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        if (g_master_state.subsystems[i].role == role) {
            return &g_master_state.subsystems[i];
        }
    }
    return NULL;
}

esp_err_t p32_master_request_subsystem_status(p32_node_role_t target)
{
    if (!g_master_initialized || !g_master_state.is_active) {
        return ESP_FAIL;
    }

    return p32_mesh_send_command(target, P32_CMD_REQUEST_STATUS, NULL, 0);
}

// Callback for received mesh messages
static void p32_master_on_message_received(const p32_mesh_message_t* message, const uint8_t* sender_mac)
{
    if (!message) return;

    // Find subsystem by role
    p32_subsystem_state_t* subsystem = NULL;
    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        if (g_master_state.subsystems[i].role == message->header.source_role) {
            subsystem = &g_master_state.subsystems[i];
            break;
        }
    }

    switch (message->header.msg_type) {
        case P32_MSG_STATUS:
            if (subsystem) {
                // Update subsystem status from message
                subsystem->battery_level = message->payload.status.battery_level;
                subsystem->cpu_load = message->payload.status.cpu_load;
                subsystem->temperature = message->payload.status.temperature - 40; // Convert back to Celsius
                subsystem->uptime_sec = message->payload.status.uptime_sec;
                subsystem->error_count = message->payload.status.error_count;
                subsystem->component_mask = message->payload.status.component_mask;
                subsystem->last_status_time = esp_timer_get_time() / 1000;

                ESP_LOGD(TAG, "Status update from %s: battery=%d%%, cpu=%d%%, temp=%d degC",
                        p32_mesh_role_to_string(message->header.source_role),
                        subsystem->battery_level, subsystem->cpu_load, subsystem->temperature);
            }
            break;

        case P32_MSG_HEARTBEAT:
            // Heartbeat handling is done in mesh layer
            break;

        case P32_MSG_COMMAND:
            // Handle commands sent to master (unusual but possible)
            ESP_LOGI(TAG, "Received command %d from %s", 
                    message->payload.command.command_id,
                    p32_mesh_role_to_string(message->header.source_role));
            break;

        case P32_MSG_EMERGENCY:
            ESP_LOGW(TAG, "Emergency message from %s", 
                    p32_mesh_role_to_string(message->header.source_role));
            // Could trigger master-level emergency response
            break;

        default:
            ESP_LOGW(TAG, "Unknown message type from %s: 0x%02X",
                    p32_mesh_role_to_string(message->header.source_role),
                    message->header.msg_type);
            break;
    }
}

// Callback for node online/offline changes
static void p32_master_on_node_change(const p32_node_info_t* node, bool is_online)
{
    if (!node) return;

    // Find subsystem
    p32_subsystem_state_t* subsystem = NULL;
    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        if (g_master_state.subsystems[i].role == node->role) {
            subsystem = &g_master_state.subsystems[i];
            break;
        }
    }

    if (subsystem) {
        p32_subsystem_status_t old_status = subsystem->status;
        
        if (is_online) {
            if (subsystem->status == P32_SUBSYS_OFFLINE) {
                subsystem->status = P32_SUBSYS_READY;
                ESP_LOGI(TAG, "Subsystem %s came online", p32_mesh_role_to_string(node->role));
            }
        } else {
            subsystem->status = P32_SUBSYS_OFFLINE;
            ESP_LOGW(TAG, "Subsystem %s went offline", p32_mesh_role_to_string(node->role));
        }

        // Update global robot status based on subsystem changes
        if (old_status != subsystem->status) {
            p32_master_update_robot_status();
        }
    }
}

// Update overall robot status based on subsystem states
void p32_master_update_robot_status(void)
{
    uint8_t online_count = 0;
    uint8_t active_count = 0;
    bool has_errors = false;

    for (int i = 0; i < P32_MAX_SUBSYSTEMS; i++) {
        if (g_master_state.subsystems[i].status != P32_SUBSYS_OFFLINE) {
            online_count++;
            
            if (g_master_state.subsystems[i].status == P32_SUBSYS_ACTIVE) {
                active_count++;
            }
            
            if (g_master_state.subsystems[i].error_count > 0) {
                has_errors = true;
            }
        }
    }

    g_master_state.robot_state.online_subsystems = online_count;
    g_master_state.robot_state.active_subsystems = active_count;
    g_master_state.robot_state.has_subsystem_errors = has_errors;

    ESP_LOGD(TAG, "Robot status: %d/%d subsystems online/active, errors: %s",
            online_count, active_count, has_errors ? "YES" : "NO");
}

// Utility functions
const char* p32_master_mood_to_string(p32_mood_t mood)
{
    switch (mood) {
        case P32_MOOD_FEAR: return "FEAR";
        case P32_MOOD_ANGER: return "ANGER";
        case P32_MOOD_IRRITATION: return "IRRITATION";
        case P32_MOOD_HAPPINESS: return "HAPPINESS";
        case P32_MOOD_CONTENTMENT: return "CONTENTMENT";
        case P32_MOOD_HUNGER: return "HUNGER";
        case P32_MOOD_CURIOSITY: return "CURIOSITY";
        case P32_MOOD_AFFECTION: return "AFFECTION";
        case P32_MOOD_NEUTRAL: return "NEUTRAL";
        default: return "UNKNOWN";
    }
}

const char* p32_master_behavior_to_string(p32_behavior_t behavior)
{
    switch (behavior) {
        case P32_BEHAVIOR_IDLE: return "IDLE";
        case P32_BEHAVIOR_ALERT: return "ALERT";
        case P32_BEHAVIOR_AGGRESSIVE: return "AGGRESSIVE";
        case P32_BEHAVIOR_PLAYFUL: return "PLAYFUL";
        case P32_BEHAVIOR_SLEEP: return "SLEEP";
        case P32_BEHAVIOR_HUNT: return "HUNT";
        case P32_BEHAVIOR_SOCIAL: return "SOCIAL";
        case P32_BEHAVIOR_DEFENSIVE: return "DEFENSIVE";
        default: return "UNKNOWN";
    }
}