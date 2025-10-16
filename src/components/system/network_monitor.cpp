// src/components/system/network_monitor.c
// Network Monitor Component - WiFi connectivity monitoring
// Component Type: SYSTEM_LEVEL (attached to torso subsystem)
// Timing: hitCount 50 (executes every 5 seconds)

#include "p32_component_config.hpp"
#include "p32_shared_state.hpp"
#include "esp_log.h"
#include "esp_wifi.h"

#ifdef ENABLE_SYSTEM_COMPONENTS

static const char *TAG = "network_monitor";
static bool s_prev_wifi_status = false;

// NO ARGUMENTS - Init function signature
esp_err_t network_monitor_init(void) {
    ESP_LOGI(TAG, "Initializing network monitor...");
    ESP_LOGI(TAG, "Monitoring WiFi connectivity and RSSI");
    
    // Initialize previous state
    s_prev_wifi_status = false;
    
    ESP_LOGI(TAG, "Network monitor initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
void network_monitor_act(void) {
    // hitCount: 50 → executes every 5 seconds
    
    // Check WiFi connection status
    wifi_ap_record_t ap_info;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    bool connected = (ret == ESP_OK);
    
    // Update shared state
    g_shared_state.wifi_connected = connected;
    
    // Log status changes
    if (connected != s_prev_wifi_status) {
        if (connected) {
            ESP_LOGI(TAG, "WiFi connected to SSID: %s", ap_info.ssid);
            ESP_LOGI(TAG, "RSSI: %d dBm", ap_info.rssi);
        } else {
            ESP_LOGW(TAG, "WiFi disconnected");
        }
        s_prev_wifi_status = connected;
        
        // Mark state dirty to trigger mesh broadcast
        // mesh_mark_state_dirty();
    }
    
    // Log signal strength if connected
    if (connected) {
        ESP_LOGD(TAG, "WiFi status at loop %u: RSSI=%d dBm, Channel=%d",
                 g_loopCount, ap_info.rssi, ap_info.primary);
    }
}

#endif // ENABLE_SYSTEM_COMPONENTS
