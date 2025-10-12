#pragma once

#include "p32_core.h"

// Audio system configuration
#define P32_AUDIO_SAMPLE_RATE 44100
#define P32_AUDIO_BITS_PER_SAMPLE 16
#define P32_AUDIO_CHANNELS 1
#define P32_AUDIO_BUFFER_SIZE 1024

// Audio clip types for different bot behaviors
typedef enum {
    AUDIO_CLIP_AMBIENT = 0,
    AUDIO_CLIP_REACTION,
    AUDIO_CLIP_COMMUNICATION,
    AUDIO_CLIP_ALERT,
    AUDIO_CLIP_COUNT
} p32_audio_clip_type_t;

// Audio component structure
typedef struct {
    i2s_port_t i2s_port;
    uint8_t bck_pin;
    uint8_t ws_pin;
    uint8_t data_pin;
    bool initialized;
    char component_id[32];
} p32_audio_device_t;

// Audio system state
typedef struct {
    p32_audio_device_t speaker;
    p32_audio_device_t microphone_left;
    p32_audio_device_t microphone_right;
    QueueHandle_t audio_queue;
    TaskHandle_t audio_task;
    bool audio_enabled;
    uint8_t volume;
} p32_audio_system_t;

// Audio clip structure for mood-based sounds
typedef struct {
    const char* filename;
    p32_audio_clip_type_t type;
    p32_mood_t associated_mood;
    uint32_t duration_ms;
    uint8_t priority;
} p32_audio_clip_t;

// Audio driver functions
esp_err_t p32_audio_init(void);
void p32_audio_act(void);
esp_err_t p32_audio_setup_speaker(const p32_component_config_t* config);
esp_err_t p32_audio_setup_microphone(const p32_component_config_t* config, bool is_left_ear);

// Audio playback
esp_err_t p32_audio_play_clip(const char* filename);
esp_err_t p32_audio_play_mood_sound(p32_mood_t mood);
esp_err_t p32_audio_play_ambient_breathing(void);
esp_err_t p32_audio_play_growl_angry(void);
esp_err_t p32_audio_play_curious_chirp(void);

// Volume and control
esp_err_t p32_audio_set_volume(uint8_t volume);
uint8_t p32_audio_get_volume(void);
esp_err_t p32_audio_stop_all(void);

// Microphone input (for future behavioral responses)
esp_err_t p32_audio_start_listening(void);
esp_err_t p32_audio_stop_listening(void);
bool p32_audio_detect_sound(void);
uint16_t p32_audio_get_sound_level(void);

// Audio task management
void p32_audio_task(void* parameters);
esp_err_t p32_audio_queue_clip(const p32_audio_clip_t* clip);

// Mood-based audio mapping
extern const p32_audio_clip_t p32_goblin_audio_clips[];
extern const p32_audio_clip_t p32_zombie_audio_clips[];
extern const p32_audio_clip_t p32_robot_audio_clips[];