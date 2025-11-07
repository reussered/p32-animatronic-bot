/**
 * @file goblin_eye_mood_display.hpp
 * @brief Generic goblin eye display with mood-based color rendering
 * 
 * This component works with ANY color schema (RGB565, RGB666, RGB888) by:
 * 1. Reading color_schema from config at initialization
 * 2. Instantiating the appropriate MoodCalculator template
 * 3. Applying mood modifications at render time
 * 
 * Configuration example:
 * {
 *   "name": "goblin_left_eye",
 *   "display_config": {
 *     "resolution": "240x240",
 *     "color_schema": "RGB565",
 *     "driver": "gc9a01"
 *   },
 *   "mood_enabled": true
 * }
 */

#pragma once

#include <cstdint>
#include <cstring>
#include "config/components/templates/mood_calculator_template.hpp"
#include "shared/Mood.hpp"
#include "include/FrameProcessor.hpp"

/**
 * @enum ColorSchema
 * @brief Supported display color formats
 */
enum ColorSchema {
    RGB565_FORMAT = 0,  // 2 bytes per pixel (GC9A01, ST7789, ST7735)
    RGB666_FORMAT = 1,  // 3 bytes per pixel (ILI9341, RA8875)
    RGB888_FORMAT = 2,  // 3 bytes per pixel (high-end)
};

/**
 * @struct EyeDisplayConfig
 * @brief Runtime display configuration
 */
struct EyeDisplayConfig {
    uint16_t width;
    uint16_t height;
    ColorSchema color_format;
    uint16_t bytes_per_pixel;
    uint32_t total_pixels;
    uint32_t buffer_size_bytes;
    bool mood_enabled;
};

/**
 * @class GoblinEyeMoodDisplay
 * @brief Mood-aware eye display renderer (format-agnostic)
 */
class GoblinEyeMoodDisplay {
private:
    EyeDisplayConfig config;
    uint8_t* frame_buffer;
    bool initialized;
    
    // Generic mood calculator wrapper
    void* mood_calculator;  // Actual type depends on color_schema
    
    // Eye animation state
    struct {
        uint16_t pupil_x;
        uint16_t pupil_y;
        uint8_t pupil_size;
        uint8_t eyelid_openness;  // 0-255
        bool blink_active;
    } eye_state;
    
public:
    GoblinEyeMoodDisplay()
        : frame_buffer(nullptr), initialized(false), mood_calculator(nullptr)
    {
        memset(&config, 0, sizeof(config));
        memset(&eye_state, 0, sizeof(eye_state));
        eye_state.pupil_size = 20;
        eye_state.eyelid_openness = 255;
    }
    
    /**
     * @brief Initialize display with auto-detected color format
     * @param width Display width in pixels
     * @param height Display height in pixels
     * @param color_schema Color format (RGB565, RGB666, RGB888)
     * @return true if initialization successful
     */
    bool init(uint16_t width, uint16_t height, ColorSchema color_schema) {
        if (frame_buffer != nullptr) {
            return false;  // Already initialized
        }
        
        config.width = width;
        config.height = height;
        config.color_format = color_schema;
        config.total_pixels = width * height;
        
        // Configure bytes per pixel and allocate buffer
        switch (color_schema) {
            case RGB565_FORMAT:
                config.bytes_per_pixel = 2;
                config.buffer_size_bytes = config.total_pixels * 2;
                mood_calculator = new MoodCalcRGB565();
                break;
                
            case RGB666_FORMAT:
                config.bytes_per_pixel = 3;
                config.buffer_size_bytes = config.total_pixels * 3;
                mood_calculator = new MoodCalcRGB666();
                break;
                
            case RGB888_FORMAT:
                config.bytes_per_pixel = 3;
                config.buffer_size_bytes = config.total_pixels * 3;
                mood_calculator = new MoodCalcRGB888();
                break;
                
            default:
                return false;
        }
        
        config.mood_enabled = true;
        
        // Allocate frame buffer
        frame_buffer = new uint8_t[config.buffer_size_bytes];
        if (!frame_buffer) {
            return false;
        }
        
        // Clear buffer
        memset(frame_buffer, 0, config.buffer_size_bytes);
        initialized = true;
        
        return true;
    }
    
    /**
     * @brief Render eye animation frame with mood effects applied
     * @param mood Current emotional state (from SharedMemory.read<Mood>())
     * @param base_color Color to render (before mood modification)
     */
    void renderFrame(const Mood& mood, uint32_t base_color_rgb888) {
        if (!initialized) return;
        
        // Dispatch to appropriate color-specific renderer
        switch (config.color_format) {
            case RGB565_FORMAT:
                renderFrameRGB565(mood, base_color_rgb888);
                break;
            case RGB666_FORMAT:
                renderFrameRGB666(mood, base_color_rgb888);
                break;
            case RGB888_FORMAT:
                renderFrameRGB888(mood, base_color_rgb888);
                break;
        }
    }
    
    /**
     * @brief Update pupil position (for gaze direction)
     */
    void setPupilPosition(uint16_t x, uint16_t y) {
        eye_state.pupil_x = (x < config.width) ? x : config.width - 1;
        eye_state.pupil_y = (y < config.height) ? y : config.height - 1;
    }
    
    /**
     * @brief Update eyelid openness (0=closed, 255=fully open)
     */
    void setEyelidOpenness(uint8_t openness) {
        eye_state.eyelid_openness = openness;
    }
    
    /**
     * @brief Trigger blink animation
     */
    void blink() {
        eye_state.blink_active = true;
    }
    
    /**
     * @brief Get frame buffer pointer
     */
    const uint8_t* getFrameBuffer() const {
        return frame_buffer;
    }
    
    /**
     * @brief Get buffer size in bytes
     */
    uint32_t getBufferSize() const {
        return config.buffer_size_bytes;
    }
    
    /**
     * @brief Get display width
     */
    uint16_t getWidth() const {
        return config.width;
    }
    
    /**
     * @brief Get display height
     */
    uint16_t getHeight() const {
        return config.height;
    }
    
    /**
     * @brief Get color schema
     */
    ColorSchema getColorSchema() const {
        return config.color_format;
    }
    
    /**
     * @brief Cleanup
     */
    ~GoblinEyeMoodDisplay() {
        if (frame_buffer) {
            delete[] frame_buffer;
            frame_buffer = nullptr;
        }
        if (mood_calculator) {
            switch (config.color_format) {
                case RGB565_FORMAT:
                    delete static_cast<MoodCalcRGB565*>(mood_calculator);
                    break;
                case RGB666_FORMAT:
                    delete static_cast<MoodCalcRGB666*>(mood_calculator);
                    break;
                case RGB888_FORMAT:
                    delete static_cast<MoodCalcRGB888*>(mood_calculator);
                    break;
            }
            mood_calculator = nullptr;
        }
    }

private:
    /**
     * @brief Render RGB565 frame with mood modifications
     */
    void renderFrameRGB565(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB565* calc = static_cast<MoodCalcRGB565*>(mood_calculator);
        
        // Convert RGB888 base color to RGB565
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB565 base_pixel(r8, g8, b8);
        
        // Set mood (convert from Mood struct to MoodState enum)
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        // Apply mood and render to buffer
        Pixel_RGB565 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Fill entire frame with mood-modified color
        uint16_t* buffer_16 = reinterpret_cast<uint16_t*>(frame_buffer);
        uint16_t pixel_value = (mood_pixel.red << 11) | (mood_pixel.green << 5) | mood_pixel.blue;
        
        for (uint32_t i = 0; i < config.total_pixels; ++i) {
            buffer_16[i] = pixel_value;
        }
    }
    
    /**
     * @brief Render RGB666 frame with mood modifications
     */
    void renderFrameRGB666(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB666* calc = static_cast<MoodCalcRGB666*>(mood_calculator);
        
        // Convert RGB888 base color to RGB666
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB666 base_pixel(r8, g8, b8);
        
        // Set mood
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        // Apply mood and render to buffer
        Pixel_RGB666 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Fill entire frame with mood-modified color (3 bytes per pixel)
        for (uint32_t i = 0; i < config.total_pixels; ++i) {
            uint32_t offset = i * 3;
            frame_buffer[offset] = mood_pixel.r;
            frame_buffer[offset + 1] = mood_pixel.g;
            frame_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    /**
     * @brief Render RGB888 frame with mood modifications
     */
    void renderFrameRGB888(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB888* calc = static_cast<MoodCalcRGB888*>(mood_calculator);
        
        // RGB888 is native format
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB888 base_pixel(r8, g8, b8);
        
        // Set mood
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        // Apply mood and render to buffer
        Pixel_RGB888 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Fill entire frame with mood-modified color (3 bytes per pixel)
        for (uint32_t i = 0; i < config.total_pixels; ++i) {
            uint32_t offset = i * 3;
            frame_buffer[offset] = mood_pixel.r;
            frame_buffer[offset + 1] = mood_pixel.g;
            frame_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    /**
     * @brief Convert from Mood struct to MoodState enum
     */
    MoodState getMoodState(const Mood& mood) const {
        // Assuming Mood struct has a 'state' or similar field
        // This is a placeholder - adjust based on actual Mood.hpp
        return NEUTRAL;  // TODO: Map actual mood field
    }
};
