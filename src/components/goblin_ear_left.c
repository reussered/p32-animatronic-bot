// P32 Component: goblin_ear_left
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "esp_log.h"
#include "esp_err.h"
#include "p32_shared_state.h"

static const char *TAG = "GOBLIN_EAR_LEFT";

// Component: goblin_ear_left component
esp_err_t goblin_ear_left_init(void) {
#ifdef SIMPLE_TEST
    printf("INIT: goblin_ear_left - goblin_ear_left component\n");
    return ESP_OK;
#endif

    // Generic component initialization
    ESP_LOGI(TAG, "Component initialized");
    return ESP_OK;
}

// Component action function - executes every 100 loops
// NO ARGUMENTS - accesses g_loopCount from p32_shared_state.h
void goblin_ear_left_act(void) {
#ifdef SIMPLE_TEST
    if (g_loopCount % 1000 == 0) {
        printf("ACT: goblin_ear_left - active (loop %llu)\n", g_loopCount);
    }
    return;
#endif

    // Generic component action
    ESP_LOGD(TAG, "Component action - loop %llu", loopCount);
}