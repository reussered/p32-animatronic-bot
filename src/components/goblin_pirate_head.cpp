/**
 * @file goblin_pirate_head.cpp
 * @brief Pirate-specific head subsystem with single eye and enhanced audio
 * 
 * Coordinates left eye, mouth, speaker, nose sensor, and both ears
 * with pirate-specific behaviors and missing right eye compensation.
 */

#include "esp_log.h"
#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "Mood.hpp"

static const char* TAG = "PIRATE_HEAD";

// Function prototypes
static void coordinate_single_eye_display(void);
static void enhance_audio_processing(void);
static void implement_defensive_scanning(void);
static void manage_pirate_expressions(void);
static void handle_combat_readiness(void);

// Single-eye optimization parameters
static const int LEFT_EYE_BOOST_FACTOR = 150;  // 50% enhancement for missing right eye
static const int AUDIO_SENSITIVITY_MULTIPLIER = 150;  // 50% boost
static const int DEFENSIVE_SCAN_INTERVAL = 20;  // More frequent scanning

// Pirate head state tracking
static bool left_side_protection_active = true;
static uint32_t last_threat_scan = 0;
static uint8_t audio_compensation_level = 50;
static bool combat_readiness_mode = false;

void goblin_pirate_head_init(void) {
    ESP_LOGI(TAG, "Initializing Pirate Head Subsystem");
    ESP_LOGI(TAG, "  Configuration: SINGLE_LEFT_EYE + ENHANCED_AUDIO");
    ESP_LOGI(TAG, "  Missing Eye: RIGHT_EYE_LOST_IN_BATTLE");
    ESP_LOGI(TAG, "  Compensation: LEFT_EYE_BOOST + AUDIO_ENHANCEMENT");
    ESP_LOGI(TAG, "  GPIO Usage: 10 pins (vs 12 for dual-eye)");
    
    // Initialize pirate-specific head state
    left_side_protection_active = true;
    audio_compensation_level = 50;
    combat_readiness_mode = false;
    
    ESP_LOGI(TAG, "Pirate head subsystem ready - Single eye terror mode active");
}

void goblin_pirate_head_act(void) {
    // hitCount: 25 -> executes every 2.5 seconds
    if ((g_loopCount % 25) != 0) return;
    
    // Pirate head coordination tasks - using direct global access
    coordinate_single_eye_display();
    enhance_audio_processing();
    implement_defensive_scanning();
    manage_pirate_expressions();
    handle_combat_readiness();
    
    if ((g_loopCount % 100) == 0) {  // Every 10 seconds
        ESP_LOGD(TAG, "Pirate head status: Eye_boost=%d%%, Audio_comp=%d%%, Combat=%s",
            LEFT_EYE_BOOST_FACTOR, audio_compensation_level,
            combat_readiness_mode ? "READY" : "PATROL");
    }
}

static void coordinate_single_eye_display(void) {
    // Enhanced left eye processing to compensate for missing right eye
    static uint8_t eye_scan_pattern = 0;
    
    // Wider field of view scanning since only one eye
    eye_scan_pattern = (eye_scan_pattern + 1) % 8;
    
    // Boost visual processing for single eye using direct global Mood access
    int enhanced_curiosity = (g_mood.curiosity() * LEFT_EYE_BOOST_FACTOR) / 100;
    if (enhanced_curiosity > 100) enhanced_curiosity = 100;
    
    // Left eye works harder to cover right blind spot
    if (g_shared_state.distance_cm > 0 && g_shared_state.distance_cm < 50) {
        // Rapid scanning when threats detected
        if ((g_loopCount % 5) == 0) {  // Every 500ms
            ESP_LOGD(TAG, "ENHANCED_EYE_SCAN: Pattern=%d, Distance=%dcm", 
                eye_scan_pattern, g_shared_state.distance_cm);
        }
    }
    
    // Protect blind side - bias left eye toward right coverage
    if (left_side_protection_active) {
        ESP_LOGV(TAG, "Single eye enhanced scanning: curiosity boosted to %d%%", enhanced_curiosity);
        ESP_LOGD(TAG, "Left eye compensating for blind spot protection");
    }
}
}

static void enhance_audio_processing(void) {
    // Missing eye compensation through enhanced hearing
    
    // Boost audio sensitivity based on threat level using direct global access
    if (g_shared_state.distance_cm > 0 && g_shared_state.distance_cm < 30) {
        audio_compensation_level = 80;  // High alert - close threat
    } else if (g_shared_state.distance_cm > 0 && g_shared_state.distance_cm < 60) {
        audio_compensation_level = 65;  // Medium alert
    } else {
        audio_compensation_level = 50;  // Baseline enhanced
    }
    
    // Simulate enhanced directional hearing
    if (g_shared_state.distance_cm > 0) {
        // Audio processing boosts situational awareness
        uint8_t curiosity_boost = (audio_compensation_level - 50) / 5;
        
        // Enhance mood based on audio compensation
        g_mood.addCuriosity(curiosity_boost);
        
        ESP_LOGD(TAG, "AUDIO_COMPENSATION: Level=%d%%, Curiosity_boost=%d", 
            audio_compensation_level, curiosity_boost);
    }
    
    // Both ears work harder to compensate for missing eye
    static uint8_t ear_scan_cycle = 0;
    ear_scan_cycle = (ear_scan_cycle + 1) % 4;
    
    // Alternate between left and right ear focus
    if ((ear_scan_cycle % 2) == 0) {
        // Left ear scanning (near blind side)
        ESP_LOGV(TAG, "LEFT_EAR_FOCUS: Protecting blind right side");
    } else {
        // Right ear scanning (good eye side)
        ESP_LOGV(TAG, "RIGHT_EAR_FOCUS: Supporting left eye coverage");
    }
}

static void implement_defensive_scanning(void) {
    // More frequent threat scanning due to blind spot
    if ((g_loopCount % DEFENSIVE_SCAN_INTERVAL) == 0) {
        last_threat_scan = g_loopCount;
        
        // Scan for threats more aggressively
        if (g_shared_state.distance_cm == 0) {
            // No immediate threats - maintain vigilance
            g_mood.addAnger(5);  // Always scanning
        } else if (g_shared_state.distance_cm < 30) {
            // Close threat - high alert
            g_mood.addAnger(10);
            g_mood.addFear(8);
            combat_readiness_mode = true;
            
            ESP_LOGW(TAG, "CLOSE_THREAT_DETECTED: %dcm - Combat ready", g_shared_state.distance_cm);
        } else if (g_shared_state.distance_cm < 60) {
            // Medium distance - cautious monitoring
            g_mood.addCuriosity(15);
            g_mood.addFear(3);
        }
    }
}

static void manage_pirate_expressions(void) {
    // Pirate facial expressions emphasize the single eye
    static uint8_t expression_timer = 0;
    expression_timer++;
    
    // Aggressive scowl is default expression
    if (g_mood.anger() > 30) {
        // Fierce single-eye glare
        ESP_LOGV(TAG, "EXPRESSION: Fierce_single_eye_glare (Anger=%d)", g_mood.anger());
    } else if (g_mood.curiosity() > 50) {
        // Suspicious single-eye squint
        ESP_LOGV(TAG, "EXPRESSION: Suspicious_squint (Curiosity=%d)", g_mood.curiosity());
    } else if (g_mood.anger() > 40) {
        // Greedy treasure-hunting look
        ESP_LOGV(TAG, "EXPRESSION: Greedy_treasure_hunt (Anger=%d)", g_mood.anger());
    } else {
        // Default weathered pirate scowl
        if ((expression_timer % 40) == 0) {  // Every 4 seconds
            ESP_LOGV(TAG, "EXPRESSION: Default_pirate_scowl");
        }
    }
    
    // Mouth expressions coordinate with single eye
    if (g_mood.anger() > 50) {
        ESP_LOGV(TAG, "MOUTH: Snarling_grimace");
    } else if (g_mood.curiosity() > 60) {
        ESP_LOGV(TAG, "MOUTH: Greedy_sneer");
    }
}

static void handle_combat_readiness(void) {
    // Combat mode triggered by close threats or high anger
    bool should_be_combat_ready = (g_shared_state.distance_cm > 0 && g_shared_state.distance_cm < 25) || 
                                  (g_mood.anger() > 60);
    
    if (should_be_combat_ready && !combat_readiness_mode) {
        combat_readiness_mode = true;
        ESP_LOGW(TAG, "ENTERING_COMBAT_MODE: Single eye focused, audio enhanced");
        
        // Boost all threat-detection systems
        g_mood.addAnger(15);
        g_mood.addCuriosity(20);
        // Reduce fear in combat - get current value, modify, and set back
        int8_t current_fear = g_mood.fear();
        g_mood.setFear((current_fear * 2) / 3);
        
    } else if (!should_be_combat_ready && combat_readiness_mode) {
        combat_readiness_mode = false;
        ESP_LOGI(TAG, "EXITING_COMBAT_MODE: Returning to patrol vigilance");
        
        // Gradually reduce heightened state
        if (g_mood.anger() > 20) {
            g_mood.addAnger(-10);
        }
    }
    
    // In combat mode, coordinate all subsystems
    if (combat_readiness_mode) {
        // Rapid single-eye scanning
        if ((g_loopCount % 3) == 0) {  // Every 300ms
            ESP_LOGD(TAG, "COMBAT_SCAN: Left_eye_active, Audio_max_sensitivity");
        }
        
        // Enhanced audio compensation
        audio_compensation_level = 90;  // Maximum compensation
    }
}
