// P32 Graduated Mood Palette System Implementation
#include "p32_mood_palette.h"
#include "esp_log.h"
#include <math.h>
#include <string.h>

static const char* TAG = "MOOD_PALETTE";

// Mood type names for debugging
const char* mood_type_names[MOOD_TYPE_COUNT] = {
    "ANGER",
    "FEAR", 
    "HAPPINESS",
    "SADNESS",
    "CURIOSITY",
    "AFFECTION",
    "IRRITATION",
    "CONTENTMENT"
};

// Current mood state and active palette
static mood_state_t current_mood = {0};
static active_palette_t active_palette = {0};

// Base mood palettes (RGB565 format)
static const mood_palette_t palette_neutral = {
    .colors = {
        0x0000,  // 0: Black
        0xFFFF,  // 1: White  
        0x07E0,  // 2: Green
        0x001F,  // 3: Blue
        0xF800,  // 4: Red
        0xFFE0,  // 5: Yellow
        0x07FF,  // 6: Cyan
        0xF81F,  // 7: Magenta
        0x7BEF,  // 8: Light Gray
        0x39C7,  // 9: Dark Gray
        0x03E0,  // 10: Dark Green
        0x0010,  // 11: Dark Blue
        0x8000,  // 12: Dark Red
        0x8400,  // 13: Dark Yellow
        0x0410,  // 14: Dark Cyan
        0x8010   // 15: Dark Magenta
    },
    .name = "neutral"
};

static const mood_palette_t palette_anger = {
    .colors = {
        0x0000,  // 0: Black (stays black)
        0xF800,  // 1: Red (was white)
        0xFD20,  // 2: Orange-Red (was green)  
        0xFBE0,  // 3: Yellow-Orange (was blue)
        0xF800,  // 4: Bright Red (was red)
        0xFFE0,  // 5: Bright Yellow (was yellow)
        0xFC00,  // 6: Red-Orange (was cyan)
        0xF810,  // 7: Red-Pink (was magenta)
        0xFD6F,  // 8: Light Red (was light gray)
        0xC986,  // 9: Dark Red-Gray (was dark gray)
        0xE000,  // 10: Deep Red (was dark green)
        0x8800,  // 11: Maroon (was dark blue)
        0xF000,  // 12: Bright Red (was dark red)
        0xFE00,  // 13: Red-Yellow (was dark yellow)
        0xF400,  // 14: Red-Orange (was dark cyan)
        0xF008   // 15: Deep Red (was dark magenta)
    },
    .name = "anger"
};

static const mood_palette_t palette_fear = {
    .colors = {
        0x0000,  // 0: Black (stays black)
        0x8410,  // 1: Dark Gray (was white)
        0x4208,  // 2: Dark Blue-Gray (was green)
        0x2104,  // 3: Very Dark Blue (was blue)
        0x4010,  // 4: Dark Purple (was red)
        0x6318,  // 5: Pale Yellow-Gray (was yellow)
        0x2945,  // 6: Dark Teal (was cyan)
        0x4814,  // 7: Dark Purple-Gray (was magenta)
        0x5AEB,  // 8: Medium Gray (was light gray)
        0x2945,  // 9: Dark Gray-Blue (was dark gray)
        0x1082,  // 10: Very Dark Green (was dark green)
        0x0008,  // 11: Very Dark Blue (was dark blue)
        0x2008,  // 12: Very Dark Red (was dark red)
        0x2104,  // 13: Dark Gray-Yellow (was dark yellow)
        0x1042,  // 14: Very Dark Cyan (was dark cyan)
        0x2008   // 15: Very Dark Magenta (was dark magenta)
    },
    .name = "fear"
};

static const mood_palette_t palette_happiness = {
    .colors = {
        0x0000,  // 0: Black (stays black)
        0xFFFF,  // 1: Bright White (was white)
        0x07E0,  // 2: Bright Green (was green)
        0x87FF,  // 3: Light Blue (was blue)
        0xFD20,  // 4: Orange (was red)
        0xFFE0,  // 5: Bright Yellow (was yellow)
        0x87FF,  // 6: Light Cyan (was cyan)
        0xFD5F,  // 7: Pink (was magenta)
        0xEF7D,  // 8: Very Light Gray (was light gray)
        0xAD55,  // 9: Light Gray (was dark gray)
        0x5FE0,  // 10: Light Green (was dark green)
        0x435F,  // 11: Light Blue (was dark blue)
        0xFD00,  // 12: Bright Orange (was dark red)
        0xF7E0,  // 13: Light Yellow (was dark yellow)
        0x5FFF,  // 14: Light Cyan (was dark cyan)
        0xFD5F   // 15: Light Pink (was dark magenta)
    },
    .name = "happiness"
};

// Additional mood palettes (contentment, curiosity, etc.) can be added here...

void mood_palette_init(void) {
    // Initialize with neutral mood
    memset(&current_mood, 0, sizeof(current_mood));
    memcpy(active_palette.colors, palette_neutral.colors, sizeof(palette_neutral.colors));
    active_palette.needs_update = false;
    
    ESP_LOGI(TAG, "Mood palette system initialized (neutral)");
}

void mood_palette_set_state(const mood_state_t* mood) {
    if (memcmp(&current_mood, mood, sizeof(mood_state_t)) != 0) {
        current_mood = *mood;
        active_palette.needs_update = true;
        
        ESP_LOGD(TAG, "Mood updated - ANGER:%d FEAR:%d HAPPY:%d", 
                 mood->anger, mood->fear, mood->happiness);
    }
}

void mood_palette_update_active_palette(void) {
    if (!active_palette.needs_update) {
        return;
    }
    
    // Start with neutral palette
    palette_color_t result_palette[PALETTE_SIZE];
    memcpy(result_palette, palette_neutral.colors, sizeof(palette_neutral.colors));
    
    // Apply mood influences in order of intensity
    // Higher intensity moods override lower intensity ones
    
    // Apply anger influence
    if (current_mood.anger > 0) {
        float anger_factor = (float)current_mood.anger / MAX_MOOD_INTENSITY;
        for (int i = 0; i < PALETTE_SIZE; i++) {
            result_palette[i] = interpolate_colors(result_palette[i], palette_anger.colors[i], anger_factor);
        }
    }
    
    // Apply fear influence (blends with existing)
    if (current_mood.fear > 0) {
        float fear_factor = (float)current_mood.fear / MAX_MOOD_INTENSITY * 0.7f; // Fear is more subtle
        for (int i = 0; i < PALETTE_SIZE; i++) {
            result_palette[i] = interpolate_colors(result_palette[i], palette_fear.colors[i], fear_factor);
        }
    }
    
    // Apply happiness influence (blends with existing)
    if (current_mood.happiness > 0) {
        float happy_factor = (float)current_mood.happiness / MAX_MOOD_INTENSITY * 0.8f;
        for (int i = 0; i < PALETTE_SIZE; i++) {
            result_palette[i] = interpolate_colors(result_palette[i], palette_happiness.colors[i], happy_factor);
        }
    }
    
    // Copy result to active palette
    memcpy(active_palette.colors, result_palette, sizeof(result_palette));
    active_palette.last_update_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    active_palette.cached_mood = current_mood;
    active_palette.needs_update = false;
    
    ESP_LOGD(TAG, "Active palette updated");
}

palette_color_t mood_palette_get_color(uint8_t palette_index) {
    if (palette_index >= PALETTE_SIZE) {
        return 0x0000; // Black for invalid indices
    }
    
    mood_palette_update_active_palette();
    return active_palette.colors[palette_index];
}

const palette_color_t* mood_palette_get_active_palette(void) {
    mood_palette_update_active_palette();
    return active_palette.colors;
}

palette_color_t rgb565_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void rgb_from_rgb565(palette_color_t color565, uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = (color565 >> 8) & 0xF8;
    *g = (color565 >> 3) & 0xFC; 
    *b = (color565 << 3) & 0xF8;
}

palette_color_t interpolate_colors(palette_color_t color1, palette_color_t color2, float factor) {
    if (factor <= 0.0f) return color1;
    if (factor >= 1.0f) return color2;
    
    // Extract RGB components
    uint8_t r1, g1, b1, r2, g2, b2;
    rgb_from_rgb565(color1, &r1, &g1, &b1);
    rgb_from_rgb565(color2, &r2, &g2, &b2);
    
    // Interpolate each component
    uint8_t r_result = r1 + (uint8_t)((r2 - r1) * factor);
    uint8_t g_result = g1 + (uint8_t)((g2 - g1) * factor);
    uint8_t b_result = b1 + (uint8_t)((b2 - b1) * factor);
    
    return rgb565_from_rgb(r_result, g_result, b_result);
}

void mood_palette_print_active_palette(void) {
    mood_palette_update_active_palette();
    ESP_LOGI(TAG, "Active Palette (RGB565):");
    for (int i = 0; i < PALETTE_SIZE; i++) {
        ESP_LOGI(TAG, "  [%2d]: 0x%04X", i, active_palette.colors[i]);
    }
}

void mood_palette_print_mood_state(const mood_state_t* mood) {
    ESP_LOGI(TAG, "Mood State:");
    ESP_LOGI(TAG, "  ANGER: %d/%d", mood->anger, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  FEAR: %d/%d", mood->fear, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  HAPPINESS: %d/%d", mood->happiness, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  CONTENTMENT: %d/%d", mood->contentment, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  CURIOSITY: %d/%d", mood->curiosity, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  IRRITATION: %d/%d", mood->irritation, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  HUNGER: %d/%d", mood->hunger, MAX_MOOD_INTENSITY);
    ESP_LOGI(TAG, "  AFFECTION: %d/%d", mood->affection, MAX_MOOD_INTENSITY);
}

// Palette getters
const mood_palette_t* mood_palette_get_neutral(void) { return &palette_neutral; }
const mood_palette_t* mood_palette_get_anger(void) { return &palette_anger; }
const mood_palette_t* mood_palette_get_fear(void) { return &palette_fear; }
const mood_palette_t* mood_palette_get_happiness(void) { return &palette_happiness; }