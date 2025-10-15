// src/components/functional/goblin_mood.cpp
// Goblin Mood Component - 8-mood emotional state machine
// Component Type: FAMILY_LEVEL (shared across all goblin family bots)
// Timing: hitCount 15 (executes every 1.5 seconds)

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "Mood.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

static const char *TAG = "goblin_mood";

// Mood decay rates (per tick)
#define ANGER_DECAY_RATE 2
#define FEAR_DECAY_RATE 3
#define HAPPINESS_DECAY_RATE 2
#define SADNESS_DECAY_RATE 1
#define CURIOSITY_DECAY_RATE 4
#define AFFECTION_DECAY_RATE 2
#define IRRITATION_DECAY_RATE 5
#define CONTENTMENT_DECAY_RATE 1
#define EXCITEMENT_DECAY_RATE 6

// NO ARGUMENTS - Init function signature
extern "C" esp_err_t goblin_mood_init(void) {
    ESP_LOGI(TAG, "Initializing goblin mood system...");
    ESP_LOGI(TAG, "8 Mood Components: ANGER, FEAR, HAPPINESS, SADNESS, CURIOSITY, AFFECTION, IRRITATION, CONTENTMENT");
    ESP_LOGI(TAG, "Value range: -128 to +127, Natural decay enabled");
    
    // Initialize all mood components to neutral (handled by personality component)
    ESP_LOGI(TAG, "Mood values initialized by personality component");
    
    // Serialize to shared state
    mood_serialize();
    
    ESP_LOGI(TAG, "Goblin mood system initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
extern "C" void goblin_mood_act(void) {
    // hitCount: 15 → executes every 1.5 seconds
    
    // Apply natural mood decay
    // Moods decay toward neutral (0) over time
    
    int current_anger = g_mood.getAnger();
    if (current_anger > 0) {
        g_mood.addAnger(-ANGER_DECAY_RATE);
    } else if (current_anger < 0) {
        g_mood.addAnger(ANGER_DECAY_RATE);
    }
    
    int current_fear = g_mood.getFear();
    if (current_fear > 0) {
        g_mood.addFear(-FEAR_DECAY_RATE);
    } else if (current_fear < 0) {
        g_mood.addFear(FEAR_DECAY_RATE);
    }
    
    int current_happiness = g_mood.getHappiness();
    if (current_happiness > 0) {
        g_mood.addHappiness(-HAPPINESS_DECAY_RATE);
    } else if (current_happiness < 0) {
        g_mood.addHappiness(HAPPINESS_DECAY_RATE);
    }
    
    int current_sadness = g_mood.getSadness();
    if (current_sadness > 0) {
        g_mood.addSadness(-SADNESS_DECAY_RATE);
    } else if (current_sadness < 0) {
        g_mood.addSadness(SADNESS_DECAY_RATE);
    }
    
    int current_curiosity = g_mood.getCuriosity();
    if (current_curiosity > 0) {
        g_mood.addCuriosity(-CURIOSITY_DECAY_RATE);
    } else if (current_curiosity < 0) {
        g_mood.addCuriosity(CURIOSITY_DECAY_RATE);
    }
    
    int current_affection = g_mood.getAffection();
    if (current_affection > 0) {
        g_mood.addAffection(-AFFECTION_DECAY_RATE);
    } else if (current_affection < 0) {
        g_mood.addAffection(AFFECTION_DECAY_RATE);
    }
    
    int current_irritation = g_mood.getIrritation();
    if (current_irritation > 0) {
        g_mood.addIrritation(-IRRITATION_DECAY_RATE);
    } else if (current_irritation < 0) {
        g_mood.addIrritation(IRRITATION_DECAY_RATE);
    }
    
    int current_contentment = g_mood.getContentment();
    if (current_contentment > 0) {
        g_mood.addContentment(-CONTENTMENT_DECAY_RATE);
    } else if (current_contentment < 0) {
        g_mood.addContentment(CONTENTMENT_DECAY_RATE);
    }
    
    int current_excitement = g_mood.getExcitement();
    if (current_excitement > 0) {
        g_mood.addExcitement(-EXCITEMENT_DECAY_RATE);
    } else if (current_excitement < 0) {
        g_mood.addExcitement(EXCITEMENT_DECAY_RATE);
    }
    
    // Serialize updated mood to shared state
    mood_serialize();
    
    ESP_LOGV(TAG, "Mood decay at loop %u: A=%d F=%d H=%d S=%d C=%d Af=%d I=%d Co=%d E=%d",
             g_loopCount,
             g_mood.getAnger(),
             g_mood.getFear(),
             g_mood.getHappiness(),
             g_mood.getSadness(),
             g_mood.getCuriosity(),
             g_mood.getAffection(),
             g_mood.getIrritation(),
             g_mood.getContentment(),
             g_mood.getExcitement());
}

#endif // ENABLE_GOBLIN_COMPONENTS
