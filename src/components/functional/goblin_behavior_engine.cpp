// src/components/functional/goblin_behavior_engine.cpp
// Goblin Behavior Engine - Selects actions based on mood and personality
// Component Type: FAMILY_LEVEL (shared across all goblin family bots)
// Timing: hitCount 10 (executes every 1 second)

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "Mood.hpp"

#ifdef ENABLE_GOBLIN_COMPONENTS

static const char *TAG = "goblin_behavior_engine";

// Behavior states
typedef enum {
    BEHAVIOR_IDLE,
    BEHAVIOR_CURIOUS,
    BEHAVIOR_AGGRESSIVE,
    BEHAVIOR_FEARFUL,
    BEHAVIOR_CONTENT
} behavior_state_t;

static behavior_state_t s_current_behavior = BEHAVIOR_IDLE;
static uint32_t s_behavior_start_loop = 0;

// Behavior transition thresholds
#define IDLE_TO_CURIOUS_THRESHOLD 50
#define CURIOUS_TO_AGGRESSIVE_THRESHOLD 70
#define AGGRESSIVE_TO_FEARFUL_THRESHOLD 80
#define ANY_TO_CONTENT_THRESHOLD 60
#define MIN_BEHAVIOR_DURATION_LOOPS 30  // 3 seconds minimum

// NO ARGUMENTS - Init function signature
extern "C" esp_err_t goblin_behavior_engine_init(void) {
    ESP_LOGI(TAG, "Initializing goblin behavior engine...");
    ESP_LOGI(TAG, "Behavior States: IDLE, CURIOUS, AGGRESSIVE, FEARFUL, CONTENT");
    ESP_LOGI(TAG, "Transition thresholds:");
    ESP_LOGI(TAG, "  Idle->Curious: %d", IDLE_TO_CURIOUS_THRESHOLD);
    ESP_LOGI(TAG, "  Curious->Aggressive: %d", CURIOUS_TO_AGGRESSIVE_THRESHOLD);
    ESP_LOGI(TAG, "  Aggressive->Fearful: %d", AGGRESSIVE_TO_FEARFUL_THRESHOLD);
    ESP_LOGI(TAG, "  Any->Content: %d", ANY_TO_CONTENT_THRESHOLD);
    
    s_current_behavior = BEHAVIOR_IDLE;
    s_behavior_start_loop = g_loopCount;
    
    ESP_LOGI(TAG, "Behavior engine initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

static const char* behavior_to_string(behavior_state_t behavior) {
    switch(behavior) {
        case BEHAVIOR_IDLE: return "IDLE";
        case BEHAVIOR_CURIOUS: return "CURIOUS";
        case BEHAVIOR_AGGRESSIVE: return "AGGRESSIVE";
        case BEHAVIOR_FEARFUL: return "FEARFUL";
        case BEHAVIOR_CONTENT: return "CONTENT";
        default: return "UNKNOWN";
    }
}

// NO ARGUMENTS - Act function signature
extern "C" void goblin_behavior_engine_act(void) {
    // hitCount: 10 -> executes every 1 second
    
    // Read current mood values
    int anger = g_mood.getAnger();
    int fear = g_mood.getFear();
    int curiosity = g_mood.getCuriosity();
    int contentment = g_mood.getContentment();
    
    // Calculate dominant mood
    behavior_state_t new_behavior = s_current_behavior;
    
    // Check if minimum behavior duration has elapsed
    uint32_t behavior_duration = g_loopCount - s_behavior_start_loop;
    if (behavior_duration < MIN_BEHAVIOR_DURATION_LOOPS) {
        // Stay in current behavior (minimum duration not met)
        ESP_LOGV(TAG, "Behavior held: %s (duration: %u loops)",
                 behavior_to_string(s_current_behavior), behavior_duration);
        return;
    }
    
    // State transitions based on mood thresholds
    if (contentment > ANY_TO_CONTENT_THRESHOLD) {
        new_behavior = BEHAVIOR_CONTENT;
    } else if (fear > AGGRESSIVE_TO_FEARFUL_THRESHOLD) {
        new_behavior = BEHAVIOR_FEARFUL;
    } else if (anger > CURIOUS_TO_AGGRESSIVE_THRESHOLD) {
        new_behavior = BEHAVIOR_AGGRESSIVE;
    } else if (curiosity > IDLE_TO_CURIOUS_THRESHOLD) {
        new_behavior = BEHAVIOR_CURIOUS;
    } else {
        new_behavior = BEHAVIOR_IDLE;
    }
    
    // Log behavior transition
    if (new_behavior != s_current_behavior) {
        ESP_LOGI(TAG, "Behavior transition at loop %u: %s -> %s",
                 g_loopCount,
                 behavior_to_string(s_current_behavior),
                 behavior_to_string(new_behavior));
        ESP_LOGI(TAG, "  Mood values: A=%d F=%d C=%d Co=%d",
                 anger, fear, curiosity, contentment);
        
        s_current_behavior = new_behavior;
        s_behavior_start_loop = g_loopCount;
        
        // Update shared state
        g_shared_state.current_behavior = (uint8_t)new_behavior;
        
        // Mark state dirty to trigger mesh broadcast
        // mesh_mark_state_dirty();
    }
    
    // Execute current behavior actions
    switch(s_current_behavior) {
        case BEHAVIOR_IDLE:
            // Idle animation and sounds
            ESP_LOGV(TAG, "Executing IDLE behavior");
            break;
            
        case BEHAVIOR_CURIOUS:
            // Curious animation (wide eyes, head tilt)
            ESP_LOGV(TAG, "Executing CURIOUS behavior");
            break;
            
        case BEHAVIOR_AGGRESSIVE:
            // Aggressive animation (narrow eyes, growl)
            ESP_LOGV(TAG, "Executing AGGRESSIVE behavior");
            break;
            
        case BEHAVIOR_FEARFUL:
            // Fearful animation (wide eyes, recoil)
            ESP_LOGV(TAG, "Executing FEARFUL behavior");
            break;
            
        case BEHAVIOR_CONTENT:
            // Content animation (relaxed expression)
            ESP_LOGV(TAG, "Executing CONTENT behavior");
            break;
    }
}

#endif // ENABLE_GOBLIN_COMPONENTS
