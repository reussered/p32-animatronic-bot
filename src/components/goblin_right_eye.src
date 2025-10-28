// P32 Component: goblin_right_eye
// Auto-generated individual component file with integrated pixel processing
// Memory footprint can be measured independently

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "components/gc9a01.hpp"
#include "components/generic_spi_display.hpp"
#include "shared/Environment.hpp"
#include "core/memory/SharedMemory.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "components/goblin_eye.hpp"
#include "core/memory/SharedMemory.hpp"

static const char *goblin_right_eye_TAG = "GOBLIN_RIGHT_EYE";

// Private static animation buffer for right eye
static uint8_t* right_eye_animation_buffer = nullptr;
static uint32_t right_eye_current_frame = 0;
static uint32_t right_eye_frame_count = 6; // Curious look animation frames
static uint32_t right_eye_frame_width = 0;
static uint32_t right_eye_frame_height = 0;
static uint32_t right_eye_total_pixels = 0;

// Function declarations
void load_right_eye_animation(void);
void load_current_frame_to_buffer_right_eye(void);

// Component: Right eye display animation with integrated pixel processing
esp_err_t goblin_right_eye_init(void) {
    ESP_LOGI(goblin_right_eye_TAG, "Initializing right eye at position [26.67, 17.78, -8.89] mm");

    // Get buffer and dimensions from display component
    right_eye_animation_buffer = generic_display_get_buffer();
    right_eye_total_pixels = generic_display_get_frame_size();
    right_eye_frame_width = generic_display_get_frame_width();
    right_eye_frame_height = right_eye_total_pixels / right_eye_frame_width;
    
    if (right_eye_animation_buffer == nullptr) {
        ESP_LOGE(goblin_right_eye_TAG, "Failed to allocate animation buffer");
        return ESP_FAIL;
    }
    
    // Shared goblin eye resources already initialized by left eye
    // Load curious look animation frames into buffer
    load_right_eye_animation();
    
    ESP_LOGI(goblin_right_eye_TAG, "Right eye initialized with %u animation frames on %ux%u display", 
             right_eye_frame_count, right_eye_frame_width, right_eye_frame_height);
    return ESP_OK;
}

// Component action function - executes every 50 loops (slower than left eye)
void goblin_right_eye_act(void) {
    // Advance animation frame based on loop count (slower pace than left eye)
    uint32_t animation_speed = 45; // Change frame every 45 loops (slower)
    uint32_t new_frame = (g_loopCount / animation_speed) % right_eye_frame_count;
    
    if (new_frame != right_eye_current_frame) {
        right_eye_current_frame = new_frame;
        load_current_frame_to_buffer_right_eye();
    }
    
    // Set context for shared processing (SPI device for right eye)
    current_spi_device = 2; // SPI_DEVICE_2 for right eye
    currentFrame = right_eye_animation_buffer;
    current_frame_size = right_eye_total_pixels;
    
    ESP_LOGV(goblin_right_eye_TAG, "Right eye frame %u ready for processing at loop %u", 
             right_eye_current_frame, g_loopCount);
}

/**
 * @brief Load curious look animation frames for right eye
 */
void load_right_eye_animation(void) {
    ESP_LOGI(goblin_right_eye_TAG, "Loading right eye curious look animation");
    
    // Initialize with curious look pattern
    // Frame 0: Normal gaze
    for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
        right_eye_animation_buffer[i] = 255; // Start with white
    }
    
    right_eye_current_frame = 0;
    ESP_LOGI(goblin_right_eye_TAG, "Right eye animation loaded");
}

/**
 * @brief Load current animation frame to processing buffer for right eye
 * Now includes distance-based focusing effect while preserving mood sensitivity
 */
void load_current_frame_to_buffer_right_eye(void) {
    // Get viewer distance from Environment in shared state
    Environment* env_ptr = GSM.read<Environment>();
    uint8_t viewer_distance = (env_ptr != nullptr) ? env_ptr->distance_cm : 128; // Default if not available
    
    // Calculate focus-dependent pupil and iris sizes
    uint32_t pupil_size = 400 + (viewer_distance * 4);        // 400-1420: larger when viewer close
    uint32_t inner_iris_size = 1200 + (viewer_distance * 8);  // 1200-3240: detailed when close
    uint32_t outer_iris_size = 3600 + (viewer_distance * 12); // 3600-6660: sharp boundaries when far
    uint32_t sclera_size = 10000;                             // Constant sclera boundary
    
    // Calculate centers based on actual display dimensions
    uint32_t center_x = right_eye_frame_width / 2;
    uint32_t center_y = right_eye_frame_height / 2;
    
    // Curious look animation pattern with focus adaptation
    switch (right_eye_current_frame) {
        case 0: // Normal open eye with adaptive focus
            for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
                uint32_t x = i % right_eye_frame_width;
                uint32_t y = i / right_eye_frame_width;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < pupil_size) { // Adaptive pupil
                    right_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < inner_iris_size) { // Inner iris - detailed when close
                    right_eye_animation_buffer[i] = 96; // Red-brown iris (mood-sensitive palette)
                } else if (distance < outer_iris_size) { // Outer iris - sharp when far
                    right_eye_animation_buffer[i] = 64; // Brown iris (mood-sensitive palette)
                } else if (distance < sclera_size) { // Sclera
                    right_eye_animation_buffer[i] = 255; // White (mood-sensitive palette)
                } else {
                    right_eye_animation_buffer[i] = 0; // Black edge
                }
            }
            break;
            
        case 1: // Slight squint with focus adaptation
            for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
                uint32_t y = i / right_eye_frame_width;
                uint32_t squint_top = right_eye_frame_height * 50 / 240;    // Scale squint boundaries
                uint32_t squint_bottom = right_eye_frame_height * 190 / 240;
                if (y > squint_top && y < squint_bottom) {
                    uint32_t x = i % right_eye_frame_width;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    if (distance < pupil_size) { // Adaptive pupil
                        right_eye_animation_buffer[i] = 0; // Black pupil
                    } else if (distance < inner_iris_size) { // Focus-adaptive iris
                        right_eye_animation_buffer[i] = 128; // Green-curious (mood-sensitive)
                    } else if (distance < outer_iris_size) { // Outer iris
                        right_eye_animation_buffer[i] = 96; // Red-brown (mood-sensitive)
                    } else {
                        right_eye_animation_buffer[i] = 224; // Light gray sclera
                    }
                } else {
                    right_eye_animation_buffer[i] = 0; // Black eyelid
                }
            }
            break;
            
        case 2: // Wide curious look with focus adaptation
            for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
                uint32_t x = i % right_eye_frame_width;
                uint32_t y = i / right_eye_frame_width;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                // Alert pupil size - smaller when surprised/focused
                uint32_t alert_pupil_size = pupil_size * 0.6; 
                
                if (distance < alert_pupil_size) { // Small adaptive pupil
                    right_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < inner_iris_size * 0.8) { // Bright inner iris
                    right_eye_animation_buffer[i] = 160; // Blue-curious (mood-sensitive)
                } else if (distance < outer_iris_size) { // Outer iris
                    right_eye_animation_buffer[i] = 128; // Green (mood-sensitive)
                } else if (distance < sclera_size * 1.2) { // Wide sclera for alert look
                    right_eye_animation_buffer[i] = 255; // Bright white
                } else {
                    right_eye_animation_buffer[i] = 32; // Dark edge
                }
            }
            break;
            
        case 3: // Narrowed focus with distance adaptation
            for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
                uint32_t y = i / right_eye_frame_width;
                if (y > right_eye_frame_height * 70 / 240 && y < right_eye_frame_height * 170 / 240) {
                    uint32_t x = i % right_eye_frame_width;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    // Concentrated focus - pupil size varies with distance
                    uint32_t focus_pupil_size = pupil_size * 0.8;
                    
                    if (distance < focus_pupil_size) {
                        right_eye_animation_buffer[i] = 0; // Black pupil
                    } else if (distance < inner_iris_size) {
                        right_eye_animation_buffer[i] = 96; // Red focus (mood-sensitive)
                    } else {
                        right_eye_animation_buffer[i] = 192; // Purple intensity (mood-sensitive)
                    }
                } else {
                    right_eye_animation_buffer[i] = 16; // Dark eyelid
                }
            }
            break;
            
        case 4: // Return to normal with focus adaptation
            for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
                uint32_t x = i % right_eye_frame_width;
                uint32_t y = i / right_eye_frame_width;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < pupil_size) { // Adaptive pupil
                    right_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < inner_iris_size) { // Inner iris
                    right_eye_animation_buffer[i] = 64; // Brown iris (mood-sensitive)
                } else if (distance < outer_iris_size) { // Outer iris
                    right_eye_animation_buffer[i] = 224; // Light (mood-sensitive)
                } else if (distance < sclera_size) { // Sclera
                    right_eye_animation_buffer[i] = 255; // White
                } else {
                    right_eye_animation_buffer[i] = 0; // Black
                }
            }
            break;
            
        case 5: // Satisfied look with focus adaptation
            for (uint32_t i = 0; i < right_eye_total_pixels; i++) {
                uint32_t y = i / right_eye_frame_width;
                if (y > right_eye_frame_height * 40 / 240 && y < right_eye_frame_height * 200 / 240) {
                    uint32_t x = i % right_eye_frame_width;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    // Relaxed focus - larger pupil for contentment
                    uint32_t relaxed_pupil_size = pupil_size * 1.2;
                    
                    if (distance < relaxed_pupil_size) {
                        right_eye_animation_buffer[i] = 0; // Black pupil
                    } else if (distance < outer_iris_size) {
                        right_eye_animation_buffer[i] = 192; // Purple satisfaction (mood-sensitive)
                    } else {
                        right_eye_animation_buffer[i] = 255; // Bright white
                    }
                } else {
                    right_eye_animation_buffer[i] = 0; // Black
                }
            }
            break;
    }
    
    ESP_LOGV(goblin_right_eye_TAG, "Loaded frame %u to right eye buffer", right_eye_current_frame);
}

#endif // ENABLE_GOBLIN_COMPONENTS
