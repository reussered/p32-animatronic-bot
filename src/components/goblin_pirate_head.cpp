/**
 * @file goblin_pirate_head.cpp
 * @brief Pirate-specific head subsystem with single eye and enhanced audio
 * 
 * Coordinates left eye, mouth, speaker, nose sensor, and both ears
 * with pirate-specific behaviors and missing right eye compensation.
 */

#include "esp_log.h"
#include "p32_shared_state.h"

static const char* TAG = "PIRATE_HEAD";

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
    
    SharedMemory local_state;
    local_state.read();
    
    // Pirate head coordination tasks
    coordinate_single_eye_display(local_state);
    enhance_audio_processing(local_state);
    implement_defensive_scanning(local_state);
    manage_pirate_expressions(local_state);
    handle_combat_readiness(local_state);
    
    // Update shared state
    local_state.write();
    
    if ((g_loopCount % 100) == 0) {  // Every 10 seconds
        ESP_LOGD(TAG, "Pirate head status: Eye_boost=%d%%, Audio_comp=%d%%, Combat=%s",
            LEFT_EYE_BOOST_FACTOR, audio_compensation_level,
            combat_readiness_mode ? "READY" : "PATROL");
    }
}

static void coordinate_single_eye_display(SharedMemory& state) {
    // Enhanced left eye processing to compensate for missing right eye
    static uint8_t eye_scan_pattern = 0;
    
    // Wider field of view scanning since only one eye
    eye_scan_pattern = (eye_scan_pattern + 1) % 8;
    
    // Boost visual processing for single eye
    int enhanced_curiosity = (state.mood.CURIOSITY * LEFT_EYE_BOOST_FACTOR) / 100;
    if (enhanced_curiosity > 100) enhanced_curiosity = 100;
    
    // Left eye works harder to cover right blind spot
    if (state.sensor_distance > 0 && state.sensor_distance < 50) {
        // Rapid scanning when threats detected
        if ((g_loopCount % 5) == 0) {  // Every 500ms
            ESP_LOGD(TAG, "ENHANCED_EYE_SCAN: Pattern=%d, Distance=%dcm", 
                eye_scan_pattern, state.sensor_distance);
        }
    }
    
    // Protect blind side - bias left eye toward right coverage
    if (left_side_protection_active) {
        // Simulate left eye compensating for right blind spot
        state.mood.CURIOSITY = enhanced_curiosity;
    }
}

static void enhance_audio_processing(SharedMemory& state) {
    // Missing eye compensation through enhanced hearing
    
    // Boost audio sensitivity based on threat level
    if (state.mood.ANGER > 40 || state.mood.FEAR > 30) {
        audio_compensation_level = 80;  // High alert
    } else if (state.sensor_distance > 0 && state.sensor_distance < 60) {
        audio_compensation_level = 65;  // Medium alert
    } else {
        audio_compensation_level = 50;  // Baseline enhanced
    }
    
    // Simulate enhanced directional hearing
    if (state.sensor_distance > 0) {
        // Audio processing boosts situational awareness
        state.mood.CURIOSITY += (audio_compensation_level - 50) / 5;
        if (state.mood.CURIOSITY > 100) state.mood.CURIOSITY = 100;
        
        ESP_LOGD(TAG, "AUDIO_COMPENSATION: Level=%d%%, Curiosity_boost=%d", 
            audio_compensation_level, state.mood.CURIOSITY);
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

static void implement_defensive_scanning(SharedMemory& state) {
    // More frequent threat scanning due to blind spot
    if ((g_loopCount % DEFENSIVE_SCAN_INTERVAL) == 0) {
        last_threat_scan = g_loopCount;
        
        // Scan for threats more aggressively
        if (state.sensor_distance == 0) {
            // No immediate threats - maintain vigilance
            state.mood.CURIOSITY += 5;  // Always scanning
        } else if (state.sensor_distance < 30) {
            // Close threat - high alert
            state.mood.ANGER += 10;
            state.mood.IRRITATION += 8;
            combat_readiness_mode = true;
            
            ESP_LOGW(TAG, "CLOSE_THREAT_DETECTED: %dcm - Combat ready", state.sensor_distance);
        } else if (state.sensor_distance < 60) {
            // Medium distance - cautious monitoring
            state.mood.CURIOSITY += 15;
            state.mood.IRRITATION += 3;
        }
    }
}

static void manage_pirate_expressions(SharedMemory& state) {
    // Pirate facial expressions emphasize the single eye
    static uint8_t expression_timer = 0;
    expression_timer++;
    
    // Aggressive scowl is default expression
    if (state.mood.ANGER > 30) {
        // Fierce single-eye glare
        ESP_LOGV(TAG, "EXPRESSION: Fierce_single_eye_glare (Anger=%d)", state.mood.ANGER);
    } else if (state.mood.CURIOSITY > 50) {
        // Suspicious single-eye squint
        ESP_LOGV(TAG, "EXPRESSION: Suspicious_squint (Curiosity=%d)", state.mood.CURIOSITY);
    } else if (state.mood.HUNGER > 40) {
        // Greedy treasure-hunting look
        ESP_LOGV(TAG, "EXPRESSION: Greedy_treasure_hunt (Hunger=%d)", state.mood.HUNGER);
    } else {
        // Default weathered pirate scowl
        if ((expression_timer % 40) == 0) {  // Every 4 seconds
            ESP_LOGV(TAG, "EXPRESSION: Default_pirate_scowl");
        }
    }
    
    // Mouth expressions coordinate with single eye
    if (state.mood.ANGER > 50) {
        ESP_LOGV(TAG, "MOUTH: Snarling_grimace");
    } else if (state.mood.HUNGER > 60) {
        ESP_LOGV(TAG, "MOUTH: Greedy_sneer");
    }
}

static void handle_combat_readiness(SharedMemory& state) {
    // Combat mode triggered by close threats or high anger
    bool should_be_combat_ready = (state.sensor_distance > 0 && state.sensor_distance < 25) || 
                                  (state.mood.ANGER > 60);
    
    if (should_be_combat_ready && !combat_readiness_mode) {
        combat_readiness_mode = true;
        ESP_LOGW(TAG, "ENTERING_COMBAT_MODE: Single eye focused, audio enhanced");
        
        // Boost all threat-detection systems
        state.mood.ANGER += 15;
        state.mood.CURIOSITY += 20;
        state.mood.FEAR = (state.mood.FEAR * 2) / 3;  // Reduce fear in combat
        
    } else if (!should_be_combat_ready && combat_readiness_mode) {
        combat_readiness_mode = false;
        ESP_LOGI(TAG, "EXITING_COMBAT_MODE: Returning to patrol vigilance");
        
        // Gradually reduce heightened state
        if (state.mood.ANGER > 20) state.mood.ANGER -= 10;
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