/**
 * System Components - Stubs for generated component table
 * These are minimal placeholder implementations
 */

#include "p32_component_tables.hpp"
#include <esp_log.h>

namespace {
    constexpr const char* TAG_HEARTBEAT = "HEARTBEAT";
    constexpr const char* TAG_NETWORK = "NETWORK";
}

extern uint64_t g_loopCount;

// Heartbeat component - just logs that system is alive
extern "C" esp_err_t p32_comp_heartbeat_init(void)
{
    ESP_LOGI(TAG_HEARTBEAT, "Heartbeat component initialized");
    return ESP_OK;
}

extern "C" void p32_comp_heartbeat_act(void)
{
    // Execute every loop - just a heartbeat indicator
    if (g_loopCount % 1000 == 0) {  // Log every 1000 loops
        ESP_LOGI(TAG_HEARTBEAT, "💓 Heartbeat - Loop: %llu", g_loopCount);
    }
}

// Network monitor component - stub for now
extern "C" esp_err_t p32_comp_network_monitor_init(void)
{
    ESP_LOGI(TAG_NETWORK, "Network monitor component initialized (stub)");
    return ESP_OK;
}

extern "C" void p32_comp_network_monitor_act(void)
{
    // Stub - does nothing for now
    if (g_loopCount % 1000 == 0) {  // Log every 1000 loops
        ESP_LOGI(TAG_NETWORK, "📡 Network monitor - Loop: %llu", g_loopCount);
    }
}
