/*
P32 Master Controller Component
Torso master controller that coordinates all subsystems via mesh networking
Integrates with P32 universal loop architecture
*/

#include "p32_comp_master_controller.h"
#include "p32_mesh_master.h"
#include "p32_comp_mesh_coordinator.h"
#include "esp_log.h"

static const char* TAG = "P32_COMP_MASTER";

// Component state
static bool g_master_component_initialized = false;
static uint64_t g_last_status_request_loop = 0;
static uint64_t g_last_mood_update_loop = 0;

// Configuration
static struct {
    bool enabled;
    uint32_t status_request_interval_loops;  // How often to request subsystem status
    uint32_t mood_update_interval_loops;     // How often to broadcast mood updates
    uint8_t expected_subsystems;
} g_master_config = {0};

esp_err_t p32_comp_master_controller_init(void)
{
    if (g_master_component_initialized) {
        ESP_LOGW(TAG, "Master controller component already initialized");
        return ESP_OK;
    }

    // Only initialize if this is a master node
    if (!p32_comp_mesh_coordinator_is_master()) {
        ESP_LOGI(TAG, "Not a master node - master controller component disabled");
        g_master_config.enabled = false;
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing P32 master controller component");

    // TODO: Load configuration from JSON positioned component
    g_master_config.enabled = true;
    g_master_config.status_request_interval_loops = 100;  // Every 10 seconds @ 100ms loop
    g_master_config.mood_update_interval_loops = 50;      // Every 5 seconds @ 100ms loop  
    g_master_config.expected_subsystems = 6;              // Head, 2 arms, 2 legs, 2 hands

    // Initialize master controller
    p32_master_config_t master_config = {
        .mesh_key = "P32_GOBLIN_MESH_2025",
        .expected_subsystems = g_master_config.expected_subsystems,
        .status_request_interval_ms = 10000,
        .subsystem_timeout_ms = 15000
    };

    esp_err_t ret = p32_master_init(&master_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize master controller: %s", esp_err_to_name(ret));
        return ret;
    }

    // Start master controller
    ret = p32_master_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start master controller: %s", esp_err_to_name(ret));
        return ret;
    }

    g_master_component_initialized = true;
    ESP_LOGI(TAG, "P32 master controller component initialized");

    return ESP_OK;
}

esp_err_t p32_comp_master_controller_act(uint64_t loop_count)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        return ESP_OK;
    }

    // Request status from all subsystems periodically
    if ((loop_count - g_last_status_request_loop) >= g_master_config.status_request_interval_loops) {
        
        const p32_master_state_t* master_state = p32_master_get_state();
        if (master_state) {
            // Request status from all known subsystems
            p32_node_role_t subsystem_roles[] = {
                P32_ROLE_SLAVE_HEAD,
                P32_ROLE_SLAVE_ARM_LEFT, 
                P32_ROLE_SLAVE_ARM_RIGHT,
                P32_ROLE_SLAVE_LEG_LEFT,
                P32_ROLE_SLAVE_LEG_RIGHT,
                P32_ROLE_SLAVE_HAND_LEFT,
                P32_ROLE_SLAVE_HAND_RIGHT
            };

            for (int i = 0; i < sizeof(subsystem_roles)/sizeof(subsystem_roles[0]); i++) {
                if (p32_mesh_is_node_online(subsystem_roles[i])) {
                    esp_err_t ret = p32_master_request_subsystem_status(subsystem_roles[i]);
                    if (ret != ESP_OK) {
                        ESP_LOGW(TAG, "Failed to request status from %s", 
                                p32_mesh_role_to_string(subsystem_roles[i]));
                    }
                }
            }

            ESP_LOGD(TAG, "Status requests sent to online subsystems (loop %llu)", loop_count);
        }
        
        g_last_status_request_loop = loop_count;
    }

    // Periodic mood updates (could be triggered by mood changes elsewhere)
    if ((loop_count - g_last_mood_update_loop) >= g_master_config.mood_update_interval_loops) {
        
        // TODO: Get current mood from mood processor component
        // For now, send a default mood update
        p32_master_set_global_mood(P32_MOOD_CONTENTMENT, 60);
        
        g_last_mood_update_loop = loop_count;
        ESP_LOGD(TAG, "Mood update broadcast (loop %llu)", loop_count);
    }

    return ESP_OK;
}

esp_err_t p32_comp_master_controller_deinit(void)
{
    if (!g_master_component_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Deinitializing P32 master controller component");

    // Master controller cleanup happens in mesh layer
    g_master_component_initialized = false;
    ESP_LOGI(TAG, "P32 master controller component deinitialized");

    return ESP_OK;
}

// Public interface functions for other components

esp_err_t p32_comp_master_controller_set_mood(p32_mood_t mood, uint8_t intensity)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        ESP_LOGW(TAG, "Master controller not available");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting global mood: %s (intensity: %d)", 
             p32_master_mood_to_string(mood), intensity);

    return p32_master_set_global_mood(mood, intensity);
}

esp_err_t p32_comp_master_controller_set_behavior(p32_behavior_t behavior)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        ESP_LOGW(TAG, "Master controller not available");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting global behavior: %s", p32_master_behavior_to_string(behavior));

    return p32_master_set_global_behavior(behavior);
}

esp_err_t p32_comp_master_controller_move_servo(p32_node_role_t subsystem, 
                                               uint8_t servo_id, 
                                               int16_t position, 
                                               uint16_t speed)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        ESP_LOGW(TAG, "Master controller not available");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Servo command: %s servo %d -> %d @ %d", 
             p32_mesh_role_to_string(subsystem), servo_id, position, speed);

    return p32_master_send_servo_command(subsystem, servo_id, position, speed);
}

esp_err_t p32_comp_master_controller_emergency_stop(void)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        ESP_LOGW(TAG, "Master controller not available");
        return ESP_FAIL;
    }

    ESP_LOGW(TAG, "EMERGENCY STOP triggered from component");
    return p32_master_emergency_stop();
}

const p32_master_state_t* p32_comp_master_controller_get_state(void)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        return NULL;
    }

    return p32_master_get_state();
}

bool p32_comp_master_controller_is_subsystem_online(p32_node_role_t subsystem)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        return false;
    }

    return p32_mesh_is_node_online(subsystem);
}

uint8_t p32_comp_master_controller_get_online_subsystem_count(void)
{
    if (!g_master_component_initialized || !g_master_config.enabled) {
        return 0;
    }

    return p32_mesh_get_online_node_count();
}