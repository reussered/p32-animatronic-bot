#pragma once

#include "p32_component_system.h"

// Display component software interface
typedef struct {
    char component_id[32];
    uint8_t cs_pin;
    spi_device_handle_t spi_handle;
    uint16_t* framebuffer;
    uint16_t current_animation;
    uint32_t last_update_time;
    bool enabled;
} p32_display_component_t;

// Display component init/act functions
esp_err_t p32_display_component_init(const p32_component_config_t* config, void** instance);
esp_err_t p32_display_component_act(void* instance, const char* action, void* params);
esp_err_t p32_display_component_cleanup(void* instance);

// Display-specific actions
#define P32_DISPLAY_ACTION_SET_ANIMATION    "set_animation"
#define P32_DISPLAY_ACTION_CLEAR           "clear"
#define P32_DISPLAY_ACTION_SET_COLOR       "set_color"
#define P32_DISPLAY_ACTION_DRAW_SHAPE      "draw_shape"

// Animation IDs that match JSON configuration
#define P32_ANIM_EYE_BLINK          1
#define P32_ANIM_EYE_CURIOUS        2
#define P32_ANIM_EYE_ANGRY          3
#define P32_ANIM_EYE_FEAR           4
#define P32_ANIM_MOUTH_SMILE        10
#define P32_ANIM_MOUTH_FROWN        11
#define P32_ANIM_MOUTH_SNARL        12
#define P32_ANIM_MOUTH_ROAR         13