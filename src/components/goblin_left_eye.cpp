/**
 * @file goblin_left_eye.cpp
 * @brief Left eye component with animation buffer management and focusing effects
 * Implements proper component isolation with shared state coordination
 */

#include "p32_shared_state.h"
#include "gc9a01.hpp"
#include "core/memory/SharedMemory.hpp"
#include "shared/Environment.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Module variables
static const char *TAG = "GOBLIN_LEFT_EYE";
static uint8_t* left_eye_animation_buffer = nullptr;
static uint32_t left_eye_current_frame = 0;
static uint32_t left_eye_frame_timer = 0;
static uint32_t frame_width = 0;
static uint32_t frame_height = 0;
static uint32_t total_pixels = 0;

/**
 * @brief Initialize left eye animation system
 */
void goblin_left_eye_init(void) {
    ESP_LOGI(TAG, "Initializing left eye animation system");
    
    // Get buffer and dimensions from display component
    left_eye_animation_buffer = getBuffer();
    total_pixels = getFrameSize();
    frame_width = getFrameRowSize();
    frame_height = total_pixels / frame_width;
    
    if (left_eye_animation_buffer == nullptr) {
        ESP_LOGE(TAG, "Failed to allocate animation buffer");
        return;
    }
    
    // Initialize buffer to white
    for (uint32_t i = 0; i < total_pixels; i++) {
        left_eye_animation_buffer[i] = 255;
    }
    
    left_eye_current_frame = 0;
    left_eye_frame_timer = 0;
    ESP_LOGI(TAG, "Left eye animation initialized with %ux%u display", frame_width, frame_height);
}

/**
 * @brief Load current animation frame to processing buffer for left eye
 * Mirrors right eye animation with distance-based focusing effect
 */
void load_current_frame_to_buffer(void) {
    // Declare local SharedMemory instance and read current state
    SharedMemory local_state;
    
    // Get viewer distance from Environment in shared state
    Environment* env_ptr = local_state.read<Environment>("Environment");
    uint8_t viewer_distance = (env_ptr != nullptr) ? env_ptr->distance_cm : 128; // Default if not available
    
    // Calculate centers based on actual display dimensions
    uint32_t center_x = frame_width / 2;
    uint32_t center_y = frame_height / 2;
    
    // Calculate focus-dependent pupil and iris sizes (scaled to display size)
    uint32_t base_scale = (frame_width + frame_height) / 2; // Average dimension for scaling
    uint32_t pupil_size = (base_scale * 400 / 240) + (viewer_distance * base_scale / 60);
    uint32_t inner_iris_size = (base_scale * 1200 / 240) + (viewer_distance * base_scale / 30);
    uint32_t outer_iris_size = (base_scale * 3600 / 240) + (viewer_distance * base_scale / 20);
    uint32_t sclera_size = (base_scale * 10000 / 240);
    
    // Mirror the right eye animation pattern with focus adaptation
    switch (left_eye_current_frame) {
        case 0: // Normal open eye with adaptive focus
            for (uint32_t i = 0; i < total_pixels; i++) {
                uint32_t x = i % frame_width;
                uint32_t y = i / frame_width;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < pupil_size) { // Adaptive pupil
                    left_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < inner_iris_size) { // Inner iris - detailed when close
                    left_eye_animation_buffer[i] = 96; // Red-brown iris (mood-sensitive palette)
                } else if (distance < outer_iris_size) { // Outer iris - sharp when far
                    left_eye_animation_buffer[i] = 64; // Brown iris (mood-sensitive palette)
                } else if (distance < sclera_size) { // Sclera
                    left_eye_animation_buffer[i] = 255; // White (mood-sensitive palette)
                } else {
                    left_eye_animation_buffer[i] = 0; // Black edge
                }
            }
            break;
            
        case 1: // Slight squint with focus adaptation  
            for (uint32_t i = 0; i < total_pixels; i++) {
                uint32_t y = i / frame_width;
                if (y > frame_height * 0.2 && y < frame_height * 0.8) {
                    uint32_t x = i % frame_width;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    if (distance < pupil_size) { // Adaptive pupil
                        left_eye_animation_buffer[i] = 0; // Black pupil
                    } else if (distance < inner_iris_size) { // Focus-adaptive iris
                        left_eye_animation_buffer[i] = 128; // Green-curious (mood-sensitive)
                    } else if (distance < outer_iris_size) { // Outer iris
                        left_eye_animation_buffer[i] = 96; // Red-brown (mood-sensitive)
                    } else {
                        left_eye_animation_buffer[i] = 224; // Light gray sclera
                    }
                } else {
                    left_eye_animation_buffer[i] = 0; // Black eyelid
                }
            }
            break;
            
        case 2: // Wide curious look with focus adaptation
            for (uint32_t i = 0; i < total_pixels; i++) {
                uint32_t x = i % frame_width;
                uint32_t y = i / frame_width;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                // Alert pupil size - smaller when surprised/focused
                uint32_t alert_pupil_size = pupil_size * 0.6; 
                
                if (distance < alert_pupil_size) { // Small adaptive pupil
                    left_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < inner_iris_size * 0.8) { // Bright inner iris
                    left_eye_animation_buffer[i] = 160; // Blue-curious (mood-sensitive)
                } else if (distance < outer_iris_size) { // Outer iris
                    left_eye_animation_buffer[i] = 128; // Green (mood-sensitive)
                } else if (distance < sclera_size * 1.2) { // Wide sclera for alert look
                    left_eye_animation_buffer[i] = 255; // Bright white
                } else {
                    left_eye_animation_buffer[i] = 32; // Dark edge
                }
            }
            break;
            
        case 3: // Narrowed focus with distance adaptation
            for (uint32_t i = 0; i < total_pixels; i++) {
                uint32_t y = i / frame_width;
                if (y > frame_height * 0.3 && y < frame_height * 0.7) {
                    uint32_t x = i % frame_width;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    // Concentrated focus - pupil size varies with distance
                    uint32_t focus_pupil_size = pupil_size * 0.8;
                    
                    if (distance < focus_pupil_size) {
                        left_eye_animation_buffer[i] = 0; // Black pupil
                    } else if (distance < inner_iris_size) {
                        left_eye_animation_buffer[i] = 96; // Red focus (mood-sensitive)
                    } else {
                        left_eye_animation_buffer[i] = 192; // Purple intensity (mood-sensitive)
                    }
                } else {
                    left_eye_animation_buffer[i] = 16; // Dark eyelid
                }
            }
            break;
            
        case 4: // Return to normal with focus adaptation
            for (uint32_t i = 0; i < total_pixels; i++) {
                uint32_t x = i % frame_width;
                uint32_t y = i / frame_width;
                uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                
                if (distance < pupil_size) { // Adaptive pupil
                    left_eye_animation_buffer[i] = 0; // Black pupil
                } else if (distance < inner_iris_size) { // Inner iris
                    left_eye_animation_buffer[i] = 64; // Brown iris (mood-sensitive)
                } else if (distance < outer_iris_size) { // Outer iris
                    left_eye_animation_buffer[i] = 224; // Light (mood-sensitive)
                } else if (distance < sclera_size) { // Sclera
                    left_eye_animation_buffer[i] = 255; // White
                } else {
                    left_eye_animation_buffer[i] = 0; // Black
                }
            }
            break;
            
        case 5: // Satisfied look with focus adaptation
            for (uint32_t i = 0; i < total_pixels; i++) {
                uint32_t y = i / frame_width;
                if (y > frame_height * 0.17 && y < frame_height * 0.83) {
                    uint32_t x = i % frame_width;
                    uint32_t distance = (x - center_x) * (x - center_x) + (y - center_y) * (y - center_y);
                    
                    // Relaxed focus - larger pupil for contentment
                    uint32_t relaxed_pupil_size = pupil_size * 1.2;
                    
                    if (distance < relaxed_pupil_size) {
                        left_eye_animation_buffer[i] = 0; // Black pupil
                    } else if (distance < outer_iris_size) {
                        left_eye_animation_buffer[i] = 192; // Purple satisfaction (mood-sensitive)
                    } else {
                        left_eye_animation_buffer[i] = 255; // Bright white
                    }
                } else {
                    left_eye_animation_buffer[i] = 0; // Black
                }
            }
            break;
    }
}

/**
 * @brief Main animation control function for left eye
 * Controls frame timing and animation sequence
 */
void goblin_left_eye_act(void) {
    // Frame timing control (advance every 30 loops ≈ 500ms at 60Hz)
    left_eye_frame_timer++;
    if (left_eye_frame_timer >= 30) {
        left_eye_frame_timer = 0;
        left_eye_current_frame = (left_eye_current_frame + 1) % 6; // Cycle through 6 frames
    }
    
    // Load current frame to buffer and set global state
    load_current_frame_to_buffer();
}
