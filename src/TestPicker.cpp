/**
 * TestPicker Component Implementation
 * 
 * Purpose: Adds comprehensive test cases to the SystemTest global variable
 * Author: reussered
 * Created: 2025-10-18
 * 
 * Features:
 * - Populates SystemTest with GC9A01 display tests
 * - Adds component validation tests
 * - Provides system health check tests
 * - Manages test execution order and priorities
 */

#include "TestPicker.hpp"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "TESTPICKER";

// External reference to SystemTest global variable
extern SystemTest g_system_test;

/**
 * Component: TestPicker
 * Function: TestPicker_init
 * Purpose: Initialize test picker and populate SystemTest with comprehensive test cases
 */
esp_err_t TestPicker_init(void) {
    ESP_LOGI(TAG, "🧪 TestPicker initializing...");
    
    // Check if SystemTest is already populated
    if (g_system_test.tests_added) {
        ESP_LOGW(TAG, "SystemTest already populated with %d tests", g_system_test.test_count);
        return ESP_OK;
    }
    
    // Clear the test array
    memset(&g_system_test, 0, sizeof(SystemTest));
    
    // Add GC9A01 Display Tests
    int test_index = 0;
    
    // Display Hardware Tests
    snprintf(g_system_test.test_names[test_index++], 64, "GC9A01_Left_Eye_Init");
    snprintf(g_system_test.test_names[test_index++], 64, "GC9A01_Right_Eye_Init");
    snprintf(g_system_test.test_names[test_index++], 64, "GC9A01_Mouth_Init");
    
    // Display Functionality Tests
    snprintf(g_system_test.test_names[test_index++], 64, "GC9A01_Clear_Screen_Test");
    snprintf(g_system_test.test_names[test_index++], 64, "GC9A01_Color_Fill_Test");
    snprintf(g_system_test.test_names[test_index++], 64, "GC9A01_Pixel_Draw_Test");
    
    // Animation System Tests
    snprintf(g_system_test.test_names[test_index++], 64, "Eye_Animation_Blink_Test");
    snprintf(g_system_test.test_names[test_index++], 64, "Eye_Animation_Movement_Test");
    snprintf(g_system_test.test_names[test_index++], 64, "Mouth_Animation_Test");
    
    // System Integration Tests
    snprintf(g_system_test.test_names[test_index++], 64, "SPI_Communication_Test");
    
    // Update test count and mark as added
    g_system_test.test_count = test_index;
    g_system_test.tests_added = true;
    
    ESP_LOGI(TAG, "✅ TestPicker added %d tests to SystemTest:", g_system_test.test_count);
    for (int i = 0; i < g_system_test.test_count; i++) {
        ESP_LOGI(TAG, "  [%d] %s", i + 1, g_system_test.test_names[i]);
    }
    
    return ESP_OK;
}

/**
 * Component: TestPicker
 * Function: TestPicker_act
 * Purpose: Monitor test execution status and provide test management
 */
esp_err_t TestPicker_act(void) {
    // TestPicker is primarily an initialization component
    // Act function can be used for test status monitoring or execution
    
    static uint32_t status_counter = 0;
    status_counter++;
    
    // Log status every 60 seconds (assuming 1Hz execution)
    if (status_counter % 60 == 0) {
        ESP_LOGI(TAG, "📊 TestPicker Status: %d tests available in SystemTest", 
                 g_system_test.test_count);
    }
    
    return ESP_OK;
}

/**
 * Get test count from SystemTest
 */
int TestPicker_get_test_count(void) {
    return g_system_test.test_count;
}

/**
 * Get specific test name by index
 */
const char* TestPicker_get_test_name(int index) {
    if (index >= 0 && index < g_system_test.test_count) {
        return g_system_test.test_names[index];
    }
    return NULL;
}

/**
 * Check if tests are properly loaded
 */
bool TestPicker_tests_ready(void) {
    return g_system_test.tests_added && g_system_test.test_count > 0;
}