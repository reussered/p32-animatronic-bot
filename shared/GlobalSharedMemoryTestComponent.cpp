// GlobalSharedMemory Test Component
// Simple test component that exercises the GSM system during startup

#include "GlobalSharedMemoryTest.hpp"
#include "GlobalSharedMemory.hpp"
#include "esp_log.h"
#include "esp_err.h"

extern "C" {

static const char* TAG = "GSM_TEST_COMPONENT";
static GlobalSharedMemory* g_gsm = nullptr;
static bool test_completed = false;

// Component initialization
esp_err_t gsm_test_init() {
    ESP_LOGI(TAG, "=== GSM Test Component Starting ===");
    
    // Create GSM instance (singleton pattern would be better, but for testing...)
    g_gsm = new GlobalSharedMemory();
    
    if (g_gsm) {
        ESP_LOGI(TAG, "✓ GSM instance created successfully");
        
        // Run basic tests during init
        test_global_shared_memory();
        test_completed = true;
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "✗ Failed to create GSM instance");
        return ESP_FAIL;
    }
}

// Component action (called in main loop)
void gsm_test_act() {
    if (!g_gsm || !test_completed) {
        return;
    }
    
    // Periodic test - update a counter every time we're called
    static uint32_t test_counter = 0;
    static uint32_t last_log_time = 0;
    
    test_counter++;
    
    // Write the counter to GSM (demonstrates write operation)
    uint32_t result = g_gsm->write<uint32_t>("test_counter", &test_counter);
    if (result != GSM_SUCCESS) {
        ESP_LOGW(TAG, "Failed to write test_counter: 0x%08x", result);
    }
    
    // Read it back (demonstrates read operation)
    uint32_t read_counter = 0;
    result = g_gsm->read<uint32_t>("test_counter", &read_counter);
    if (result != GSM_SUCCESS) {
        ESP_LOGW(TAG, "Failed to read test_counter: 0x%08x", result);
        return;
    }
    
    // Log progress every 1000 loops
    if (test_counter - last_log_time >= 1000) {
        ESP_LOGI(TAG, "GSM test counter: %d (read back: %d)", test_counter, read_counter);
        
        // Test find() operation
        uint32_t* ptr = g_gsm->find<uint32_t>("test_counter");
        if (ptr) {
            ESP_LOGI(TAG, "find() returned pointer, value: %d", *ptr);
        }
        
        // Show memory usage
        ESP_LOGI(TAG, "GSM memory usage: %d bytes, %d variables", 
                g_gsm->get_total_memory(), 
                g_gsm->get_variable_count());
        
        last_log_time = test_counter;
    }
}

} // extern "C"