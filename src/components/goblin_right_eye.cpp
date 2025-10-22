// P32 Component: goblin_right_eye
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

static const char *TAG = "GOBLIN_RIGHT_EYE";

// Private static animation buffer for right eye
static uint8_t right_eye_animation_buffer[57600];  // 240x240 pixels
static uint32_t right_eye_current_frame = 0;
static uint32_t right_eye_frame_count = 6; // Curious look animation frames

// External GSM instance
extern SharedMemory GSM;

// Function declarations
void load_right_eye_animation(void);
void load_current_frame_to_buffer(void);

// Component: Right eye display animation with integrated pixel processing
esp_err_t goblin_right_eye_init(void) {
    ESP_LOGI(TAG, "Initializing right eye at position [26.67, 17.78, -8.89] mm");
    
    // Shared goblin eye resources already initialized by left eye
    // Load curious look animation frames into buffer
    load_right_eye_animation();
    
    ESP_LOGI(TAG, "Right eye initialized with %u animation frames", right_eye_frame_count);
    return ESP_OK;
}

// Component action function - executes every 50 loops (slower than left eye)
void goblin_right_eye_act(void) {
    // Advance animation frame based on loop count (slower pace than left eye)
    uint32_t animation_speed = 45; // Change frame every 45 loops (slower)
    uint32_t new_frame = (g_loopCount / animation_speed) % right_eye_frame_count;
    
    if (new_frame != right_eye_current_frame) {
        right_eye_current_frame = new_frame;
        load_current_frame_to_buffer();
    }
    
    // Set context for shared processing (SPI device for right eye)
    current_spi_device = 2; // SPI_DEVICE_2 for right eye
    currentFrame = right_eye_animation_buffer;
    current_frame_size = PIXELS_PER_FRAME;
    
    ESP_LOGV(TAG, "Right eye frame %u ready for processing at loop %u", 
             right_eye_current_frame, g_loopCount);
}

/**
 * @brief Load curious look animation frames for right eye
 */
void load_right_eye_animation(void) {
    ESP_LOGI(TAG, "Loading right eye curious look animation");
    
    // Initialize with curious look pattern
    // Frame 0: Normal gaze
    for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
        right_eye_animation_buffer[i] = 255; // Start with white
    }
    
    right_eye_current_frame = 0;
    ESP_LOGI(TAG, "Right eye animation loaded");
}

/**
 * @brief Load current animation frame to processing buffer for right eye
 */
void load_current_frame_to_buffer(void) {
    // Curious look animation pattern (more complex than left eye blink)
    switch (right_eye_current_frame) {
        case 0: // Normal open eye
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t x = i % FRAME_WIDTH;
                uint32_t y = i / FRAME_WIDTH;
                uint32_t center_x = FRAME_WIDTH / 2;
                uint32_t center_y = FRAME_HEIGHT / 2;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < 1600) { // Inner iris
                    right_eye_animation_buffer[i] = 96; // Red-brown iris
                } else if (distance < 4900) { // Outer iris
                    right_eye_animation_buffer[i] = 64; // Brown iris
                } else if (distance < 10000) { // Sclera
                    right_eye_animation_buffer[i] = 255; // White
                } else {
                    right_eye_animation_buffer[i] = 0; // Black edge
                }
            }
            break;
            
        case 1: // Slight squint
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t y = i / FRAME_WIDTH;
                if (y > 50 && y < 190) {
                    uint32_t x = i % FRAME_WIDTH;
                    uint32_t center_x = FRAME_WIDTH / 2;
                    uint32_t center_y = FRAME_HEIGHT / 2;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    if (distance < 1600) {
                        right_eye_animation_buffer[i] = 128; // Green-curious
                    } else if (distance < 3600) {
                        right_eye_animation_buffer[i] = 96; // Red-brown
                    } else {
                        right_eye_animation_buffer[i] = 224; // Light gray
                    }
                } else {
                    right_eye_animation_buffer[i] = 0; // Black eyelid
                }
            }
            break;
            
        case 2: // Wide curious look
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t x = i % FRAME_WIDTH;
                uint32_t y = i / FRAME_WIDTH;
                uint32_t center_x = FRAME_WIDTH / 2;
                uint32_t center_y = FRAME_HEIGHT / 2;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < 900) { // Small pupil
                    right_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < 2500) { // Bright iris
                    right_eye_animation_buffer[i] = 160; // Blue-curious
                } else if (distance < 6400) { // Outer iris
                    right_eye_animation_buffer[i] = 128; // Green
                } else if (distance < 12100) { // Wide sclera
                    right_eye_animation_buffer[i] = 255; // Bright white
                } else {
                    right_eye_animation_buffer[i] = 32; // Dark edge
                }
            }
            break;
            
        case 3: // Narrowed focus
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t y = i / FRAME_WIDTH;
                if (y > 70 && y < 170) {
                    uint32_t x = i % FRAME_WIDTH;
                    uint32_t center_x = FRAME_WIDTH / 2;
                    uint32_t center_y = FRAME_HEIGHT / 2;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    if (distance < 2500) {
                        right_eye_animation_buffer[i] = 96; // Red focus
                    } else {
                        right_eye_animation_buffer[i] = 192; // Purple intensity
                    }
                } else {
                    right_eye_animation_buffer[i] = 16; // Dark eyelid
                }
            }
            break;
            
        case 4: // Return to normal
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t x = i % FRAME_WIDTH;
                uint32_t y = i / FRAME_WIDTH;
                uint32_t center_x = FRAME_WIDTH / 2;
                uint32_t center_y = FRAME_HEIGHT / 2;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < 1600) {
                    right_eye_animation_buffer[i] = 64; // Brown iris
                } else if (distance < 4900) {
                    right_eye_animation_buffer[i] = 224; // Light
                } else if (distance < 10000) {
                    right_eye_animation_buffer[i] = 255; // White
                } else {
                    right_eye_animation_buffer[i] = 0; // Black
                }
            }
            break;
            
        case 5: // Satisfied look
            for (uint32_t i = 0; i < PIXELS_PER_FRAME; i++) {
                uint32_t y = i / FRAME_WIDTH;
                if (y > 40 && y < 200) {
                    uint32_t x = i % FRAME_WIDTH;
                    uint32_t center_x = FRAME_WIDTH / 2;
                    uint32_t center_y = FRAME_HEIGHT / 2;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    if (distance < 3600) {
                        right_eye_animation_buffer[i] = 192; // Purple satisfaction
                    } else {
                        right_eye_animation_buffer[i] = 255; // Bright white
                    }
                } else {
                    right_eye_animation_buffer[i] = 0; // Black
                }
            }
            break;
    }
    
    ESP_LOGV(TAG, "Loaded frame %u to right eye buffer", right_eye_current_frame);
}

#endif // ENABLE_GOBLIN_COMPONENTS
