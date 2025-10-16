// P32 Eye Graphics Implementation
// Renders pixel-perfect eyes with pupil, iris, sclera, and eyelids

#include "p32_eye_graphics.hpp"
#include <string.h>
#include <math.h>
#include "esp_log.h"

static const char *TAG = "EYE_GFX";

// Helper: Clamp value to range
static inline int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Helper: Calculate distance between two points
static inline float distance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrtf((float)(dx * dx + dy * dy));
}

// Helper: Blend two RGB565 colors with alpha
static inline uint16_t blend_rgb565(uint16_t fg, uint16_t bg, float alpha) {
    if (alpha >= 1.0f) return fg;
    if (alpha <= 0.0f) return bg;
    
    // Extract RGB components
    uint8_t r1 = (fg >> 11) & 0x1F;
    uint8_t g1 = (fg >> 5) & 0x3F;
    uint8_t b1 = fg & 0x1F;
    
    uint8_t r2 = (bg >> 11) & 0x1F;
    uint8_t g2 = (bg >> 5) & 0x3F;
    uint8_t b2 = bg & 0x1F;
    
    // Blend
    uint8_t r = (uint8_t)(r1 * alpha + r2 * (1.0f - alpha));
    uint8_t g = (uint8_t)(g1 * alpha + g2 * (1.0f - alpha));
    uint8_t b = (uint8_t)(b1 * alpha + b2 * (1.0f - alpha));
    
    return (r << 11) | (g << 5) | b;
}

void eye_graphics_init_default(eye_graphics_config_t* config) {
    if (!config) return;
    
    config->display_width = EYE_DISPLAY_WIDTH;
    config->display_height = EYE_DISPLAY_HEIGHT;
    config->center_x = EYE_CENTER_X;
    config->center_y = EYE_CENTER_Y;
    
    // Eyeball fills 80% of display
    config->eyeball_radius = 0.8f;
    
    // Iris is 40% of eyeball
    config->iris_radius = 0.4f;
    
    // Pupil ranges from 20% to 60% of iris
    config->pupil_radius_min = 0.2f;
    config->pupil_radius_max = 0.6f;
    
    // Default goblin colors
    config->sclera_color = COLOR_SCLERA_WHITE;
    config->iris_color = COLOR_IRIS_GOBLIN;
    config->pupil_color = COLOR_PUPIL_BLACK;
    config->eyelid_color = COLOR_EYELID_SKIN;
    
    ESP_LOGI(TAG, "Eye graphics initialized (goblin defaults)");
}

void eye_graphics_fill_circle(uint16_t* framebuffer,
                               int center_x, int center_y,
                               int radius, uint16_t color) {
    if (!framebuffer || radius <= 0) return;
    
    int radius_sq = radius * radius;
    
    // Bounding box for circle
    int min_x = clamp(center_x - radius, 0, EYE_DISPLAY_WIDTH - 1);
    int max_x = clamp(center_x + radius, 0, EYE_DISPLAY_WIDTH - 1);
    int min_y = clamp(center_y - radius, 0, EYE_DISPLAY_HEIGHT - 1);
    int max_y = clamp(center_y + radius, 0, EYE_DISPLAY_HEIGHT - 1);
    
    // Fill circle using distance check
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            int dx = x - center_x;
            int dy = y - center_y;
            int dist_sq = dx * dx + dy * dy;
            
            if (dist_sq <= radius_sq) {
                int index = y * EYE_DISPLAY_WIDTH + x;
                framebuffer[index] = color;
            }
        }
    }
}

void eye_graphics_draw_circle(uint16_t* framebuffer,
                               int center_x, int center_y,
                               int radius, uint16_t color,
                               int thickness) {
    if (!framebuffer || radius <= 0 || thickness <= 0) return;
    
    int inner_radius_sq = (radius - thickness) * (radius - thickness);
    int outer_radius_sq = radius * radius;
    
    int min_x = clamp(center_x - radius, 0, EYE_DISPLAY_WIDTH - 1);
    int max_x = clamp(center_x + radius, 0, EYE_DISPLAY_WIDTH - 1);
    int min_y = clamp(center_y - radius, 0, EYE_DISPLAY_HEIGHT - 1);
    int max_y = clamp(center_y + radius, 0, EYE_DISPLAY_HEIGHT - 1);
    
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            int dx = x - center_x;
            int dy = y - center_y;
            int dist_sq = dx * dx + dy * dy;
            
            // Draw ring
            if (dist_sq >= inner_radius_sq && dist_sq <= outer_radius_sq) {
                int index = y * EYE_DISPLAY_WIDTH + x;
                framebuffer[index] = color;
            }
        }
    }
}

void eye_graphics_fill_rect(uint16_t* framebuffer,
                             int x, int y,
                             int width, int height,
                             uint16_t color) {
    if (!framebuffer) return;
    
    int x1 = clamp(x, 0, EYE_DISPLAY_WIDTH - 1);
    int x2 = clamp(x + width, 0, EYE_DISPLAY_WIDTH);
    int y1 = clamp(y, 0, EYE_DISPLAY_HEIGHT - 1);
    int y2 = clamp(y + height, 0, EYE_DISPLAY_HEIGHT);
    
    for (int py = y1; py < y2; py++) {
        for (int px = x1; px < x2; px++) {
            int index = py * EYE_DISPLAY_WIDTH + px;
            framebuffer[index] = color;
        }
    }
}

void eye_graphics_draw_eyeball(uint16_t* framebuffer,
                                const eye_graphics_config_t* config,
                                const eye_keyframe_t* frame) {
    if (!framebuffer || !config || !frame) return;
    
    // Calculate actual pixel radii
    int eyeball_radius_px = (int)(config->eyeball_radius * config->display_width / 2);
    int iris_radius_px = (int)(config->iris_radius * eyeball_radius_px);
    
    // Pupil size from animation frame (0.0-1.0)
    float pupil_size = clamp(frame->pupil_size, 0.0f, 1.0f);
    float pupil_radius_ratio = config->pupil_radius_min + 
                              (config->pupil_radius_max - config->pupil_radius_min) * pupil_size;
    int pupil_radius_px = (int)(pupil_radius_ratio * iris_radius_px);
    
    // Draw layers from back to front
    
    // 1. Sclera (white of eye)
    eye_graphics_fill_circle(framebuffer, 
                             config->center_x, config->center_y,
                             eyeball_radius_px, config->sclera_color);
    
    // 2. Iris (colored part)
    // Use frame color if specified, otherwise default
    uint16_t iris_color = config->iris_color;
    if (frame->color != 0) {
        // Convert 0xRRGGBB to RGB565
        uint8_t r = (frame->color >> 16) & 0xFF;
        uint8_t g = (frame->color >> 8) & 0xFF;
        uint8_t b = frame->color & 0xFF;
        iris_color = RGB565(r, g, b);
    }
    
    eye_graphics_fill_circle(framebuffer,
                             config->center_x, config->center_y,
                             iris_radius_px, iris_color);
    
    // 3. Pupil (black center)
    eye_graphics_fill_circle(framebuffer,
                             config->center_x, config->center_y,
                             pupil_radius_px, config->pupil_color);
    
    // 4. Add iris detail (darker ring around iris edge for depth)
    uint16_t iris_edge_color = blend_rgb565(config->pupil_color, iris_color, 0.3f);
    eye_graphics_draw_circle(framebuffer,
                             config->center_x, config->center_y,
                             iris_radius_px, iris_edge_color, 2);
}

void eye_graphics_draw_eyelids(uint16_t* framebuffer,
                                const eye_graphics_config_t* config,
                                float eye_openness) {
    if (!framebuffer || !config) return;
    
    // Clamp openness to 0.0-1.0
    eye_openness = clamp(eye_openness, 0.0f, 1.0f);
    
    // Calculate eyeball vertical extent
    int eyeball_radius_px = (int)(config->eyeball_radius * config->display_width / 2);
    int top_edge = config->center_y - eyeball_radius_px;
    int bottom_edge = config->center_y + eyeball_radius_px;
    int vertical_range = bottom_edge - top_edge;
    
    // Eye openness controls how much of vertical range is visible
    // 1.0 = fully open (no eyelid coverage)
    // 0.0 = fully closed (eyelids meet in middle)
    
    int top_eyelid_y = top_edge + (int)((1.0f - eye_openness) * vertical_range / 2);
    int bottom_eyelid_y = bottom_edge - (int)((1.0f - eye_openness) * vertical_range / 2);
    
    // Draw top eyelid (covers from top of display to eyelid position)
    if (top_eyelid_y > top_edge) {
        eye_graphics_fill_rect(framebuffer,
                               0, 0,
                               config->display_width, top_eyelid_y,
                               config->eyelid_color);
    }
    
    // Draw bottom eyelid (covers from eyelid position to bottom of display)
    if (bottom_eyelid_y < bottom_edge) {
        eye_graphics_fill_rect(framebuffer,
                               0, bottom_eyelid_y,
                               config->display_width, config->display_height - bottom_eyelid_y,
                               config->eyelid_color);
    }
}

void eye_graphics_apply_expression(uint16_t* framebuffer,
                                    const eye_graphics_config_t* config,
                                    eye_expression_t expression) {
    if (!framebuffer || !config) return;
    
    int eyeball_radius_px = (int)(config->eyeball_radius * config->display_width / 2);
    
    switch (expression) {
        case EYE_ANGRY_NARROW:
            // Angry: Add diagonal eyelid overlay (furrowed brow effect)
            for (int y = 0; y < config->display_height / 3; y++) {
                int offset = y / 2;  // Diagonal slope
                eye_graphics_fill_rect(framebuffer,
                                       0, y,
                                       config->display_width / 2 + offset, 1,
                                       config->eyelid_color);
            }
            break;
            
        case EYE_HAPPY_SQUINT:
            // Happy: Squint effect (slightly close top and bottom)
            eye_graphics_fill_rect(framebuffer,
                                   0, 0,
                                   config->display_width, config->display_height / 6,
                                   config->eyelid_color);
            eye_graphics_fill_rect(framebuffer,
                                   0, config->display_height * 5 / 6,
                                   config->display_width, config->display_height / 6,
                                   config->eyelid_color);
            break;
            
        case EYE_SURPRISED_WIDE:
            // Surprised: No eyelid overlay (eyes wide open)
            break;
            
        case EYE_SLEEPY_DROOPY:
            // Sleepy: Heavy top eyelid
            eye_graphics_fill_rect(framebuffer,
                                   0, 0,
                                   config->display_width, config->display_height / 4,
                                   config->eyelid_color);
            break;
            
        case EYE_NORMAL:
        default:
            // Normal: No additional overlay
            break;
    }
}

void eye_graphics_render_frame(uint16_t* framebuffer,
                                const eye_graphics_config_t* config,
                                const eye_keyframe_t* frame) {
    if (!framebuffer || !config || !frame) {
        ESP_LOGE(TAG, "Invalid parameters for render_frame");
        return;
    }
    
    // Clear framebuffer to eyelid color (background)
    for (int i = 0; i < EYE_DISPLAY_WIDTH * EYE_DISPLAY_HEIGHT; i++) {
        framebuffer[i] = config->eyelid_color;
    }
    
    // Draw eyeball (sclera, iris, pupil)
    eye_graphics_draw_eyeball(framebuffer, config, frame);
    
    // Draw eyelids based on openness
    eye_graphics_draw_eyelids(framebuffer, config, frame->eye_openness);
    
    // Apply expression overlay
    eye_graphics_apply_expression(framebuffer, config, frame->expression);
    
    ESP_LOGD(TAG, "Rendered frame: openness=%.2f, pupil=%.2f, expr=%d",
             frame->eye_openness, frame->pupil_size, frame->expression);
}
