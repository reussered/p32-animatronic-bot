// P32 Mood System Implementation - C++ with efficient delta calculations
#include "Mood.hpp"
#include <cstdio>

// Mood-to-color effect mappings
// Each mood component has RGB multipliers that define its color signature
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

// Debug helper - print mood state
void printMoodState(const Mood& mood, const char* label) {
    printf("=== %s MOOD STATE ===\n", label ? label : "CURRENT");
    
    for(int i = 0; i < Mood::componentCount; ++i) {
        if(mood.components[i] != 0) {  // Show positive AND negative moods
            printf("  %s: %d", Mood::getComponentName(i), mood.components[i]);
            
            // Show color effect for this mood
            const MoodColorEffect& effect = moodColorEffects[i];
            printf(" (R:%.2f G:%.2f B:%.2f)\n", 
                   effect.red_multiplier, 
                   effect.green_multiplier, 
                   effect.blue_multiplier);
        }
    }
    
    if(!mood.hasAnyMood()) {
        printf("  NEUTRAL (no active moods)\n");
    }
    printf("\n");
}

// Calculate total color delta between two moods
MoodColorDelta calculateMoodDelta(const Mood& oldMood, const Mood& newMood) {
    MoodColorDelta totalDelta;
    
    printf("Calculating mood delta...\n");
    
    for(int i = 0; i < Mood::componentCount; ++i) {
        int8_t componentDelta = newMood.components[i] - oldMood.components[i];
        
        if(componentDelta != 0) {
            const MoodColorEffect& effect = moodColorEffects[i];
            
            // Calculate color contribution from this mood change
            int8_t redContrib = static_cast<int8_t>(componentDelta * effect.red_multiplier);
            int8_t greenContrib = static_cast<int8_t>(componentDelta * effect.green_multiplier);
            int8_t blueContrib = static_cast<int8_t>(componentDelta * effect.blue_multiplier);
            
            totalDelta.red_delta += redContrib;
            totalDelta.green_delta += greenContrib;
            totalDelta.blue_delta += blueContrib;
            
            printf("  %s delta: %+d -> R:%+d G:%+d B:%+d\n", 
                   Mood::getComponentName(i), (int)componentDelta, 
                   (int)redContrib, (int)greenContrib, (int)blueContrib);
        }
    }
    
    printf("TOTAL DELTA: R:%+d G:%+d B:%+d\n\n", 
           (int)totalDelta.red_delta, (int)totalDelta.green_delta, (int)totalDelta.blue_delta);
    
    return totalDelta;
}

// Test function to demonstrate the mood system
void testMoodSystem() {
    printf("=== MOOD SYSTEM TEST ===\n\n");
    
    // Create neutral mood
    Mood neutralMood;
    printMoodState(neutralMood, "NEUTRAL");
    
    // Create angry mood
    Mood angryMood;
    angryMood.anger() = 100; // Mid-level anger (within int8_t range)
    printMoodState(angryMood, "ANGRY");
    
    // Calculate delta from neutral to angry
    MoodColorDelta delta1 = calculateMoodDelta(neutralMood, angryMood);
    
    // Create happy mood
    Mood happyMood;
    happyMood.happiness() = 120; // High happiness (within int8_t range)
    printMoodState(happyMood, "HAPPY");
    
    // Calculate delta from angry to happy
    MoodColorDelta delta2 = calculateMoodDelta(angryMood, happyMood);
    
    // Create complex mixed mood
    Mood mixedMood;
    mixedMood.curiosity() = 30;
    mixedMood.affection() = 40;
    mixedMood.irritation() = 20;
    printMoodState(mixedMood, "MIXED");
    
    // Calculate delta to mixed mood
    MoodColorDelta delta3 = calculateMoodDelta(happyMood, mixedMood);
    
    // Test the new EXCITEMENT mood
    Mood excitedMood;
    excitedMood.excitement() = 80; // High excitement
    printMoodState(excitedMood, "EXCITED");
    
    // Calculate delta to excitement
    MoodColorDelta delta4 = calculateMoodDelta(mixedMood, excitedMood);
    
    // Test mood comparison operators
    printf("=== OPERATOR TESTS ===\n");
    printf("neutralMood == angryMood: %s\n", (neutralMood == angryMood) ? "true" : "false");
    printf("neutralMood != angryMood: %s\n", (neutralMood != angryMood) ? "true" : "false");
    printf("angryMood == angryMood: %s\n", (angryMood == angryMood) ? "true" : "false");
    
    // Test assignment
    Mood copyMood = angryMood;
    printf("copyMood == angryMood: %s\n", (copyMood == angryMood) ? "true" : "false");
    
    printf("\n=== OVERFLOW PROTECTION TEST ===\n");
    
    // Test mood overflow protection
    Mood overflowMood;
    overflowMood.anger() = 100;
    printf("Initial anger: %d\n", (int)overflowMood.anger());
    
    // This should clamp to 127, not overflow to negative
    overflowMood.addAnger(50);  // 100 + 50 = 150, clamped to 127
    printf("After adding 50: %d (should be 127)\n", (int)overflowMood.anger());
    
    // Test negative overflow protection
    overflowMood.addAnger(-300); // 127 - 300 = -173, clamped to -128
    printf("After subtracting 300: %d (should be -128)\n", (int)overflowMood.anger());
    
    // Test MoodColorDelta overflow protection
    MoodColorDelta overflowDelta1(100, 50, -80);
    MoodColorDelta overflowDelta2(50, 100, -60);
    printf("OverflowDelta1: R:%d G:%d B:%d\n", (int)overflowDelta1.red_delta, (int)overflowDelta1.green_delta, (int)overflowDelta1.blue_delta);
    printf("OverflowDelta2: R:%d G:%d B:%d\n", (int)overflowDelta2.red_delta, (int)overflowDelta2.green_delta, (int)overflowDelta2.blue_delta);
    
    overflowDelta1 += overflowDelta2;  // Should clamp: R=127, G=127, B=-128
    printf("After adding: R:%d G:%d B:%d (should be R:127 G:127 B:-128)\n", 
           (int)overflowDelta1.red_delta, (int)overflowDelta1.green_delta, (int)overflowDelta1.blue_delta);
    
    printf("\n=== ALL TESTS COMPLETE ===\n");
}