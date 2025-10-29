/*
P32 Mesh Coordinator Component
Integrates ESP-NOW mesh networking as a P32 component in the universal loop architecture
*/

#include "p32_comp_mesh_coordinator.h"
#include "p32_mesh_coordinator.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char* TAG = "P32_COMP_MESH";

// Component state
static bool g_mesh_component_initialized = false;
static p32_node_role_t g_local_role = P32_ROLE_MASTER;
static uint64_t g_last_heartbeat_loop = 0;
static uint64_t g_last_status_loop = 0;

// Configuration loaded from JSON
static struct {
    bool enabled;
    p32_node_role_t role;
    const char* mesh_key;
    uint32_t heartbeat_interval_loops;
    uint32_t status_interval_loops;
} g_mesh_config = {0};

esp_err_t p32_comp_mesh_coordinator_init(void)
{
    if (g_mesh_component_initialized) {
        ESP_LOGW(TAG, "Mesh coordinator component already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing P32 mesh coordinator component");

    // TODO: Load configuration from JSON positioned component
    // For now, use default configuration based on build defines
    g_mesh_config.enabled = true;
    
#if defined(P32_MESH_MASTER_NODE)
    g_mesh_config.role = P32_ROLE_MASTER;
    ESP_LOGI(TAG, "Configured as MASTER node");
#elif defined(P32_MESH_SLAVE_HEAD)
    g_mesh_config.role = P32_ROLE_SLAVE_HEAD;
    ESP_LOGI(TAG, "Configured as HEAD slave node");
#elif defined(P32_MESH_SLAVE_ARM_LEFT)
    g_mesh_config.role = P32_ROLE_SLAVE_ARM_LEFT;
    ESP_LOGI(TAG, "Configured as ARM_LEFT slave node");
#elif defined(P32_MESH_SLAVE_ARM_RIGHT)
    g_mesh_config.role = P32_ROLE_SLAVE_ARM_RIGHT;
    ESP_LOGI(TAG, "Configured as ARM_RIGHT slave node");
#else
    // Default to master for development
    g_mesh_config.role = P32_ROLE_MASTER;
    ESP_LOGI(TAG, "Defaulting to MASTER node (no role defined)");
#endif

    g_mesh_config.mesh_key = "P32_GOBLIN_MESH_2025";  // TODO: Load from config
    g_mesh_config.heartbeat_interval_loops = 10;       // Every 1 second @ 100ms loop
    g_mesh_config.status_interval_loops = 50;          // Every 5 seconds @ 100ms loop

    g_local_role = g_mesh_config.role;

    // Initialize mesh networking
    esp_err_t ret = p32_mesh_init(g_local_role, g_mesh_config.mesh_key);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize mesh networking: %s", esp_err_to_name(ret));
        return ret;
    }

    // Start mesh networking
    ret = p32_mesh_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start mesh networking: %s", esp_err_to_name(ret));
        return ret;
    }

    g_mesh_component_initialized = true;
    ESP_LOGI(TAG, "P32 mesh coordinator component initialized as %s", 
             p32_mesh_role_to_string(g_local_role));

    return ESP_OK;
}

esp_err_t p32_comp_mesh_coordinator_act(uint64_t loop_count)
{
    if (!g_mesh_component_initialized || !g_mesh_config.enabled) {
        return ESP_OK;
    }

    // Send heartbeat at regular intervals
    if ((loop_count - g_last_heartbeat_loop) >= g_mesh_config.heartbeat_interval_loops) {
        esp_err_t ret = p32_mesh_send_heartbeat();
        if (ret == ESP_OK) {
            ESP_LOGD(TAG, "Heartbeat sent (loop %llu)", loop_count);
        } else {
            ESP_LOGW(TAG, "Failed to send heartbeat: %s", esp_err_to_name(ret));
        }
        g_last_heartbeat_loop = loop_count;
    }

    // Send status updates (for slave nodes)
    if (g_local_role != P32_ROLE_MASTER && 
        (loop_count - g_last_status_loop) >= g_mesh_config.status_interval_loops) {
        
        esp_err_t ret = p32_mesh_send_status();
        if (ret == ESP_OK) {
            ESP_LOGD(TAG, "Status sent (loop %llu)", loop_count);
        } else {
            ESP_LOGW(TAG, "Failed to send status: %s", esp_err_to_name(ret));
        }
        g_last_status_loop = loop_count;
    }

    return ESP_OK;
}

esp_err_t p32_comp_mesh_coordinator_deinit(void)
{
    if (!g_mesh_component_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Deinitializing P32 mesh coordinator component");

    // Stop and deinitialize mesh networking
    p32_mesh_stop();
    p32_mesh_deinit();

    g_mesh_component_initialized = false;
    ESP_LOGI(TAG, "P32 mesh coordinator component deinitialized");

    return ESP_OK;
}

// Utility functions for other components to use
bool p32_comp_mesh_coordinator_is_online(void)
{
    return g_mesh_component_initialized && g_mesh_config.enabled;
}

p32_node_role_t p32_comp_mesh_coordinator_get_role(void)
{
    return g_local_role;
}

bool p32_comp_mesh_coordinator_is_master(void)
{
    return g_local_role == P32_ROLE_MASTER;
}

uint8_t p32_comp_mesh_coordinator_get_online_nodes(void)
{
    if (!g_mesh_component_initialized) {
        return 0;
    }
    
    return p32_mesh_get_online_node_count();
}

// Send command from component (wrapper)
esp_err_t p32_comp_mesh_coordinator_send_command(p32_node_role_t dest_role, 
                                                 uint8_t command_id, 
                                                 const uint8_t* params, 
                                                 uint16_t param_len)
{
    if (!g_mesh_component_initialized) {
        ESP_LOGW(TAG, "Mesh coordinator not initialized");
        return ESP_FAIL;
    }

    return p32_mesh_send_command(dest_role, command_id, params, param_len);
}
