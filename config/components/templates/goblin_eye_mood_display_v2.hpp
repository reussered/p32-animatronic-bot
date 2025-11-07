/**
 * @file goblin_eye_mood_display_v2.hpp
 * @brief Streaming eye display with mood-based color rendering
 * 
 * ARCHITECTURE:
 * - Display hardware interface (gc9a01, st7789, etc) specified in component chain
 * - Single frame buffer: malloc at init, reuse each frame, free at cleanup
 * - Color schema determines pixel format only (RGB565/666/888)
 * - Mood modifications applied during render
 * - Frame sent to display driver, buffer reused next frame
 * 
 * Configuration:
 * {
 *   "name": "goblin_left_eye",
 *   "hardware_instance": "gc9a01_left_eye",  // Display chip instance
 *   "resolution": "240x240",
 *   "color_schema": "RGB565",                 // Only used to determine bytes/pixel
 *   "mood_enabled": true
 * }
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "config/components/templates/mood_calculator_template.hpp"
#include "shared/Mood.hpp"

enum ColorSchema {
    RGB565_FORMAT = 0,  // 2 bytes per pixel (GC9A01, ST7789, ST7735)
    RGB666_FORMAT = 1,  // 3 bytes per pixel (ILI9341, RA8875)
    RGB888_FORMAT = 2,  // 3 bytes per pixel (high-end)
};

class GoblinEyeMoodDisplay {
public:
    // Callback: display driver sends frame to hardware
    typedef void (*SendFrameCallback)(const uint8_t* buffer, uint32_t size);

private:
    // Display configuration
    uint16_t width;
    uint16_t height;
    ColorSchema color_format;
    uint16_t bytes_per_pixel;
    uint32_t total_pixels;
    uint32_t buffer_size_bytes;
    
    // Single frame buffer (malloc once, reuse forever)
    uint8_t* frame_buffer;
    bool initialized;
    
    // Mood calculator (template-instantiated per color format)
    void* mood_calculator;
    
    // Display driver callback
    SendFrameCallback send_frame_cb;
    
    // Eye state
    struct {
        uint16_t pupil_x;
        uint16_t pupil_y;
        uint8_t pupil_size;
        uint8_t eyelid_openness;
        bool blink_active;
    } eye_state;
    
public:
    GoblinEyeMoodDisplay()
        : frame_buffer(nullptr), initialized(false), mood_calculator(nullptr),
          send_frame_cb(nullptr)
    {
        memset(&eye_state, 0, sizeof(eye_state));
        eye_state.pupil_size = 20;
        eye_state.eyelid_openness = 255;
    }
    
    /**
     * Initialize: malloc buffer once
     * @param w Display width
     * @param h Display height
     * @param schema Color format (determines bytes/pixel only)
     * @param callback Function to send frame to display hardware
     */
    bool init(uint16_t w, uint16_t h, ColorSchema schema, SendFrameCallback callback) {
        if (initialized) return false;
        if (!callback) return false;
        
        width = w;
        height = h;
        color_format = schema;
        send_frame_cb = callback;
        total_pixels = width * height;
        
        // Determine bytes per pixel from color schema
        switch (schema) {
            case RGB565_FORMAT:
                bytes_per_pixel = 2;
                mood_calculator = new MoodCalcRGB565();
                break;
            case RGB666_FORMAT:
                bytes_per_pixel = 3;
                mood_calculator = new MoodCalcRGB666();
                break;
            case RGB888_FORMAT:
                bytes_per_pixel = 3;
                mood_calculator = new MoodCalcRGB888();
                break;
            default:
                return false;
        }
        
        buffer_size_bytes = total_pixels * bytes_per_pixel;
        
        // Malloc once - this buffer is reused every frame
        frame_buffer = (uint8_t*)malloc(buffer_size_bytes);
        if (!frame_buffer) {
            return false;
        }
        
        memset(frame_buffer, 0, buffer_size_bytes);
        initialized = true;
        
        return true;
    }
    
    /**
     * Render frame: fill buffer with mood-modified color
     * @param mood Current emotional state
     * @param base_color Base color before mood modification
     */
    void renderFrame(const Mood& mood, uint32_t base_color_rgb888) {
        if (!initialized) return;
        
        switch (color_format) {
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
     * Send rendered frame to display hardware
     * Then buffer is immediately available for next frame
     */
    void sendToDisplay() {
        if (initialized && send_frame_cb) {
            send_frame_cb(frame_buffer, buffer_size_bytes);
        }
    }
    
    // Animation control
    void setPupilPosition(uint16_t x, uint16_t y) {
        eye_state.pupil_x = (x < width) ? x : width - 1;
        eye_state.pupil_y = (y < height) ? y : height - 1;
    }
    
    void setEyelidOpenness(uint8_t openness) {
        eye_state.eyelid_openness = openness;
    }
    
    void blink() {
        eye_state.blink_active = true;
    }
    
    // Accessors
    const uint8_t* getBuffer() const { return frame_buffer; }
    uint32_t getBufferSize() const { return buffer_size_bytes; }
    uint16_t getWidth() const { return width; }
    uint16_t getHeight() const { return height; }
    uint32_t getTotalPixels() const { return total_pixels; }
    
    /**
     * Cleanup: free buffer
     */
    ~GoblinEyeMoodDisplay() {
        if (frame_buffer) {
            free(frame_buffer);
            frame_buffer = nullptr;
        }
        if (mood_calculator) {
            switch (color_format) {
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
        initialized = false;
    }

private:
    void renderFrameRGB565(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB565* calc = static_cast<MoodCalcRGB565*>(mood_calculator);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB565 base_pixel(r8, g8, b8);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        Pixel_RGB565 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        uint16_t* buffer_16 = reinterpret_cast<uint16_t*>(frame_buffer);
        uint16_t pixel_value = (mood_pixel.red << 11) | (mood_pixel.green << 5) | mood_pixel.blue;
        
        for (uint32_t i = 0; i < total_pixels; ++i) {
            buffer_16[i] = pixel_value;
        }
    }
    
    void renderFrameRGB666(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB666* calc = static_cast<MoodCalcRGB666*>(mood_calculator);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB666 base_pixel(r8, g8, b8);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        Pixel_RGB666 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        for (uint32_t i = 0; i < total_pixels; ++i) {
            uint32_t offset = i * 3;
            frame_buffer[offset] = mood_pixel.r;
            frame_buffer[offset + 1] = mood_pixel.g;
            frame_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    void renderFrameRGB888(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB888* calc = static_cast<MoodCalcRGB888*>(mood_calculator);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB888 base_pixel(r8, g8, b8);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        Pixel_RGB888 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        for (uint32_t i = 0; i < total_pixels; ++i) {
            uint32_t offset = i * 3;
            frame_buffer[offset] = mood_pixel.r;
            frame_buffer[offset + 1] = mood_pixel.g;
            frame_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    MoodState getMoodState(const Mood& mood) const {
        return NEUTRAL;  // TODO: map from Mood struct
    }
};
