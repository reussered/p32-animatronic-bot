// P32 Graduated Mood Palette System
// Interpolates between mood palettes based on intensity levels (0-999)

#ifndef P32_MOOD_PALETTE_H
#define P32_MOOD_PALETTE_H

#include <stdint.h>

#define PALETTE_SIZE 16
#define MAX_MOOD_INTENSITY 999

// RGB565 color type for efficient storage
typedef uint16_t palette_color_t;

// Mood types enum - matches p32_core.h mood system
typedef enum {
    MOOD_FEAR = 0,
    MOOD_ANGER = 1,
    MOOD_IRRITATION = 2, 
    MOOD_HAPPINESS = 3,
    MOOD_CONTENTMENT = 4,
    MOOD_HUNGER = 5,
    MOOD_CURIOSITY = 6,
    MOOD_AFFECTION = 7,
    MOOD_EXCITEMENT = 8,
    MOOD_TYPE_COUNT = 9
} mood_type_t;

// Mood type names for debugging
extern const char* mood_type_names[MOOD_TYPE_COUNT];

// Mood palette definitions
typedef struct {
    palette_color_t colors[PALETTE_SIZE];
    const char* name;
} mood_palette_t;

// Current mood state - matches p32_core.h enum order
typedef struct {
    uint16_t fear;         // 0-999
    uint16_t anger;        // 0-999
    uint16_t irritation;   // 0-999
    uint16_t happiness;    // 0-999
    uint16_t contentment;  // 0-999
    uint16_t hunger;       // 0-999
    uint16_t curiosity;    // 0-999
    uint16_t affection;    // 0-999
    uint16_t excitement;   // 0-999
} mood_state_t;

// Active palette (computed from mood state)
typedef struct {
    palette_color_t colors[PALETTE_SIZE];
    uint32_t last_update_ms;
    mood_state_t cached_mood;
    bool needs_update;
} active_palette_t;

// Function declarations
void mood_palette_init(void);
void mood_palette_set_state(const mood_state_t* mood);
void mood_palette_update_active_palette(void);
palette_color_t mood_palette_get_color(uint8_t palette_index);
const palette_color_t* mood_palette_get_active_palette(void);

// Mood palette access
const mood_palette_t* mood_palette_get_neutral(void);
const mood_palette_t* mood_palette_get_anger(void);
const mood_palette_t* mood_palette_get_fear(void);
const mood_palette_t* mood_palette_get_happiness(void);
const mood_palette_t* mood_palette_get_contentment(void);
const mood_palette_t* mood_palette_get_curiosity(void);
const mood_palette_t* mood_palette_get_irritation(void);
const mood_palette_t* mood_palette_get_hunger(void);
const mood_palette_t* mood_palette_get_affection(void);

// Color utilities
palette_color_t rgb565_from_rgb(uint8_t r, uint8_t g, uint8_t b);
void rgb_from_rgb565(palette_color_t color565, uint8_t* r, uint8_t* g, uint8_t* b);
palette_color_t interpolate_colors(palette_color_t color1, palette_color_t color2, float factor);

// Helper functions for delta-based mood transitions
const palette_color_t* mood_palette_get_neutral_palette(void);
const palette_color_t* mood_palette_get_palette_for_mood(mood_type_t mood_type);
palette_color_t mood_palette_blend_colors(palette_color_t base_color, palette_color_t mood_color, float blend_factor);

// Debug functions
void mood_palette_print_active_palette(void);
void mood_palette_print_mood_state(const mood_state_t* mood);

#endif // P32_MOOD_PALETTE_H