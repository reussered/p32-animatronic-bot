// P32 Eye Display Animation Implementation
// Simulates 200x200 display window with eye animations

#include "p32_eye_display.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

static const char *TAG = "EYE_DISPLAY";

// Built-in test animations (compiled-in for now)
eye_animation_t goblin_blink_animation = {
    .name = "goblin_blink",
    .duration_ms = 800,
    .loop = false,
    .keyframe_count = 4,
    .keyframes = {
        {0,   0.7f, 0.8f, 0x4444ff, EYE_NORMAL},      // Open
        {200, 0.7f, 0.4f, 0x4444ff, EYE_NORMAL},      // Half closed
        {400, 0.7f, 0.0f, 0x4444ff, EYE_NORMAL},      // Closed
        {800, 0.7f, 0.8f, 0x4444ff, EYE_NORMAL}       // Open
    }
};

eye_animation_t goblin_angry_stare_animation = {
    .name = "angry_stare",
    .duration_ms = 2000,
    .loop = true,
    .keyframe_count = 3,
    .keyframes = {
        {0,    0.6f, 0.4f, 0xff4444, EYE_ANGRY_NARROW}, // Angry narrow
        {1000, 0.8f, 0.3f, 0xff2222, EYE_ANGRY_NARROW}, // More intense
        {2000, 0.6f, 0.4f, 0xff4444, EYE_ANGRY_NARROW}  // Back to start
    }
};

eye_animation_t goblin_curious_look_animation = {
    .name = "curious_look", 
    .duration_ms = 1500,
    .loop = false,
    .keyframe_count = 3,
    .keyframes = {
        {0,    0.5f, 0.9f, 0x44ff44, EYE_SURPRISED_WIDE}, // Wide open
        {750,  0.7f, 0.7f, 0x44ff44, EYE_NORMAL},         // Normal
        {1500, 0.5f, 0.9f, 0x44ff44, EYE_SURPRISED_WIDE}  // Wide again
    }
};

// Initialize display
esp_err_t eye_display_init(eye_display_t* display, const char* eye_name) {
    if (!display || !eye_name) {
        return ESP_ERR_INVALID_ARG;
    }
    
    memset(display, 0, sizeof(eye_display_t));
    snprintf(display->title, sizeof(display->title), "P32 %s Display", eye_name);
    display->active = false;
    
    ESP_LOGI(TAG, "Initialized %s (%dx%d display)", 
             display->title, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    return ESP_OK;
}

// Linear interpolation between keyframes
static void interpolate_keyframe(eye_keyframe_t* result, 
                                const eye_keyframe_t* k1, 
                                const eye_keyframe_t* k2, 
                                float t) {
    result->time_ms = k1->time_ms + (uint32_t)((k2->time_ms - k1->time_ms) * t);
    result->pupil_size = k1->pupil_size + (k2->pupil_size - k1->pupil_size) * t;
    result->eye_openness = k1->eye_openness + (k2->eye_openness - k1->eye_openness) * t;
    
    // Color interpolation (simple)
    uint8_t r1 = (k1->color >> 16) & 0xFF, g1 = (k1->color >> 8) & 0xFF, b1 = k1->color & 0xFF;
    uint8_t r2 = (k2->color >> 16) & 0xFF, g2 = (k2->color >> 8) & 0xFF, b2 = k2->color & 0xFF;
    
    uint8_t r = (uint8_t)(r1 + (r2 - r1) * t);
    uint8_t g = (uint8_t)(g1 + (g2 - g1) * t);
    uint8_t b = (uint8_t)(b1 + (b2 - b1) * t);
    result->color = (r << 16) | (g << 8) | b;
    
    result->expression = t < 0.5f ? k1->expression : k2->expression;
}

// Start animation
esp_err_t eye_display_start_animation(eye_display_t* display, eye_animation_t* animation) {
    if (!display || !animation) {
        return ESP_ERR_INVALID_ARG;
    }
    
    display->current_animation = animation;
    display->animation_start_time = (uint32_t)(esp_timer_get_time() / 1000); // Convert to ms
    display->last_frame_time = 0;
    display->active = true;
    
    // Set initial frame
    if (animation->keyframe_count > 0) {
        display->current_frame = animation->keyframes[0];
    }
    
    ESP_LOGI(TAG, "%s: Starting animation '%s' (%d ms, %s)", 
             display->title, animation->name, animation->duration_ms,
             animation->loop ? "looped" : "once");
    
    return ESP_OK;
}

// Update animation state
esp_err_t eye_display_update(eye_display_t* display, uint32_t current_time_ms) {
    if (!display || !display->active || !display->current_animation) {
        return ESP_OK;
    }
    
    eye_animation_t* anim = display->current_animation;
    uint32_t elapsed = current_time_ms - display->animation_start_time;
    
    // Handle looping
    if (elapsed >= anim->duration_ms) {
        if (anim->loop) {
            elapsed = elapsed % anim->duration_ms;
            display->animation_start_time = current_time_ms - elapsed;
        } else {
            // Animation finished
            display->active = false;
            ESP_LOGI(TAG, "%s: Animation '%s' completed", display->title, anim->name);
            return ESP_OK;
        }
    }
    
    // Find current keyframes for interpolation
    eye_keyframe_t* k1 = &anim->keyframes[0];
    eye_keyframe_t* k2 = &anim->keyframes[anim->keyframe_count - 1];
    
    for (int i = 0; i < anim->keyframe_count - 1; i++) {
        if (elapsed >= anim->keyframes[i].time_ms && elapsed <= anim->keyframes[i + 1].time_ms) {
            k1 = &anim->keyframes[i];
            k2 = &anim->keyframes[i + 1];
            break;
        }
    }
    
    // Interpolate between keyframes
    if (k1->time_ms != k2->time_ms) {
        float t = (float)(elapsed - k1->time_ms) / (float)(k2->time_ms - k1->time_ms);
        interpolate_keyframe(&display->current_frame, k1, k2, t);
    } else {
        display->current_frame = *k1;
    }
    
    return ESP_OK;
}

// Render display (simulated as ASCII art for now)
esp_err_t eye_display_render(eye_display_t* display) {
    if (!display || !display->active) {
        return ESP_OK;
    }
    
    eye_keyframe_t* frame = &display->current_frame;
    
    // Convert values to display characters
    const char* expression_str;
    switch (frame->expression) {
        case EYE_ANGRY_NARROW: expression_str = "ANGRY"; break;
        case EYE_HAPPY_SQUINT: expression_str = "HAPPY"; break; 
        case EYE_SURPRISED_WIDE: expression_str = "SURPRISED"; break;
        case EYE_SLEEPY_DROOPY: expression_str = "SLEEPY"; break;
        default: expression_str = "NORMAL"; break;
    }
    
    // Simulate display window output
    printf("\n┌────────────────────────────────┐\n");
    printf("│ %s (200x200)      │\n", display->title);
    printf("├────────────────────────────────┤\n");
    printf("│                                │\n");
    
    // Eye visualization based on openness
    if (frame->eye_openness > 0.8f) {
        printf("│        ●●●●●●●●●●●●●●        │\n");
        printf("│      ●●               ●●      │\n"); 
        printf("│    ●●    ◉◉◉◉◉◉◉◉    ●●    │\n");
        printf("│  ●●      ◉◉◉◉◉◉◉◉      ●●  │\n");
        printf("│    ●●    ◉◉◉◉◉◉◉◉    ●●    │\n");
        printf("│      ●●               ●●      │\n");
        printf("│        ●●●●●●●●●●●●●●        │\n");
    } else if (frame->eye_openness > 0.4f) {
        printf("│                                │\n");
        printf("│        ████████████████        │\n");
        printf("│      ██    ◉◉◉◉◉◉    ██      │\n");
        printf("│        ████████████████        │\n");
        printf("│                                │\n");
    } else if (frame->eye_openness > 0.1f) {
        printf("│                                │\n");
        printf("│                                │\n");
        printf("│        ▄▄▄▄▄▄▄▄▄▄▄▄▄▄        │\n");
        printf("│                                │\n");
        printf("│                                │\n");
    } else {
        printf("│                                │\n");
        printf("│                                │\n");
        printf("│        ________________        │\n");
        printf("│                                │\n");
        printf("│                                │\n");
    }
    
    printf("│                                │\n");
    printf("├────────────────────────────────┤\n");
    printf("│ %s | Open:%.1f | Pupil:%.1f │\n", 
           expression_str, frame->eye_openness, frame->pupil_size);
    printf("│ Color: #%06" PRIx32 " | Time: %" PRIu32 "ms    │\n", 
           frame->color, frame->time_ms);
    printf("└────────────────────────────────┘\n");
    
    return ESP_OK;
}

// Cleanup
void eye_display_cleanup(eye_display_t* display) {
    if (display) {
        display->active = false;
        display->current_animation = NULL;
        ESP_LOGI(TAG, "%s: Display cleaned up", display->title);
    }
}