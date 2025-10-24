/**
 * @file debug_controller.cpp
 * @brief System-level debug flag controller
 * @author Auto-generated from JSON specification
 */

#include "include/core/memory/SharedMemory.hpp"
#include "shared/Environment.hpp"
#include "p32_shared_state.h"
#include "esp_log.h"

static const char* TAG = "DEBUG_CTRL";

// External GSM instance
extern SharedMemory GSM;

/**
 * @brief Initialize debug controller component
 * Called once during system startup
 */
void debug_controller_init(void) {
    ESP_LOGI(TAG, "Initializing system debug controller");
    
    // Read current environment state (creates with defaults if doesn't exist)
    Environment* env = GSM.read<Environment>("environment");
    if (!env) {
        ESP_LOGE(TAG, "Failed to access Environment from SharedMemory");
        return;
    }
    
    // Set default debug flags based on build configuration
    env->debugging_enabled = true;
    env->verbose_logging = true;
    env->performance_monitoring = false;
    env->display_diagnostics = true;
    
    // Write back to SharedMemory to broadcast to all subsystems
    GSM.write<Environment>("environment");
    
    ESP_LOGI(TAG, "Debug flags initialized - debugging: %s, verbose: %s, perf: %s, display: %s",
        env->debugging_enabled ? "ON" : "OFF",
        env->verbose_logging ? "ON" : "OFF", 
        env->performance_monitoring ? "ON" : "OFF",
        env->display_diagnostics ? "ON" : "OFF");
}

/**
 * @brief Execute debug controller logic
 * Called every 100 loop iterations based on hitCount
 */
void debug_controller_act(void) {
    // Read current environment state
    Environment* env = GSM.read<Environment>("environment");
    if (!env) return;
    
    // Could add runtime debug flag changes based on:
    // - Serial commands
    // - Button presses  
    // - Network commands
    // - Time-based changes
    
    // For now, just monitor and log state changes periodically
    static uint32_t last_log_loop = 0;
    if (g_loopCount - last_log_loop > 10000) { // Every ~10 seconds
        ESP_LOGD(TAG, "Debug state - debug:%s verbose:%s perf:%s display:%s",
            env->debugging_enabled ? "1" : "0",
            env->verbose_logging ? "1" : "0",
            env->performance_monitoring ? "1" : "0", 
            env->display_diagnostics ? "1" : "0");
        last_log_loop = g_loopCount;
    }
}