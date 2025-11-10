/**
 * @file goblin_mouth_mood_display_v2.hpp
 * @brief Streaming mouth display with chunking and mood effects
 * 
 * ARCHITECTURE:
 * - Display hardware interface (ili9341, ra8875, etc) specified in component chain
 * - Single chunk buffer: malloc at init, reuse for each chunk, free at cleanup
 * - Color schema determines pixel format only (RGB565/666/888)
 * - Large displays divided into horizontal chunks for memory efficiency
 * - Each chunk: render  send to display  buffer ready for next chunk
 * 
 * Configuration:
 * {
 *   "name": "goblin_mouth",
 *   "hardware_instance": "ili9341_mouth",    // Display chip instance
 *   "resolution": "480x320",
 *   "color_schema": "RGB666",                 // Only used for bytes/pixel
 *   "chunking": {
 *     "enabled": true,
 *     "chunk_height": 80
 *   },
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
    RGB565_FORMAT = 0,  // 2 bytes per pixel
    RGB666_FORMAT = 1,  // 3 bytes per pixel
    RGB888_FORMAT = 2,  // 3 bytes per pixel
};

class GoblinMouthMoodDisplay {
public:
    // Callback: display driver sends chunk to hardware
    typedef void (*SendChunkCallback)(const uint8_t* buffer, uint32_t size, 
                                     uint16_t start_y, uint16_t height);

private:
    // Display configuration
    uint16_t width;
    uint16_t height;
    ColorSchema color_format;
    uint16_t bytes_per_pixel;
    
    // Chunking parameters
    bool chunking_enabled;
    uint16_t chunk_height;
    uint16_t total_chunks;
    uint32_t chunk_pixels;
    uint32_t chunk_size_bytes;
    
    // Single chunk buffer (malloc once, reuse forever)
    uint8_t* chunk_buffer;
    bool initialized;
    
    // Mood calculator (template-instantiated per color format)
    void* mood_calculator;
    
    // Display driver callback
    SendChunkCallback send_chunk_cb;
    
    // Rendering state
    uint16_t current_chunk_index;
    
    // Mouth state
    struct {
        uint8_t mouth_open;
        uint8_t expression;
        bool smile_active;
    } mouth_state;
    
public:
    GoblinMouthMoodDisplay()
        : chunk_buffer(nullptr), initialized(false), mood_calculator(nullptr),
          send_chunk_cb(nullptr), current_chunk_index(0)
    {
        memset(&mouth_state, 0, sizeof(mouth_state));
        mouth_state.mouth_open = 128;
    }
    
    /**
     * Initialize: malloc chunk buffer once
     * @param w Display width
     * @param h Display height
     * @param schema Color format (determines bytes/pixel only)
     * @param enable_chunking Whether to use chunking
     * @param chunk_h Height of each chunk (80 typical)
     * @param callback Function to send chunks to display hardware
     */
    bool init(uint16_t w, uint16_t h, ColorSchema schema,
              bool enable_chunking, uint16_t chunk_h, SendChunkCallback callback)
    {
        if (initialized) return false;
        if (!callback) return false;
        
        width = w;
        height = h;
        color_format = schema;
        send_chunk_cb = callback;
        chunking_enabled = enable_chunking;
        
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
        
        // Calculate chunking
        if (chunking_enabled) {
            chunk_height = (chunk_h > height) ? height : chunk_h;
            total_chunks = (height + chunk_height - 1) / chunk_height;
            chunk_pixels = width * chunk_height;
            chunk_size_bytes = chunk_pixels * bytes_per_pixel;
        } else {
            chunk_height = height;
            total_chunks = 1;
            chunk_pixels = width * height;
            chunk_size_bytes = chunk_pixels * bytes_per_pixel;
        }
        
        // Malloc once - this buffer is reused for each chunk
        chunk_buffer = (uint8_t*)malloc(chunk_size_bytes);
        if (!chunk_buffer) {
            return false;
        }
        
        memset(chunk_buffer, 0, chunk_size_bytes);
        initialized = true;
        current_chunk_index = 0;
        
        return true;
    }
    
    /**
     * Render next chunk: fill buffer with mood-modified color
     * @param mood Current emotional state
     * @param base_color Base color before mood modification
     * @return true if chunk rendered, false if all chunks done
     */
    bool renderNextChunk(const Mood& mood, uint32_t base_color_rgb888) {
        if (!initialized || current_chunk_index >= total_chunks) {
            return false;
        }
        
        switch (color_format) {
            case RGB565_FORMAT:
                renderChunkRGB565(mood, base_color_rgb888);
                break;
            case RGB666_FORMAT:
                renderChunkRGB666(mood, base_color_rgb888);
                break;
            case RGB888_FORMAT:
                renderChunkRGB888(mood, base_color_rgb888);
                break;
        }
        
        return true;
    }
    
    /**
     * Send current chunk to display hardware
     * Then buffer is immediately available for next chunk
     */
    void sendChunkToDisplay() {
        if (initialized && send_chunk_cb && current_chunk_index < total_chunks) {
            uint16_t start_y = current_chunk_index * chunk_height;
            send_chunk_cb(chunk_buffer, chunk_size_bytes, start_y, chunk_height);
            current_chunk_index++;
        }
    }
    
    /**
     * Reset for next frame cycle
     */
    void resetChunkIndex() {
        current_chunk_index = 0;
    }
    
    // Animation control
    void setMouthOpen(uint8_t openness) {
        mouth_state.mouth_open = openness;
    }
    
    void setExpression(uint8_t expr) {
        mouth_state.expression = expr;
    }
    
    void setSmile(bool active) {
        mouth_state.smile_active = active;
    }
    
    // Accessors
    const uint8_t* getChunkBuffer() const { return chunk_buffer; }
    uint32_t getChunkSize() const { return chunk_size_bytes; }
    uint16_t getTotalChunks() const { return total_chunks; }
    uint16_t getChunkHeight() const { return chunk_height; }
    uint16_t getWidth() const { return width; }
    uint16_t getHeight() const { return height; }
    uint16_t getCurrentChunkIndex() const { return current_chunk_index; }
    
    /**
     * Cleanup: free chunk buffer
     */
    ~GoblinMouthMoodDisplay() {
        if (chunk_buffer) {
            free(chunk_buffer);
            chunk_buffer = nullptr;
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
    void renderChunkRGB565(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB565* calc = static_cast<MoodCalcRGB565*>(mood_calculator);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB565 base_pixel(r8, g8, b8);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        Pixel_RGB565 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        uint16_t* buffer_16 = reinterpret_cast<uint16_t*>(chunk_buffer);
        uint16_t pixel_value = (mood_pixel.red << 11) | (mood_pixel.green << 5) | mood_pixel.blue;
        
        for (uint32_t i = 0; i < chunk_pixels; ++i) {
            buffer_16[i] = pixel_value;
        }
    }
    
    void renderChunkRGB666(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB666* calc = static_cast<MoodCalcRGB666*>(mood_calculator);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB666 base_pixel(r8, g8, b8);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        Pixel_RGB666 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        for (uint32_t i = 0; i < chunk_pixels; ++i) {
            uint32_t offset = i * 3;
            chunk_buffer[offset] = mood_pixel.r;
            chunk_buffer[offset + 1] = mood_pixel.g;
            chunk_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    void renderChunkRGB888(const Mood& mood, uint32_t base_color_rgb888) {
        MoodCalcRGB888* calc = static_cast<MoodCalcRGB888*>(mood_calculator);
        
        uint8_t r8 = (base_color_rgb888 >> 16) & 0xFF;
        uint8_t g8 = (base_color_rgb888 >> 8) & 0xFF;
        uint8_t b8 = base_color_rgb888 & 0xFF;
        Pixel_RGB888 base_pixel(r8, g8, b8);
        
        MoodState mood_state = getMoodState(mood);
        float intensity = static_cast<float>(mood.intensity) / 255.0f;
        calc->setMood(mood_state, intensity);
        
        Pixel_RGB888 mood_pixel = calc->applyMoodDelta(base_pixel);
        
        for (uint32_t i = 0; i < chunk_pixels; ++i) {
            uint32_t offset = i * 3;
            chunk_buffer[offset] = mood_pixel.r;
            chunk_buffer[offset + 1] = mood_pixel.g;
            chunk_buffer[offset + 2] = mood_pixel.b;
        }
    }
    
    MoodState getMoodState(const Mood& mood) const {
        return NEUTRAL;  // TODO: map from Mood struct
    }
};
