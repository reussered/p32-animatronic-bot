/*
P32 Mesh Software Coordinator Component
High-level mesh coordination logic for distributed robot control
Handles subsystem state management, command routing, and behavior coordination
*/

#include "p32_comp_mesh_software_coordinator.h"
#include "p32_comp_mesh_coordinator.h"
#include "p32_comp_master_controller.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char* TAG = "P32_MESH_SW";

// Component state
static bool g_mesh_sw_initialized = false;
static uint64_t g_last_subsystem_check_loop = 0;
static uint64_t g_last_behavior_sync_loop = 0;
static uint64_t g_last_health_report_loop = 0;

// Software coordination configuration
static struct {
    bool enabled;
    uint32_t subsystem_check_interval_loops;
    uint32_t behavior_sync_interval_loops;
    uint32_t health_report_interval_loops;
    uint8_t max_offline_tolerance_count;
} g_mesh_sw_config = {0};

// Subsystem health tracking
static struct {
    uint8_t offline_count[8];  // Count offline periods for each subsystem role
    uint32_t last_seen_loop[8];
    bool health_warning_sent[8];
} g_subsystem_health = {0};

esp_err_t p32_comp_mesh_software_coordinator_init(void)
{
    if (g_mesh_sw_initialized) {
        ESP_LOGW(TAG, "Mesh software coordinator already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing P32 mesh software coordinator");

    // Initialize configuration
    g_mesh_sw_config.enabled = true;
    g_mesh_sw_config.subsystem_check_interval_loops = 20;  // Every 2 seconds @ 100ms loop
    g_mesh_sw_config.behavior_sync_interval_loops = 100;   // Every 10 seconds @ 100ms loop
    g_mesh_sw_config.health_report_interval_loops = 300;   // Every 30 seconds @ 100ms loop
    g_mesh_sw_config.max_offline_tolerance_count = 3;      // Allow 3 offline periods before warning

    // Initialize subsystem health tracking
    for (int i = 0; i < 8; i++) {
        g_subsystem_health.offline_count[i] = 0;
        g_subsystem_health.last_seen_loop[i] = 0;
        g_subsystem_health.health_warning_sent[i] = false;
    }

    g_mesh_sw_initialized = true;
    ESP_LOGI(TAG, "P32 mesh software coordinator initialized");

    return ESP_OK;
}

esp_err_t p32_comp_mesh_software_coordinator_act(uint64_t loop_count)
{
    if (!g_mesh_sw_initialized || !g_mesh_sw_config.enabled) {
        return ESP_OK;
    }

    // Only run coordination logic if mesh networking is online
    if (!p32_comp_mesh_coordinator_is_online()) {
        return ESP_OK;
    }

    // Subsystem health monitoring
    if ((loop_count - g_last_subsystem_check_loop) >= g_mesh_sw_config.subsystem_check_interval_loops) {
        p32_mesh_sw_check_subsystem_health(loop_count);
        g_last_subsystem_check_loop = loop_count;
    }

    // Behavior synchronization (if master)
    if (p32_comp_mesh_coordinator_is_master() && 
        (loop_count - g_last_behavior_sync_loop) >= g_mesh_sw_config.behavior_sync_interval_loops) {
        
        p32_mesh_sw_sync_behaviors(loop_count);
        g_last_behavior_sync_loop = loop_count;
    }

    // Health reporting (if master)
    if (p32_comp_mesh_coordinator_is_master() && 
        (loop_count - g_last_health_report_loop) >= g_mesh_sw_config.health_report_interval_loops) {
        
        p32_mesh_sw_report_overall_health(loop_count);
        g_last_health_report_loop = loop_count;
    }

    return ESP_OK;
}

esp_err_t p32_comp_mesh_software_coordinator_deinit(void)
{
    if (!g_mesh_sw_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Deinitializing P32 mesh software coordinator");

    g_mesh_sw_initialized = false;
    ESP_LOGI(TAG, "P32 mesh software coordinator deinitialized");

    return ESP_OK;
}

// Internal coordination functions

static void p32_mesh_sw_check_subsystem_health(uint64_t current_loop)
{
    ESP_LOGD(TAG, "Checking subsystem health (loop %llu)", current_loop);

    // Define expected subsystem roles
    p32_node_role_t expected_roles[] = {
        P32_ROLE_SLAVE_HEAD,
        P32_ROLE_SLAVE_ARM_LEFT,
        P32_ROLE_SLAVE_ARM_RIGHT,
        P32_ROLE_SLAVE_LEG_LEFT,
        P32_ROLE_SLAVE_LEG_RIGHT,
        P32_ROLE_SLAVE_HAND_LEFT,
        P32_ROLE_SLAVE_HAND_RIGHT
    };

    int healthy_count = 0;
    int total_expected = sizeof(expected_roles) / sizeof(expected_roles[0]);

    for (int i = 0; i < total_expected; i++) {
        p32_node_role_t role = expected_roles[i];
        int role_index = (int)role - (int)P32_ROLE_SLAVE_HEAD; // Convert to array index
        
        if (role_index >= 0 && role_index < 8) {
            bool is_online = false;
            
            // Check if using master controller component
            if (p32_comp_mesh_coordinator_is_master()) {
                is_online = p32_comp_master_controller_is_subsystem_online(role);
            } else {
                is_online = p32_mesh_is_node_online(role);
            }

            if (is_online) {
                // Subsystem is healthy
                g_subsystem_health.offline_count[role_index] = 0;
                g_subsystem_health.last_seen_loop[role_index] = current_loop;
                g_subsystem_health.health_warning_sent[role_index] = false;
                healthy_count++;
                
                ESP_LOGD(TAG, "Subsystem %s: HEALTHY", p32_mesh_role_to_string(role));
            } else {
                // Subsystem is offline
                g_subsystem_health.offline_count[role_index]++;
                
                ESP_LOGD(TAG, "Subsystem %s: OFFLINE (count: %d)", 
                        p32_mesh_role_to_string(role), 
                        g_subsystem_health.offline_count[role_index]);

                // Send warning if offline count exceeds tolerance
                if (g_subsystem_health.offline_count[role_index] >= g_mesh_sw_config.max_offline_tolerance_count &&
                    !g_subsystem_health.health_warning_sent[role_index]) {
                    
                    ESP_LOGW(TAG, "HEALTH WARNING: Subsystem %s has been offline for %d check periods",
                            p32_mesh_role_to_string(role),
                            g_subsystem_health.offline_count[role_index]);
                    
                    g_subsystem_health.health_warning_sent[role_index] = true;
                }
            }
        }
    }

    ESP_LOGD(TAG, "Subsystem health check: %d/%d systems healthy", healthy_count, total_expected);
}

static void p32_mesh_sw_sync_behaviors(uint64_t current_loop)
{
    ESP_LOGD(TAG, "Synchronizing behaviors across subsystems (loop %llu)", current_loop);

    // Get current robot state from master controller
    const p32_master_state_t* master_state = p32_comp_master_controller_get_state();
    if (!master_state) {
        ESP_LOGW(TAG, "Cannot sync behaviors - master controller not available");
        return;
    }

    // Check if global behavior needs to be updated based on subsystem states
    uint8_t online_subsystems = p32_comp_master_controller_get_online_subsystem_count();
    
    // Adaptive behavior based on subsystem availability
    if (online_subsystems < 3) {
        // If less than 3 subsystems are online, go to safe mode
        ESP_LOGI(TAG, "Low subsystem count (%d) - switching to safe behavior", online_subsystems);
        p32_comp_master_controller_set_behavior(P32_BEHAVIOR_DEFENSIVE);
    } else if (online_subsystems >= 5) {
        // If most subsystems are online, allow normal operation
        ESP_LOGD(TAG, "Good subsystem count (%d) - normal operation", online_subsystems);
        // Could update to P32_BEHAVIOR_ACTIVE or based on mood
    }

    // Sync mood intensity based on overall system health
    if (master_state->robot_state.has_subsystem_errors) {
        ESP_LOGD(TAG, "Subsystem errors detected - reducing mood intensity");
        // Could reduce mood intensity or switch to cautious mood
    }
}

static void p32_mesh_sw_report_overall_health(uint64_t current_loop)
{
    ESP_LOGI(TAG, "=== MESH NETWORK HEALTH REPORT (Loop %llu) ===", current_loop);

    // Network statistics
    uint8_t total_nodes = p32_comp_mesh_coordinator_get_online_nodes();
    ESP_LOGI(TAG, "Total online nodes: %d", total_nodes);

    // Master controller statistics (if available)
    if (p32_comp_mesh_coordinator_is_master()) {
        const p32_master_state_t* master_state = p32_comp_master_controller_get_state();
        if (master_state) {
            ESP_LOGI(TAG, "Master controller status:");
            ESP_LOGI(TAG, "  Online subsystems: %d", master_state->robot_state.online_subsystems);
            ESP_LOGI(TAG, "  Active subsystems: %d", master_state->robot_state.active_subsystems);
            ESP_LOGI(TAG, "  Current mood: %s (intensity: %d)", 
                    p32_master_mood_to_string(master_state->robot_state.current_mood),
                    master_state->robot_state.mood_intensity);
            ESP_LOGI(TAG, "  Global behavior: %s", 
                    p32_master_behavior_to_string(master_state->robot_state.global_behavior));
            ESP_LOGI(TAG, "  Emergency stop: %s", 
                    master_state->robot_state.is_emergency_stop ? "ACTIVE" : "CLEAR");
            ESP_LOGI(TAG, "  Subsystem errors: %s", 
                    master_state->robot_state.has_subsystem_errors ? "YES" : "NO");
        }
    }

    // Subsystem health summary
    int healthy_subsystems = 0;
    int warning_subsystems = 0;
    
    for (int i = 0; i < 8; i++) {
        if (g_subsystem_health.offline_count[i] == 0) {
            healthy_subsystems++;
        } else if (g_subsystem_health.offline_count[i] >= g_mesh_sw_config.max_offline_tolerance_count) {
            warning_subsystems++;
        }
    }

    ESP_LOGI(TAG, "Subsystem health: %d healthy, %d warnings", healthy_subsystems, warning_subsystems);
    ESP_LOGI(TAG, "=== END HEALTH REPORT ===");
}

// Public interface functions

bool p32_comp_mesh_software_coordinator_is_healthy(void)
{
    if (!g_mesh_sw_initialized) {
        return false;
    }

    // System is healthy if mesh is online and at least 3 subsystems are working
    return p32_comp_mesh_coordinator_is_online() && 
           p32_comp_master_controller_get_online_subsystem_count() >= 3;
}

uint8_t p32_comp_mesh_software_coordinator_get_health_score(void)
{
    if (!g_mesh_sw_initialized) {
        return 0;
    }

    // Calculate health score based on online subsystems and error states
    uint8_t online_count = p32_comp_master_controller_get_online_subsystem_count();
    uint8_t max_expected = 7; // Head, 2 arms, 2 legs, 2 hands
    
    // Base score from subsystem availability
    uint8_t base_score = (online_count * 100) / max_expected;
    
    // Reduce score for subsystem errors
    const p32_master_state_t* master_state = p32_comp_master_controller_get_state();
    if (master_state && master_state->robot_state.has_subsystem_errors) {
        base_score = (base_score * 80) / 100; // 20% penalty for errors
    }
    
    // Ensure mesh networking is working
    if (!p32_comp_mesh_coordinator_is_online()) {
        base_score = (base_score * 50) / 100; // 50% penalty for no mesh
    }
    
    return base_score > 100 ? 100 : base_score;
}

esp_err_t p32_comp_mesh_software_coordinator_request_emergency_stop(void)
{
    if (!g_mesh_sw_initialized) {
        return ESP_FAIL;
    }

    ESP_LOGW(TAG, "Emergency stop requested through software coordinator");
    
    if (p32_comp_mesh_coordinator_is_master()) {
        return p32_comp_master_controller_emergency_stop();
    } else {
        // For slave nodes, send emergency message through mesh
        return p32_comp_mesh_coordinator_send_command(P32_ROLE_MASTER, 0xFF, NULL, 0);
    }
}