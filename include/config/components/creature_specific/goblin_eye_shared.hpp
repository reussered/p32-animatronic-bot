// Shared constants and data for goblin eye components

#ifndef GOBLIN_EYE_SHARED_HPP
#define GOBLIN_EYE_SHARED_HPP

#include "Mood.hpp"

// Chunk dimensions (240x30 pixels = 7200 pixels per chunk)
static constexpr uint16_t display_width = 240;
static constexpr uint16_t rows_per_chunk = 30;
static constexpr uint32_t pixels_per_chunk = display_width * rows_per_chunk;

// Goblin personality multiplier
static constexpr float GOBLIN_EMOTION_INTENSITY = 1.5f;

// Mood-to-color mapping
static const MoodColorEffect goblin_mood_effects[Mood::componentCount] = {
    // ANGER: Red tint, reduces green/blue
    MoodColorEffect(0.8f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY),
    // FEAR: Blue tint, pale
    MoodColorEffect(-0.2f * GOBLIN_EMOTION_INTENSITY, -0.2f * GOBLIN_EMOTION_INTENSITY, 0.6f * GOBLIN_EMOTION_INTENSITY),
    // HAPPINESS: Yellow/warm tint
    MoodColorEffect(0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY, 0.1f * GOBLIN_EMOTION_INTENSITY),
    // SADNESS: Desaturate
    MoodColorEffect(-0.3f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY, -0.1f * GOBLIN_EMOTION_INTENSITY),
    // CURIOSITY: Green tint
    MoodColorEffect(0.1f * GOBLIN_EMOTION_INTENSITY, 0.7f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY),
    // AFFECTION: Purple/warm tint
    MoodColorEffect(0.4f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY, 0.4f * GOBLIN_EMOTION_INTENSITY),
    // IRRITATION: Orange-red tint
    MoodColorEffect(0.6f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY, -0.2f * GOBLIN_EMOTION_INTENSITY),
    // CONTENTMENT: Warm, slightly yellow
    MoodColorEffect(0.3f * GOBLIN_EMOTION_INTENSITY, 0.4f * GOBLIN_EMOTION_INTENSITY, 0.1f * GOBLIN_EMOTION_INTENSITY),
    // EXCITEMENT: Bright, all colors up
    MoodColorEffect(0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY)
};

#endif // GOBLIN_EYE_SHARED_HPP