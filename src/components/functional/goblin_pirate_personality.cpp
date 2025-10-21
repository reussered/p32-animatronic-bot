/**
 * @file goblin_pirate_personality.cpp
 * @brief Pirate-specific personality engine for one-eyed goblin variants
 * 
 * Implements enhanced aggression, treasure obsession, and compensatory behaviors
 * for battle-scarred goblins who lost their right eye in combat.
 */

#include "esp_log.h"
#include "p32_shared_state.h"

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
    
    SharedMemory local_state;
    local_state.read();
    
    // Apply pirate-specific personality modifiers
    apply_pirate_aggression_boost(local_state);
    handle_treasure_obsession_behavior(local_state);
    implement_eye_loss_compensation(local_state);
    manage_pirate_mood_dynamics(local_state);
    execute_territorial_behaviors(local_state);
    
    // Update shared state with pirate modifications
    local_state.write();
    
    if ((g_loopCount % 150) == 0) {  // Every 15 seconds
        ESP_LOGD(TAG, "Pirate state: Suspicion=%d%%, Aggression=%d, Treasure_Hunt=%s",
            current_suspicion_level,
            local_state.mood.ANGER,
            (local_state.mood.CURIOSITY > 50) ? "ACTIVE" : "DORMANT");
    }
}

static void apply_pirate_aggression_boost(SharedMemory& state) {
    // Pirates are naturally more aggressive due to harsh life
    int base_anger = state.mood.ANGER;
    int boosted_anger = (int)(base_anger * ANGER_AMPLIFICATION);
    
    // Add escalation for repeated threats
    if (state.sensor_distance > 0 && state.sensor_distance < 30) {
        threat_escalation_timer++;
        if (threat_escalation_timer > 3) {  // Sustained close presence
            boosted_anger += AGGRESSION_ESCALATION_RATE;
            ESP_LOGD(TAG, "Threat escalation: %d -> %d anger", base_anger, boosted_anger);
        }
    } else {
        threat_escalation_timer = 0;  // Reset when threat moves away
    }
    
    // Cap anger to prevent overflow
    state.mood.ANGER = (boosted_anger > 100) ? 100 : boosted_anger;
    
    // Suppress fear - pirates are battle-hardened
    state.mood.FEAR = (int)(state.mood.FEAR * FEAR_SUPPRESSION);
}

static void handle_treasure_obsession_behavior(SharedMemory& state) {
    // Simulate treasure detection based on movement patterns
    bool potential_treasure_detected = false;
    
    // Rapid movements might indicate hiding/revealing treasure
    if (state.sensor_distance > 0) {
        static uint8_t last_distance = 0;
        uint8_t distance_change = abs(state.sensor_distance - last_distance);
        
        if (distance_change > 20) {  // Rapid movement
            potential_treasure_detected = true;
            last_treasure_response = g_loopCount;
        }
        last_distance = state.sensor_distance;
    }
    
    // Boost curiosity for treasure hunting
    if (potential_treasure_detected || (g_loopCount - last_treasure_response) < 100) {
        int treasure_curiosity = (int)(state.mood.CURIOSITY * CURIOSITY_TREASURE_BOOST);
        state.mood.CURIOSITY = (treasure_curiosity > 100) ? 100 : treasure_curiosity;
        
        // Increase hunger when treasure hunting (greed)
        int treasure_hunger = (int)(state.mood.HUNGER * HUNGER_INTENSITY);
        state.mood.HUNGER = (treasure_hunger > 100) ? 100 : treasure_hunger;
        
        ESP_LOGD(TAG, "TREASURE HUNT MODE: Curiosity boosted to %d", state.mood.CURIOSITY);
    }
}

static void implement_eye_loss_compensation(SharedMemory& state) {
    // Missing right eye - compensate with enhanced left-side awareness
    if (left_side_defensive_mode) {
        // Boost suspicion when activity detected
        if (state.sensor_distance > 0 && state.sensor_distance < 60) {
            current_suspicion_level += EYE_LOSS_COMPENSATION_FACTOR;
            if (current_suspicion_level > 100) current_suspicion_level = 100;
            
            // Translate suspicion into irritation and caution
            state.mood.IRRITATION += (current_suspicion_level - SUSPICION_BASE_LEVEL) / 4;
            if (state.mood.IRRITATION > 100) state.mood.IRRITATION = 100;
        } else {
            // Gradually reduce suspicion when no threats
            if (current_suspicion_level > SUSPICION_BASE_LEVEL) {
                current_suspicion_level--;
            }
        }
    }
    
    // Eye loss creates hypervigilance
    if (state.mood.CURIOSITY < 30 && state.sensor_distance == 0) {
        state.mood.CURIOSITY += 10;  // Always scanning for threats
    }
}

static void manage_pirate_mood_dynamics(SharedMemory& state) {
    // Pirates have volatile mood swings
    static uint8_t mood_cycle_counter = 0;
    mood_cycle_counter++;
    
    // Periodic contentment drops (restless pirate nature)
    if ((mood_cycle_counter % 20) == 0) {
        if (state.mood.CONTENTMENT > 30) {
            state.mood.CONTENTMENT -= 15;  // Pirates are never truly content
            state.mood.HUNGER += 10;       // Always wanting more
        }
    }
    
    // Affection remains very low (trust issues from pirate life)
    if (state.mood.AFFECTION > 10) {
        state.mood.AFFECTION = 5;  // Pirates don't trust easily
    }
    
    // Happiness is rare and brief for pirates
    if (state.mood.HAPPINESS > 20) {
        state.mood.HAPPINESS = (state.mood.HAPPINESS * 3) / 4;  // 25% decay
    }
}

static void execute_territorial_behaviors(SharedMemory& state) {
    // Pirates are extremely territorial
    if (state.sensor_distance > 0 && state.sensor_distance < 40) {
        // Close proximity triggers territorial response
        state.mood.ANGER += 5;
        state.mood.IRRITATION += 10;
        
        // Reduce contentment (territory invasion)
        if (state.mood.CONTENTMENT > 10) {
            state.mood.CONTENTMENT -= 8;
        }
        
        ESP_LOGD(TAG, "TERRITORIAL DEFENSE: Distance=%dcm, Anger=%d", 
            state.sensor_distance, state.mood.ANGER);
    }
    
    // Very close proximity triggers combat readiness
    if (state.sensor_distance > 0 && state.sensor_distance < 20) {
        state.mood.ANGER += 10;
        state.mood.FEAR = (state.mood.FEAR * 2) / 3;  // Reduce fear, increase aggression
        
        ESP_LOGW(TAG, "COMBAT READINESS: Preparing for battle at %dcm", state.sensor_distance);
    }
}