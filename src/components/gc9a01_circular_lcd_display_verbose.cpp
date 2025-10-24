/**
 * @file GC9A01 Circular LCD Display.cpp
 * @brief GC9A01 Circular LCD Display component implementation
 * @author Auto-generated from JSON specification
 */

#include "gc9a01.hpp"
#include "include/core/memory/SharedMemory.hpp"
#include "p32_shared_state.h"
#include "esp_log.h"

static const char* TAG = "GC9A01_VERBOSE";

// External GSM instance
extern SharedMemory GSM;

/**
 * @brief Initialize GC9A01 Circular LCD Display component
 * Called once during system startup
 */
/**
 * @brief Initialize GC9A01 Circular LCD Display component
 * Called once during system startup
 */
void GC9A01_Circular_LCD_Display_init(void) {
    ESP_LOGI(TAG, "Initializing GC9A01 Circular LCD Display component");
    
    // Initialize the underlying GC9A01 hardware driver
    gc9a01_init();
    
    ESP_LOGI(TAG, "GC9A01 Circular LCD Display component initialization complete");
}

/**
 * @brief Execute GC9A01 Circular LCD Display component logic  
 * Called every loop iteration based on hitCount: 1
 */
/**
 * @brief Execute GC9A01 Circular LCD Display component logic  
 * Called every loop iteration based on hitCount: 1
 */
void GC9A01_Circular_LCD_Display_act(void) {
    // Delegate to the underlying GC9A01 hardware driver
    gc9a01_act();
    
    // Log verbose information every 1000 loops
    if (g_loopCount % 1000 == 0) {
        ESP_LOGV(TAG, "GC9A01 display processing - loop %lu", g_loopCount);
    }
}
