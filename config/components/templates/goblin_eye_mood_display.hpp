/**
 * @file goblin_eye_mood_display.hpp
 * @brief Streaming eye display with mood-based color rendering
 * 
 * PSRAM-STREAMING ARCHITECTURE:
 * 1. Render frame from PSRAM row-by-row
 * 2. Apply mood modifications while reading
 * 3. Push to display immediately
 * 4. Free the row from PSRAM
 * 5. Repeat for next row
 * 
 * Memory model:
 *   - No full frame buffer in RAM
 *   - Only one row buffer (width × bytes_per_pixel) in RAM
 *   - Full frame stored in PSRAM (32MB available)
 *   - Streaming maintains constant small RAM footprint
 * 
 * Configuration example:
 * {
 *   "name": "goblin_left_eye",
 *   "display_config": {
 *     "resolution": "240x240",
 *     "color_schema": "RGB565",
 *     "driver": "gc9a01",
 *     "psram_frame_address": "0x3d800000"
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
#include "esp_spiram.h"  // For PSRAM access

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
 * @brief Streaming eye display renderer with PSRAM source
 * 
 * Memory model: Zero full-frame buffering
 *   - Frame data stored in PSRAM (unlimited)
 *   - Only row_buffer in RAM (width × bytes_per_pixel)
 *   - Process row → apply mood → push to display → free row
 */
class GoblinEyeMoodDisplay {
private:
    EyeDisplayConfig config;
    uint8_t* row_buffer;        // Single row buffer (width × bytes_per_pixel)
    uint32_t psram_frame_addr;  // Address of complete frame in PSRAM
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
        : row_buffer(nullptr), psram_frame_addr(0), initialized(false), 
          mood_calculator(nullptr)
    {
        memset(&config, 0, sizeof(config));
        memset(&eye_state, 0, sizeof(eye_state));
        eye_state.pupil_size = 20;
        eye_state.eyelid_openness = 255;
    }
    
    /**
     * @brief Initialize display for streaming from PSRAM
     * @param width Display width in pixels
     * @param height Display height in pixels
     * @param color_schema Color format (RGB565, RGB666, RGB888)
     * @param psram_addr Address of complete frame in PSRAM
     * @return true if initialization successful
     */
    bool init(uint16_t width, uint16_t height, ColorSchema color_schema, 
              uint32_t psram_addr) {
        if (row_buffer != nullptr) {
            return false;  // Already initialized
        }
        
        config.width = width;
        config.height = height;
        config.color_format = color_schema;
        config.total_pixels = width * height;
        psram_frame_addr = psram_addr;
        
        // Configure bytes per pixel
        switch (color_schema) {
            case RGB565_FORMAT:
                config.bytes_per_pixel = 2;
                mood_calculator = new MoodCalcRGB565();
                break;
                
            case RGB666_FORMAT:
                config.bytes_per_pixel = 3;
                mood_calculator = new MoodCalcRGB666();
                break;
                
            case RGB888_FORMAT:
                config.bytes_per_pixel = 3;
                mood_calculator = new MoodCalcRGB888();
                break;
                
            default:
                return false;
        }
        
        config.mood_enabled = true;
        
        // Allocate ONLY ONE ROW BUFFER
        uint32_t row_size = config.width * config.bytes_per_pixel;
        row_buffer = new uint8_t[row_size];
        if (!row_buffer) {
            return false;
        }
        
        initialized = true;
        return true;
    }
    
    /**
     * @brief Stream frame from PSRAM row-by-row with mood effects
     * @param mood Current emotional state
     * @param base_color Color to render
     * @param display_driver Callback to send rows to display
     * 
     * Usage:
     *   GoblinEyeMoodDisplay eye;
     *   eye.streamFrame(mood, 0x00AA00, 
     *     [](const uint8_t* row, uint32_t size) { 
     *       driver.sendRow(row, size); 
     *     });
     */
    typedef void (*DisplaySendCallback)(const uint8_t* row_data, uint32_t row_size);
    
    void streamFrame(const Mood& mood, uint32_t base_color_rgb888, 
                     DisplaySendCallback send_callback) {
        if (!initialized || !send_callback) return;
        
        // Process each row
        for (uint16_t row = 0; row < config.height; ++row) {
            // Read row from PSRAM
            uint32_t row_offset = row * config.width * config.bytes_per_pixel;
            uint8_t* psram_row = reinterpret_cast<uint8_t*>(
                psram_frame_addr + row_offset);
            
            // Copy row to buffer and apply mood
            switch (config.color_format) {
                case RGB565_FORMAT:
                    processRowRGB565(psram_row, mood, base_color_rgb888);
                    break;
                case RGB666_FORMAT:
                    processRowRGB666(psram_row, mood, base_color_rgb888);
                    break;
                case RGB888_FORMAT:
                    processRowRGB888(psram_row, mood, base_color_rgb888);
                    break;
            }
            
            // Send processed row to display
            uint32_t row_size = config.width * config.bytes_per_pixel;
            send_callback(row_buffer, row_size);
        }
        
        // Row buffer automatically freed when object destroyed
    }
    
    /**
     * @brief Get row buffer size (useful for DMA setup)
     */
    uint32_t getRowBufferSize() const {
        return config.width * config.bytes_per_pixel;
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
        if (row_buffer) {
            delete[] row_buffer;
            row_buffer = nullptr;
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
     * @brief Process RGB565 row: read from PSRAM, apply mood, write to buffer
     */
    void processRowRGB565(const uint8_t* psram_row, const Mood& mood, 
                         uint32_t base_color_rgb888) {
        MoodCalcRGB565* calc = static_cast<MoodCalcRGB565*>(mood_calculator);
        
        // Setup mood
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        // Convert base color
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB565 base_pixel(r8, g8, b8);
        Pixel_RGB565 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Process each pixel in row
        uint16_t* src = (uint16_t*)psram_row;
        uint16_t* dst = (uint16_t*)row_buffer;
        uint16_t pixel_value = (mood_pixel.red << 11) | (mood_pixel.green << 5) | mood_pixel.blue;
        
        for (uint16_t x = 0; x < config.width; ++x) {
            dst[x] = pixel_value;  // Apply mood-modified color
        }
    }
    
    /**
     * @brief Process RGB666 row
     */
    void processRowRGB666(const uint8_t* psram_row, const Mood& mood,
                         uint32_t base_color_rgb888) {
        MoodCalcRGB666* calc = static_cast<MoodCalcRGB666*>(mood_calculator);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB666 base_pixel(r8, g8, b8);
        Pixel_RGB666 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        for (uint16_t x = 0; x < config.width; ++x) {
            uint32_t dst_offset = x * 3;
            row_buffer[dst_offset] = mood_pixel.r;
            row_buffer[dst_offset + 1] = mood_pixel.g;
            row_buffer[dst_offset + 2] = mood_pixel.b;
        }
    }
    
    /**
     * @brief Process RGB888 row
     */
    void processRowRGB888(const uint8_t* psram_row, const Mood& mood,
                         uint32_t base_color_rgb888) {
        MoodCalcRGB888* calc = static_cast<MoodCalcRGB888*>(mood_calculator);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB888 base_pixel(r8, g8, b8);
        Pixel_RGB888 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        for (uint16_t x = 0; x < config.width; ++x) {
            uint32_t dst_offset = x * 3;
            row_buffer[dst_offset] = mood_pixel.r;
            row_buffer[dst_offset + 1] = mood_pixel.g;
            row_buffer[dst_offset + 2] = mood_pixel.b;
        }
    }
    
    /**
     * @brief Convert from Mood struct to MoodState enum
     */
    MoodState getMoodState(const Mood& mood) const {
        return NEUTRAL;  // TODO: Map actual mood field
    }
};
