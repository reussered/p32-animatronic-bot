// HW-496 MEMS Microphone Component
// Uses generic microphone driver for analog audio input

#include "esp_log.h"
#include "hw496_microphone.hpp"
#include "core/memory/SharedMemory.hpp"

static const char *TAG = "hw496_microphone";

// Audio sample buffer for processing
#define AUDIO_BUFFER_SIZE 256
static int16_t audio_buffer[AUDIO_BUFFER_SIZE];
static size_t buffer_index = 0;

void hw496_microphone_init(void) {
    ESP_LOGI(TAG, "HW-496 microphone init");

    // The generic_mic_driver_init() and adc_bus_init() are called automatically by the component system
    // Configure microphone-specific settings here

    // Configure gain based on HW-496 specifications (adjustable 25x-125x, default 50x)
    // This would be configurable via JSON in a full implementation
    generic_mic_driver_set_gain(1.0f); // Start with unity gain

    // Enable noise gate with reasonable threshold
    generic_mic_driver_enable_noise_gate(true);
    generic_mic_driver_set_threshold(50); // Low threshold for sensitive detection

    ESP_LOGI(TAG, "HW-496 microphone initialized");
}

void hw496_microphone_act(void) {
    // Read microphone sample using generic driver
    int raw_sample;
    esp_err_t ret = generic_mic_driver_read_sample(&raw_sample);

    if (ret == ESP_OK) {
        // Convert to 16-bit signed sample (ADC is 12-bit, so shift left by 4)
        int16_t processed_sample = (int16_t)(raw_sample << 4);

        // Store in circular buffer
        audio_buffer[buffer_index] = processed_sample;
        buffer_index = (buffer_index + 1) % AUDIO_BUFFER_SIZE;

        // Store microphone data in SharedMemory for other components
        // TODO: Define proper MicrophoneData structure
        // GSM.write<MicrophoneData>({processed_sample, buffer_index, etc.});

        // Basic audio processing - detect sound events
        static int sample_sum = 0;
        static size_t sample_count = 0;

        sample_sum += abs(processed_sample);
        sample_count++;

        // Every 32 samples, calculate average and check for sound detection
        if (sample_count >= 32) {
            int average_level = sample_sum / sample_count;

            // Simple sound detection threshold
            if (average_level > 1000) { // Adjust threshold based on testing
                ESP_LOGD(TAG, "Sound detected, level: %d", average_level);
                // Could trigger events in SharedMemory here
            }

            // Reset for next batch
            sample_sum = 0;
            sample_count = 0;
        }
    } else {
        ESP_LOGW(TAG, "Failed to read microphone sample: %s", esp_err_to_name(ret));
    }
}