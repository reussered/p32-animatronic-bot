// P32 Component: goblin_left_eye
// Auto-generated individual component file with integrated pixel processing
// Memory footprint can be measured independently

#include "p32_component_config.h"
#include "p32_shared_state.h"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "components/goblin_eye.hpp"
#include "core/memory/SharedMemory.hpp"

// Display constants for GC9A01 240x240 circular display
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 240
#define PIXELS_PER_FRAME (240 * 240)  // 57600 pixels
#define BYTES_PER_PIXEL 2  // RGB565 format

static const char *TAG = "GOBLIN_LEFT_EYE";

// Private static animation buffer for left eye
static uint8_t left_eye_animation_buffer[57600];  // 240x240 pixels
static uint32_t left_eye_current_frame = 0;
static uint32_t left_eye_frame_count = 4; // Blink animation frames

// External GSM instance
extern SharedMemory GSM;

// Function declarations
void load_left_eye_animation(void);
void load_current_frame_to_buffer(void);

// Component: Left eye display animation with integrated pixel processing
esp_err_t goblin_left_eye_init(void) {
    ESP_LOGI(TAG, "Initializing left eye at position [-26.67, 17.78, -8.89] mm");
    
    // Initialize shared goblin eye resources (palette, etc.)
    goblin_eye_init();
    
    // Load simple blink animation frames into buffer
    load_left_eye_animation();
    
    ESP_LOGI(TAG, "Left eye initialized with %u animation frames", left_eye_frame_count);
    return ESP_OK;
}

// Component action function - executes every 5 loops
void goblin_left_eye_act(void) {
    // Advance animation frame based on loop count
    uint32_t animation_speed = 30; // Change frame every 30 loops
    uint32_t new_frame = (g_loopCount / animation_speed) % left_eye_frame_count;
    
    if (new_frame != left_eye_current_frame) {
        left_eye_current_frame = new_frame;
        load_current_frame_to_buffer();
    }
    
    // Set context for shared processing (SPI device for left eye)
    current_spi_device = 1; // SPI_DEVICE_1 for left eye
    currentFrame = left_eye_animation_buffer;
    current_frame_size = PIXELS_PER_FRAME;
    
    ESP_LOGV(TAG, "Left eye frame %u ready for processing at loop %u", 
             left_eye_current_frame, g_loopCount);
}

/**
 * @brief Load blink animation frames for left eye
 */
void load_left_eye_animation(void) {
    ESP_LOGI(TAG, "Loading left eye blink animation");
    
    // Initialize with simple blink pattern
    // Frame 0: Fully open eye
    for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
        left_eye_animation_buffer[i] = 255; // Start with white
    }
    
    left_eye_current_frame = 0;
    ESP_LOGI(TAG, "Left eye animation loaded");
}

/**
 * @brief Load current animation frame to processing buffer
 */
void load_current_frame_to_buffer(void) {
    // Simple blink animation pattern
    switch (left_eye_current_frame) {
        case 0: // Fully open
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t y = i / FRAME_WIDTH;
                if (y > 60 && y < 180) {
                    left_eye_animation_buffer[i] = 64; // Brown iris
                } else {
                    left_eye_animation_buffer[i] = 255; // White sclera
                }
            }
            break;
            
        case 1: // Partially closed
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t y = i / FRAME_WIDTH;
                if (y > 80 && y < 160) {
                    left_eye_animation_buffer[i] = 64; // Brown iris
                } else if (y > 70 && y < 170) {
                    left_eye_animation_buffer[i] = 224; // Light gray
                } else {
                    left_eye_animation_buffer[i] = 0; // Black eyelid
                }
            }
            break;
            
        case 2: // Nearly closed
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t y = i / FRAME_WIDTH;
                if (y > 110 && y < 130) {
                    left_eye_animation_buffer[i] = 32; // Dark slit
                } else {
                    left_eye_animation_buffer[i] = 0; // Black eyelid
                }
            }
            break;
            
        case 3: // Fully closed
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                left_eye_animation_buffer[i] = 0; // All black
            }
            break;
    }
    
    ESP_LOGV(TAG, "Loaded frame %u to left eye buffer", left_eye_current_frame);
}

#endif // ENABLE_GOBLIN_COMPONENTS
