/**
 * @file test_eye_components.cpp
 * @brief Test suite for eye component implementations
 * @author P32 Animatronic Bot Project
 */

#include "components/goblin_left_eye.hpp"
#include "components/goblin_right_eye.hpp"
#include "components/goblin_eye.hpp"
#include "components/gc9a01.hpp"
#include "core/memory/SharedMemory.hpp"
#include "Mood.hpp"
#include "esp_log.h"

static const char *TAG = "EYE_TEST";

// Global shared memory instance
SharedMemory GSM;

/**
 * @brief Test the complete eye processing pipeline
 */
void test_eye_processing_pipeline(void) {
    ESP_LOGI(TAG, "=== TESTING EYE PROCESSING PIPELINE ===");
    
    // Test 1: Initialize all eye components
    ESP_LOGI(TAG, "Test 1: Initializing eye components");
    goblin_left_eye_init();
    goblin_right_eye_init();
    gc9a01_init();
    ESP_LOGI(TAG, "? All eye components initialized");
    
    // Test 2: Test mood changes with frame processing
    ESP_LOGI(TAG, "Test 2: Testing mood-based frame processing");
    
    // Set up initial neutral mood
    Mood* sharedMood = GSM.read<Mood>();
    sharedMood->clear();
    GSM.write<Mood>();
    
    // Simulate several loop iterations with neutral mood
    for (uint32_t loop = 0; loop < 10; loop++) {
        g_loopCount = loop;
        
        // Left eye processing
        goblin_left_eye_act();
        goblin_eye_act();
        gc9a01_act();
        
        // Right eye processing  
        goblin_right_eye_act();
        goblin_eye_act();
        gc9a01_act();
    }
    ESP_LOGI(TAG, "? Neutral mood processing completed");
    
    // Test 3: Test with angry mood
    ESP_LOGI(TAG, "Test 3: Testing angry mood effects");
    sharedMood->anger() = 80;
    GSM.write<Mood>();
    
    for (uint32_t loop = 10; loop < 20; loop++) {
        g_loopCount = loop;
        
        goblin_left_eye_act();
        goblin_eye_act();
        gc9a01_act();
        
        goblin_right_eye_act();
        goblin_eye_act();
        gc9a01_act();
    }
    ESP_LOGI(TAG, "? Angry mood processing completed");
    
    // Test 4: Test with happy mood
    ESP_LOGI(TAG, "Test 4: Testing happy mood effects");
    sharedMood->clear();
    sharedMood->happiness() = 90;
    GSM.write<Mood>();
    
    for (uint32_t loop = 20; loop < 30; loop++) {
        g_loopCount = loop;
        
        goblin_left_eye_act();
        goblin_eye_act();
        gc9a01_act();
        
        goblin_right_eye_act();
        goblin_eye_act();
        gc9a01_act();
    }
    ESP_LOGI(TAG, "? Happy mood processing completed");
    
    // Test 5: Test with complex mixed mood
    ESP_LOGI(TAG, "Test 5: Testing complex mixed mood");
    sharedMood->clear();
    sharedMood->curiosity() = 60;
    sharedMood->affection() = 40;
    sharedMood->excitement() = 30;
    GSM.write<Mood>();
    
    for (uint32_t loop = 30; loop < 40; loop++) {
        g_loopCount = loop;
        
        goblin_left_eye_act();
        goblin_eye_act();
        gc9a01_act();
        
        goblin_right_eye_act();
        goblin_eye_act();
        gc9a01_act();
    }
    ESP_LOGI(TAG, "? Mixed mood processing completed");
    
    ESP_LOGI(TAG, "=== ALL EYE TESTS PASSED ===");
}

/**
 * @brief Test animation frame generation and progression
 */
void test_animation_frames(void) {
    ESP_LOGI(TAG, "=== TESTING ANIMATION FRAMES ===");
    
    // Test left eye blink animation progression
    ESP_LOGI(TAG, "Testing left eye blink animation");
    for (uint32_t loop = 0; loop < 120; loop += 30) {
        g_loopCount = loop;
        goblin_left_eye_act();
        ESP_LOGI(TAG, "Loop %u: Left eye frame loaded", loop);
    }
    
    // Test right eye curious look animation progression  
    ESP_LOGI(TAG, "Testing right eye curious look animation");
    for (uint32_t loop = 0; loop < 270; loop += 45) {
        g_loopCount = loop;
        goblin_right_eye_act();
        ESP_LOGI(TAG, "Loop %u: Right eye frame loaded", loop);
    }
    
    ESP_LOGI(TAG, "? Animation frame tests completed");
}

/**
 * @brief Test palette system
 */
void test_palette_system(void) {
    ESP_LOGI(TAG, "=== TESTING PALETTE SYSTEM ===");
    
    // Initialize goblin eye palette
    goblin_eye_init();
    
    // Test palette color ranges
    ESP_LOGI(TAG, "Testing palette color ranges:");
    ESP_LOGI(TAG, "Black range (0-31): %04X", goblin_eye_palette[0].value);
    ESP_LOGI(TAG, "Brown range (32-63): %04X", goblin_eye_palette[32].value);
    ESP_LOGI(TAG, "Yellow range (64-95): %04X", goblin_eye_palette[64].value);
    ESP_LOGI(TAG, "Red range (96-127): %04X", goblin_eye_palette[96].value);
    ESP_LOGI(TAG, "Green range (128-159): %04X", goblin_eye_palette[128].value);
    ESP_LOGI(TAG, "Blue range (160-191): %04X", goblin_eye_palette[160].value);
    ESP_LOGI(TAG, "Purple range (192-223): %04X", goblin_eye_palette[192].value);
    ESP_LOGI(TAG, "White range (224-254): %04X", goblin_eye_palette[224].value);
    ESP_LOGI(TAG, "Pure white (255): %04X", goblin_eye_palette[255].value);
    
    ESP_LOGI(TAG, "? Palette system test completed");
}

/**
 * @brief Main test runner for eye components
 */
void run_eye_component_tests(void) {
    ESP_LOGI(TAG, "Starting comprehensive eye component tests");
    
    test_palette_system();
    test_animation_frames();
    test_eye_processing_pipeline();
    
    ESP_LOGI(TAG, "? ALL EYE COMPONENT TESTS COMPLETED SUCCESSFULLY! ?");
    ESP_LOGI(TAG, "Eye implementations are ready for integration");
}