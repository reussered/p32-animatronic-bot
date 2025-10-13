#include "p32_component_config.h"

#ifdef ENABLE_COMPLEX_AUDIO
#include "p32_audio.h"

static const char* TAG = "P32_AUDIO";
static p32_audio_system_t g_audio_system = {0};

// Goblin audio clips for mood-based responses
const p32_audio_clip_t p32_goblin_audio_clips[] = {
    {"ambient_breathing.wav", AUDIO_CLIP_AMBIENT, MOOD_CONTENTMENT, 3000, 1},
    {"growl_angry.wav", AUDIO_CLIP_REACTION, MOOD_ANGER, 2000, 8},
    {"curious_chirp.wav", AUDIO_CLIP_COMMUNICATION, MOOD_CURIOSITY, 1000, 5},
    {NULL, 0, 0, 0, 0} // Terminator
};

esp_err_t p32_audio_init(void) {
    ESP_LOGI(TAG, "Initializing audio system...");
    
    memset(&g_audio_system, 0, sizeof(p32_audio_system_t));
    
    // Create audio queue for clip management
    g_audio_system.audio_queue = xQueueCreate(10, sizeof(p32_audio_clip_t));
    if (g_audio_system.audio_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create audio queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Configure I2S for speaker output (already initialized in core)
    g_audio_system.speaker.i2s_port = I2S_NUM_0;
    g_audio_system.speaker.bck_pin = 26;
    g_audio_system.speaker.ws_pin = 25;
    g_audio_system.speaker.data_pin = 22;
    strncpy(g_audio_system.speaker.component_id, "goblin_speaker", 
            sizeof(g_audio_system.speaker.component_id));
    g_audio_system.speaker.initialized = true;
    
    // Set default volume
    g_audio_system.volume = 70;
    g_audio_system.audio_enabled = true;
    
    // Create audio processing task
    BaseType_t ret = xTaskCreate(p32_audio_task, "p32_audio", 4096, NULL, 5, &g_audio_system.audio_task);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create audio task");
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Audio system initialized successfully");
    return ESP_OK;
}

void p32_audio_act(void) {
    extern uint32_t loopCount;
    
    // Process audio queue and ambient sounds every 20 loops (about 2.5Hz if main loop is 50Hz)
    if (loopCount % 20 == 0) {
        if (g_audio_system.audio_enabled) {
            // Audio processing is handled by the audio task
            // This could trigger ambient breathing or other periodic sounds
            static uint32_t ambient_counter = 0;
            ambient_counter++;
            
            // Play ambient breathing every 200 loops (about every 4 seconds at 50Hz)
            if (ambient_counter >= 200) {
                p32_audio_play_ambient_breathing();
                ambient_counter = 0;
            }
        }
    }
}

esp_err_t p32_audio_play_mood_sound(p32_mood_t mood) {
    if (!g_audio_system.audio_enabled) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Find appropriate audio clip for mood
    for (int i = 0; p32_goblin_audio_clips[i].filename != NULL; i++) {
        if (p32_goblin_audio_clips[i].associated_mood == mood) {
            ESP_LOGI(TAG, "Playing mood sound: %s for mood %d", 
                     p32_goblin_audio_clips[i].filename, mood);
            return p32_audio_queue_clip(&p32_goblin_audio_clips[i]);
        }
    }
    
    ESP_LOGW(TAG, "No audio clip found for mood: %d", mood);
    return ESP_ERR_NOT_FOUND;
}

esp_err_t p32_audio_play_ambient_breathing(void) {
    return p32_audio_play_clip("ambient_breathing.wav");
}

esp_err_t p32_audio_play_growl_angry(void) {
    return p32_audio_play_clip("growl_angry.wav");
}

esp_err_t p32_audio_play_curious_chirp(void) {
    return p32_audio_play_clip("curious_chirp.wav");
}

esp_err_t p32_audio_play_clip(const char* filename) {
    if (!g_audio_system.audio_enabled || !filename) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "Playing audio clip: %s", filename);
    
    // For now, generate a simple tone to verify audio system works
    // In full implementation, this would load and play WAV files
    
    const int sample_count = 1000;
    int16_t samples[sample_count];
    
    // Generate a simple sine wave tone (440 Hz A note)
    for (int i = 0; i < sample_count; i++) {
        float angle = 2.0f * M_PI * 440.0f * i / P32_AUDIO_SAMPLE_RATE;
        samples[i] = (int16_t)(sin(angle) * 16000 * g_audio_system.volume / 100);
    }
    
    // Write samples to I2S
    size_t bytes_written = 0;
    esp_err_t ret = i2s_write(g_audio_system.speaker.i2s_port, samples, 
                              sizeof(samples), &bytes_written, pdMS_TO_TICKS(1000));
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write audio data: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Audio playback complete: %d bytes written", bytes_written);
    return ESP_OK;
}

esp_err_t p32_audio_set_volume(uint8_t volume) {
    if (volume > 100) {
        volume = 100;
    }
    
    g_audio_system.volume = volume;
    ESP_LOGI(TAG, "Volume set to: %d%%", volume);
    
    return ESP_OK;
}

uint8_t p32_audio_get_volume(void) {
    return g_audio_system.volume;
}

esp_err_t p32_audio_stop_all(void) {
    if (!g_audio_system.audio_enabled) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Stop I2S transmission
    esp_err_t ret = i2s_stop(g_audio_system.speaker.i2s_port);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop I2S: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Clear audio queue
    xQueueReset(g_audio_system.audio_queue);
    
    ESP_LOGI(TAG, "All audio stopped");
    return ESP_OK;
}

esp_err_t p32_audio_queue_clip(const p32_audio_clip_t* clip) {
    if (!clip || !g_audio_system.audio_queue) {
        return ESP_ERR_INVALID_ARG;
    }
    
    BaseType_t ret = xQueueSend(g_audio_system.audio_queue, clip, pdMS_TO_TICKS(100));
    if (ret != pdTRUE) {
        ESP_LOGW(TAG, "Audio queue full, dropping clip: %s", clip->filename);
        return ESP_ERR_TIMEOUT;
    }
    
    return ESP_OK;
}

void p32_audio_task(void* parameters) {
    ESP_LOGI(TAG, "Audio task started");
    
    p32_audio_clip_t clip;
    
    while (1) {
        // Wait for audio clips in queue
        if (xQueueReceive(g_audio_system.audio_queue, &clip, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Processing audio clip: %s", clip.filename);
            
            // Play the clip
            p32_audio_play_clip(clip.filename);
            
            // Delay based on clip duration
            vTaskDelay(pdMS_TO_TICKS(clip.duration_ms));
        }
    }
}

#endif // ENABLE_COMPLEX_AUDIO
