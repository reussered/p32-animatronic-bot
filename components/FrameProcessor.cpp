// P32 Frame Processor Implementation
#include "FrameProcessor.hpp"
#include <cstdio>
#include <cstring>
#include <inttypes.h>

// Test function demonstrating the mood-based frame processing
void testFrameProcessor() {
    printf("=== FRAME PROCESSOR TEST ===\n\n");
    
    // Create test indexed pixel data (simulating eye animation frame)
    const uint8_t testPixelData[16] = {
        0, 1, 1, 0,  // Simple 4x4 pattern
        1, 2, 2, 1,  // 0=black, 1=white, 2=blue
        1, 2, 2, 1,
        0, 1, 1, 0
    };
    
    // Create base palette  
    RGB565Pixel basePalette[3] = {
        RGB565Pixel(0, 0, 0),       // 0: Black
        RGB565Pixel(255, 255, 255), // 1: White  
        RGB565Pixel(0, 100, 255)    // 2: Blue
    };
    
    // Create animation frame
    AnimationFrame testFrame(testPixelData, 4, 4);
    testFrame.initializeFromSource(basePalette, 3);
    
    printf("Created 4x4 test frame\n");
    printf("Base colors: Black, White, Blue\n\n");
    
    // Test 1: Neutral mood (no change expected)
    Mood neutralMood;
    printf("=== TEST 1: NEUTRAL MOOD ===\n");
    testFrame.renderWithMood(neutralMood);
    
    // Print first few pixels
    const RGB565Pixel* pixels = testFrame.getPixels();
    printf("First 4 pixels (should be unchanged):\n");
    for(int i = 0; i < 4; i++) {
        printf("  Pixel %d: R:%d G:%d B:%d\n", i, 
               pixels[i].red(), pixels[i].green(), pixels[i].blue());
    }
    
    // Test 2: Add anger (should add red, reduce green/blue)
    printf("\n=== TEST 2: ADD ANGER ===\n");
    Mood angryMood;
    angryMood.anger() = 50;  // Moderate anger
    testFrame.renderWithMood(angryMood);
    
    printf("After adding anger=50 (should be more red):\n");
    for(int i = 0; i < 4; i++) {
        printf("  Pixel %d: R:%d G:%d B:%d\n", i,
               pixels[i].red(), pixels[i].green(), pixels[i].blue());
    }
    
    // Test 3: Change to happiness (should add yellow/warm tones)
    printf("\n=== TEST 3: CHANGE TO HAPPINESS ===\n");
    Mood happyMood;
    happyMood.happiness() = 60; // Moderate happiness
    testFrame.renderWithMood(happyMood);
    
    printf("After changing to happiness=60 (should be more yellow):\n");
    for(int i = 0; i < 4; i++) {
        printf("  Pixel %d: R:%d G:%d B:%d\n", i,
               pixels[i].red(), pixels[i].green(), pixels[i].blue());
    }
    
    // Test 4: Same mood again (should skip processing)
    printf("\n=== TEST 4: SAME MOOD (NO CHANGE) ===\n");
    printf("Rendering with same happiness mood again...\n");
    testFrame.renderWithMood(happyMood); // Should do nothing
    
    printf("Pixels should be identical (no processing occurred):\n");
    for(int i = 0; i < 4; i++) {
        printf("  Pixel %d: R:%d G:%d B:%d\n", i,
               pixels[i].red(), pixels[i].green(), pixels[i].blue());
    }
    
    // Test 5: Complex mixed mood
    printf("\n=== TEST 5: COMPLEX MIXED MOOD ===\n");
    Mood mixedMood;
    mixedMood.fear() = 30;
    mixedMood.curiosity() = 40;
    mixedMood.affection() = 25;
    testFrame.renderWithMood(mixedMood);
    
    printf("After mixed mood (fear+curiosity+affection):\n");
    for(int i = 0; i < 4; i++) {
        printf("  Pixel %d: R:%d G:%d B:%d\n", i,
               pixels[i].red(), pixels[i].green(), pixels[i].blue());
    }
    
    printf("\n=== FRAME PROCESSOR TEST COMPLETE ===\n");
}

// Test animation with multiple frames
void testMoodAnimation() {
    printf("\n=== MOOD ANIMATION TEST ===\n");
    
    // Create animation with 3 frames
    MoodAnimation eyeAnimation("BLINK", 3);
    
    // Frame 0: Eye open (mostly white)
    const uint8_t frame0Data[4] = { 1, 1, 1, 1 }; // All white
    AnimationFrame* frame0 = new AnimationFrame(frame0Data, 2, 2);
    
    // Frame 1: Eye half-closed (mixed)  
    const uint8_t frame1Data[4] = { 0, 1, 1, 0 }; // Black corners
    AnimationFrame* frame1 = new AnimationFrame(frame1Data, 2, 2);
    
    // Frame 2: Eye closed (mostly black)
    const uint8_t frame2Data[4] = { 0, 0, 0, 0 }; // All black
    AnimationFrame* frame2 = new AnimationFrame(frame2Data, 2, 2);
    
    // Set up base palette
    RGB565Pixel basePalette[2] = {
        RGB565Pixel(0, 0, 0),       // 0: Black (closed)
        RGB565Pixel(255, 255, 255)  // 1: White (open)
    };
    
    // Initialize frames
    frame0->initializeFromSource(basePalette, 2);
    frame1->initializeFromSource(basePalette, 2); 
    frame2->initializeFromSource(basePalette, 2);
    
    // Add frames to animation
    eyeAnimation.setFrame(0, frame0);
    eyeAnimation.setFrame(1, frame1);
    eyeAnimation.setFrame(2, frame2);
    
    printf("Created BLINK animation with 3 frames\n");
    
    // Test mood changes across different frames
    Mood testMood;
    
    for(uint32_t frameIdx = 0; frameIdx < 3; frameIdx++) {
        eyeAnimation.setCurrentFrame(frameIdx);
        printf("\n--- Frame %" PRIu32 " ---\n", frameIdx);
        
        // Test anger on this frame
        testMood.clear();
        testMood.anger() = 70;
        eyeAnimation.updateCurrentFrameWithMood(testMood);
        
        const RGB565Pixel* pixels = eyeAnimation.getCurrentFrame()->getPixels();
        printf("With anger=70: R:%d G:%d B:%d\n", 
               pixels[0].red(), pixels[0].green(), pixels[0].blue());
        
        // Test happiness on same frame
        testMood.clear();
        testMood.happiness() = 80;
        eyeAnimation.updateCurrentFrameWithMood(testMood);
        
        printf("With happiness=80: R:%d G:%d B:%d\n",
               pixels[0].red(), pixels[0].green(), pixels[0].blue());
    }
    
    printf("\n=== ANIMATION TEST COMPLETE ===\n");
}