#include "p32_component_config.h"

#ifdef ENABLE_SIMPLE_MOOD
#include "p32_core.h"

static const char* TAG = "P32_MOOD_SIMPLE";
static p32_mood_t current_mood = MOOD_CONTENTMENT;

// Simple mood names for logging
static const char* mood_names[] = {
    "FEAR", "ANGER", "IRRITATION", "HAPPINESS", 
    "CONTENTMENT", "HUNGER", "CURIOSITY", "AFFECTION"
};

esp_err_t p32_mood_engine_init(void) {
    ESP_LOGI(TAG, "Simple mood engine initialized");
    ESP_LOGI(TAG, "- 8 emotional states available");
    ESP_LOGI(TAG, "- Starting mood: %s", mood_names[current_mood]);
    return ESP_OK;
}

esp_err_t p32_set_mood(p32_mood_t new_mood) {
    if (new_mood >= MOOD_COUNT) {
        ESP_LOGW(TAG, "Invalid mood: %d", new_mood);
        return ESP_ERR_INVALID_ARG;
    }
    
    int old_mood = current_mood;
    current_mood = new_mood;
    
    ESP_LOGI(TAG, "Mood changed: %s -> %s", 
             mood_names[old_mood], mood_names[current_mood]);
    
    // Trigger simple responses based on mood
    switch (current_mood) {
        case MOOD_FEAR:
            ESP_LOGI(TAG, "Behavior: Eyes wide, step back");
            break;
        case MOOD_ANGER:
            ESP_LOGI(TAG, "Behavior: Eyes narrow, lean forward");
            break;
        case MOOD_IRRITATION:
            ESP_LOGI(TAG, "Behavior: Eye roll, head shake");
            break;
        case MOOD_HAPPINESS:
            ESP_LOGI(TAG, "Behavior: Eyes brighten, smile");
            break;
        case MOOD_CONTENTMENT:
            ESP_LOGI(TAG, "Behavior: Calm expression, slow blink");
            break;
        case MOOD_HUNGER:
            ESP_LOGI(TAG, "Behavior: Eyes scan, mouth open");
            break;
        case MOOD_CURIOSITY:
            ESP_LOGI(TAG, "Behavior: Head tilt, eyes track");
            break;
        case MOOD_AFFECTION:
            ESP_LOGI(TAG, "Behavior: Warm eyes, gentle movement");
            break;
        case MOOD_COUNT:
            // This shouldn't happen in normal operation
            ESP_LOGW(TAG, "Invalid mood state: MOOD_COUNT");
            break;
    }
    
    return ESP_OK;
}

p32_mood_t p32_get_mood(void) {
    return current_mood;
}

esp_err_t p32_mood_process_stimulus(float distance, bool motion, bool touch) {
    ESP_LOGD(TAG, "Processing stimulus: dist=%.1f, motion=%d, touch=%d", 
             distance, motion, touch);
    
    // Simple mood changes based on input
    if (touch) {
        p32_set_mood(7); // AFFECTION
    } else if (distance < 10.0f && motion) {
        p32_set_mood(0); // FEAR  
    } else if (distance < 20.0f && motion) {
        p32_set_mood(6); // CURIOSITY
    } else if (motion) {
        p32_set_mood(3); // HAPPINESS
    }
    
    return ESP_OK;
}

#endif // ENABLE_SIMPLE_MOOD
