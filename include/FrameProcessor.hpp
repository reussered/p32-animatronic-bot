// P32 Frame Processor - Implements your efficient mood-based rendering
// Each frame remembers its last mood and only recalculates when mood changes

#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP

#include "Mood.hpp"
#include <cstdint>
#include <cstdio>

// RGB565 pixel for ESP32 displays
struct RGB565Pixel {
    uint16_t value;
    
    RGB565Pixel() : value(0) {}
    RGB565Pixel(uint16_t v) : value(v) {}
    RGB565Pixel(uint8_t r, uint8_t g, uint8_t b) {
        // Pack RGB888 to RGB565
        value = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    
    // Extract RGB components (0-255 range)
    uint8_t red() const { return (value >> 8) & 0xF8; }
    uint8_t green() const { return (value >> 3) & 0xFC; }  
    uint8_t blue() const { return (value << 3) & 0xF8; }
    
    // Apply mood color delta with clamping
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

// Animation frame with mood-based processing
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
            
            #ifdef SIMPLE_TEST
            printf("Frame updated with mood delta R:%+d G:%+d B:%+d\n", 
                   (int)totalDelta.red_delta, (int)totalDelta.green_delta, (int)totalDelta.blue_delta);
            #endif
        }
        // If mood unchanged, do nothing - use cached pixels!
    }
    
    // Calculate total color change for all mood deltas
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

// Animation collection managing multiple frames
class MoodAnimation {
private:
    AnimationFrame** frames;
    uint32_t frameCount;
    uint32_t currentFrameIndex;
    char animationName[32];
    
public:
    MoodAnimation(const char* name, uint32_t count) : frameCount(count), currentFrameIndex(0) {
        strncpy(animationName, name, sizeof(animationName) - 1);
        frames = new AnimationFrame*[count];
        for(uint32_t i = 0; i < count; i++) {
            frames[i] = nullptr; // Initialize later
        }
    }
    
    ~MoodAnimation() {
        for(uint32_t i = 0; i < frameCount; i++) {
            delete frames[i];
        }
        delete[] frames;
    }
    
    void setFrame(uint32_t index, AnimationFrame* frame) {
        if(index < frameCount) {
            delete frames[index]; // Clean up existing
            frames[index] = frame;
        }
    }
    
    AnimationFrame* getCurrentFrame() {
        return (currentFrameIndex < frameCount) ? frames[currentFrameIndex] : nullptr;
    }
    
    AnimationFrame* getFrame(uint32_t index) {
        return (index < frameCount) ? frames[index] : nullptr;
    }
    
    void setCurrentFrame(uint32_t index) {
        if(index < frameCount) {
            currentFrameIndex = index;
        }
    }
    
    // Update current frame with global mood - implements your core optimization!
    void updateCurrentFrameWithMood(const Mood& globalMood) {
        AnimationFrame* current = getCurrentFrame();
        if(current) {
            current->renderWithMood(globalMood);
        }
    }
    
    uint32_t getFrameCount() const { return frameCount; }
    uint32_t getCurrentFrameIndex() const { return currentFrameIndex; }
    const char* getName() const { return animationName; }
};

#endif // FRAME_PROCESSOR_HPP