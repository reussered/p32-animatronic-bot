// P32 Efficient Frame Mood Processor
// Single calculation per frame - applies same color delta to all pixels

#ifndef FRAME_MOOD_PROCESSOR_HPP
#define FRAME_MOOD_PROCESSOR_HPP

#include "Mood.hpp"
#include <cstdint>

// RGB565 pixel format for ESP32 displays
struct RGB565Pixel {
    uint16_t value;
    
    RGB565Pixel() : value(0) {}
    RGB565Pixel(uint16_t v) : value(v) {}
    RGB565Pixel(uint8_t r, uint8_t g, uint8_t b) {
        value = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    
    // Extract RGB components
    uint8_t red() const { return (value >> 8) & 0xF8; }
    uint8_t green() const { return (value >> 3) & 0xFC; }
    uint8_t blue() const { return (value << 3) & 0xF8; }
    
    // Apply color delta with clamping
    void applyDelta(const MoodColorDelta& delta) {
        int16_t r = red() + delta.red_delta;
        int16_t g = green() + delta.green_delta;
        int16_t b = blue() + delta.blue_delta;
        
        // Clamp to valid ranges
        r = (r < 0) ? 0 : (r > 255) ? 255 : r;
        g = (g < 0) ? 0 : (g > 255) ? 255 : g;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;
        
        value = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
};

// Animation frame with mood tracking
class AnimationFrame {
private:
    Mood oldMood;                    // Last mood this frame was rendered with
    RGB565Pixel* frameBuffer;        // Actual pixel data
    uint32_t pixelCount;            // Number of pixels in frame
    bool needsUpdate;               // True if mood changed
    
public:
    AnimationFrame(RGB565Pixel* buffer, uint32_t count) 
        : frameBuffer(buffer), pixelCount(count), needsUpdate(true) {}
    
    // Main update function - implements your optimization!
    void updateWithMood(const Mood& currentGlobalMood) {
        if(oldMood != currentGlobalMood) {
            // Step 1: Calculate TOTAL color delta for all mood changes (ONCE per frame)
            MoodColorDelta totalDelta = calculateTotalMoodDelta(oldMood, currentGlobalMood);
            
            // Step 2: Apply the same delta to EVERY pixel in frame (fast loop)
            applyDeltaToAllPixels(totalDelta);
            
            // Step 3: Remember the new mood
            oldMood = currentGlobalMood;
            needsUpdate = false;
        }
        // If mood hasn't changed, do nothing - use cached colors!
    }
    
    // Calculate total color change for all mood component changes
    MoodColorDelta calculateTotalMoodDelta(const Mood& oldMood, const Mood& newMood) {
        MoodColorDelta totalDelta;
        
        for(int i = 0; i < Mood::componentCount; ++i) {
            int16_t moodDelta = newMood.components[i] - oldMood.components[i];
            if(moodDelta != 0) {
                const MoodColorEffect& effect = moodColorEffects[i];
                
                // Add this mood's contribution to total delta
                totalDelta.red_delta += static_cast<int16_t>(moodDelta * effect.red_multiplier);
                totalDelta.green_delta += static_cast<int16_t>(moodDelta * effect.green_multiplier);
                totalDelta.blue_delta += static_cast<int16_t>(moodDelta * effect.blue_multiplier);
            }
        }
        
        return totalDelta;
    }
    
    // Apply the same color delta to every pixel (very fast!)
    void applyDeltaToAllPixels(const MoodColorDelta& delta) {
        for(uint32_t i = 0; i < pixelCount; ++i) {
            frameBuffer[i].applyDelta(delta);
        }
    }
    
    // Getters
    const RGB565Pixel* getPixels() const { return frameBuffer; }
    uint32_t getPixelCount() const { return pixelCount; }
    const Mood& getLastMood() const { return oldMood; }
    bool needsMoodUpdate() const { return needsUpdate; }
    
    // Force mood update on next render
    void markDirty() { needsUpdate = true; }
};

// Frame collection for managing multiple animation frames
class MoodAnimationFrames {
private:
    AnimationFrame* frames;
    uint32_t frameCount;
    uint32_t currentFrameIndex;
    
public:
    MoodAnimationFrames(uint32_t count) : frameCount(count), currentFrameIndex(0) {
        frames = new AnimationFrame[count];
    }
    
    ~MoodAnimationFrames() {
        delete[] frames;
    }
    
    AnimationFrame& getCurrentFrame() {
        return frames[currentFrameIndex];
    }
    
    AnimationFrame& getFrame(uint32_t index) {
        return (index < frameCount) ? frames[index] : frames[0];
    }
    
    void setCurrentFrame(uint32_t index) {
        if(index < frameCount) {
            currentFrameIndex = index;
        }
    }
    
    uint32_t getFrameCount() const { return frameCount; }
    uint32_t getCurrentFrameIndex() const { return currentFrameIndex; }
    
    // Update current frame with global mood
    void updateCurrentFrameWithMood(const Mood& globalMood) {
        getCurrentFrame().updateWithMood(globalMood);
    }
};

#endif // FRAME_MOOD_PROCESSOR_HPP