#pragma once

/**
 * @file mood_effects.hpp
 * @brief Template functions for applying mood-based color effects to display buffers
 * 
 * This file defines reusable mood adjustment templates for all creature eye displays.
 * Each creature family (goblin, bear, cat, etc.) can use the same template with
 * their own MoodColorEffect coefficients defined in their component.
 * 
 * USAGE:
 *   // In your eye component (e.g., goblin_eye.src):
 *   #include "config/shared_headers/mood_effects.hpp"
 *   
 *   // Define your creature's mood-to-color mapping
 *   static const MoodColorEffect goblin_mood_effects[Mood::componentCount] = {
 *       MoodColorEffect(0.8f, -0.3f, -0.3f),  // ANGER: Red tint
 *       // ... more mood components
 *   };
 *   
 *   // In your act() function:
 *   if (color_schema == "RGB565") {
 *       adjustMood<Pixel_RGB565>(currentFrame, frame_size, goblin_mood_effects);
 *   }
 */

#include <cstdint>
#include "Mood.hpp"

/**
 * @struct MoodColorEffect
 * @brief Color effect multipliers for a single mood component
 * 
 * Defines how a mood component (anger, fear, etc.) affects R, G, B channels.
 * Values are multipliers that are scaled by mood intensity (-128 to +127).
 * 
 * Example:
 *   MoodColorEffect(0.8f, -0.3f, -0.3f)
 *   - Anger (value 80): adds 0.8*80 = 64 red, removes 0.3*80 = 24 green/blue
 */
struct MoodColorEffect {
    float red_multiplier;    // Red channel effect (-1.0 to +1.0)
    float green_multiplier;  // Green channel effect (-1.0 to +1.0)
    float blue_multiplier;   // Blue channel effect (-1.0 to +1.0)
    
    MoodColorEffect(float r = 0.0f, float g = 0.0f, float b = 0.0f)
        : red_multiplier(r), green_multiplier(g), blue_multiplier(b)
    {
    }
};

/**
 * @brief Apply mood-based color adjustments to a pixel buffer
 * 
 * Template function that modifies pixel colors based on current mood state.
 * Works with any pixel format (RGB565, RGB666, RGB888, etc.) through template specialization.
 * 
 * @tparam PixelType Pixel format class (e.g., Pixel_RGB565, Pixel_RGB666)
 * @param buffer_ptr Pointer to pixel buffer (unsigned char* cast to PixelType* internally)
 * @param pixel_count Total number of pixels in buffer
 * @param mood Current mood state
 * @param mood_effects Array of MoodColorEffect for each mood component
 */
template<class PixelType>
void adjustMood(
    unsigned char* buffer_ptr,
    uint32_t pixel_count,
    const Mood& mood,
    const MoodColorEffect* mood_effects)
{
    if (buffer_ptr == nullptr || pixel_count == 0 || mood_effects == nullptr) {
        return;
    }
    
    // Transform unsigned char buffer to typed pixel buffer using clean interface
    PixelType* buffer = PixelType::fromBytes(buffer_ptr);
    
    // Calculate total mood color delta
    // Each mood component contributes its effect scaled by intensity
    float delta_r = 0.0f;
    float delta_g = 0.0f;
    float delta_b = 0.0f;
    
    for (int i = 0; i < Mood::componentCount; ++i) {
        const MoodColorEffect& effect = mood_effects[i];
        int8_t component_value = mood.components[i];
        
        // Scale mood component value (-128 to +127) to effect
        float intensity = component_value / 128.0f;  // -1.0 to +1.0 range
        
        delta_r += effect.red_multiplier * intensity;
        delta_g += effect.green_multiplier * intensity;
        delta_b += effect.blue_multiplier * intensity;
    }
    
    // Clamp deltas to valid range
    if (delta_r > 1.0f) delta_r = 1.0f;
    if (delta_r < -1.0f) delta_r = -1.0f;
    if (delta_g > 1.0f) delta_g = 1.0f;
    if (delta_g < -1.0f) delta_g = -1.0f;
    if (delta_b > 1.0f) delta_b = 1.0f;
    if (delta_b < -1.0f) delta_b = -1.0f;
    
    // Convert deltas to RGB values (0-255 range)
    int16_t r_delta = (int16_t)(delta_r * 255.0f);
    int16_t g_delta = (int16_t)(delta_g * 255.0f);
    int16_t b_delta = (int16_t)(delta_b * 255.0f);
    
    // Apply to each pixel in buffer using saturating addition
    for (uint32_t i = 0; i < pixel_count; ++i) {
        PixelType& pixel = buffer[i];
        
        // Each PixelType must implement saturating add operators
        // Example for RGB565:
        //   pixel.r += r_delta;  (clamps internally)
        //   pixel.g += g_delta;
        //   pixel.b += b_delta;
        
        pixel.r = pixel.r + r_delta;  // Relies on PixelType's operator+= implementation
        pixel.g = pixel.g + g_delta;
        pixel.b = pixel.b + b_delta;
    }
}

/**
 * @brief Overload: adjustMood using external Mood from SharedMemory
 * 
 * Convenience overload that reads Mood from GSM automatically.
 * 
 * @tparam PixelType Pixel format class
 * @param buffer Pointer to pixel buffer
 * @param pixel_count Total number of pixels in buffer
 * @param mood_effects Array of MoodColorEffect for each mood component
 */
template<class PixelType>
void adjustMoodFromGSM(
    PixelType* buffer,
    uint32_t pixel_count,
    const MoodColorEffect* mood_effects)
{
    #include "core/memory/SharedMemory.hpp"
    
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr == nullptr) {
        return;
    }
    
    adjustMood<PixelType>(buffer, pixel_count, *mood_ptr, mood_effects);
}
