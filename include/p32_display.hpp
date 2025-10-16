#pragma once

#include "p32_core.h"

// Display driver for GC9A01 circular displays
#define GC9A01_WIDTH 240
#define GC9A01_HEIGHT 240
#define GC9A01_BUFFER_SIZE (GC9A01_WIDTH * GC9A01_HEIGHT * 2)

// Eye animation states
typedef enum {
    EYE_ANIM_BLINK = 0,
    EYE_ANIM_LOOK_LEFT,
    EYE_ANIM_LOOK_RIGHT,
    EYE_ANIM_LOOK_UP,
    EYE_ANIM_LOOK_DOWN,
    EYE_ANIM_ANGRY_STARE,
    EYE_ANIM_CURIOUS_LOOK,
    EYE_ANIM_FEAR_WIDE,
    EYE_ANIM_EXCITED_SPARKLE,
    EYE_ANIM_COUNT
} p32_eye_animation_t;

// Mouth animation states
typedef enum {
    MOUTH_ANIM_CLOSED = 0,
    MOUTH_ANIM_SMILE,
    MOUTH_ANIM_FROWN,
    MOUTH_ANIM_SNARL,
    MOUTH_ANIM_SPEAK,
    MOUTH_ANIM_ROAR,
    MOUTH_ANIM_COUNT
} p32_mouth_animation_t;

// Display component structure
typedef struct {
    spi_device_handle_t spi_handle;
    uint8_t cs_pin;
    uint8_t dc_pin;
    uint8_t reset_pin;
    uint16_t* framebuffer;
    bool initialized;
    char component_id[32];
} p32_display_t;

// Display system state
typedef struct {
    p32_display_t left_eye;
    p32_display_t right_eye;
    p32_display_t mouth;
    p32_eye_animation_t current_eye_anim;
    p32_mouth_animation_t current_mouth_anim;
    bool displays_enabled;
} p32_display_system_t;

// Display driver functions
esp_err_t p32_display_init(void);
void p32_display_act(void);
esp_err_t p32_display_setup_device(p32_display_t* display, const p32_component_config_t* config);
esp_err_t p32_display_clear(p32_display_t* display, uint16_t color);
esp_err_t p32_display_update(p32_display_t* display);

// Animation functions
esp_err_t p32_display_set_eye_animation(p32_eye_animation_t animation);
esp_err_t p32_display_set_mouth_animation(p32_mouth_animation_t animation);
esp_err_t p32_display_update_mood_animations(p32_mood_t mood);

// Eye-specific animations
esp_err_t p32_display_draw_eye_blink(p32_display_t* display);
esp_err_t p32_display_draw_eye_angry(p32_display_t* display);
esp_err_t p32_display_draw_eye_curious(p32_display_t* display);
esp_err_t p32_display_draw_eye_fear(p32_display_t* display);

// Mouth-specific animations  
esp_err_t p32_display_draw_mouth_smile(p32_display_t* display);
esp_err_t p32_display_draw_mouth_frown(p32_display_t* display);
esp_err_t p32_display_draw_mouth_snarl(p32_display_t* display);
esp_err_t p32_display_draw_mouth_roar(p32_display_t* display);

// Color and graphics utilities
uint16_t p32_display_rgb565(uint8_t r, uint8_t g, uint8_t b);
void p32_display_draw_circle(uint16_t* buffer, int x, int y, int radius, uint16_t color);
void p32_display_draw_rect(uint16_t* buffer, int x, int y, int w, int h, uint16_t color);
void p32_display_fill_buffer(uint16_t* buffer, uint16_t color);

// Note: Abandoned palette-based system - using direct RGB565 colors in animation functions