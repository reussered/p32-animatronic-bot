// goblin_eye_shared.hpp
// Shared constants and types for goblin eye components

#ifndef GOBLIN_EYE_SHARED_HPP
#define GOBLIN_EYE_SHARED_HPP

#include "config/components/hardware/gc9a01.hdr"

// Display dimensions (defined in goblin_left_eye.src)
extern const uint16_t display_width;
extern const uint16_t display_height;
extern const uint16_t rows_per_chunk;
extern const uint32_t pixels_per_chunk;
extern const uint32_t total_pixels;

// Mood effect structure
struct MoodColorEffect {
    float red_multiplier;
    float green_multiplier;
    float blue_multiplier;

    constexpr MoodColorEffect(float r, float g, float b)
        : red_multiplier(r), green_multiplier(g), blue_multiplier(b) {}
};

// Goblin personality mood effects (9 components)
static const MoodColorEffect goblin_mood_effects[Mood::componentCount] = {
    // ANGER: Red tint, reduces green/blue
    MoodColorEffect(0.8f, -0.3f, -0.3f),
    // FEAR: Blue tint, pale
    MoodColorEffect(-0.2f, -0.2f, 0.6f),
    // HAPPINESS: Yellow/warm tint
    MoodColorEffect(0.5f, 0.5f, 0.1f),
    // SADNESS: Desaturate
    MoodColorEffect(-0.3f, -0.3f, -0.1f),
    // CURIOSITY: Green tint
    MoodColorEffect(0.1f, 0.7f, 0.2f),
    // AFFECTION: Purple/warm tint
    MoodColorEffect(0.4f, 0.2f, 0.4f),
    // IRRITATION: Orange-red tint
    MoodColorEffect(0.6f, 0.2f, -0.2f),
    // CONTENTMENT: Warm, slightly yellow
    MoodColorEffect(0.3f, 0.4f, 0.1f),
    // EXCITEMENT: Bright, all colors up
    MoodColorEffect(0.5f, 0.5f, 0.5f)
};

#endif // GOBLIN_EYE_SHARED_HPP