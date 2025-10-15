// src/components/functional/goblin_personality.cpp
// Goblin Personality Component - Family personality traits
// Component Type: FAMILY_LEVEL (shared across all goblin family bots)
// Timing: hitCount 25 (executes every 2.5 seconds)

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "Mood.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

static const char *TAG = "goblin_personality";

// Goblin personality traits
#define GOBLIN_BASE_AGGRESSION 60
#define GOBLIN_BASE_CURIOSITY 70
#define GOBLIN_BASE_FEAR 20
#define GOBLIN_APPROACH_DISTANCE_CM 50
#define GOBLIN_FLEE_DISTANCE_CM 20

// NO ARGUMENTS - Init function signature
extern "C" esp_err_t goblin_personality_init(void) {
    ESP_LOGI(TAG, "Initializing goblin personality...");
    ESP_LOGI(TAG, "Traits: Mischievous, Aggressive, Curious, Easily Startled");
    ESP_LOGI(TAG, "Base values: Aggression=%d, Curiosity=%d, Fear=%d",
             GOBLIN_BASE_AGGRESSION, GOBLIN_BASE_CURIOSITY, GOBLIN_BASE_FEAR);
    
    // Initialize mood with goblin personality defaults
    g_mood.setAnger(GOBLIN_BASE_AGGRESSION);
    g_mood.setCuriosity(GOBLIN_BASE_CURIOSITY);
    g_mood.setFear(GOBLIN_BASE_FEAR);
    g_mood.setContentment(40);
    
    // Serialize to shared state for mesh synchronization
    mood_serialize();
    
    ESP_LOGI(TAG, "Goblin personality initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
extern "C" void goblin_personality_act(void) {
    // hitCount: 25 → executes every 2.5 seconds
    
    // Apply personality-driven mood adjustments based on sensor inputs
    uint8_t distance = g_shared_state.distance_cm;
    
    // Goblin is CURIOUS about nearby objects
    if (distance > 0 && distance < GOBLIN_APPROACH_DISTANCE_CM) {
        g_mood.addCuriosity(5);  // Increase curiosity
        g_mood.addExcitement(3); // Slight excitement
        
        ESP_LOGD(TAG, "Object detected at %dcm - increasing curiosity", distance);
    }
    
    // Goblin is AGGRESSIVE when approached too closely
    if (distance > 0 && distance < GOBLIN_FLEE_DISTANCE_CM) {
        g_mood.addAnger(10);     // Increase anger
        g_mood.addFear(5);       // Slight fear
        g_mood.addIrritation(8); // Irritation at invasion
        
        ESP_LOGD(TAG, "Too close at %dcm - increasing aggression", distance);
    }
    
    // Natural mood decay toward personality baseline
    if (g_loopCount % 40 == 0) {  // Every 4 seconds
        // Decay toward baseline aggression
        int current_anger = g_mood.getAnger();
        if (current_anger > GOBLIN_BASE_AGGRESSION) {
            g_mood.addAnger(-2);
        } else if (current_anger < GOBLIN_BASE_AGGRESSION) {
            g_mood.addAnger(1);
        }
        
        // Decay toward baseline curiosity
        int current_curiosity = g_mood.getCuriosity();
        if (current_curiosity > GOBLIN_BASE_CURIOSITY) {
            g_mood.addCuriosity(-2);
        } else if (current_curiosity < GOBLIN_BASE_CURIOSITY) {
            g_mood.addCuriosity(1);
        }
    }
    
    // Serialize updated mood to shared state
    mood_serialize();
    
    ESP_LOGV(TAG, "Personality tick at loop %u: ANGER=%d, CURIOSITY=%d, FEAR=%d",
             g_loopCount, g_mood.getAnger(), g_mood.getCuriosity(), g_mood.getFear());
}

#endif // ENABLE_GOBLIN_COMPONENTS
