// P32 Eye Graphics Renderer
// Generates pixel-perfect eye graphics for GC9A01 displays
// Uses animation keyframes to render pupil, iris, sclera with smooth interpolation

#ifndef P32_EYE_GRAPHICS_HPP
#define P32_EYE_GRAPHICS_HPP

#include <stdint.h>
#include "p32_eye_display.hpp"

// Display parameters
#define EYE_DISPLAY_WIDTH 240
#define EYE_DISPLAY_HEIGHT 240
#define EYE_CENTER_X 120
#define EYE_CENTER_Y 120

// RGB565 color definitions
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

// Standard eye colors
#define COLOR_SCLERA_WHITE   RGB565(255, 255, 255)  // Eye white
#define COLOR_IRIS_GOBLIN    RGB565(80, 255, 80)    // Greenish goblin eye
#define COLOR_PUPIL_BLACK    RGB565(0, 0, 0)        // Black pupil
#define COLOR_IRIS_ANGRY     RGB565(255, 80, 80)    // Reddish angry eye
#define COLOR_IRIS_CURIOUS   RGB565(80, 200, 255)   // Blueish curious eye
#define COLOR_EYELID_SKIN    RGB565(120, 150, 100)  // Goblin skin tone

/**
 * Eye graphics configuration
 */
typedef struct {
    uint16_t display_width;
    uint16_t display_height;
    int center_x;
    int center_y;
    
    // Eyeball dimensions (percentage of display)
    float eyeball_radius;      // 0.0-1.0 of display width
    float iris_radius;         // 0.0-1.0 of eyeball radius
    float pupil_radius_min;    // Min pupil size (0.0-1.0 of iris)
    float pupil_radius_max;    // Max pupil size (dilated)
    
    // Colors
    uint16_t sclera_color;
    uint16_t iris_color;
    uint16_t pupil_color;
    uint16_t eyelid_color;
    
} eye_graphics_config_t;

/**
 * Initialize eye graphics system with default goblin configuration
 */
void eye_graphics_init_default(eye_graphics_config_t* config);

/**
 * Render complete eye frame to RGB565 framebuffer
 * 
 * @param framebuffer Output buffer (240x240 RGB565 = 115,200 bytes)
 * @param config Eye appearance configuration
 * @param frame Animation keyframe with pupil size, openness, expression
 */
void eye_graphics_render_frame(uint16_t* framebuffer, 
                                const eye_graphics_config_t* config,
                                const eye_keyframe_t* frame);

/**
 * Render circular eyeball (sclera + iris + pupil)
 * 
 * @param framebuffer Output buffer
 * @param config Eye configuration
 * @param frame Animation frame (pupil size from frame->pupil_size)
 */
void eye_graphics_draw_eyeball(uint16_t* framebuffer,
                                const eye_graphics_config_t* config,
                                const eye_keyframe_t* frame);

/**
 * Draw eyelids (top and bottom) based on eye openness
 * 
 * @param framebuffer Output buffer (drawn on top of eyeball)
 * @param config Eye configuration
 * @param eye_openness 0.0 = closed, 1.0 = fully open
 */
void eye_graphics_draw_eyelids(uint16_t* framebuffer,
                                const eye_graphics_config_t* config,
                                float eye_openness);

/**
 * Apply expression overlay (angry squint, happy squint, etc.)
 * 
 * @param framebuffer Output buffer
 * @param config Eye configuration
 * @param expression Expression type
 */
void eye_graphics_apply_expression(uint16_t* framebuffer,
                                    const eye_graphics_config_t* config,
                                    eye_expression_t expression);

/**
 * Fill circular region with color (helper for iris/pupil)
 * 
 * @param framebuffer Output buffer
 * @param center_x Circle center X
 * @param center_y Circle center Y
 * @param radius Circle radius in pixels
 * @param color RGB565 color
 */
void eye_graphics_fill_circle(uint16_t* framebuffer,
                               int center_x, int center_y,
                               int radius, uint16_t color);

/**
 * Draw anti-aliased circle outline (for smooth edges)
 * 
 * @param framebuffer Output buffer
 * @param center_x Circle center X
 * @param center_y Circle center Y
 * @param radius Circle radius
 * @param color RGB565 color
 * @param thickness Line thickness
 */
void eye_graphics_draw_circle(uint16_t* framebuffer,
                               int center_x, int center_y,
                               int radius, uint16_t color,
                               int thickness);

/**
 * Fill rectangular region (for eyelids)
 * 
 * @param framebuffer Output buffer
 * @param x Top-left X
 * @param y Top-left Y  
 * @param width Rectangle width
 * @param height Rectangle height
 * @param color RGB565 color
 */
void eye_graphics_fill_rect(uint16_t* framebuffer,
                             int x, int y,
                             int width, int height,
                             uint16_t color);

#endif // P32_EYE_GRAPHICS_HPP
