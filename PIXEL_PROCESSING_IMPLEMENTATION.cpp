// P32 Animatronic Bot - Complete Pixel Processing Implementation
// Found in repository: FrameProcessor.hpp/cpp and FrameMoodProcessor.hpp
// This is the actual "black magic" code that processes pixels with mood

#include "Mood.hpp"
#include "core/memory/SharedMemory.hpp"

// The core pixel processing loop - implements your mood-based frame optimization
void processPixelsWithMood(uint8_t* currentFrame, uint32_t frame_size) {
    // Get current global mood from shared memory
    SharedMemory gsm;
    Mood currentGlobalMood = gsm.read<Mood>();
    
    // Check if mood changed (your optimization!)
    static Mood oldMood;
    if (oldMood != currentGlobalMood) {
        // Step 1: Calculate TOTAL color delta (once per frame)
        MoodColorDelta totalDelta = calculateTotalMoodDelta(oldMood, currentGlobalMood);
        
        // Step 2: Apply same delta to ALL pixels (the "black magic" loop)
        for (uint32_t pixel = 0; pixel < frame_size; pixel++) {
            // Convert pixel from palette index to RGB565
            RGB565Pixel pixelValue = goblin_eye_palette[currentFrame[pixel]];
            
            // Apply mood color delta with clamping
            pixelValue.applyColorDelta(totalDelta);
            
            // Write modified pixel back to frame buffer
            ((uint16_t*)currentFrame)[pixel] = pixelValue.value;
        }
        
        // Step 3: Remember new mood
        oldMood = currentGlobalMood;
        
        ESP_LOGV("GOBLIN_EYE", "Frame updated with mood delta R:%+d G:%+d B:%+d at loop %u", 
                 (int)totalDelta.red_delta, (int)totalDelta.green_delta, 
                 (int)totalDelta.blue_delta, g_loopCount);
    }
    // If mood hasn't changed, do nothing - use cached colors!
}

// Calculate total color change for all mood component changes
MoodColorDelta calculateTotalMoodDelta(const Mood& oldMood, const Mood& newMood) {
    MoodColorDelta totalDelta;
    
    for(int i = 0; i < Mood::componentCount; ++i) {
        int8_t moodDelta = newMood.components[i] - oldMood.components[i];
        if(moodDelta != 0) {
            const MoodColorEffect& effect = moodColorEffects[i];
            
            // Calculate color contribution from this mood change
            MoodColorDelta componentDelta(
                static_cast<int8_t>(moodDelta * effect.red_multiplier),
                static_cast<int8_t>(moodDelta * effect.green_multiplier),  
                static_cast<int8_t>(moodDelta * effect.blue_multiplier)
            );
            
            // Accumulate with overflow protection
            totalDelta += componentDelta;
        }
    }
    
    return totalDelta;
}

// RGB565 pixel processing with mood delta application
struct RGB565Pixel {
    uint16_t value;
    
    RGB565Pixel() : value(0) {}
    RGB565Pixel(uint16_t v) : value(v) {}
    RGB565Pixel(uint8_t r, uint8_t g, uint8_t b) {
        value = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    
    // Extract RGB components (0-255 range)
    uint8_t red() const { return (value >> 8) & 0xF8; }
    uint8_t green() const { return (value >> 3) & 0xFC; }  
    uint8_t blue() const { return (value << 3) & 0xF8; }
    
    // Apply mood color delta with clamping - THE BLACK MAGIC!
    void applyColorDelta(const MoodColorDelta& delta) {
        int16_t r = red() + delta.red_delta;
        int16_t g = green() + delta.green_delta;
        int16_t b = blue() + delta.blue_delta;
        
        // Clamp to valid RGB ranges
        r = (r < 0) ? 0 : (r > 255) ? 255 : r;
        g = (g < 0) ? 0 : (g > 255) ? 255 : g;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;
        
        value = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
};

// Mood color effects (from Mood.cpp)
const MoodColorEffect moodColorEffects[Mood::componentCount] = {
    // ANGER - Red tones, reduces green/blue
    MoodColorEffect(0.8f, -0.3f, -0.2f),
    
    // FEAR - Desaturates, adds blue-gray tones
    MoodColorEffect(-0.2f, -0.2f, 0.4f),
    
    // HAPPINESS - Bright yellows/oranges, enhances warm colors
    MoodColorEffect(0.4f, 0.6f, 0.1f),
    
    // SADNESS - Blue tones, reduces brightness
    MoodColorEffect(-0.3f, -0.4f, 0.3f),
    
    // CURIOSITY - Green/cyan tones, enhances brightness
    MoodColorEffect(0.1f, 0.5f, 0.3f),
    
    // AFFECTION - Pink/warm tones, soft colors
    MoodColorEffect(0.5f, 0.2f, 0.4f),
    
    // IRRITATION - Sharp reds/oranges, similar to anger but more orange
    MoodColorEffect(0.7f, 0.3f, -0.4f),
    
    // CONTENTMENT - Soft greens, balanced, slight warmth
    MoodColorEffect(0.1f, 0.4f, 0.1f),
    
    // EXCITEMENT - Electric cyan/bright colors, high energy
    MoodColorEffect(0.3f, 0.7f, 0.9f)
};

// Alternative implementation using animation frames with caching
class AnimationFrame {
private:
    Mood oldMood;                    // Last mood this frame was rendered with
    RGB565Pixel* pixelBuffer;        // Frame pixel data
    const uint8_t* sourcePixels;    // Original indexed pixel data (ROM/Flash)
    uint32_t pixelCount;            // Number of pixels
    uint16_t width, height;         // Frame dimensions
    bool isDirty;                   // True if needs recalculation
    
public:
    AnimationFrame(const uint8_t* source_data, uint16_t w, uint16_t h) 
        : sourcePixels(source_data), pixelCount(w * h), width(w), height(h), isDirty(true) {
        // Allocate RGB565 buffer for rendered pixels
        pixelBuffer = new RGB565Pixel[pixelCount];
    }
    
    ~AnimationFrame() {
        delete[] pixelBuffer;
    }
    
    // Core optimization: Your if(oldMood != currentGlobalMood) logic
    void renderWithMood(const Mood& currentGlobalMood) {
        if (oldMood != currentGlobalMood) {
            // Mood changed - recalculate frame colors
            
            // Step 1: Calculate TOTAL color delta (once per frame)
            MoodColorDelta totalDelta = calculateTotalMoodDelta(oldMood, currentGlobalMood);
            
            // Step 2: Apply same delta to ALL pixels (fast loop)
            applyDeltaToAllPixels(totalDelta);
            
            // Step 3: Remember new mood
            oldMood = currentGlobalMood;
            isDirty = false;
            
            ESP_LOGV("FRAME", "Frame updated with mood delta R:%+d G:%+d B:%+d", 
                     (int)totalDelta.red_delta, (int)totalDelta.green_delta, (int)totalDelta.blue_delta);
        }
        // If mood hasn't changed, do nothing - use cached pixels!
    }
    
    // Apply the same color delta to every pixel in frame
    void applyDeltaToAllPixels(const MoodColorDelta& delta) {
        for(uint32_t i = 0; i < pixelCount; ++i) {
            pixelBuffer[i].applyColorDelta(delta);
        }
    }
    
    // Initialize frame from source indexed data + base palette
    void initializeFromSource(const RGB565Pixel* basePalette, uint8_t paletteSize) {
        for(uint32_t i = 0; i < pixelCount; ++i) {
            uint8_t paletteIndex = sourcePixels[i];
            if(paletteIndex < paletteSize) {
                pixelBuffer[i] = basePalette[paletteIndex];
            } else {
                pixelBuffer[i] = RGB565Pixel(0, 0, 0); // Black for invalid indices
            }
        }
        isDirty = true; // Force mood recalculation
    }
    
    // Getters
    const RGB565Pixel* getPixels() const { return pixelBuffer; }
    uint32_t getPixelCount() const { return pixelCount; }
    uint16_t getWidth() const { return width; }
    uint16_t getHeight() const { return height; }
    const Mood& getLastMood() const { return oldMood; }
    bool needsUpdate() const { return isDirty; }
    
    void markDirty() { isDirty = true; }
};

/*
USAGE IN EYE COMPONENTS:

void goblin_left_eye_act(void) {
    static AnimationFrame* currentAnimation = nullptr;
    
    // Load animation if needed
    if (!currentAnimation) {
        currentAnimation = loadEyeAnimation("blink_frames");
    }
    
    // Get current mood and apply to frame
    SharedMemory gsm;
    Mood currentMood = gsm.read<Mood>();
    currentAnimation->renderWithMood(currentMood);
    
    // Set up for GC9A01 display (dynamic pin assignment via spi_bus)
    currentFrame = (uint8_t*)currentAnimation->getPixels();
}

BENEFITS:
1. Single calculation per frame instead of per-pixel calculation
2. Automatic mood change detection with caching
3. Consistent color deltas across all pixels
4. Memory efficient with palette-based source data
5. Fast RGB565 pixel processing optimized for ESP32
*/