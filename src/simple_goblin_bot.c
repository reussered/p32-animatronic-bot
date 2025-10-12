#include "p32_core.h"

static const char* TAG = "SIMPLE_GOBLIN_BOT";

// ========================================
// COMPONENT FUNCTION POINTER STRUCTURES
// ========================================
typedef struct {
    esp_err_t (*init)(void);
    esp_err_t (*show_eyes)(const char* expression);
    esp_err_t (*show_mouth)(const char* expression);
} simple_display_component_t;

typedef struct {
    esp_err_t (*init)(void);
    esp_err_t (*play)(const char* sound);
} simple_audio_component_t;

typedef struct {
    esp_err_t (*init)(void);
    float (*read_distance)(void);
} simple_sensor_component_t;

typedef struct {
    esp_err_t (*init)(void);
    esp_err_t (*set_mood)(int mood);
    int (*get_mood)(void);
} simple_mood_component_t;

typedef struct {
    esp_err_t (*init)(void);
    esp_err_t (*react_to_distance)(float distance);
    esp_err_t (*random_behavior)(void);
} simple_goblin_component_t;

// ========================================
// SIMPLE DISPLAY COMPONENT
// ========================================
esp_err_t simple_display_init(void) {
    ESP_LOGI(TAG, "Simple Display: 3x GC9A01 displays initialized (SPI pins 12,13,14 + CS 15,16,17)");
    return ESP_OK;
}

esp_err_t simple_display_show_eyes(const char* expression) {
    ESP_LOGI(TAG, "Simple Display: Eyes showing '%s'", expression);
    return ESP_OK;
}

esp_err_t simple_display_show_mouth(const char* expression) {
    ESP_LOGI(TAG, "Simple Display: Mouth showing '%s'", expression);
    return ESP_OK;
}

// ========================================
// SIMPLE AUDIO COMPONENT  
// ========================================
esp_err_t simple_audio_init(void) {
    ESP_LOGI(TAG, "Simple Audio: I2S speaker initialized (pins 4,5,6)");
    return ESP_OK;
}

esp_err_t simple_audio_play(const char* sound) {
    ESP_LOGI(TAG, "Simple Audio: Playing '%s'", sound);
    return ESP_OK;
}

// ========================================
// SIMPLE SENSOR COMPONENT
// ========================================
static float simulated_distance = 50.0f;

esp_err_t simple_sensor_init(void) {
    ESP_LOGI(TAG, "Simple Sensor: HC-SR04 ultrasonic initialized (pins 9,10)");
    return ESP_OK;
}

float simple_sensor_read_distance(void) {
    // Simulate varying distance
    simulated_distance += (float)(esp_random() % 20 - 10) / 5.0f;
    if (simulated_distance < 5.0f) simulated_distance = 5.0f;
    if (simulated_distance > 100.0f) simulated_distance = 100.0f;
    
    ESP_LOGI(TAG, "Simple Sensor: Distance %.1f cm", simulated_distance);
    return simulated_distance;
}

// ========================================
// SIMPLE MOOD COMPONENT
// ========================================
typedef enum {
    MOOD_FEAR = 0,
    MOOD_ANGER = 1, 
    MOOD_IRRITATION = 2,
    MOOD_HAPPINESS = 3,
    MOOD_CONTENTMENT = 4,
    MOOD_HUNGER = 5,
    MOOD_CURIOSITY = 6,
    MOOD_AFFECTION = 7
} simple_mood_t;

static simple_mood_t current_mood = MOOD_CONTENTMENT;
static const char* mood_names[] = {
    "FEAR", "ANGER", "IRRITATION", "HAPPINESS", 
    "CONTENTMENT", "HUNGER", "CURIOSITY", "AFFECTION"
};

esp_err_t simple_mood_init(void) {
    ESP_LOGI(TAG, "Simple Mood: Engine initialized, starting mood: %s", mood_names[current_mood]);
    return ESP_OK;
}

esp_err_t simple_mood_set(simple_mood_t new_mood) {
    if (new_mood != current_mood) {
        ESP_LOGI(TAG, "Simple Mood: %s -> %s", mood_names[current_mood], mood_names[new_mood]);
        current_mood = new_mood;
    }
    return ESP_OK;
}

simple_mood_t simple_mood_get(void) {
    return current_mood;
}

// ========================================
// SIMPLE GOBLIN PERSONALITY COMPONENT
// ========================================
esp_err_t simple_goblin_init(void) {
    ESP_LOGI(TAG, "Simple Goblin: Personality initialized - mischievous and curious!");
    return ESP_OK;
}

esp_err_t simple_goblin_react_to_distance(float distance) {
    if (distance < 15.0f) {
        ESP_LOGI(TAG, "Simple Goblin: *VERY CURIOUS* - something close!");
        simple_mood_set(MOOD_CURIOSITY);
        simple_display_show_eyes("wide_curious");
        simple_display_show_mouth("open_surprise");
        simple_audio_play("curious_chirp");
        
    } else if (distance < 30.0f) {
        ESP_LOGI(TAG, "Simple Goblin: *interested* - watching...");
        simple_mood_set(MOOD_CURIOSITY);
        simple_display_show_eyes("tracking");
        simple_display_show_mouth("slight_smile");
        
    } else if (distance < 60.0f) {
        ESP_LOGI(TAG, "Simple Goblin: *content* - all is well");
        simple_mood_set(MOOD_CONTENTMENT);
        simple_display_show_eyes("calm");
        simple_display_show_mouth("neutral");
        
    } else {
        ESP_LOGI(TAG, "Simple Goblin: *bored* - nothing happening...");
        simple_mood_set(MOOD_CONTENTMENT);
        simple_display_show_eyes("sleepy_blink");
        simple_display_show_mouth("yawn");
    }
    
    return ESP_OK;
}

esp_err_t simple_goblin_random_behavior(void) {
    static uint32_t behavior_counter = 0;
    behavior_counter++;
    
    // Random goblin mischief every so often
    if ((esp_random() % 100) < 5) { // 5% chance
        switch (behavior_counter % 4) {
            case 0:
                ESP_LOGI(TAG, "Simple Goblin: *mischievous grin*");
                simple_display_show_eyes("sly_wink");
                simple_display_show_mouth("evil_grin");
                simple_audio_play("goblin_giggle");
                simple_mood_set(MOOD_HAPPINESS);
                break;
                
            case 1:
                ESP_LOGI(TAG, "Simple Goblin: *suspicious glare*");
                simple_display_show_eyes("narrow_suspicious");
                simple_display_show_mouth("frown");
                simple_mood_set(MOOD_IRRITATION);
                break;
                
            case 2:
                ESP_LOGI(TAG, "Simple Goblin: *ear twitch*");
                simple_audio_play("ear_twitch_sound");
                break;
                
            case 3:
                ESP_LOGI(TAG, "Simple Goblin: *curious head tilt*");
                simple_display_show_eyes("tilted_curious");
                simple_mood_set(MOOD_CURIOSITY);
                break;
        }
    }
    
    return ESP_OK;
}

// ========================================
// COMPONENT INSTANCES WITH FUNCTION POINTERS
// ========================================
static simple_display_component_t display_component = {
    .init = simple_display_init,
    .show_eyes = simple_display_show_eyes,
    .show_mouth = simple_display_show_mouth
};

static simple_audio_component_t audio_component = {
    .init = simple_audio_init,
    .play = simple_audio_play
};

static simple_sensor_component_t sensor_component = {
    .init = simple_sensor_init,
    .read_distance = simple_sensor_read_distance
};

static simple_mood_component_t mood_component = {
    .init = simple_mood_init,
    .set_mood = (esp_err_t (*)(int))simple_mood_set,
    .get_mood = (int (*)(void))simple_mood_get
};

static simple_goblin_component_t goblin_component = {
    .init = simple_goblin_init,
    .react_to_distance = simple_goblin_react_to_distance,
    .random_behavior = simple_goblin_random_behavior
};

// ========================================
// SIMPLE GOBLIN BOT MAIN PROGRAM
// ========================================
void app_main() {
    ESP_LOGI(TAG, "=== SIMPLE GOBLIN BOT STARTING ===");
    ESP_LOGI(TAG, "ESP32-S3 DevKit, ESP-IDF %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Free heap: %lu bytes", esp_get_free_heap_size());
    
    // Initialize all simple components via function pointers
    ESP_LOGI(TAG, "Initializing simple goblin components...");
    ESP_ERROR_CHECK(display_component.init());
    ESP_ERROR_CHECK(audio_component.init());
    ESP_ERROR_CHECK(sensor_component.init());
    ESP_ERROR_CHECK(mood_component.init());
    ESP_ERROR_CHECK(goblin_component.init());
    
    ESP_LOGI(TAG, "=== SIMPLE GOBLIN BOT READY ===");
    ESP_LOGI(TAG, "Goblin behaviors: Curious, mischievous, reactive");
    
    // Initial goblin greeting via function pointers
    display_component.show_eyes("bright_happy");
    display_component.show_mouth("big_smile");
    audio_component.play("goblin_hello");
    mood_component.set_mood(MOOD_HAPPINESS);
    
    uint32_t loop_count = 0;
    uint32_t last_sensor_read = 0;
    uint32_t last_behavior_update = 0;
    
    // Main goblin behavior loop
    while (1) {
        uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        loop_count++;
        
        // Read sensors every 200ms via function pointers
        if (current_time - last_sensor_read >= 200) {
            last_sensor_read = current_time;
            
            float distance = sensor_component.read_distance();
            goblin_component.react_to_distance(distance);
        }
        
        // Random goblin behaviors every 3 seconds via function pointers
        if (current_time - last_behavior_update >= 3000) {
            last_behavior_update = current_time;
            goblin_component.random_behavior();
        }
        
        // System heartbeat every 10 seconds
        if (loop_count % 1000 == 0) {
            ESP_LOGI(TAG, "Goblin heartbeat - Loop %lu, Mood: %s, Free heap: %lu", 
                     loop_count, mood_names[current_mood], esp_get_free_heap_size());
        }
        
        // 10ms loop (100Hz)
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}