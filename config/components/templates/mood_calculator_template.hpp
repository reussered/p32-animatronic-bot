/**
 * @file mood_calculator_template.hpp
 * @brief Template-based mood calculator that adapts to any color schema
 * 
 * This template allows mood-based color modifications to work with RGB565, RGB666, RGB888, etc.
 * Simply instantiate MoodCalculator<PixelType> where PixelType is your color schema.
 * 
 * Usage:
 *   MoodCalculator<Pixel_RGB565> mood_calc;
 *   mood_calc.setMood(ANGER, 0.8f);
 *   Pixel_RGB565 angry_color = mood_calc.applyMoodDelta(base_color);
 */

#pragma once

#include <cstdint>
#include <cmath>
#include "config/shared_headers/color_schema.hpp"
#include "shared/Mood.hpp"

/**
 * @struct MoodColorDelta
 * @brief Color modification parameters for a specific mood
 */
struct MoodColorDelta {
    int8_t red_delta;      // -128 to +127 shift in red channel
    int8_t green_delta;    // -128 to +127 shift in green channel
    int8_t blue_delta;     // -128 to +127 shift in blue channel
    float intensity;       // 0.0 to 2.0x multiplier
    bool enable_glow;      // Add brightness overlay
    bool enable_desaturation;  // Reduce saturation (fear, confusion)
    bool enable_warmth;    // Shift toward red/yellow (happiness, comfort)
    bool enable_coolness;  // Shift toward blue/cyan (sadness, calm)
};

/**
 * @class MoodCalculator
 * @brief Generic mood-to-color calculator working with any pixel format
 * 
 * Template parameter PixelType must have:
 *   - red(), green(), blue() accessors returning 0-255
 *   - Constructors compatible with (r, g, b) arguments
 *   - Move semantics support
 */
template<typename PixelType>
class MoodCalculator {
private:
    static constexpr uint8_t MAX_8BIT = 255;
    static constexpr uint8_t HALF_8BIT = 128;
    
    // Mood color delta lookup table
    MoodColorDelta mood_deltas[MOOD_STATE_COUNT];
    
    // Current mood state
    MoodState current_mood;
    float current_intensity;
    
public:
    MoodCalculator() 
        : current_mood(NEUTRAL), current_intensity(1.0f) 
    {
        initializeDefaultMoodPalette();
    }
    
    /**
     * @brief Initialize default mood color mappings
     * Customize these for your creature personality
     */
    void initializeDefaultMoodPalette() {
        // ANGER: Red-shifted, high intensity
        mood_deltas[ANGER] = {
            .red_delta = 40,
            .green_delta = -30,
            .blue_delta = -30,
            .intensity = 1.5f,
            .enable_glow = true,
            .enable_desaturation = false,
            .enable_warmth = true,
            .enable_coolness = false
        };
        
        // CURIOSITY: Green-shifted with sparkle
        mood_deltas[CURIOSITY] = {
            .red_delta = -20,
            .green_delta = 35,
            .blue_delta = -10,
            .intensity = 1.2f,
            .enable_glow = false,
            .enable_desaturation = false,
            .enable_warmth = false,
            .enable_coolness = false
        };
        
        // FEAR: Cyan-blue, dilated pupils
        mood_deltas[FEAR] = {
            .red_delta = -40,
            .green_delta = 20,
            .blue_delta = 50,
            .intensity = 1.8f,
            .enable_glow = false,
            .enable_desaturation = false,
            .enable_warmth = false,
            .enable_coolness = true
        };
        
        // HAPPINESS: Warm yellow-gold
        mood_deltas[HAPPINESS] = {
            .red_delta = 30,
            .green_delta = 25,
            .blue_delta = -20,
            .intensity = 1.1f,
            .enable_glow = true,
            .enable_desaturation = false,
            .enable_warmth = true,
            .enable_coolness = false
        };
        
        // SADNESS: Desaturated cool tones
        mood_deltas[SADNESS] = {
            .red_delta = -15,
            .green_delta = -15,
            .blue_delta = 10,
            .intensity = 0.8f,
            .enable_glow = false,
            .enable_desaturation = true,
            .enable_warmth = false,
            .enable_coolness = true
        };
        
        // NEUTRAL: No modification (baseline)
        mood_deltas[NEUTRAL] = {
            .red_delta = 0,
            .green_delta = 0,
            .blue_delta = 0,
            .intensity = 1.0f,
            .enable_glow = false,
            .enable_desaturation = false,
            .enable_warmth = false,
            .enable_coolness = false
        };
    }
    
    /**
     * @brief Set current mood and intensity
     * @param mood The emotional state (from Mood.hpp enum)
     * @param intensity Strength 0.0 (no effect) to 1.0 (full effect)
     */
    void setMood(MoodState mood, float intensity = 1.0f) {
        current_mood = mood;
        current_intensity = (intensity < 0.0f) ? 0.0f : 
                          (intensity > 1.0f) ? 1.0f : intensity;
    }
    
    /**
     * @brief Apply mood-based color modification to a pixel
     * @param base_pixel The original color
     * @return Modified pixel with mood applied
     */
    PixelType applyMoodDelta(const PixelType& base_pixel) const {
        // Get mood parameters scaled by current intensity
        const MoodColorDelta& delta = mood_deltas[current_mood];
        float scaled_intensity = delta.intensity * current_intensity;
        
        // Extract base RGB (0-255 range)
        int16_t r = base_pixel.red();
        int16_t g = base_pixel.green();
        int16_t b = base_pixel.blue();
        
        // Apply delta shifts with clamping
        r = applyDeltaWithClamp(r, delta.red_delta, scaled_intensity);
        g = applyDeltaWithClamp(g, delta.green_delta, scaled_intensity);
        b = applyDeltaWithClamp(b, delta.blue_delta, scaled_intensity);
        
        // Apply special effects
        if (delta.enable_glow) {
            r = static_cast<uint8_t>((r + 40) > MAX_8BIT ? MAX_8BIT : r + 40);
            g = static_cast<uint8_t>((g + 40) > MAX_8BIT ? MAX_8BIT : g + 40);
            b = static_cast<uint8_t>((b + 20) > MAX_8BIT ? MAX_8BIT : b + 20);
        }
        
        if (delta.enable_desaturation) {
            // Reduce color saturation by averaging toward gray
            uint8_t gray = static_cast<uint8_t>((r + g + b) / 3);
            r = static_cast<uint8_t>(r * 0.6f + gray * 0.4f);
            g = static_cast<uint8_t>(g * 0.6f + gray * 0.4f);
            b = static_cast<uint8_t>(b * 0.6f + gray * 0.4f);
        }
        
        if (delta.enable_warmth) {
            // Increase red and yellow, reduce blue
            r = static_cast<uint8_t>((r * 1.1f) > MAX_8BIT ? MAX_8BIT : r * 1.1f);
            g = static_cast<uint8_t>((g * 1.05f) > MAX_8BIT ? MAX_8BIT : g * 1.05f);
            b = static_cast<uint8_t>(b * 0.9f);
        }
        
        if (delta.enable_coolness) {
            // Increase blue and cyan, reduce red
            r = static_cast<uint8_t>(r * 0.85f);
            g = static_cast<uint8_t>(g * 0.95f);
            b = static_cast<uint8_t>((b * 1.15f) > MAX_8BIT ? MAX_8BIT : b * 1.15f);
        }
        
        // Clamp final values and return
        return PixelType(
            static_cast<uint8_t>(r > MAX_8BIT ? MAX_8BIT : r),
            static_cast<uint8_t>(g > MAX_8BIT ? MAX_8BIT : g),
            static_cast<uint8_t>(b > MAX_8BIT ? MAX_8BIT : b)
        );
    }
    
    /**
     * @brief Customize mood color delta for specific mood
     * @param mood Which emotional state to modify
     * @param delta New color shift parameters
     */
    void setMoodDelta(MoodState mood, const MoodColorDelta& delta) {
        if (mood < MOOD_STATE_COUNT) {
            mood_deltas[mood] = delta;
        }
    }
    
    /**
     * @brief Get current mood configuration
     */
    const MoodColorDelta& getMoodDelta(MoodState mood) const {
        return mood_deltas[mood];
    }
    
    /**
     * @brief Get currently active mood
     */
    MoodState getCurrentMood() const {
        return current_mood;
    }
    
    /**
     * @brief Get current intensity factor
     */
    float getCurrentIntensity() const {
        return current_intensity;
    }

private:
    /**
     * @brief Apply delta shift with proper clamping to 8-bit range
     */
    int16_t applyDeltaWithClamp(int16_t value, int8_t delta, float intensity) const {
        // Apply intensity scaling to delta
        int16_t scaled_delta = static_cast<int16_t>(static_cast<float>(delta) * intensity);
        int16_t result = value + scaled_delta;
        
        // Clamp to 0-255
        if (result < 0) return 0;
        if (result > MAX_8BIT) return MAX_8BIT;
        return result;
    }
};

/**
 * @typedef MoodCalcRGB565
 * @brief Pre-instantiated mood calculator for RGB565 displays (GC9A01, ST7789, ST7735)
 */
typedef MoodCalculator<Pixel_RGB565> MoodCalcRGB565;

/**
 * @typedef MoodCalcRGB666
 * @brief Pre-instantiated mood calculator for RGB666 displays (ILI9341, RA8875)
 */
typedef MoodCalculator<Pixel_RGB666> MoodCalcRGB666;

/**
 * @typedef MoodCalcRGB888
 * @brief Pre-instantiated mood calculator for RGB888 displays (high-end displays)
 */
typedef MoodCalculator<Pixel_RGB888> MoodCalcRGB888;
