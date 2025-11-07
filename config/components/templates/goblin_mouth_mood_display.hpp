/**
 * @file goblin_mouth_mood_display.hpp
 * @brief Mouth display with chunking support for large high-resolution displays
 * 
 * Supports rendering large displays (ILI9341 800x480, RA8875) by:
 * 1. Dividing the display into horizontal chunks
 * 2. Rendering one chunk at a time into a single buffer
 * 3. Sending chunks to driver sequentially
 * 
 * Memory-efficient: Only ONE chunk buffer in RAM at a time.
 * Trade-off: Slower refresh rate for large displays (e.g., 15 FPS vs 60 FPS)
 * 
 * Configuration:
 * {
 *   "name": "goblin_mouth",
 *   "display_config": {
 *     "resolution": "480x320",
 *     "color_schema": "RGB666",
 *     "driver": "ili9341"
 *   },
 *   "chunking": {
 *     "enabled": true,
 *     "chunk_height": 80,
 *     "sequential_render": true
 *   },
 *   "mood_enabled": true
 * }
 */

#pragma once

#include <cstdint>
#include <cstring>
#include "config/components/templates/mood_calculator_template.hpp"
#include "shared/Mood.hpp"

/**
 * @struct MouthDisplayConfig
 * @brief Runtime mouth display configuration
 */
struct MouthDisplayConfig {
    uint16_t width;
    uint16_t height;
    ColorSchema color_format;
    uint16_t bytes_per_pixel;
    uint32_t total_pixels;
    uint32_t buffer_size_bytes;
    
    // Chunking parameters
    bool chunking_enabled;
    uint16_t chunk_height;
    uint16_t total_chunks;
    uint32_t chunk_size_bytes;
    uint32_t chunk_pixels;
    
    bool mood_enabled;
};

/**
 * @class GoblinMouthMoodDisplay
 * @brief Large mouth display with mood-based color rendering and chunking
 * 
 * Supports displays up to 1000x1000+ pixels by chunking into manageable pieces.
 */
class GoblinMouthMoodDisplay {
private:
    MouthDisplayConfig config;
    uint8_t* chunk_buffer;      // One chunk at a time
    bool initialized;
    
    // Generic mood calculator wrapper
    void* mood_calculator;
    
    // Animation state
    struct {
        uint16_t mouth_open;     // 0-255
        uint16_t tongue_position;
        bool smile_active;
        uint8_t expression;      // 0-255 (various expressions)
    } mouth_state;
    
    uint16_t current_chunk_index;  // For sequential rendering
    
public:
    GoblinMouthMoodDisplay()
        : chunk_buffer(nullptr), initialized(false), mood_calculator(nullptr),
          current_chunk_index(0)
    {
        memset(&config, 0, sizeof(config));
        memset(&mouth_state, 0, sizeof(mouth_state));
        mouth_state.mouth_open = 128;  // Half-open by default
    }
    
    /**
     * @brief Initialize display with auto-detected color format and chunking
     * @param width Display width in pixels
     * @param height Display height in pixels
     * @param color_schema Color format (RGB565, RGB666, RGB888)
     * @param enable_chunking Whether to enable chunked rendering (for large displays)
     * @param chunk_height Height of each chunk in pixels (e.g., 80)
     * @return true if initialization successful
     */
    bool init(uint16_t width, uint16_t height, ColorSchema color_schema,
              bool enable_chunking = true, uint16_t chunk_height = 80)
    {
        if (chunk_buffer != nullptr) {
            return false;  // Already initialized
        }
        
        config.width = width;
        config.height = height;
        config.color_format = color_schema;
        config.total_pixels = width * height;
        config.chunking_enabled = enable_chunking;
        config.chunk_height = chunk_height;
        config.mood_enabled = true;
        
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
        
        // Calculate chunking parameters
        if (config.chunking_enabled) {
            config.chunk_height = (chunk_height > config.height) ? 
                                  config.height : chunk_height;
            config.total_chunks = (config.height + config.chunk_height - 1) / 
                                  config.chunk_height;
            config.chunk_pixels = config.width * config.chunk_height;
            config.chunk_size_bytes = config.chunk_pixels * config.bytes_per_pixel;
        } else {
            config.total_chunks = 1;
            config.chunk_pixels = config.total_pixels;
            config.chunk_size_bytes = config.total_pixels * config.bytes_per_pixel;
            config.chunk_height = config.height;
        }
        
        config.buffer_size_bytes = config.chunk_size_bytes;
        
        // Allocate SINGLE chunk buffer (not entire frame!)
        chunk_buffer = new uint8_t[config.chunk_size_bytes];
        if (!chunk_buffer) {
            return false;
        }
        
        memset(chunk_buffer, 0, config.chunk_size_bytes);
        initialized = true;
        current_chunk_index = 0;
        
        return true;
    }
    
    /**
     * @brief Render next chunk and return pointer to it
     * @param mood Current emotional state
     * @param base_color Base color before mood modification
     * @return Pointer to chunk buffer, or nullptr if all chunks rendered
     * 
     * Usage:
     *   const uint8_t* chunk_ptr = mouth.renderNextChunk(mood, 0xFF0000);
     *   while (chunk_ptr) {
     *     driver.sendChunk(chunk_ptr, mouth.getChunkSize());
     *     chunk_ptr = mouth.renderNextChunk(mood, 0xFF0000);
     *   }
     */
    const uint8_t* renderNextChunk(const Mood& mood, uint32_t base_color_rgb888) {
        if (!initialized || current_chunk_index >= config.total_chunks) {
            current_chunk_index = 0;
            return nullptr;
        }
        
        // Dispatch to appropriate color-specific renderer
        switch (config.color_format) {
            case RGB565_FORMAT:
                renderChunkRGB565(mood, base_color_rgb888, current_chunk_index);
                break;
            case RGB666_FORMAT:
                renderChunkRGB666(mood, base_color_rgb888, current_chunk_index);
                break;
            case RGB888_FORMAT:
                renderChunkRGB888(mood, base_color_rgb888, current_chunk_index);
                break;
        }
        
        current_chunk_index++;
        return chunk_buffer;
    }
    
    /**
     * @brief Reset chunk index for next frame (call after all chunks sent)
     */
    void resetChunkIndex() {
        current_chunk_index = 0;
    }
    
    /**
     * @brief Set mouth open amount (0=closed, 255=fully open)
     */
    void setMouthOpen(uint8_t openness) {
        mouth_state.mouth_open = openness;
    }
    
    /**
     * @brief Set mouth expression (0=neutral, 255=extreme)
     */
    void setExpression(uint8_t expression) {
        mouth_state.expression = expression;
    }
    
    /**
     * @brief Enable/disable smile
     */
    void setSmile(bool active) {
        mouth_state.smile_active = active;
    }
    
    /**
     * @brief Get current chunk buffer
     */
    const uint8_t* getChunkBuffer() const {
        return chunk_buffer;
    }
    
    /**
     * @brief Get current chunk size in bytes
     */
    uint32_t getChunkSize() const {
        return config.chunk_size_bytes;
    }
    
    /**
     * @brief Get number of chunks to render per frame
     */
    uint16_t getTotalChunks() const {
        return config.total_chunks;
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
     * @brief Get current chunk index
     */
    uint16_t getCurrentChunkIndex() const {
        return current_chunk_index;
    }
    
    /**
     * @brief Cleanup
     */
    ~GoblinMouthMoodDisplay() {
        if (chunk_buffer) {
            delete[] chunk_buffer;
            chunk_buffer = nullptr;
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
     * @brief Render single chunk in RGB565 format
     */
    void renderChunkRGB565(const Mood& mood, uint32_t base_color_rgb888, 
                          uint16_t chunk_index) {
        MoodCalcRGB565* calc = static_cast<MoodCalcRGB565*>(mood_calculator);
        
        // Convert RGB888 base color to RGB565
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB565 base_pixel(r8, g8, b8);
        
        // Set mood
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        // Apply mood
        Pixel_RGB565 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Fill chunk buffer
        uint16_t* buffer_16 = reinterpret_cast<uint16_t*>(chunk_buffer);
        uint16_t pixel_value = (mood_pixel.red << 11) | (mood_pixel.green << 5) | 
                              mood_pixel.blue;
        
        for (uint32_t i = 0; i < config.chunk_pixels; ++i) {
            buffer_16[i] = pixel_value;
        }
    }
    
    /**
     * @brief Render single chunk in RGB666 format
     */
    void renderChunkRGB666(const Mood& mood, uint32_t base_color_rgb888, 
                          uint16_t chunk_index) {
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
        
        // Apply mood
        Pixel_RGB666 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Fill chunk buffer
        for (uint32_t i = 0; i < config.chunk_pixels; ++i) {
            uint32_t offset = i * 3;
            chunk_buffer[offset] = mood_pixel.r;
            chunk_buffer[offset + 1] = mood_pixel.g;
            chunk_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    /**
     * @brief Render single chunk in RGB888 format
     */
    void renderChunkRGB888(const Mood& mood, uint32_t base_color_rgb888, 
                          uint16_t chunk_index) {
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
        
        // Apply mood
        Pixel_RGB888 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        // Fill chunk buffer
        for (uint32_t i = 0; i < config.chunk_pixels; ++i) {
            uint32_t offset = i * 3;
            chunk_buffer[offset] = mood_pixel.r;
            chunk_buffer[offset + 1] = mood_pixel.g;
            chunk_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    /**
     * @brief Convert from Mood struct to MoodState enum
     */
    MoodState getMoodState(const Mood& mood) const {
        return NEUTRAL;  // TODO: Map actual mood field from Mood.hpp
    }
};
