// src/components/goblin_speaker_STUB.c
// Temporary stub implementation for testing without physical speaker hardware
// Replace with real implementation when I2S amplifier and speakers arrive

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "Mood.hpp"
#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>

#ifdef ENABLE_GOBLIN_COMPONENTS

static const char *TAG = "goblin_speaker_STUB";
static bool s_initialized = false;

// Define missing behavior constants for compilation
#define BEHAVIOR_GREETING 1
#define BEHAVIOR_IDLE 0

/**
 * @brief Initialize speaker stub (hardware not available)
 * 
 * This stub allows testing of the complete component system without
 * physical speaker hardware. When the I2S amplifier and speakers arrive:
 * 1. Replace this file with real goblin_speaker.c implementation
 * 2. Add I2S driver initialization
 * 3. Configure I2S pins per interface JSON
 * 4. Test audio output
 */
void goblin_speaker_init(void) {
    ESP_LOGW(TAG, "╔════════════════════════════════════════════════════════╗");
    ESP_LOGW(TAG, "║  SPEAKER STUB ACTIVE - Hardware not available         ║");
    ESP_LOGW(TAG, "║  Audio output will be logged to serial console only   ║");
    ESP_LOGW(TAG, "║  Visual feedback via eye displays will substitute     ║");
    ESP_LOGW(TAG, "╚════════════════════════════════════════════════════════╝");
    
    s_initialized = true;
}

/**
 * @brief Speaker stub act function (runs every 700ms per hitCount: 7)
 * 
 * Instead of playing audio:
 * - Logs what WOULD be played to serial console
 * - Provides visual feedback via eye brightness/color
 * - Simulates audio timing for animation synchronization
 */
void goblin_speaker_act(void) {
    if (!s_initialized) {
        return;
    }
    
    // Check if system wants to play audio
    if (g_shared_state.is_speaking) {
        // Log audio event (substitute for actual playback)
        ESP_LOGI(TAG, "🔊 AUDIO EVENT [%.3f sec]: Speaking (volume: simulated)", 
            esp_timer_get_time() / 1000000.0);
        
        // Simulate audio level for visual feedback
        uint8_t simulated_audio_level = 180;  // Mock "loud speaking" level
        
        // Provide visual feedback via mood system
        // Eyes will flash/brighten to indicate audio activity
        // Note: Mood is managed within SharedMemory (GSM), not as global
        // GSM.getCurrentMood().addExcitement(15);  // Would increase excitement during speech
        
        // Log simulated audio characteristics
        ESP_LOGD(TAG, "  Audio level: %d/255 (simulated)", simulated_audio_level);
        ESP_LOGD(TAG, "  Mood excitement would be increased for visual feedback");
        
    } else {
        // Idle state - log periodically
        if (g_loopCount % 100 == 0) {  // Every 10 seconds
            ESP_LOGD(TAG, "⏸️  Audio idle (no playback requested)");
        }
    }
    
    // Simulate audio-driven animations
    // When real hardware arrives, this will sync with actual audio amplitude
    // Note: Behavior state should be accessed through Environment class in SharedMemory
    // if (GSM.getEnvironment().current_behavior == BEHAVIOR_GREETING) {
    if (true) {  // Simplified for stub - always simulate greeting behavior
        ESP_LOGD(TAG, "  Would play: greeting_chirp.wav");
        ESP_LOGD(TAG, "  Would sync: eye animations with audio peaks");
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
