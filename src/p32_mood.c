#include "p32_component_config.h"

#ifdef ENABLE_COMPLEX_MOOD
#include "p32_mood.h"
#include "p32_display.h"
#include "p32_audio.h"

static const char* TAG = "P32_MOOD";
static p32_mood_engine_t g_mood_engine = {0};

// Mood name mappings
static const char* mood_names[MOOD_COUNT] = {
    "FEAR",
    "ANGER", 
    "IRRITATION",
    "HAPPINESS",
    "CONTENTMENT",
    "HUNGER",
    "CURIOSITY",
    "AFFECTION",
    "EXCITEMENT"
};

// Goblin mood transition rules
const p32_mood_transition_t p32_goblin_mood_transitions[] = {
    // Natural mood decays back to contentment
    {MOOD_FEAR, MOOD_CONTENTMENT, 5000, 3},
    {MOOD_ANGER, MOOD_IRRITATION, 3000, 6},
    {MOOD_IRRITATION, MOOD_CONTENTMENT, 4000, 4},
    {MOOD_HAPPINESS, MOOD_CONTENTMENT, 8000, 2},
    {MOOD_HUNGER, MOOD_IRRITATION, 10000, 5},
    {MOOD_CURIOSITY, MOOD_CONTENTMENT, 6000, 3},
    {MOOD_AFFECTION, MOOD_HAPPINESS, 7000, 2},
    {MOOD_EXCITEMENT, MOOD_HAPPINESS, 6000, 4},
    
    // Mood escalation paths
    {MOOD_IRRITATION, MOOD_ANGER, 2000, 7},
    {MOOD_CONTENTMENT, MOOD_CURIOSITY, 1000, 4},
    {MOOD_HAPPINESS, MOOD_EXCITEMENT, 3000, 6},
    {MOOD_CURIOSITY, MOOD_EXCITEMENT, 2000, 5},
    
    {0, 0, 0, 0} // Terminator
};

esp_err_t p32_mood_engine_init(void) {
    ESP_LOGI(TAG, "Initializing mood engine...");
    
    memset(&g_mood_engine, 0, sizeof(p32_mood_engine_t));
    
    // Create mood request queue
    g_mood_engine.mood_queue = xQueueCreate(10, sizeof(p32_mood_request_t));
    if (g_mood_engine.mood_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create mood queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize mood state
    g_mood_engine.current_mood = MOOD_CONTENTMENT;
    g_mood_engine.target_mood = MOOD_CONTENTMENT;
    g_mood_engine.mood_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    g_mood_engine.mood_duration_ms = 0; // Infinite duration for contentment
    g_mood_engine.transitioning = false;
    
    // Create mood processing task
    BaseType_t ret = xTaskCreate(p32_mood_task, "p32_mood", 4096, NULL, 3, &g_mood_engine.mood_task);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create mood task");
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Mood engine initialized - Initial mood: %s", 
             p32_mood_to_string(g_mood_engine.current_mood));
    return ESP_OK;
}

void p32_mood_engine_act(void) {
    extern uint64_t loopCount;
    
    // Process mood transitions every 50 loops (about 1Hz if main loop is 50Hz)
    if (loopCount % 50 == 0) {
        // Check for mood timeout and decay
        uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        uint32_t mood_elapsed = current_time - g_mood_engine.mood_start_time;
        
        if (mood_elapsed > g_mood_engine.mood_duration_ms && !g_mood_engine.transitioning) {
            // Mood has expired, decay to neutral/contentment
            ESP_LOGI(TAG, "Mood %s expired, decaying to contentment", 
                     p32_mood_to_string(g_mood_engine.current_mood));
            p32_mood_request_change(MOOD_CONTENTMENT, 10000, 1);
        }
    }
}

esp_err_t p32_mood_request_change(p32_mood_t mood, uint32_t duration_ms, uint8_t priority) {
    if (mood >= MOOD_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    p32_mood_request_t request = {
        .new_mood = mood,
        .duration_ms = duration_ms,
        .priority = priority,
        .force_change = false
    };
    
    BaseType_t ret = xQueueSend(g_mood_engine.mood_queue, &request, pdMS_TO_TICKS(100));
    if (ret != pdTRUE) {
        ESP_LOGW(TAG, "Mood request queue full, dropping request for mood: %s", 
                 p32_mood_to_string(mood));
        return ESP_ERR_TIMEOUT;
    }
    
    ESP_LOGI(TAG, "Mood change requested: %s (duration: %lums, priority: %d)",
             p32_mood_to_string(mood), duration_ms, priority);
    
    return ESP_OK;
}

esp_err_t p32_mood_force_change(p32_mood_t mood) {
    if (mood >= MOOD_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    p32_mood_request_t request = {
        .new_mood = mood,
        .duration_ms = 5000, // Default 5 second duration
        .priority = 10,      // Maximum priority
        .force_change = true
    };
    
    BaseType_t ret = xQueueSend(g_mood_engine.mood_queue, &request, pdMS_TO_TICKS(1000));
    if (ret != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue forced mood change");
        return ESP_ERR_TIMEOUT;
    }
    
    ESP_LOGI(TAG, "Forced mood change: %s", p32_mood_to_string(mood));
    return ESP_OK;
}

bool p32_mood_is_transitioning(void) {
    return g_mood_engine.transitioning;
}

uint32_t p32_mood_get_remaining_duration(void) {
    if (g_mood_engine.mood_duration_ms == 0) {
        return UINT32_MAX; // Infinite duration
    }
    
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t elapsed = current_time - g_mood_engine.mood_start_time;
    
    if (elapsed >= g_mood_engine.mood_duration_ms) {
        return 0;
    }
    
    return g_mood_engine.mood_duration_ms - elapsed;
}

void p32_mood_task(void* parameters) {
    ESP_LOGI(TAG, "Mood task started");
    
    p32_mood_request_t request;
    const TickType_t process_interval = pdMS_TO_TICKS(500); // Process every 500ms
    
    while (1) {
        // Check for mood change requests
        if (xQueueReceive(g_mood_engine.mood_queue, &request, process_interval) == pdTRUE) {
            ESP_LOGI(TAG, "Processing mood request: %s -> %s", 
                     p32_mood_to_string(g_mood_engine.current_mood),
                     p32_mood_to_string(request.new_mood));
            
            // Apply mood change
            g_mood_engine.current_mood = request.new_mood;
            g_mood_engine.target_mood = request.new_mood;
            g_mood_engine.mood_start_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
            g_mood_engine.mood_duration_ms = request.duration_ms;
            g_mood_engine.transitioning = true;
            
            // Update system mood
            p32_set_mood(request.new_mood);
            
            // Update visual and audio responses
            p32_display_update_mood_animations(request.new_mood);
            p32_audio_play_mood_sound(request.new_mood);
            
            g_mood_engine.transitioning = false;
            
            ESP_LOGI(TAG, "Mood changed to: %s (duration: %lums)",
                     p32_mood_to_string(request.new_mood), request.duration_ms);
        }
        
        // Check for automatic mood transitions
        p32_mood_process_transitions();
    }
}

esp_err_t p32_mood_process_transitions(void) {
    uint32_t remaining_duration = p32_mood_get_remaining_duration();
    
    // If current mood has expired, check for natural transitions
    if (remaining_duration == 0 && g_mood_engine.mood_duration_ms > 0) {
        ESP_LOGI(TAG, "Mood %s expired, checking transitions...", 
                 p32_mood_to_string(g_mood_engine.current_mood));
        
        // Find natural transition for current mood
        for (int i = 0; p32_goblin_mood_transitions[i].from_mood != 0; i++) {
            if (p32_goblin_mood_transitions[i].from_mood == g_mood_engine.current_mood) {
                p32_mood_t next_mood = p32_goblin_mood_transitions[i].to_mood;
                uint32_t transition_duration = p32_goblin_mood_transitions[i].transition_time_ms;
                
                ESP_LOGI(TAG, "Natural transition: %s -> %s",
                         p32_mood_to_string(g_mood_engine.current_mood),
                         p32_mood_to_string(next_mood));
                
                // Request mood change with low priority (natural transition)
                p32_mood_request_change(next_mood, transition_duration, 2);
                break;
            }
        }
    }
    
    return ESP_OK;
}

const char* p32_mood_to_string(p32_mood_t mood) {
    if (mood >= MOOD_COUNT) {
        return "UNKNOWN";
    }
    return mood_names[mood];
}

p32_mood_t p32_string_to_mood(const char* mood_str) {
    if (!mood_str) {
        return MOOD_CONTENTMENT;
    }
    
    for (int i = 0; i < MOOD_COUNT; i++) {
        if (strcmp(mood_str, mood_names[i]) == 0) {
            return (p32_mood_t)i;
        }
    }
    
    return MOOD_CONTENTMENT; // Default fallback
}

#endif // ENABLE_COMPLEX_MOOD
