#include "subsystems/goblin_head/goblin_head_component_functions.hpp"

// Auto-generated component aggregation file

// --- Begin: config\components\hardware\hw496_microphone.src ---
// HW-496 MEMS Microphone Component
// Uses generic microphone driver with HW496-specific configuration

#include "esp_log.h"
#include "components/hardware/hw496_microphone.hdr"
#include "../drivers/generic_mic_driver.hdr"
#include "core/memory/SharedMemory.hpp"

static const char *TAG_hw496_microphone = "hw496_microphone";

// HW496-specific configuration
// Based on HW496 datasheet: adjustable gain (25x-125x), 58dB SNR, 20Hz-20kHz response
static const float HW496_DEFAULT_GAIN = 2.0f;  // 50x gain setting
static const int HW496_NOISE_GATE_THRESHOLD = 50;  // Adjust based on testing
static const bool HW496_NOISE_GATE_ENABLED = true;

esp_err_t hw496_microphone_init(void) {
    ESP_LOGI(TAG_hw496_microphone, "HW-496 microphone init");

    // HW496-specific hardware initialization
    // Note: generic_mic_driver is initialized separately as a dependency
    // HW496 uses default generic_mic_driver settings

    ESP_LOGI(TAG_hw496_microphone, "HW-496 microphone initialized (using generic driver defaults)");
    return ESP_OK;
}

void hw496_microphone_act(void) {
    // Read microphone data from SharedMemory (written by generic_mic_driver)
    MicrophoneData* mic_data = GSM.read<MicrophoneData>();

    if (mic_data && mic_data->driver_initialized) {
        // HW496-specific processing or monitoring
        // For now, log the data for testing
        ESP_LOGD(TAG_hw496_microphone, "HW496 data: raw=%d, processed=%d, voltage=%dmV, gain=%.1f, sound=%s",
                 mic_data->raw_sample, mic_data->processed_sample, mic_data->voltage_mv,
                 mic_data->gain_applied, mic_data->sound_detected ? "detected" : "none");

        // HW496 could add hardware-specific processing here
        // For example, adjusting gain based on audio levels, or triggering hardware responses
    } else {
        ESP_LOGW(TAG_hw496_microphone, "Microphone data not available or driver not initialized");
    }
}
// --- End: config\components\hardware\hw496_microphone.src ---

// --- Begin: config\components\hardware\gc9a01.src ---
// gc9a01 component implementation
// This component is a data provider. It defines the physical characteristics
// of the GC9A01 display but contains no active logic.

#include "esp_log.h"
#include "esp_heap_caps.h"
#include <stdlib.h>

static const char *TAG_gc9a01 = "gc9a01";

// Define GC9A01_InitialPixel
// Note: value should be set before allocating buffers containing GC9A01_Pixels
GC9A01_Pixel GC9A01_InitialPixel;

// Display Buffer Interface - provides buffer allocation and size info
// to positioned components (eyes, mouth, etc.)

/**
 * @brief Get display width in pixels
 * @return Display width (240 for GC9A01)
 */
size_t gc9a01_get_width(void) {
    return 240;
}

/**
 * @brief Get display height in pixels
 * @return Display height (240 for GC9A01)
 */
size_t gc9a01_get_height(void) {
    return 240;
}

/**
 * @brief Get total frame size in pixels
 * @return Total pixels in one frame (width * height)
 */
size_t getFrameSize(void) {
    return 240 * 240;  // 57,600 pixels
}

/**
 * @brief Get pixels per row (display width)
 * @return Number of pixels per row
 */
size_t getFrameRowSize(void) {
    return 240;
}

/**
 * @brief Get display size in pixels
 * @return Total number of pixels in one frame
 */
size_t getDisplaySize(void) {
    return 240 * 240;  // 57,600 pixels
}

esp_err_t gc9a01_init(void) {
    ESP_LOGI(TAG_gc9a01, "gc9a01 component initialized (passive)");
    return ESP_OK;
}

void gc9a01_act(void) {
    // This component is passive and does nothing in its act function.
}
// --- End: config\components\hardware\gc9a01.src ---

// --- Begin: config\components\drivers\generic_spi_data_driver.src ---
// generic_spi_data_driver component implementation
// Driver for generic SPI devices using the spi_data_bus

#include "esp_log.h"

static const char *TAG = "generic_spi_data_driver";

esp_err_t generic_spi_data_driver_init(void) {
    ESP_LOGI(TAG, "Initializing generic SPI data driver");
    // Initialization logic for a generic SPI device will go here.
    // This would typically involve adding a device to the spi_data_bus.
    return ESP_OK;
}

void generic_spi_data_driver_act(void) {
    // Active logic for the driver, e.g., polling a sensor.
}
// --- End: config\components\drivers\generic_spi_data_driver.src ---

// --- Begin: config\components\drivers\generic_spi_display.src ---
// generic_spi_display component implementation
// Generic SPI display interface for basic drawing operations

#include "esp_log.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "components/spi_display_bus.hdr"
#include "driver/gpio.h"

static const char *TAG_generic_spi_display = "generic_spi_display";

// Display commands
#define GC9A01_SWRESET 0x01
#define GC9A01_SLPOUT 0x11
#define GC9A01_DISPON 0x29
#define GC9A01_CASET  0x2A
#define GC9A01_RASET  0x2B
#define GC9A01_RAMWR  0x2C

// Internal helper functions to send commands and data
static void generic_spi_write_data(spi_device_handle_t spi, const uint8_t *data, size_t len) {
    if (!spi) return;
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0) return;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // D/C = 1 for data
    ret = spi_device_polling_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);
}

static void spi_write_command(spi_device_handle_t spi, const uint8_t cmd) {
    if (!spi) return;
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // D/C = 0 for command
    ret = spi_device_polling_transmit(spi, &t);
    ESP_ERROR_CHECK(ret);
}

esp_err_t generic_spi_display_init(void) {
    ESP_LOGI(TAG_generic_spi_display, "Initializing generic SPI display with CS=%d", cur_spi_pin.cs);

    if (cur_spi_pin.handle == NULL) {
        ESP_LOGE(TAG_generic_spi_display, "SPI handle is NULL, cannot initialize display.");
        return ESP_FAIL;
    }

    // Hardware reset
    if (cur_spi_pin.rst != -1) {
        gpio_set_level((gpio_num_t)cur_spi_pin.rst, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level((gpio_num_t)cur_spi_pin.rst, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // Software reset
    spi_write_command(cur_spi_pin.handle, GC9A01_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));

    // Sleep out
    spi_write_command(cur_spi_pin.handle, GC9A01_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(500));

    // Display on
    spi_write_command(cur_spi_pin.handle, GC9A01_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG_generic_spi_display, "Display with CS=%d initialized successfully", cur_spi_pin.cs);
    return ESP_OK;
}

void generic_spi_display_act(void) {
    if (cur_spi_pin.handle == NULL) {
        ESP_LOGE(TAG_generic_spi_display, "act: SPI handle is NULL for CS=%d", cur_spi_pin.cs);
        return;
    }

    // Get the appropriate framebuffer based on which display we're driving
    // CS=3 is left eye, CS=6 is right eye
    uint16_t* framebuffer = nullptr;
    if (cur_spi_pin.cs == 3) {
        framebuffer = goblin_left_eye_get_buffer();
    } else if (cur_spi_pin.cs == 6) {
        framebuffer = goblin_right_eye_get_buffer();
    }

    if (!framebuffer) {
        ESP_LOGE(TAG_generic_spi_display, "No framebuffer available for CS=%d", cur_spi_pin.cs);
        return;
    }

    // Set column address (0-239)
    spi_write_command(cur_spi_pin.handle, GC9A01_CASET);
    uint8_t col_data[] = {0x00, 0x00, 0x00, 0xEF};
    generic_spi_write_data(cur_spi_pin.handle, col_data, 4);

    // Set row address (0-239)
    spi_write_command(cur_spi_pin.handle, GC9A01_RASET);
    uint8_t row_data[] = {0x00, 0x00, 0x00, 0xEF};
    generic_spi_write_data(cur_spi_pin.handle, row_data, 4);

    // Memory write
    spi_write_command(cur_spi_pin.handle, GC9A01_RAMWR);

    // Send the actual framebuffer data
    // Convert RGB565 pixels to byte stream (big-endian)
    const size_t total_pixels = 240 * 240;
    const size_t row_size = 240 * 2;  // 240 pixels * 2 bytes per pixel
    uint8_t* row_buffer = (uint8_t*)malloc(row_size);
    
    if (!row_buffer) {
        ESP_LOGE(TAG_generic_spi_display, "Failed to allocate row buffer");
        return;
    }

    // Send framebuffer row by row
    for (uint32_t row = 0; row < 240; row++) {
        uint16_t* row_pixels = framebuffer + (row * 240);
        
        // Convert uint16_t pixels to byte stream
        for (uint32_t col = 0; col < 240; col++) {
            uint16_t pixel = row_pixels[col];
            row_buffer[col * 2] = pixel >> 8;       // High byte
            row_buffer[col * 2 + 1] = pixel & 0xFF; // Low byte
        }
        
        generic_spi_write_data(cur_spi_pin.handle, row_buffer, row_size);
    }
    
    free(row_buffer);
}
// --- End: config\components\drivers\generic_spi_display.src ---

// --- Begin: config\components\creature_specific\goblin_eye.src ---
// goblin_eye component implementation
// Shared goblin eye processing logic with Pixel-based mood rendering
// Uses display-agnostic Pixel types with saturation arithmetic

#include "esp_log.h"
#include "components/gc9a01.hdr"  // For Pixel type
#include "Mood.hpp"
#include "core/memory/SharedMemory.hpp"

static const char *TAG_goblin_eye = "goblin_eye";

// Shared frame buffer (set by left/right eye positioned components)
static Pixel* currentFrame = nullptr;
static uint32_t current_frame_size = 0;

// Goblin personality multiplier - goblins show emotions STRONGLY (1.5x)
// Compare to bears which might use 0.5x (stoic) or cats which use 0.8x (aloof)
static constexpr float GOBLIN_EMOTION_INTENSITY = 1.5f;

// Mood tracking for optimization
static Mood lastMood;
static bool mood_initialized = false;

// Mood-to-color mapping for goblin eyes (defined later in this file)
static const MoodColorEffect goblin_mood_effects[Mood::componentCount] = {
    // ANGER: Red tint, reduces green/blue
    MoodColorEffect(0.8f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY),
    // FEAR: Blue tint, pale (reduces all slightly, increases blue)
    MoodColorEffect(-0.2f * GOBLIN_EMOTION_INTENSITY, -0.2f * GOBLIN_EMOTION_INTENSITY, 0.6f * GOBLIN_EMOTION_INTENSITY),
    // HAPPINESS: Yellow/warm tint (increase red+green)
    MoodColorEffect(0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY, 0.1f * GOBLIN_EMOTION_INTENSITY),
    // SADNESS: Desaturate (reduce all colors)
    MoodColorEffect(-0.3f * GOBLIN_EMOTION_INTENSITY, -0.3f * GOBLIN_EMOTION_INTENSITY, -0.1f * GOBLIN_EMOTION_INTENSITY),
    // CURIOSITY: Green tint (goblins get greenish when curious)
    MoodColorEffect(0.1f * GOBLIN_EMOTION_INTENSITY, 0.7f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY),
    // AFFECTION: Purple/warm tint
    MoodColorEffect(0.4f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY, 0.4f * GOBLIN_EMOTION_INTENSITY),
    // IRRITATION: Orange-red tint
    MoodColorEffect(0.6f * GOBLIN_EMOTION_INTENSITY, 0.2f * GOBLIN_EMOTION_INTENSITY, -0.2f * GOBLIN_EMOTION_INTENSITY),
    // CONTENTMENT: Warm, slightly yellow
    MoodColorEffect(0.3f * GOBLIN_EMOTION_INTENSITY, 0.4f * GOBLIN_EMOTION_INTENSITY, 0.1f * GOBLIN_EMOTION_INTENSITY),
    // EXCITEMENT: Bright, all colors up
    MoodColorEffect(0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY, 0.5f * GOBLIN_EMOTION_INTENSITY)
};

esp_err_t goblin_eye_init(void) 
{
    ESP_LOGI(TAG_goblin_eye, "Initializing shared goblin eye resources");
    
    // Clear mood tracking
    lastMood.clear();
    mood_initialized = false;
    
    ESP_LOGI(TAG_goblin_eye, "Goblin eye mood processing initialized (emotion intensity: %.1fx)", 
             GOBLIN_EMOTION_INTENSITY);
    return ESP_OK;
}

void goblin_eye_act(void) 
{
    // Only process if we have a valid frame set by left/right eye components
    if (currentFrame == nullptr || current_frame_size == 0) 
    {
        return;
    }
    
    // Get current global mood from shared memory
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr == nullptr) 
    {
        return;  // No mood data available
    }
    Mood currentMood = *mood_ptr;
    
    // Check if mood changed (optimization - only recalculate when mood changes)
    if (!mood_initialized || lastMood != currentMood) 
    {
        // Step 1: Calculate mood color deltas (once per mood change)
        Pixel moodTint;  // Accumulated color adjustment based on mood
        moodTint.red = 0;
        moodTint.green = 0;
        moodTint.blue = 0;
        
        for (int i = 0; i < Mood::componentCount; ++i) 
        {
            int8_t moodValue = currentMood.components[i];
            if (moodValue != 0) 
            {
                const MoodColorEffect& effect = goblin_mood_effects[i];
                
                // Calculate color contribution (scaled by mood intensity)
                // moodValue ranges from -128 to +127
                // Normalize to fractional multiplier and apply effect
                float intensity = static_cast<float>(moodValue) / 127.0f;
                
                int16_t red_contrib = static_cast<int16_t>(intensity * effect.red_multiplier * Pixel::maxRed);
                int16_t green_contrib = static_cast<int16_t>(intensity * effect.green_multiplier * Pixel::maxGreen);
                int16_t blue_contrib = static_cast<int16_t>(intensity * effect.blue_multiplier * Pixel::maxBlue);
                
                // Accumulate using Pixel's saturating addition
                Pixel contribution;
                contribution.red = (red_contrib > 0) ? red_contrib : 0;
                contribution.green = (green_contrib > 0) ? green_contrib : 0;
                contribution.blue = (blue_contrib > 0) ? blue_contrib : 0;
                
                moodTint += contribution;
            }
        }
        
        // Step 2: Apply mood tint to ALL pixels in the frame
        for (uint32_t pixel = 0; pixel < current_frame_size; pixel++) 
        {
            // Add mood tint using saturating arithmetic
            currentFrame[pixel] += moodTint;
        }
        
        // Step 3: Remember new mood
        lastMood = currentMood;
        mood_initialized = true;
        
        ESP_LOGV(TAG_goblin_eye, "Frame updated with mood tint R:%u G:%u B:%u", 
                 moodTint.red, moodTint.green, moodTint.blue);
    }
    // If mood hasn't changed, frame colors remain cached - no processing needed!
}
// --- End: config\components\creature_specific\goblin_eye.src ---

// --- Begin: config\components\positioned\goblin_left_ear.src ---
// goblin_left_ear component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_left_ear = "goblin_left_ear";

esp_err_t goblin_left_ear_init(void) {
    ESP_LOGI(TAG_goblin_left_ear, "goblin_left_ear init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_left_ear_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_left_ear, "goblin_left_ear act");
}
// --- End: config\components\positioned\goblin_left_ear.src ---

// --- Begin: config\components\positioned\goblin_left_eye.src ---
// goblin_left_eye component implementation
// Positioned component that manages the left eye display buffer

#include "esp_log.h"
#include <cstdint>

static const char *TAG_goblin_left_eye = "goblin_left_eye";

// Display buffer for left eye - now using Pixel class
static Pixel* left_display_buffer = nullptr;
static Pixel* left_current_frame_ptr = nullptr;
static size_t left_frame_offset = 0;  // Offset in pixels

/**
 * @brief Initialize left eye display buffer
 * Allocates framebuffer using Pixel class and Display Buffer Interface
 */
esp_err_t goblin_left_eye_init(void) {
    ESP_LOGI(TAG_goblin_left_eye, "Initializing left eye display buffer");
    
    // Allocate display buffer - array of Pixels
    uint32_t display_size = getDisplaySize();
    left_display_buffer = new Pixel[display_size];
    
    if (!left_display_buffer) {
        ESP_LOGE(TAG_goblin_left_eye, "Failed to allocate left eye buffer");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize frame pointer and offset
    left_frame_offset = 0;
    left_current_frame_ptr = left_display_buffer;
    
    // Initialize buffer with test pattern - red for left eye
    Pixel red = Pixel::Red();
    for (uint32_t i = 0; i < display_size; i++) {
        left_display_buffer[i] = red;
    }
    
    ESP_LOGI(TAG_goblin_left_eye, "Left eye buffer allocated: %u pixels (%.1f KB)", 
             display_size, (display_size * sizeof(Pixel)) / 1024.0f);
    
    return ESP_OK;
}

/**
 * @brief Get pointer to current frame in left eye display buffer
 * @return Pointer to Pixel array at current offset, or nullptr if not initialized
 */
Pixel* goblin_left_eye_get_buffer(void) {
    return left_current_frame_ptr;
}

/**
 * @brief Update left eye animation and advance frame pointer
 * Implements frame cycling with offset management
 */
void goblin_left_eye_act(void) {
    if (!left_display_buffer) return;
    
    // Calculate current frame pointer with offset (in pixels)
    left_current_frame_ptr = left_display_buffer + left_frame_offset;
    
    // Advance to next frame
    left_frame_offset += getFrameSize();
    
    // Wrap around if we exceed buffer size
    if (left_frame_offset >= getDisplaySize()) {
        left_frame_offset = 0;
    }
}
// --- End: config\components\positioned\goblin_left_eye.src ---

// --- Begin: config\components\positioned\goblin_mouth.src ---
// goblin_mouth component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_mouth = "goblin_mouth";

esp_err_t goblin_mouth_init(void) {
    ESP_LOGI(TAG_goblin_mouth, "goblin_mouth init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_mouth_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_mouth, "goblin_mouth act");
}
// --- End: config\components\positioned\goblin_mouth.src ---

// --- Begin: config\components\positioned\goblin_nose.src ---
// goblin_nose component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_nose = "goblin_nose";

esp_err_t goblin_nose_init(void) {
    ESP_LOGI(TAG_goblin_nose, "goblin_nose init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_nose_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_nose, "goblin_nose act");
}
// --- End: config\components\positioned\goblin_nose.src ---

// --- Begin: config\components\positioned\goblin_right_ear.src ---
// goblin_right_ear component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_right_ear = "goblin_right_ear";

esp_err_t goblin_right_ear_init(void) {
    ESP_LOGI(TAG_goblin_right_ear, "goblin_right_ear init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_right_ear_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_right_ear, "goblin_right_ear act");
}
// --- End: config\components\positioned\goblin_right_ear.src ---

// --- Begin: config\components\positioned\goblin_right_eye.src ---
// goblin_right_eye component implementation
// Positioned component that manages the right eye display buffer

#include "esp_log.h"
#include <cstdint>

static const char *TAG_goblin_right_eye = "goblin_right_eye";

// Display buffer for right eye - now using Pixel class
static Pixel* right_display_buffer = nullptr;
static Pixel* right_current_frame_ptr = nullptr;
static size_t right_frame_offset = 0;  // Offset in pixels

/**
 * @brief Initialize right eye display buffer
 * Allocates framebuffer using Pixel class and Display Buffer Interface
 */
esp_err_t goblin_right_eye_init(void) {
    ESP_LOGI(TAG_goblin_right_eye, "Initializing right eye display buffer");
    
    // Allocate display buffer - array of Pixels
    uint32_t display_size = getDisplaySize();
    right_display_buffer = new Pixel[display_size];
    
    if (!right_display_buffer) {
        ESP_LOGE(TAG_goblin_right_eye, "Failed to allocate right eye buffer");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize frame pointer and offset
    right_frame_offset = 0;
    right_current_frame_ptr = right_display_buffer;
    
    // Initialize buffer with test pattern - green for right eye
    Pixel green = Pixel::Green();
    for (uint32_t i = 0; i < display_size; i++) {
        right_display_buffer[i] = green;
    }
    
    ESP_LOGI(TAG_goblin_right_eye, "Right eye buffer allocated: %u pixels (%.1f KB)", 
             display_size, (display_size * sizeof(Pixel)) / 1024.0f);
    
    return ESP_OK;
}

/**
 * @brief Get pointer to current frame in right eye display buffer
 * @return Pointer to Pixel array at current offset, or nullptr if not initialized
 */
Pixel* goblin_right_eye_get_buffer(void) {
    return right_current_frame_ptr;
}

/**
 * @brief Update right eye animation and advance frame pointer
 * Implements frame cycling with offset management
 */
void goblin_right_eye_act(void) {
    if (!right_display_buffer) return;
    
    // Calculate current frame pointer with offset (in pixels)
    right_current_frame_ptr = right_display_buffer + right_frame_offset;
    
    // Advance to next frame
    right_frame_offset += getFrameSize();
    
    // Wrap around if we exceed buffer size
    if (right_frame_offset >= getDisplaySize()) {
        right_frame_offset = 0;
    }
}
// --- End: config\components\positioned\goblin_right_eye.src ---

// --- Begin: config\components\positioned\goblin_speaker.src ---
// goblin_speaker component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_goblin_speaker = "goblin_speaker";

esp_err_t goblin_speaker_init(void) {
    ESP_LOGI(TAG_goblin_speaker, "goblin_speaker init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_speaker_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_goblin_speaker, "goblin_speaker act");
}
// --- End: config\components\positioned\goblin_speaker.src ---

// NOTE: Source for component 'hc_sr04_sensor' not found.

// --- Begin: config\components\hardware\servo_sg90_micro.src ---
// servo_sg90_micro component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_servo_sg90_micro = "servo_sg90_micro";

esp_err_t servo_sg90_micro_init(void) {
    ESP_LOGI(TAG_servo_sg90_micro, "servo_sg90_micro init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void servo_sg90_micro_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_servo_sg90_micro, "servo_sg90_micro act");
}
// --- End: config\components\hardware\servo_sg90_micro.src ---

// --- Begin: config\components\hardware\speaker.src ---
// speaker component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_speaker = "speaker";

esp_err_t speaker_init(void) {
    ESP_LOGI(TAG_speaker, "speaker init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void speaker_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_speaker, "speaker act");
}
// --- End: config\components\hardware\speaker.src ---

// --- Begin: config\components\interfaces\spi_bus.src ---
// spi_bus component implementation
// ESP32 VSPI bus interface for SPI communication with dynamic pin assignment

#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_s3_r8n16_pin_assignments.h"

#include <stddef.h>

struct spi_device_pinset_t {
    spi_device_pinset_t()
        : mosi(-1), miso(-1), clk(-1), cs(-1), dc(-1), backlight(-1) {}

    int mosi;
    int miso;
    int clk;
    int cs;
    int dc;
    int backlight;
};

static constexpr size_t SPI_DEVICE_SLOT_COUNT = 32U;
static spi_device_pinset_t spi_device_pins[SPI_DEVICE_SLOT_COUNT];
spi_device_pinset_t cur_spi_pin;
static size_t spi_device_count = 0;
static size_t spi_active_device_index = 0;

static const char *TAG_spi_bus = "spi_bus";

// SPI bus pins assigned at runtime to honour the shared pin allocator
static int spi_mosi_pin = -1;
static int spi_miso_pin = -1;
static int spi_clk_pin = -1;
static int spi_reset_pin = -1;

static bool spi_initialized = false;

static constexpr size_t REQUIRED_UNIQUE_PINS = 3;  // CS, DC, backlight

static int claim_next_spi_pin(void) {
    for (size_t i = 0; i < spi_assignable_count; i++) {
        const int candidate = spi_assignable[i];
        bool already_claimed = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == candidate) {
                already_claimed = true;
                break;
            }
        }
        if (!already_claimed) {
            if (assigned_pins_count >= (sizeof(assigned_pins) / sizeof(assigned_pins[0]))) {
                ESP_LOGE(TAG_spi_bus, "Assigned pin buffer exhausted");
                return -1;
            }
            assigned_pins[assigned_pins_count++] = candidate;
            return candidate;
        }
    }
    return -1;
}

esp_err_t spi_bus_init(void) {
    const size_t device_index = spi_device_count;
    if (device_index >= SPI_DEVICE_SLOT_COUNT) {
        ESP_LOGE(TAG_spi_bus, "No remaining SPI device slots available for allocation");
        return ESP_FAIL;
    }

    if (!spi_initialized) {
        ESP_LOGI(TAG_spi_bus, "Assigning shared SPI pins for first device");

        spi_mosi_pin = claim_next_spi_pin();
        spi_miso_pin = claim_next_spi_pin();
        spi_clk_pin = claim_next_spi_pin();

        if (spi_mosi_pin < 0 || spi_miso_pin < 0 || spi_clk_pin < 0) {
            ESP_LOGE(TAG_spi_bus, "Unable to assign shared SPI pins (MOSI:%d MISO:%d CLK:%d)",
                     spi_mosi_pin, spi_miso_pin, spi_clk_pin);
            return ESP_FAIL;
        }

        spi_reset_pin = claim_next_spi_pin();
        if (spi_reset_pin < 0) {
            ESP_LOGE(TAG_spi_bus, "Unable to assign shared SPI reset pin");
            return ESP_FAIL;
        }

        ESP_LOGI(TAG_spi_bus, "Shared SPI pins assigned MOSI:%d MISO:%d CLK:%d RESET:%d",
                 spi_mosi_pin, spi_miso_pin, spi_clk_pin, spi_reset_pin);

        const spi_bus_config_t bus_cfg = {
            .mosi_io_num = spi_mosi_pin,
            .miso_io_num = spi_miso_pin,
            .sclk_io_num = spi_clk_pin,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 240 * 240 * 2 + 8,
        };

        const esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG_spi_bus, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        spi_initialized = true;
        ESP_LOGI(TAG_spi_bus, "SPI bus initialized successfully");
    } else {
        ESP_LOGI(TAG_spi_bus, "Reusing shared SPI pins MOSI:%d MISO:%d CLK:%d RESET:%d",
                 spi_mosi_pin, spi_miso_pin, spi_clk_pin, spi_reset_pin);
    }

    int unique_allocations[REQUIRED_UNIQUE_PINS];
    for (size_t i = 0; i < REQUIRED_UNIQUE_PINS; ++i) {
        unique_allocations[i] = claim_next_spi_pin();
        if (unique_allocations[i] < 0) {
            ESP_LOGE(TAG_spi_bus, "Unable to assign unique SPI pin index %u for device %u",
                     (unsigned)i, (unsigned)device_index);
            return ESP_FAIL;
        }
    }

    spi_device_pins[device_index].mosi = spi_mosi_pin;
    spi_device_pins[device_index].miso = spi_miso_pin;
    spi_device_pins[device_index].clk = spi_clk_pin;
    spi_device_pins[device_index].cs = unique_allocations[0];
    spi_device_pins[device_index].dc = unique_allocations[1];
    spi_device_pins[device_index].backlight = unique_allocations[2];

    spi_device_count++;

    ESP_LOGI(TAG_spi_bus,
             "Device %u assigned SPI pins MOSI:%d MISO:%d CLK:%d CS:%d DC:%d BL:%d",
             (unsigned)device_index,
             spi_device_pins[device_index].mosi,
             spi_device_pins[device_index].miso,
             spi_device_pins[device_index].clk,
             spi_device_pins[device_index].cs,
             spi_device_pins[device_index].dc,
             spi_device_pins[device_index].backlight);

    return ESP_OK;
}

void spi_bus_act(void) {
    static int device = 0;

    if (spi_device_count == 0) {
        device = 0;
        cur_spi_pin = spi_device_pinset_t();
        return;
    }

    if (device < 0 || static_cast<size_t>(device) >= spi_device_count ||
        spi_device_pins[static_cast<size_t>(device)].cs == -1) {
        device = 0;
    }

    cur_spi_pin = spi_device_pins[static_cast<size_t>(device)];

    ++device;
    if (static_cast<size_t>(device) >= spi_device_count) {
        device = 0;
    }
}

int spi_bus_get_mosi_pin(void) {
    return spi_mosi_pin;
}

int spi_bus_get_miso_pin(void) {
    return spi_miso_pin;
}

int spi_bus_get_sclk_pin(void) {
    return spi_clk_pin;
}

int spi_bus_get_reset_pin(void) {
    return spi_reset_pin;
}

static bool spi_device_index_valid(int device_index) {
    return device_index >= 0 && static_cast<size_t>(device_index) < spi_device_count;
}

int spi_bus_get_cs_pin(int device_index) {
    if (!spi_device_index_valid(device_index)) {
        return -1;
    }
    return spi_device_pins[static_cast<size_t>(device_index)].cs;
}

int spi_bus_get_dc_pin(int device_index) {
    if (!spi_device_index_valid(device_index)) {
        return -1;
    }
    return spi_device_pins[static_cast<size_t>(device_index)].dc;
}

int spi_bus_get_backlight_pin(int device_index) {
    if (!spi_device_index_valid(device_index)) {
        return -1;
    }
    return spi_device_pins[static_cast<size_t>(device_index)].backlight;
}
// --- End: config\components\interfaces\spi_bus.src ---

// --- Begin: config\components\drivers\spi_display_bus.src ---
// spi_display_bus driver implementation
// Provides dedicated SPI bus allocation for display peripherals

#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_s3_r8n16_pin_assignments.h"
#include "components/spi_display_bus.hdr"

#include <stddef.h>
#include <stdio.h>

static constexpr size_t SPI_DISPLAY_SLOT_COUNT = 32U;
static spi_display_pinset_t spi_display_slots[SPI_DISPLAY_SLOT_COUNT];
spi_display_pinset_t cur_spi_pin;

static const char *TAG_spi_display_bus = "spi_display_bus";

static spi_display_pinset_t shared_display_pins;

static int get_next_assignable(const int *assignable, size_t assignable_count) {
    for (size_t i = 0; i < assignable_count; ++i) {
        const int candidate = assignable[i];
        bool already_claimed = false;
        for (size_t j = 0; j < assigned_pins_count; ++j) {
            if (assigned_pins[j] == candidate) {
                already_claimed = true;
                break;
            }
        }

        if (!already_claimed) {
            if (assigned_pins_count >= (sizeof(assigned_pins) / sizeof(assigned_pins[0]))) {
                ESP_LOGE(TAG_spi_display_bus, "Assigned pin buffer exhausted");
                esp_system_abort("Assigned pin buffer exhausted");
            }

            assigned_pins[assigned_pins_count++] = candidate;
            return candidate;
        }
    }

    ESP_LOGE(TAG_spi_display_bus, "No assignable pins remain for SPI display bus");
    esp_system_abort("SPI display bus ran out of assignable pins");
    return -1;
}

esp_err_t spi_display_bus_init(void) {
    static size_t device_count = 0;

    if (device_count >= SPI_DISPLAY_SLOT_COUNT) 
    {
        ESP_LOGE(TAG_spi_display_bus, "No remaining SPI display device slots available");
        return ESP_FAIL;
    }

    // On the very first call, allocate shared pins and initialize the bus
    if (device_count == 0) 
    {
        spi_display_slots[0].mosi = get_next_assignable(spi_assignable, spi_assignable_count);
        spi_display_slots[0].clk = get_next_assignable(spi_assignable, spi_assignable_count);
        spi_display_slots[0].rst = get_next_assignable(spi_assignable, spi_assignable_count);

        spi_bus_config_t bus_cfg = {
            .mosi_io_num = spi_display_slots[0].mosi,
            .miso_io_num = -1,
            .sclk_io_num = spi_display_slots[0].clk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 240 * 240 * 2 + 8,
            .flags = SPICOMMON_BUSFLAG_MASTER,
            .intr_flags = 0
        };

        const esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) 
        {
            ESP_LOGE(TAG_spi_display_bus, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI(TAG_spi_display_bus,
                 "Shared SPI display pins assigned MOSI:%d CLK:%d RST:%d",
                 spi_display_slots[0].mosi,
                 spi_display_slots[0].clk,
                 spi_display_slots[0].rst);
    }

    // For subsequent devices, copy the shared pin configuration from the first device
    if (device_count > 0)
    {
        spi_display_slots[device_count].mosi = spi_display_slots[0].mosi;
        spi_display_slots[device_count].clk = spi_display_slots[0].clk;
        spi_display_slots[device_count].rst = spi_display_slots[0].rst;
    }

    // For every device (including the first), allocate its unique pins and add to bus
    spi_display_slots[device_count].cs = get_next_assignable(spi_assignable, spi_assignable_count);
    spi_display_slots[device_count].dc = get_next_assignable(spi_assignable, spi_assignable_count);
    spi_display_slots[device_count].bl = get_next_assignable(spi_assignable, spi_assignable_count);
    
    // Configure RST, DC, and BL as outputs
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    if (spi_display_slots[device_count].rst != -1) {
        io_conf.pin_bit_mask = (1ULL << spi_display_slots[device_count].rst);
        gpio_config(&io_conf);
        gpio_set_level((gpio_num_t)spi_display_slots[device_count].rst, 1);  // Set high initially
    }
    
    if (spi_display_slots[device_count].dc != -1) {
        io_conf.pin_bit_mask = (1ULL << spi_display_slots[device_count].dc);
        gpio_config(&io_conf);
    }
    
    if (spi_display_slots[device_count].bl != -1) {
        io_conf.pin_bit_mask = (1ULL << spi_display_slots[device_count].bl);
        gpio_config(&io_conf);
        gpio_set_level((gpio_num_t)spi_display_slots[device_count].bl, 1);  // Turn on backlight
    }
    
    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 40 * 1000 * 1000, // 40 MHz
        .input_delay_ns = 0,
        .spics_io_num = spi_display_slots[device_count].cs,
        .queue_size = 7,
        .pre_cb = NULL, // Set D/C line high for data
        .post_cb = NULL
    };

    esp_err_t ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &spi_display_slots[device_count].handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_spi_display_bus, "spi_bus_add_device failed for CS=%d: %s", spi_display_slots[device_count].cs, esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG_spi_display_bus,
             "Display slot %u assigned unique pins CS:%d DC:%d BL:%d and handle %p",
             static_cast<unsigned>(device_count),
             spi_display_slots[device_count].cs,
             spi_display_slots[device_count].dc,
             spi_display_slots[device_count].bl,
             (void*)spi_display_slots[device_count].handle);

    // Set cur_spi_pin to the device we just created so the next component can use it immediately
    cur_spi_pin = spi_display_slots[device_count];

    device_count++;
    return ESP_OK;
}

void spi_display_bus_act(void) 
{
    static int device = 0;

    // If the current device slot is uninitialized, reset to the first device.
    // This handles both the end-of-list and the empty-list cases.
    if (device >= SPI_DISPLAY_SLOT_COUNT || spi_display_slots[device].cs == -1) 
    {
        device = 0;
    }

    // If the first slot is also uninitialized, there's nothing to do.
    if (spi_display_slots[device].cs == -1)
    {
        return;
    }

    // Set the current global pinset for other components to use
    cur_spi_pin = spi_display_slots[device];

    // Move to the next device for the next call
    device++;
}
// --- End: config\components\drivers\spi_display_bus.src ---
