/**
 * @file goblin_pirate_personality.cpp
 * @brief Pirate-specific personality engine for one-eyed goblin variants
 * 
 * Implements enhanced aggression, treasure obsession, and compensatory behaviors
 * for battle-scarred goblins who lost their right eye in combat.
 */

#include "esp_log.h"
#include "p32_shared_state.h"
#include "core/memory/SharedMemory.hpp"
#include "Mood.hpp"

static const char* TAG = "PIRATE_PERSONALITY";

// Pirate personality thresholds
static const int TREASURE_DETECTION_BOOST = 40;
static const int AGGRESSION_ESCALATION_RATE = 15;
static const int SUSPICION_BASE_LEVEL = 60;
static const int EYE_LOSS_COMPENSATION_FACTOR = 25;

// Pirate-specific mood modifiers
static const float ANGER_AMPLIFICATION = 1.5f;
static const float FEAR_SUPPRESSION = 0.3f;
static const float CURIOSITY_TREASURE_BOOST = 2.0f;
static const float HUNGER_INTENSITY = 1.8f;

// Behavioral state tracking
static uint32_t last_treasure_response = 0;
static uint32_t threat_escalation_timer = 0;
static bool left_side_defensive_mode = false;
static uint8_t current_suspicion_level = SUSPICION_BASE_LEVEL;

void goblin_pirate_personality_init(void) {
    ESP_LOGI(TAG, "Initializing Pirate Personality Engine");
    ESP_LOGI(TAG, "  Archetype: PIRATE_SCOUNDREL");
    ESP_LOGI(TAG, "  Eye Loss Compensation: ACTIVE");
    ESP_LOGI(TAG, "  Treasure Obsession: HIGH");
    ESP_LOGI(TAG, "  Base Aggression: ENHANCED");
    ESP_LOGI(TAG, "  Suspicion Level: %d%%", SUSPICION_BASE_LEVEL);
    
    // Initialize pirate-specific state
    current_suspicion_level = SUSPICION_BASE_LEVEL;
    left_side_defensive_mode = true;  // Protect blind side
    
    ESP_LOGI(TAG, "Pirate personality engine ready - Terror of the Seas mode active");
}

void goblin_pirate_personality_act(void) {
    // hitCount: 30 -> executes every 3 seconds
    if ((g_loopCount % 30) != 0) return;
    
    // Get shared mood state
    Mood* mood = GSM.read<Mood>();
    
    if (!mood) return;  // Safety check
    
    // Apply pirate-specific personality modifiers
    apply_pirate_aggression_boost(mood);
    handle_treasure_obsession_behavior(mood);
    implement_eye_loss_compensation(mood);
    manage_pirate_mood_dynamics(mood);
    execute_territorial_behaviors(mood);
    
    // Synchronize changes back to other subsystems
    GSM.write<Mood>();
    
    if ((g_loopCount % 150) == 0) {  // Every 15 seconds
        ESP_LOGD(TAG, "Pirate state: Suspicion=%d%%, Aggression=%d, Treasure_Hunt=%s",
            current_suspicion_level,
            mood->anger(),
            (mood->curiosity() > 50) ? "ACTIVE" : "DORMANT");
    }
}

static void apply_pirate_aggression_boost(Mood* mood) {
    // Pirates are naturally more aggressive due to harsh life
    int base_anger = mood->anger();
    int boosted_anger = (int)(base_anger * ANGER_AMPLIFICATION);
    
    // Add escalation for repeated threats (simplified without distance sensor)
    threat_escalation_timer++;
    if (threat_escalation_timer > 3) {  // Sustained presence
        boosted_anger += AGGRESSION_ESCALATION_RATE;
        ESP_LOGD(TAG, "Threat escalation: %d -> %d anger", base_anger, boosted_anger);
    }
    
    // Cap anger to prevent overflow
    mood->anger() = (boosted_anger > 100) ? 100 : boosted_anger;
    
    // Suppress fear - pirates are battle-hardened
    mood->fear() = (int)(mood->fear() * FEAR_SUPPRESSION);
}

static void handle_treasure_obsession_behavior(Mood* mood) {
    // Simulate treasure detection based on time patterns (simplified without distance sensor)
    bool potential_treasure_detected = false;
    
    // Periodic treasure hunting behavior
    if ((g_loopCount % 100) == 0) {  // Every 10 seconds
        potential_treasure_detected = true;
        last_treasure_response = g_loopCount;
    }
    
    // Boost curiosity for treasure hunting
    if (potential_treasure_detected || (g_loopCount - last_treasure_response) < 100) {
        int treasure_curiosity = (int)(mood->curiosity() * CURIOSITY_TREASURE_BOOST);
        mood->curiosity() = (treasure_curiosity > 100) ? 100 : treasure_curiosity;
        
        // Increase excitement when treasure hunting (greed)
        int treasure_excitement = (int)(mood->excitement() * HUNGER_INTENSITY);
        mood->excitement() = (treasure_excitement > 100) ? 100 : treasure_excitement;
        
        ESP_LOGD(TAG, "TREASURE HUNT MODE: Curiosity boosted to %d", mood->curiosity());
    }
}

static void implement_eye_loss_compensation(Mood* mood) {
    // Missing right eye - compensate with enhanced suspicion
    if (left_side_defensive_mode) {
        // Boost suspicion periodically
        current_suspicion_level += EYE_LOSS_COMPENSATION_FACTOR;
        if (current_suspicion_level > 100) current_suspicion_level = 100;
        
        // Translate suspicion into irritation
        mood->irritation() += (current_suspicion_level - SUSPICION_BASE_LEVEL) / 4;
        if (mood->irritation() > 100) mood->irritation() = 100;
    } else {
        // Gradually reduce suspicion when not in defensive mode
        if (current_suspicion_level > SUSPICION_BASE_LEVEL) {
            current_suspicion_level--;
        }
    }
    
    // Eye loss creates hypervigilance
    if (mood->curiosity() < 30) {
        mood->curiosity() += 10;  // Always scanning for threats
    }
}

static void manage_pirate_mood_dynamics(Mood* mood) {
    // Pirates have volatile mood swings
    static uint8_t mood_cycle_counter = 0;
    mood_cycle_counter++;
    
    // Periodic contentment drops (restless pirate nature)
    if ((mood_cycle_counter % 20) == 0) {
        if (mood->contentment() > 30) {
            mood->contentment() -= 15;  // Pirates are never truly content
            mood->irritation() += 10;    // Always wanting more (greed as irritation)
        }
    }
    
    // Affection remains very low (trust issues from pirate life)
    if (mood->affection() > 10) {
        mood->affection() = 5;  // Pirates don't trust easily
    }
    
    // Happiness is rare and brief for pirates
    if (mood->happiness() > 20) {
        mood->happiness() = (mood->happiness() * 3) / 4;  // 25% decay
    }
}

static void execute_territorial_behaviors(Mood* mood) {
    // Pirates are extremely territorial - simplified without distance sensor
    static uint8_t territorial_counter = 0;
    territorial_counter++;
    
    // Periodic territorial behavior
    if ((territorial_counter % 50) == 0) {  // Every 5 seconds
        mood->anger() += 5;
        mood->irritation() += 10;
        
        // Reduce contentment (territory defense)
        if (mood->contentment() > 10) {
            mood->contentment() -= 8;
        }
        
        ESP_LOGD(TAG, "TERRITORIAL DEFENSE: Anger=%d", mood->anger());
    }
    
    // Occasional combat readiness
    if ((territorial_counter % 100) == 0) {  // Every 10 seconds
        mood->anger() += 10;
        mood->fear() = (mood->fear() * 2) / 3;  // Reduce fear, increase aggression
        
        ESP_LOGW(TAG, "COMBAT READINESS: Preparing for battle");
    }
}