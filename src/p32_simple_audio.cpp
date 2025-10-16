#include "p32_component_config.hpp"

#ifdef ENABLE_SIMPLE_AUDIO
// SIMPLE AUDIO COMPONENT
#include "p32_core.hpp"

static const char* TAG = "P32_SIMPLE_AUDIO";

// Simple audio state
typedef struct {
    bool initialized;
    uint8_t volume;
    bool playing;
    uint32_t last_sound;
    char current_sound[32];
} p32_simple_audio_t;

static p32_simple_audio_t g_audio = {0};

// Simple audio initialization
esp_err_t p32_simple_audio_init(void) {
    ESP_LOGI(TAG, "Initializing simple audio system...");
    
    g_audio.initialized = true;
    g_audio.volume = 128; // 50% volume
    g_audio.playing = false;
    g_audio.last_sound = 0;
    strcpy(g_audio.current_sound, "none");
    
    ESP_LOGI(TAG, "Audio system initialized");
    return ESP_OK;
}

// Simple audio update
void p32_simple_audio_update(void) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Simple sound timeout (sounds play for 2 seconds)
    if (g_audio.playing && (current_time - g_audio.last_sound > 2000)) {
        g_audio.playing = false;
        ESP_LOGI(TAG, "Sound '%s' finished playing", g_audio.current_sound);
        strcpy(g_audio.current_sound, "none");
    }
}

// Simple play sound
esp_err_t p32_simple_audio_play_sound(const char* sound_name) {
    if (!g_audio.initialized) return ESP_ERR_INVALID_STATE;
    
    ESP_LOGI(TAG, "Playing sound: %s (volume: %d)", sound_name, g_audio.volume);
    
    strncpy(g_audio.current_sound, sound_name, sizeof(g_audio.current_sound) - 1);
    g_audio.current_sound[sizeof(g_audio.current_sound) - 1] = '\0';
    g_audio.playing = true;
    g_audio.last_sound = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    return ESP_OK;
}

// Simple set volume
esp_err_t p32_simple_audio_set_volume(uint8_t volume) {
    g_audio.volume = volume;
    ESP_LOGI(TAG, "Audio volume set to %d", volume);
    return ESP_OK;
}

// Simple play personality sound
esp_err_t p32_simple_audio_play_personality_sound(const char* personality, const char* emotion) {
    char sound_name[64];
    snprintf(sound_name, sizeof(sound_name), "%s_%s", personality, emotion);
    
    ESP_LOGI(TAG, "Playing personality sound: %s", sound_name);
    return p32_simple_audio_play_sound(sound_name);
}

// Simple audio status
bool p32_simple_audio_is_playing(void) {
    return g_audio.playing;
}

const char* p32_simple_audio_current_sound(void) {
    return g_audio.current_sound;
}

// Simple audio action (stub for component system)
esp_err_t p32_audio_component_act(void* instance, const char* action, void* params) {
    ESP_LOGI(TAG, "Audio action: %s", action);
    
    if (strcmp(action, "update") == 0) {
        p32_simple_audio_update();
    } else if (strcmp(action, "play_sound") == 0) {
        char* sound_name = (char*)params;
        if (sound_name) {
            p32_simple_audio_play_sound(sound_name);
        }
    } else if (strcmp(action, "set_volume") == 0) {
        uint8_t* volume = (uint8_t*)params;
        if (volume) {
            p32_simple_audio_set_volume(*volume);
        }
    } else if (strcmp(action, "stop") == 0) {
        g_audio.playing = false;
        strcpy(g_audio.current_sound, "none");
        ESP_LOGI(TAG, "Audio stopped");
    }
    
    return ESP_OK;
}
#endif // 0 - TEMPORARILY DISABLED
