// P32 Component: system_core
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "SYSTEM_CORE";

// Component: Core system initialization
esp_err_t system_core_init(void) {
#ifdef SIMPLE_TEST
    printf("INIT: system_core - Core system initialization\n");
    return ESP_OK;
#endif

    // System component initialization
    ESP_LOGI(TAG, "System component initialized");
    return ESP_OK;
}

// Component action function - executes every 1000 loops
void system_core_act(uint32_t loopCount) {
#ifdef SIMPLE_TEST
    if (loopCount % 1000 == 0) {
        printf("ACT: system_core - active (loop %lu)\n", loopCount);
    }
    return;
#endif

    // System maintenance tasks
    if (loopCount % 5000 == 0) {
        ESP_LOGD(TAG, "System maintenance - loop %lu", loopCount);
    }
}