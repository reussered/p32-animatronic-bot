// P32 Component: goblin_right_eye
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

static const char *TAG = "GOBLIN_RIGHT_EYE";
static eye_display_t right_eye_display;

// Component: Right eye display animation
esp_err_t goblin_right_eye_init(void) {
    esp_err_t ret;
    
#ifdef SIMPLE_TEST
    printf("INIT: goblin_right_eye - Right eye display animation\n");
    
    // Initialize right eye display  
    ret = eye_display_init(&right_eye_display, "RIGHT EYE");
    if (ret == ESP_OK) {
        // Start with curious look (different from left eye)
        eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
        printf("       Right eye display initialized with curious look\n");
    }
    return ret;
#endif

    // Full initialization for real hardware
    ret = eye_display_init(&right_eye_display, "RIGHT EYE");
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Right eye display initialized");
        eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
    }
    return ret;
}

// Component action function - executes every 50 loops
void goblin_right_eye_act(void) {
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000); // Convert to ms
    
#ifdef SIMPLE_TEST
    // Update and render the eye display animation
    eye_display_update(&right_eye_display, current_time);
    
    // TODO: Integrate new C++ Mood and FrameProcessor system here
    // The old frame cache system has been replaced with efficient mood-based transitions
    
    // Send animation data to PC display server (non-blocking)
    web_client_send_animation_data_for_component("RIGHT_EYE", &right_eye_display);
    
    // Render display every few loops, offset from left eye
    if ((loopCount + 10) % 20 == 0) { // Offset by 10 loops from left eye
        printf("\n=== RIGHT EYE (Loop %lu) ===\n", loopCount);
        eye_display_render(&right_eye_display);
        
        // Start new animations when current finishes
        if (!right_eye_display.active) {
            // Different cycle pattern from left eye
            static int anim_cycle = 1; // Start offset
            switch (anim_cycle % 3) {
                case 0:
                    printf("Starting CURIOUS LOOK animation...\n");
                    eye_display_start_animation(&right_eye_display, &goblin_curious_look_animation);
                    break;
                case 1:
                    printf("Starting BLINK animation...\n");
                    eye_display_start_animation(&right_eye_display, &goblin_blink_animation);
                    break;
                case 2:
                    printf("Starting ANGRY STARE animation...\n");
                    eye_display_start_animation(&right_eye_display, &goblin_angry_stare_animation);
                    break;
            }
            anim_cycle++;
        }
    }
    return;
#endif

    // Full hardware version
    eye_display_update(&right_eye_display, current_time);
    
    // Render to actual SPI display (TODO: implement SPI driver calls)
    if ((loopCount + 2) % 5 == 0) { // Slightly offset from left eye
        ESP_LOGD(TAG, "Updating SPI display - openness: %.2f", 
                 right_eye_display.current_frame.eye_openness);
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
