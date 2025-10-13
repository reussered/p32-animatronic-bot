// P32 Eye Display Animation System
// Renders eye animations from JSON files in a simulated display window

#ifndef P32_EYE_DISPLAY_H
#define P32_EYE_DISPLAY_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

// Display configuration
#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200
#define MAX_ANIMATION_NAME 64
#define MAX_KEYFRAMES 16

// Eye expression types
typedef enum {
    EYE_NORMAL = 0,
    EYE_ANGRY_NARROW,
    EYE_HAPPY_SQUINT,
    EYE_SURPRISED_WIDE,
    EYE_SLEEPY_DROOPY
} eye_expression_t;

// Eye animation keyframe
typedef struct {
    uint32_t time_ms;           // Time in animation
    float pupil_size;           // 0.0 - 1.0
    float eye_openness;         // 0.0 - 1.0 (0=closed, 1=wide open)
    uint32_t color;             // RGB color (0xRRGGBB)
    eye_expression_t expression; // Eye shape/expression
} eye_keyframe_t;

// Animation definition
typedef struct {
    char name[MAX_ANIMATION_NAME];
    uint32_t duration_ms;
    bool loop;
    uint8_t keyframe_count;
    eye_keyframe_t keyframes[MAX_KEYFRAMES];
} eye_animation_t;

// Display state
typedef struct {
    bool active;
    char title[64];             // Window title
    eye_animation_t* current_animation;
    uint32_t animation_start_time;
    uint32_t last_frame_time;
    eye_keyframe_t current_frame;
} eye_display_t;

// Function declarations
esp_err_t eye_display_init(eye_display_t* display, const char* eye_name);
esp_err_t eye_display_load_animation(eye_display_t* display, const char* animation_path);
esp_err_t eye_display_start_animation(eye_display_t* display, eye_animation_t* animation);
esp_err_t eye_display_update(eye_display_t* display, uint32_t current_time_ms);
esp_err_t eye_display_render(eye_display_t* display);
void eye_display_cleanup(eye_display_t* display);

// Built-in animations for testing
extern eye_animation_t goblin_blink_animation;
extern eye_animation_t goblin_angry_stare_animation;
extern eye_animation_t goblin_curious_look_animation;

#endif // P32_EYE_DISPLAY_H