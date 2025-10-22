// P32 Component: goblin_left_eye
// Auto-generated individual component file
// Memory footprint can be measured independently

#include "p32_component_config.h"
#include "p32_shared_state.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "p32_eye_display.h"
#include "p32_web_client.h"
// Note: Using FrameProcessor.hpp for direct RGB565 pixel manipulation

static const char *TAG = "GOBLIN_LEFT_EYE";
static eye_display_t left_eye_display;
static bool web_client_initialized = false;

// Component: Left eye display animation
esp_err_t goblin_left_eye_init(void) {
    esp_err_t ret;
    
#ifdef SIMPLE_TEST
    printf("INIT: goblin_left_eye - Left eye display animation\n");
    
    // Initialize left eye display
    ret = eye_display_init(&left_eye_display, "LEFT EYE");
    if (ret == ESP_OK) {
        // Start with a blink animation
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
        printf("       Left eye display initialized with blink animation\n");
        
        // Initialize web client streaming on first eye init
        if (!web_client_initialized) {
            web_client_init();
            web_client_initialized = true;
            printf("       Web client streaming to PC enabled (HTTP JSON)\n");
        }
    }
    return ret;
#endif

    // Full initialization for real hardware
    ret = eye_display_init(&left_eye_display, "LEFT EYE");
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Left eye display initialized");
        eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
    }
    return ret;
}

// Component action function - executes every 5 loops
void goblin_left_eye_act(void) {
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000); // Convert to ms
    
#ifdef SIMPLE_TEST
    // Update and render the eye display animation
    eye_display_update(&left_eye_display, current_time);
    
    // TODO: Integrate new C++ Mood and FrameProcessor system here
    // The old frame cache system has been replaced with efficient mood-based transitions
    
    // Send animation data to PC display server (non-blocking)
    web_client_send_animation_data_for_component("LEFT_EYE", &left_eye_display);
    
    // Render local display every few loops to avoid spam
    if (loopCount % 50 == 0) { // Every 5 seconds at 100ms loop delay  
        printf("\n=== LEFT EYE (Loop %lu) ===\n", loopCount);
        eye_display_render(&left_eye_display);
        
        // Start new animations periodically for demo
        if (!left_eye_display.active) {
            // Cycle through animations
            static int anim_cycle = 0;
            switch (anim_cycle % 3) {
                case 0:
                    printf("Starting BLINK animation...\n");
                    eye_display_start_animation(&left_eye_display, &goblin_blink_animation);
                    break;
                case 1:
                    printf("Starting ANGRY STARE animation...\n");
                    eye_display_start_animation(&left_eye_display, &goblin_angry_stare_animation);
                    break;
                case 2:
                    printf("Starting CURIOUS LOOK animation...\n");
                    eye_display_start_animation(&left_eye_display, &goblin_curious_look_animation);
                    break;
            }
            anim_cycle++;
        }
    }
    return;
#endif

    // Full hardware version
    eye_display_update(&left_eye_display, current_time);
    
    // Render to actual SPI display (TODO: implement SPI driver calls)
    if (loopCount % 5 == 0) { // Update display at ~20 FPS (every 500ms / 5 loops)
        ESP_LOGD(TAG, "Updating SPI display - openness: %.2f", 
                 left_eye_display.current_frame.eye_openness);
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
