#include "gc9a01.hpp"
#include "esp_log.h"

static const char *TAG = "DISPLAY_INTERFACE_TEST";

/**
 * Simple test function to verify the display interface works
 */
extern "C" void test_display_interface() {
    ESP_LOGI(TAG, "Testing display buffer interface...");
    
    // Test getting buffer dimensions
    uint32_t frame_size = gc9a01_getFrameSize();
    uint32_t row_size = gc9a01_getFrameRowSize();
    
    ESP_LOGI(TAG, "Frame size: %lu pixels", frame_size);
    ESP_LOGI(TAG, "Row size: %lu pixels", row_size);
    
    // Test buffer allocation
    uint8_t* buffer = gc9a01_getBuffer();
    if (buffer != nullptr) {
        ESP_LOGI(TAG, "Buffer allocated successfully at %p", buffer);
        
        // Fill buffer with test pattern
        for (uint32_t i = 0; i < frame_size; i++) {
            buffer[i] = (uint8_t)(i % 256);
        }
        
        ESP_LOGI(TAG, "Test pattern written to buffer");
        
        // Free the buffer
        free(buffer);
        ESP_LOGI(TAG, "Buffer freed successfully");
    } else {
        ESP_LOGE(TAG, "Failed to allocate buffer");
    }
    
    ESP_LOGI(TAG, "Display interface test completed");
}