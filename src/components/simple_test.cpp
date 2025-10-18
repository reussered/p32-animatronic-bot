#include "testing_framework.hpp"
#include "esp_log.h"

static const char* TAG = "SIMPLE_TEST";

// Global testing framework instance - this is what makes testing active
TestFramework g_TestFramework;

/**
 * @brief Initialize the simple test component
 * 
 * This component's mere presence activates testing mode.
 * Other components check g_Testing to decide behavior.
 */
esp_err_t simple_test_init(void)
{
    ESP_LOGI(TAG, "🧪 TESTING FRAMEWORK ACTIVATED");
    ESP_LOGI(TAG, "    Mode: SIMPLE_TEST (printf-based output)");
    ESP_LOGI(TAG, "    Hardware initialization: BYPASSED");
    ESP_LOGI(TAG, "    Animation rendering: CONSOLE OUTPUT");
    return ESP_OK;
}

/**
 * @brief Simple test component action - Test Orchestrator
 * 
 * This automatically cycles through different test modes to validate
 * all components systematically. Add/remove this component to enable/disable testing.
 */
void simple_test_act(void)
{
    static int cycle_counter = 0;
    static int current_phase = 0;
    
    cycle_counter++;
    
    // Every 5 seconds at 12.6Hz (63 cycles), advance to next test phase
    if (cycle_counter % 63 == 0) {
        current_phase++;
        
        switch (current_phase % 6) {
            case 0:
                g_Testing = TEST_MODE_SIMPLE;
                ESP_LOGI(TAG, "🧪 PHASE 1: SIMPLE TEST MODE");
                ESP_LOGI(TAG, "    → Components use printf output");
                break;
                
            case 1:
                g_Testing = TEST_MODE_ANIMATION;
                ESP_LOGI(TAG, "🧪 PHASE 2: ANIMATION TEST MODE");
                ESP_LOGI(TAG, "    → Testing animation systems without hardware");
                break;
                
            case 2:
                g_Testing = TEST_MODE_HARDWARE;
                ESP_LOGI(TAG, "🧪 PHASE 3: HARDWARE TEST MODE");
                ESP_LOGI(TAG, "    → Testing with detailed hardware logging");
                break;
                
            case 3:
                g_Testing = TEST_MODE_SIMULATION;
                ESP_LOGI(TAG, "🧪 PHASE 4: SIMULATION MODE");
                ESP_LOGI(TAG, "    → Full simulation with mock hardware");
                break;
                
            case 4:
                g_Testing = TEST_MODE_DISABLED;
                ESP_LOGI(TAG, "🧪 PHASE 5: PRODUCTION MODE");
                ESP_LOGI(TAG, "    → Normal hardware operation");
                break;
                
            case 5:
                ESP_LOGI(TAG, "🧪 PHASE 6: CYCLING BACK TO START");
                ESP_LOGI(TAG, "    → Comprehensive test cycle complete!");
                current_phase = -1; // Will become 0 on next increment
                break;
        }
    }
    
    // Heartbeat every 2 seconds
    if (cycle_counter % 25 == 0) {
        ESP_LOGI(TAG, "🧪 Test Orchestrator: Phase %d, Mode %d (%d cycles)", 
                 current_phase % 6, g_Testing, cycle_counter);
    }
}