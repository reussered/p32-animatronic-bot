#include "core/SharedMemory.hpp"

// Global instance for system-wide shared memory
SharedMemory g_GlobalSharedMemory;

extern "C" {
#include "esp_err.h"
}

// System-level component init function
esp_err_t GlobalSharedMemory_init(void) {
    g_GlobalSharedMemory.espnow_init();
    return ESP_OK;
}

// System-level component act function (stub)
esp_err_t GlobalSharedMemory_act(void) {
    // No operation
    return ESP_OK;
}
