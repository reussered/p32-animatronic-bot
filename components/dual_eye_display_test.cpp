// ============================================================================
// Goblin Head Dual Eye Display Test Harness
// Tests initialization and basic functionality of dual GC9A01 displays
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "p32_component_registry.hpp"

static const char *TAG = "DUAL_EYE_TEST";

// Test pattern colors for validation
#define TEST_COLOR_RED     0xF800
#define TEST_COLOR_GREEN   0x07E0
#define TEST_COLOR_BLUE    0x001F
#define TEST_COLOR_WHITE   0xFFFF
#define TEST_COLOR_BLACK   0x0000

// ============================================================================
// Test Functions
// ============================================================================

esp_err_t test_display_initialization(void) {
    ESP_LOGI(TAG, "Testing display component initialization...");

    // Initialize all components in the generated order
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (initTable[i] != NULL) {
            esp_err_t err = initTable[i]();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Component %d initialization failed: %s", i, esp_err_to_name(err));
                return err;
            }
            ESP_LOGI(TAG, "Component %d initialized successfully", i);
        }
    }

    ESP_LOGI(TAG, "All display components initialized successfully!");
    return ESP_OK;
}

void test_display_actions(void) {
    ESP_LOGI(TAG, "Testing display component actions...");

    // Execute action functions with timing control
    static uint32_t loop_counter = 0;
    loop_counter++;

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (actTable[i] != NULL && (loop_counter % hitCountTable[i]) == 0) {
            actTable[i]();
            ESP_LOGD(TAG, "Executed action for component %d", i);
        }
    }
}

// ============================================================================
// Main Test Task
// ============================================================================

void dual_eye_test_task(void *pvParameters) {
    ESP_LOGI(TAG, "Starting Goblin Head Dual Eye Display Test");

    // Step 1: Test component initialization
    esp_err_t init_result = test_display_initialization();
    if (init_result != ESP_OK) {
        ESP_LOGE(TAG, "Display initialization test FAILED");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Display initialization test PASSED");

    // Step 2: Test component actions in loop
    ESP_LOGI(TAG, "Starting continuous display action testing...");
    ESP_LOGI(TAG, "Monitor serial output for component execution");

    while (1) {
        test_display_actions();
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms delay between test cycles
    }
}

// ============================================================================
// Public Interface
// ============================================================================

esp_err_t dual_eye_display_test_start(void) {
    ESP_LOGI(TAG, "Creating dual eye display test task...");

    xTaskCreate(
        dual_eye_test_task,
        "dual_eye_test",
        4096,
        NULL,
        5,
        NULL
    );

    return ESP_OK;
}