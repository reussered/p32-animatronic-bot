// Goblin Mouth Display Component - STUB IMPLEMENTATION
// Hardware not yet available - placeholder for future integration
//
// This component will be replaced with full implementation when:
// 1. GC9A01 mouth display hardware arrives
// 2. SPI_DEVICE_3 pins are assigned
// 3. Mouth animation assets are ready
//
// Future: May become autonomous ESP32 subsystem node (see MOUTH-SUBSYSTEM-ARCHITECTURE.md)

#include "p32_component_config.h"
#include "p32_shared_state.h"  // Access to g_loopCount and all globals
#include <esp_log.h>

#ifdef ENABLE_GOBLIN_COMPONENTS

static const char *TAG = "goblin_mouth";

// Stub initialization - does nothing but log
esp_err_t goblin_mouth_init(void) {
    // NO ARGUMENTS - access globals directly
    ESP_LOGI(TAG, "Goblin mouth component initializing (STUB - hardware not available)");
    ESP_LOGI(TAG, "Position: [0, -1.05 INCH, 0] (below nose center)");
    ESP_LOGI(TAG, "Interface: SPI_DEVICE_3 (not yet wired)");
    ESP_LOGI(TAG, "Future: May become autonomous ESP32 mesh subsystem node");
    
    return ESP_OK;
}

// Stub act function - does nothing (placeholder for future mouth animations)
void goblin_mouth_act(void) {
    // NO ARGUMENTS - access g_loopCount from global shared state
    // hitCount: 3 → executes every 300ms (for smooth mouth animation when implemented)
    
    // Currently does nothing - waiting for hardware
    // Future implementation will:
    // - Read mood state from g_shared_state.mood_serialized[]
    // - Render mouth expressions (snarl, grin, laugh, etc.)
    // - Synchronize with speech audio
    // - Update GC9A01 display via SPI_DEVICE_3
    
    ESP_LOGV(TAG, "Mouth act (STUB) at loop %u - hardware pending", g_loopCount);
}

#endif // ENABLE_GOBLIN_COMPONENTS
