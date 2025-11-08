#include "subsystems/goblin_head/goblin_head_component_functions.hpp"
#include "components/interfaces/spi_display_bus.hdr"

// Auto-generated component aggregation file

// --- Begin: config\components\hardware\gc9a01.src ---
// gc9a01 component implementation
// This component is a data provider. It defines the physical characteristics
// of the GC9A01 display but contains no active logic.

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "config/components/hardware/gc9a01.hdr"
#include <stdlib.h>

static const char *TAG_gc9a01 = "gc9a01";

// Define GC9A01_InitialPixel
// Note: value should be set before allocating buffers containing GC9A01_Pixels
Pixel GC9A01_InitialPixel;

// Display Buffer Interface - provides buffer allocation and size info
// to positioned components (eyes, mouth, etc.)

/**
 * @brief Get display width in pixels
 * @return Display width (240 for GC9A01)
 */
uint16_t gc9a01_get_width(void) {
    return 240;
}

/**
 * @brief Get display height in pixels
 * @return Display height (240 for GC9A01)
 */
uint16_t gc9a01_get_height(void) {
    return 240;
}

/**
 * @brief Get total frame size in pixels
 * @return Total pixels in one frame (width * height)
 */
uint32_t getFrameSize(void) {
    return 240 * 240;  // 57,600 pixels
}

/**
 * @brief Get pixels per row (display width)
 * @return Number of pixels per row
 */
uint32_t getFrameRowSize(void) {
    return 240;
}

/**
 * @brief Get display size in pixels
 * @return Total number of pixels in one frame
 */
uint32_t getDisplaySize(void) {
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

// --- Begin: config\components\hardware\generic_spi_display.src ---
// generic_spi_display component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_generic_spi_display = "generic_spi_display";

esp_err_t generic_spi_display_init(void) {
    ESP_LOGI(TAG_generic_spi_display, "generic_spi_display init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void generic_spi_display_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_generic_spi_display, "generic_spi_display act");
}
// --- End: config\components\hardware\generic_spi_display.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_eye.src ---
// goblin_eye component implementation
// Generic goblin eye rendering using mood-based color effects
// Works with any positioned component (left_eye, right_eye, mouth) that provides display_buffer

#include "esp_log.h"
#include "Mood.hpp"
#include "core/memory/SharedMemory.hpp"
#include "config/shared_headers/mood_effects.hpp"

static const char *TAG_goblin_eye = "goblin_eye";

// Goblin emotion intensity multiplier - goblins show emotions STRONGLY (1.5x)
// Compare to bears which might use 0.5x (stoic) or cats which use 0.8x (aloof)
static constexpr float GOBLIN_EMOTION_INTENSITY = 1.5f;

// Mood tracking for optimization
static Mood lastMood;
static bool mood_initialized = false;

// Mood-to-color mapping for goblin eyes (defined here, used by adjustMood template)
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
    ESP_LOGI(TAG_goblin_eye, "Initializing goblin eye mood processing");
    
    // Clear mood tracking
    lastMood.clear();
    mood_initialized = false;
    
    ESP_LOGI(TAG_goblin_eye, "Goblin eye initialized (emotion intensity: %.1fx)", 
             GOBLIN_EMOTION_INTENSITY);
    return ESP_OK;
}

void goblin_eye_act(void)
{
    // Get display buffer from the positioned component (left_eye, right_eye, or mouth)
    // Note: display_buffer is file-scoped static in the positioned component
    // This component must be linked after the positioned component in the build
    
    // Get current global mood from shared memory
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr == nullptr) {
        return;
    }
    
    // Check if mood changed (optimization - only render when mood changes)
    if (lastMood != *mood_ptr || !mood_initialized) {
        // Apply mood effects to display buffer using generic template
        // The template internally uses Pixel_RGB565::fromBytes() to cast the buffer
        
        // Buffer size: 240x240 * 2 bytes per pixel = 115,200 bytes
        const uint32_t DISPLAY_PIXELS = 240 * 240;
        
        adjustMood<Pixel_RGB565>(
            display_buffer,        // unsigned char* from positioned component
            DISPLAY_PIXELS,
            *mood_ptr,
            goblin_mood_effects
        );
        
        lastMood = *mood_ptr;
        mood_initialized = true;
        
        ESP_LOGD(TAG_goblin_eye, "Applied mood effects to goblin eye buffer");
    }
}
    if (color_schema == nullptr)
    {
        return;  // Not configured yet
    }
    
    if (strcmp(color_schema, "Pixel_RGB444") == 0)
    {
        adjustBufferPersonality<Pixel_RGB444>();
    }
    else if (strcmp(color_schema, "Pixel_RGB555") == 0)
    {
        adjustBufferPersonality<Pixel_RGB555>();
    }
    else if (strcmp(color_schema, "Pixel_RGB565") == 0)
    {
        adjustBufferPersonality<Pixel_RGB565>();
    }
    else if (strcmp(color_schema, "Pixel_RGB666") == 0)
    {
        adjustBufferPersonality<Pixel_RGB666>();
    }
    else if (strcmp(color_schema, "Pixel_RGB888") == 0)
    {
        adjustBufferPersonality<Pixel_RGB888>();
    }
    else if (strcmp(color_schema, "Pixel_Grayscale") == 0)
    {
        adjustBufferPersonality<Pixel_Grayscale>();
    }
    else
    {
        ESP_LOGW(TAG_goblin_eye, "Unknown color schema: %s", color_schema);
    }
}
// --- End: config\bots\bot_families\goblins\head\goblin_eye.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_left_ear.src ---
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
// --- End: config\bots\bot_families\goblins\head\goblin_left_ear.src ---

// Inherited fields for goblin_left_eye
static char* color_schema = "Pixel_RGB565";
static char* display_width = 240;
static char* display_height = 240;
static char* bytes_per_pixel = 2;

// --- Begin: config\bots\bot_families\goblins\head\goblin_left_eye.src ---
// goblin_left_eye.src - Allocate shared display buffer and left eye position
// Component chain: goblin_left_eye (allocate) -> goblin_eye (render) -> generic_spi_display (send & free)

#include "esp_log.h"
#include "shared_headers/color_schema.hpp"

static const char* TAG = "goblin_left_eye";

// Shared display buffer - allocated here, used by goblin_eye, freed by generic_spi_display
// Type: unsigned char* (cast to PixelType* inside adjustMood based on color_schema)
static unsigned char* display_buffer = nullptr;

// Eye position (left eye relative to skull center)
struct EyePosition {
    int16_t x;      // -50 = left of center
    int16_t y;      // +30 = above center
    int16_t z;      // -35 = slightly back
} left_eye_position = {-50, 30, -35};

// Display configuration - from use_fields in goblin_left_eye.json
// Declared static here (first definition), used by child components via extern
static uint32_t display_width = 240;
static uint32_t display_height = 240;
static uint32_t bytes_per_pixel = 2;
static char* color_schema = "Pixel_RGB565";

void goblin_left_eye_init(void)
{

}
void goblin_left_eye_act(void) {

    size_t display_buffer_size_in_pixels = display_width * display_height;
    size_t display_buffer_size = display_buffer_size_in_pixels * bytes_per_pixel;
    
    ESP_LOGI(TAG, "Allocating display buffer for left eye (%u x %u, %u bytes total)",
             display_width, display_height, display_buffer_size);
    
    // Allocate shared buffer as unsigned char*
    // goblin_eye will cast to PixelType* based on color_schema
    display_buffer = (unsigned char*)malloc(display_buffer_size);
    if (!display_buffer) {
        ESP_LOGE(TAG, "Failed to allocate %u bytes for display buffer", display_buffer_size);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with neutral color (dark green iris)
    uint16_t neutral = 0x0400;  // RGB565: (0, 8, 0)
    uint16_t* buffer_u16 = (uint16_t*)display_buffer;
    for (uint32_t i = 0; i < display_buffer_size_in_pixels; i++) {
        buffer_u16[i] = neutral;
    }
    
    ESP_LOGI(TAG, "Display buffer allocated (position: %d,%d,%d mm)",
             left_eye_position.x, left_eye_position.y, left_eye_position.z);
    
    return ESP_OK;
}
// --- End: config\bots\bot_families\goblins\head\goblin_left_eye.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_mouth.src ---
// goblin_mouth.src - Allocate shared display buffer for mouth
// Component chain: goblin_mouth (allocate) -> goblin_mouth_render (render) -> generic_spi_display (send & free)

#include "esp_log.h"
#include "config/shared_headers/color_schema.hpp"

static const char* TAG = "goblin_mouth";

// Shared display buffer - allocated here, used by goblin_mouth_render, freed by generic_spi_display
// Type: unsigned char* (cast to PixelType* inside adjustMood based on color_schema)
static unsigned char* display_buffer = nullptr;

// Mouth position (relative to skull center)
struct MouthPosition {
    int16_t x;      // 0 = centered
    int16_t y;      // -80 = below center
    int16_t z;      // -20 = slightly back
} mouth_position = {0, -80, -20};

// Display configuration - set from use_fields in JSON
// These values are injected at compile time via header includes
extern uint32_t display_width;
extern uint32_t display_height;
extern uint32_t bytes_per_pixel;

esp_err_t goblin_mouth_init(void) {
    size_t display_buffer_size = display_width * display_height * bytes_per_pixel;
    
    ESP_LOGI(TAG, "Allocating display buffer for mouth (%u x %u, %u bytes total)",
             display_width, display_height, display_buffer_size);
    
    // Allocate shared buffer as unsigned char*
    // goblin_mouth_render will cast to PixelType* based on color_schema
    display_buffer = (unsigned char*)malloc(display_buffer_size);
    if (!display_buffer) {
        ESP_LOGE(TAG, "Failed to allocate %u bytes for display buffer", display_buffer_size);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with neutral color (dark green)
    uint16_t neutral = 0x0400;  // RGB565: (0, 8, 0)
    uint16_t* buffer_u16 = (uint16_t*)display_buffer;
    for (uint32_t i = 0; i < (display_buffer_size / bytes_per_pixel); i++) {
        buffer_u16[i] = neutral;
    }
    
    ESP_LOGI(TAG, "Display buffer allocated (position: %d,%d,%d mm)",
             mouth_position.x, mouth_position.y, mouth_position.z);
    
    return ESP_OK;
}

void goblin_mouth_act(void) {
    // Buffer is managed by component chain
    // goblin_mouth_render renders into it, generic_spi_display frees it
}
// --- End: config\bots\bot_families\goblins\head\goblin_mouth.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_nose.src ---
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
// --- End: config\bots\bot_families\goblins\head\goblin_nose.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_right_ear.src ---
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
// --- End: config\bots\bot_families\goblins\head\goblin_right_ear.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_right_eye.src ---
// goblin_right_eye.src - Allocate shared display buffer for right eye
// Component chain: goblin_right_eye (allocate) ? goblin_eye (render) ? generic_spi_display (send & free)

#include "esp_log.h"

static const char* TAG = "goblin_right_eye";

// Shared display buffer - allocated here, used by goblin_eye, freed by generic_spi_display
// Type: unsigned char* (cast to PixelType* inside adjustMood based on color_schema)
static unsigned char* display_buffer = nullptr;
static const uint32_t DISPLAY_WIDTH = 240;
static const uint32_t DISPLAY_HEIGHT = 240;
static const uint32_t DISPLAY_PIXELS = DISPLAY_WIDTH * DISPLAY_HEIGHT;
static const uint32_t DISPLAY_BYTES_RGB565 = DISPLAY_PIXELS * 2;  // RGB565 = 2 bytes/pixel

// Eye position (right eye relative to skull center)
struct EyePosition {
    int16_t x;      // +50 = right of center
    int16_t y;      // +30 = above center
    int16_t z;      // -35 = slightly back
} right_eye_position = {50, 30, -35};

esp_err_t goblin_right_eye_init(void)
{
    ESP_LOGI(TAG, "Allocating display buffer for right eye (%u bytes)", DISPLAY_BYTES_RGB565);
    
    // Allocate shared buffer as unsigned char*
    // goblin_eye will cast to PixelType* (Pixel_RGB565*) based on color_schema
    display_buffer = (unsigned char*)malloc(DISPLAY_BYTES_RGB565);
    if (!display_buffer) {
        ESP_LOGE(TAG, "Failed to allocate %u bytes for display buffer", DISPLAY_BYTES_RGB565);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with neutral color (green iris)
    // As uint16_t: 0x0400 = RGB565 (0, 8, 0) = dark green
    uint16_t neutral = 0x0400;
    uint16_t* buffer_u16 = (uint16_t*)display_buffer;
    for (uint32_t i = 0; i < DISPLAY_PIXELS; i++) {
        buffer_u16[i] = neutral;
    }
    
    ESP_LOGI(TAG, "Display buffer allocated (position: %d,%d,%d mm)",
             right_eye_position.x, right_eye_position.y, right_eye_position.z);
    
    return ESP_OK;
}

void goblin_right_eye_act(void)
{
    // Buffer is managed by component chain
    // goblin_eye renders into it, generic_spi_display frees it
}
// --- End: config\bots\bot_families\goblins\head\goblin_right_eye.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_speaker.src ---
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
// --- End: config\bots\bot_families\goblins\head\goblin_speaker.src ---

// --- Begin: config\components\hardware\hc_sr04_sensor.src ---
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @file hc_sr04_sensor.src
 * @brief Implementation for hc_sr04_sensor component
 * 
 * TODO: Implement component initialization and action logic
 */

/**
 * @brief Initialize hc_sr04_sensor
 * Performs any setup needed for this component
 */
esp_err_t hc_sr04_sensor_init(void)
{
    // TODO: Implement initialization
    return ESP_OK;
}

/**
 * @brief Execute hc_sr04_sensor action
 * Called periodically during subsystem dispatch
 */
void hc_sr04_sensor_act(void)
{
    // TODO: Implement component behavior
}
// --- End: config\components\hardware\hc_sr04_sensor.src ---

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

// --- Begin: config\components\interfaces\i2s_bus.src ---
// i2s_bus component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_i2s_bus = "i2s_bus";

esp_err_t i2s_bus_0_init(void) {
    ESP_LOGI(TAG_i2s_bus, "i2s_bus init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void i2s_bus_0_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_i2s_bus, "i2s_bus act");
}
// --- End: config\components\interfaces\i2s_bus.src ---

// --- Begin: config\components\drivers\i2s_driver.src ---
// i2s_driver component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
static const char *TAG_i2s_driver = "i2s_driver";

esp_err_t i2s_driver_init(void) {
    ESP_LOGI(TAG_i2s_driver, "i2s_driver init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void i2s_driver_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_i2s_driver, "i2s_driver act");
}
// --- End: config\components\drivers\i2s_driver.src ---

// --- Begin: config\components\hardware\ili9341.src ---
// ili9341 component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
#include "ili9341.hdr"

static const char *TAG_ili9341 = "ili9341";

// Define the initial pixel value (default black)
ILI9341_Pixel ILI9341_InitialPixel = {0, 0, 0};

esp_err_t ili9341_init(void) {
    ESP_LOGI(TAG_ili9341, "ili9341 init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void ili9341_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD(TAG_ili9341, "ili9341 act");
}

size_t ili9341_get_width(void) {
    return 320;
}

size_t ili9341_get_height(void) {
    return 240;
}

size_t ili9341_getFrameSize(void) {
    return ili9341_get_width() * ili9341_get_height() * sizeof(ILI9341_Pixel);
}
// --- End: config\components\hardware\ili9341.src ---

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

// --- Begin: config\components\interfaces\spi_display_bus.src ---
// spi_display_bus component implementation
// Dedicated SPI bus for display devices with dynamic pin assignment

#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp32_s3_r8n16_pin_assignments.h"
#include "components/spi_display_bus.hdr"

#include <stddef.h>

static constexpr size_t SPI_DISPLAY_SLOT_COUNT = 32U;
static spi_display_pinset_t spi_display_slots[SPI_DISPLAY_SLOT_COUNT];
spi_display_pinset_t cur_spi_display_pin;

static const char *TAG_spi_display_bus = "spi_display_bus";

static spi_display_pinset_t shared_display_pins;

// Helper to claim the next available GPIO from the provided assignable list
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
    return -1; // Unreachable, abort will terminate execution
}

esp_err_t spi_display_bus_init(void) {
    ESP_LOGI(TAG_spi_display_bus, "=== SPI_DISPLAY_BUS_INIT STARTED ===");

    // Locate the first available slot for a display device
    size_t slot = 0;
    while (slot < SPI_DISPLAY_SLOT_COUNT && spi_display_slots[slot].cs != -1) {
        ++slot;
    }

    if (slot >= SPI_DISPLAY_SLOT_COUNT) {
        ESP_LOGE(TAG_spi_display_bus, "No remaining SPI display device slots available");
        return ESP_FAIL;
    }

    // Assign shared pins (MOSI, CLK, RESET) once for the bus
    const bool shared_unassigned = (shared_display_pins.mosi < 0);
    if (shared_unassigned) {
        shared_display_pins.mosi = get_next_assignable(spi_assignable, spi_assignable_count);
        shared_display_pins.clk = get_next_assignable(spi_assignable, spi_assignable_count);
        shared_display_pins.rst = get_next_assignable(spi_assignable, spi_assignable_count);

        spi_bus_config_t bus_cfg = {
            .mosi_io_num = shared_display_pins.mosi,
            .miso_io_num = -1,
            .sclk_io_num = shared_display_pins.clk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 240 * 240 * 2 + 8,
        };

        const esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
        ESP_LOGI(TAG_spi_display_bus, "SPI bus initialize result: %s", esp_err_to_name(ret));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG_spi_display_bus, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI(TAG_spi_display_bus,
                 "Shared SPI display pins assigned MOSI:%d CLK:%d RST:%d",
                 shared_display_pins.mosi,
                 shared_display_pins.clk,
                 shared_display_pins.rst);
    }

    spi_display_pinset_t pins = shared_display_pins;
    pins.cs = get_next_assignable(spi_assignable, spi_assignable_count);
    pins.dc = get_next_assignable(spi_assignable, spi_assignable_count);
    pins.bl = get_next_assignable(spi_assignable, spi_assignable_count);
    pins.handle = nullptr;

    spi_display_slots[slot] = pins;

    ESP_LOGI(TAG_spi_display_bus,
             "Display slot %u assigned pins MOSI:%d CLK:%d CS:%d DC:%d BL:%d RST:%d",
             static_cast<unsigned>(slot),
             pins.mosi,
             pins.clk,
             pins.cs,
             pins.dc,
             pins.bl,
             pins.rst);

    return ESP_OK;
}

void spi_display_bus_act(void) {
    static int slot = 0;

    if (spi_display_slots[0].cs == -1) {
        slot = 0;
        cur_spi_display_pin = spi_display_pinset_t();
        return;
    }

    if (slot < 0 || static_cast<size_t>(slot) >= SPI_DISPLAY_SLOT_COUNT ||
        spi_display_slots[static_cast<size_t>(slot)].cs == -1) {
        slot = 0;
    }

    cur_spi_display_pin = spi_display_slots[static_cast<size_t>(slot)];

    ++slot;
    if (static_cast<size_t>(slot) >= SPI_DISPLAY_SLOT_COUNT ||
        spi_display_slots[static_cast<size_t>(slot)].cs == -1) {
        slot = 0;
    }
}
// --- End: config\components\interfaces\spi_display_bus.src ---
