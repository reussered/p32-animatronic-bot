#include "subsystems/goblin_head/goblin_head_component_functions.hpp"
#include "components/interfaces/spi_display_bus.hdr"
#include "core/memory/SharedMemory.hpp"
#include "with.hpp"
#include "config/shared_headers/PixelType.hpp"
#include "shared/MicrophoneData.hpp"
#include "esp_random.h"

// Auto-generated component aggregation file

// Subsystem-scoped static variables (shared across all components in this file)
static uint32_t display_width = 240;
static uint32_t display_height = 240;
static uint32_t bytes_per_pixel = 2;  // RGB565
static uint8_t* front_buffer = NULL;
static uint8_t* back_buffer = NULL;
static uint32_t display_size = 0;
static int current_row_count = 10;

// --- Begin: config\components\hardware\gc9a01.src ---
// gc9a01 component implementation
// Defines display parameters via gc9a01.hdr for upstream components
// Actual display I/O handled by lower-level driver (generic_spi_display)

#include "esp_log.h"

esp_err_t gc9a01_init(void)
{
    ESP_LOGI("gc9a01", "gc9a01 display initialized");
    return ESP_OK;
}

void gc9a01_act(void)
{
    // No-op: display I/O handled by lower layers
}
// --- End: config\components\hardware\gc9a01.src ---

// --- Begin: config\components\drivers\generic_spi_display.src ---
// generic_spi_display component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
esp_err_t generic_spi_display_init(void) {
    ESP_LOGI("generic_spi_display", "generic_spi_display init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void generic_spi_display_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD("generic_spi_display", "generic_spi_display act");
}
// --- End: config\components\drivers\generic_spi_display.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_eye.src ---
// goblin_eye component implementation
// Generic goblin eye rendering using mood-based color effects
// Works with any positioned component (left_eye, right_eye, mouth) that provides display_buffer

#include "esp_log.h"
#include "shared_headers/color_schema.hpp"
#include "shared/mood.hpp"
#include "core/memory/SharedMemory.hpp"

// Note: Display configuration and buffers declared at top of generated file (subsystem scope)

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
    ESP_LOGI("goblin_eye", "Initializing goblin eye mood processing");
    
    // Clear mood tracking
    lastMood.clear();
    mood_initialized = false;
    
    ESP_LOGI("goblin_eye", "Goblin eye initialized (emotion intensity: %.1fx)", 
             GOBLIN_EMOTION_INTENSITY);
    return ESP_OK;
}

void goblin_eye_act(void)
{
    // Apply mood effects to front_buffer (allocated by goblin_left_eye)
    // Note: Variables are file-scoped static shared across this subsystem
    
    // Check if buffer is available
    if (front_buffer == NULL || display_size == 0) {
        return;
    }
    
    // Get current global mood from shared memory
    Mood* mood_ptr = GSM.read<Mood>();
    if (mood_ptr == nullptr) {
        return;
    }
    
    // Check if mood changed (optimization - only render when mood changes)
    if (lastMood != *mood_ptr || !mood_initialized) {
        // Calculate pixel count from buffer size
        const uint32_t pixel_count = display_size / bytes_per_pixel;
        
        // Apply mood effects to display buffer
        adjustMood<Pixel_RGB565>(
            front_buffer,
            pixel_count,
            *mood_ptr,
            goblin_mood_effects
        );
        
        lastMood = *mood_ptr;
        mood_initialized = true;
        
        ESP_LOGD("goblin_eye", "Applied mood effects to buffer (%u pixels)", pixel_count);
    }
}
// --- End: config\bots\bot_families\goblins\head\goblin_eye.src ---

// --- Begin: config\components\creature_specific\goblin_head_neck_motor.src ---
// Removed: #include "goblin_head_neck_motor.hdr" - .hdr content aggregated into .hpp
#include "esp_log.h"

// Stub implementations for missing neck_motor_3dof functions
// TODO: These should come from hardware/motors/neck_motor_3dof component
esp_err_t neck_motor_3dof_init(void) {
    ESP_LOGW("neck_motor_3dof", "Stub init - no hardware");
    return ESP_OK;
}

void neck_motor_3dof_act(void) {
    // No-op
}

esp_err_t neck_set_pan(int32_t position, uint32_t speed) {
    ESP_LOGD("neck_motor_3dof", "Stub pan: %ld @ %lu", position, speed);
    return ESP_OK;
}

esp_err_t neck_set_tilt(int32_t position, uint32_t speed) {
    ESP_LOGD("neck_motor_3dof", "Stub tilt: %ld @ %lu", position, speed);
    return ESP_OK;
}

esp_err_t neck_set_roll(int32_t position, uint32_t speed) {
    ESP_LOGD("neck_motor_3dof", "Stub roll: %ld @ %lu", position, speed);
    return ESP_OK;
}

// Initialize the goblin head neck motor controller
esp_err_t goblin_head_neck_motor_init(void)
{
    ESP_LOGI("goblin_head_neck_motor", "Initializing goblin head neck motor controller");
    
    // Initialize the underlying hardware
    esp_err_t ret = neck_motor_3dof_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE("goblin_head_neck_motor", "Failed to initialize neck motor 3DOF hardware: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Center the neck to neutral position
    ret = goblin_head_neck_motor_center();
    if (ret != ESP_OK)
    {
        ESP_LOGE("goblin_head_neck_motor", "Failed to center neck to neutral position: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI("goblin_head_neck_motor", "Goblin head neck motor controller initialized successfully");
    return ESP_OK;
}

// Main activity loop for neck motor
void goblin_head_neck_motor_act(void)
{
    // Delegate to hardware layer
    neck_motor_3dof_act();
}

// Set specific pose in degrees
esp_err_t goblin_head_neck_motor_set_pose(float pan_degrees, float tilt_degrees, float roll_degrees)
{
    // Validate ranges for goblin-specific limits
    if (pan_degrees < -60.0f || pan_degrees > 60.0f)
    {
        ESP_LOGW("goblin_head_neck_motor", "Pan angle %.1f out of range [-60, 60], clamping", pan_degrees);
        pan_degrees = (pan_degrees < -60.0f) ? -60.0f : 60.0f;
    }
    
    if (tilt_degrees < -30.0f || tilt_degrees > 45.0f)
    {
        ESP_LOGW("goblin_head_neck_motor", "Tilt angle %.1f out of range [-30, 45], clamping", tilt_degrees);
        tilt_degrees = (tilt_degrees < -30.0f) ? -30.0f : 45.0f;
    }
    
    if (roll_degrees < -15.0f || roll_degrees > 15.0f)
    {
        ESP_LOGW("goblin_head_neck_motor", "Roll angle %.1f out of range [-15, 15], clamping", roll_degrees);
        roll_degrees = (roll_degrees < -15.0f) ? -15.0f : 15.0f;
    }
    
    // Call hardware layer with validated parameters (convert degrees to steps)
    esp_err_t err;
    err = neck_set_pan((int32_t)(pan_degrees * 106.67f), 1000);
    if (err != ESP_OK) return err;
    err = neck_set_tilt((int32_t)(tilt_degrees * 106.67f), 1000);
    if (err != ESP_OK) return err;
    err = neck_set_roll((int32_t)(roll_degrees * 106.67f), 1000);
    return err;
}

// Center neck to neutral position
esp_err_t goblin_head_neck_motor_center(void)
{
    ESP_LOGI("goblin_head_neck_motor", "Centering neck to neutral position");
    return goblin_head_neck_motor_set_pose(0.0f, 0.0f, 0.0f);
}

// Perform a nodding motion
esp_err_t goblin_head_neck_motor_nod(void)
{
    ESP_LOGI("goblin_head_neck_motor", "Performing nod gesture");
    
    esp_err_t ret = goblin_head_neck_motor_set_pose(0.0f, 15.0f, 0.0f);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(500));
    
    ret = goblin_head_neck_motor_set_pose(0.0f, -10.0f, 0.0f);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(500));
    
    return goblin_head_neck_motor_center();
}

// Perform a head shake motion
esp_err_t goblin_head_neck_motor_shake(void)
{
    ESP_LOGI("goblin_head_neck_motor", "Performing shake gesture");
    
    esp_err_t ret = goblin_head_neck_motor_set_pose(-30.0f, 0.0f, 0.0f);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(300));
    
    ret = goblin_head_neck_motor_set_pose(30.0f, 0.0f, 0.0f);
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(300));
    
    return goblin_head_neck_motor_center();
}
// --- End: config\components\creature_specific\goblin_head_neck_motor.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_left_eye.src ---
// goblin_left_eye.src - Allocate shared display buffer and left eye position
// Component chain: goblin_left_eye (allocate) -> goblin_eye (render) -> generic_spi_display (send & free)

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "shared_headers/color_schema.hpp"

// Note: Display configuration and buffers declared in goblin_eye.src (processed first)

// Eye position (left eye relative to skull center)
struct EyePosition {
    int16_t x;      // -50 = left of center
    int16_t y;      // +30 = above center
    int16_t z;      // -35 = slightly back
} left_eye_position = {-50, 30, -35};

esp_err_t goblin_left_eye_init(void)
{
    // Start with a baseline row count (can be tuned based on available RAM)
    current_row_count = 10;  // Initial: 10 rows at a time
    return ESP_OK;
}

void goblin_left_eye_act(void)
{
    // Allocate buffers if not already done
    if (!front_buffer || !back_buffer)
    {
        // Calculate buffer size based on current row count
        display_size = display_width * current_row_count * bytes_per_pixel;
        
        ESP_LOGI("goblin_left_eye", "Allocating display buffers for left eye (%u rows x %u width, %u bytes each)",
                 current_row_count, display_width, display_size);
            
        // Allocate front buffer (DMA-capable internal RAM)
        front_buffer = (uint8_t*)heap_caps_malloc(display_size, MALLOC_CAP_DMA);
        if (!front_buffer)
        {
            ESP_LOGE("goblin_left_eye", "Failed to allocate %u bytes for front buffer", display_size);
            return;
        }
        
        // Allocate back buffer (DMA-capable internal RAM)
        back_buffer = (uint8_t*)heap_caps_malloc(display_size, MALLOC_CAP_DMA);
        if (!back_buffer)
        {
            ESP_LOGE("goblin_left_eye", "Failed to allocate %u bytes for back buffer", display_size);
            free(front_buffer);
            front_buffer = NULL;
            return;
        }
        
        // Initialize both buffers with neutral color (dark green iris)
        uint16_t neutral = 0x0400;  // RGB565: (0, 8, 0)
        uint16_t* front_u16 = (uint16_t*)front_buffer;
        uint16_t* back_u16 = (uint16_t*)back_buffer;
        
        size_t buffer_size_in_pixels = display_width * current_row_count;
        for (uint32_t i = 0; i < buffer_size_in_pixels; i++) {
            front_u16[i] = neutral;
            back_u16[i] = neutral;
        }
        
        ESP_LOGI("goblin_left_eye", "Display buffers allocated (position: %d,%d,%d mm)",
                 left_eye_position.x, left_eye_position.y, left_eye_position.z);
    }
    
    // === MOOD PROCESSING SECTION ===
    // TODO: Add mood-based eye rendering here
    // This is where goblin_eye rendering logic would go
    
    // Buffers are now allocated and available for the next component in the chain
    // (Data flows via file-scoped static variables in the single compilation unit)
}
// --- End: config\bots\bot_families\goblins\head\goblin_left_eye.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_mouth.src ---
// goblin_mouth.src - Allocate shared display buffer for mouth
// Component chain: goblin_mouth (allocate) -> goblin_mouth_render (render) -> generic_spi_display (send & free)

#include "esp_log.h"
#include "config/shared_headers/color_schema.hpp"

// Shared display buffer - allocated here, used by goblin_mouth_render, freed by generic_spi_display
// Type: unsigned char* (cast to PixelType* inside adjustMood based on color_schema)
static unsigned char* mouth_display_buffer = nullptr;

// Mouth position (relative to skull center)
struct GoblinMouthPosition {
    int16_t x;      // 0 = centered
    int16_t y;      // -80 = below center
    int16_t z;      // -20 = slightly back
} mouth_position = {0, -80, -20};

// Display configuration comes from file-scoped statics defined in goblin_left_eye.src
// (all .src files in this subsystem share the same compilation unit)

esp_err_t goblin_mouth_init(void) {
    size_t display_buffer_size = display_width * display_height * bytes_per_pixel;
    
    ESP_LOGI("goblin_mouth", "Allocating display buffer for mouth (%u x %u, %u bytes total)",
             display_width, display_height, display_buffer_size);
    
    // Allocate shared buffer as unsigned char*
    // goblin_mouth_render will cast to PixelType* based on color_schema
    mouth_display_buffer = (unsigned char*)malloc(display_buffer_size);
    if (!mouth_display_buffer) {
        ESP_LOGE("goblin_mouth", "Failed to allocate %u bytes for display buffer", display_buffer_size);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with neutral color (dark green)
    uint16_t neutral = 0x0400;  // RGB565: (0, 8, 0)
    uint16_t* buffer_u16 = (uint16_t*)mouth_display_buffer;
    for (uint32_t i = 0; i < (display_buffer_size / bytes_per_pixel); i++) {
        buffer_u16[i] = neutral;
    }
    
    ESP_LOGI("goblin_mouth", "Display buffer allocated (position: %d,%d,%d mm)",
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
// Integrates HC-SR04 ultrasonic sensor for proximity detection with audio responses

#include "esp_log.h"
// Removed: #include "components/hardware/hc_sr04.hdr" - .hdr content aggregated into .hpp
// Removed: #include "components/hardware/speaker.hdr" - .hdr content aggregated into .hpp

// Nose sensor state
typedef struct {
    float last_distance_cm;       // Last valid distance reading
    uint32_t reading_count;       // Total readings taken
    uint32_t valid_readings;      // Number of valid readings
    bool proximity_alert;         // True when object is very close
} goblin_nose_state_t;

static goblin_nose_state_t nose_state = {
    .last_distance_cm = -1.0f,
    .reading_count = 0,
    .valid_readings = 0,
    .proximity_alert = false
};

// Proximity thresholds
#define PROXIMITY_ALERT_CM 10.0f    // Alert if object closer than 10cm
#define CLOSE_DISTANCE_CM 20.0f     // Close distance threshold
#define FAR_DISTANCE_CM 100.0f      // Far distance threshold

/**
 * @brief Initialize goblin nose with HC-SR04 sensor
 */
esp_err_t goblin_nose_init(void) {
    ESP_LOGI("goblin_nose", "Initializing goblin nose with HC-SR04 sensor");
    
    // Initialize the underlying HC-SR04 hardware
    esp_err_t ret = hc_sr04_init();
    if (ret != ESP_OK) {
        ESP_LOGE("goblin_nose", "Failed to initialize HC-SR04 sensor: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI("goblin_nose", "Goblin nose ready - proximity sensing enabled");
    return ESP_OK;
}

/**
 * @brief Process nose sensor readings and update state
 */
void goblin_nose_act(void) {
    // Update the HC-SR04 hardware readings
    hc_sr04_act();
    
    nose_state.reading_count++;
    
    // Get current distance reading
    float distance_cm = hc_sr04_get_distance_cm();
    bool is_valid = hc_sr04_is_valid_reading();
    
    if (is_valid) {
        nose_state.valid_readings++;
        nose_state.last_distance_cm = distance_cm;
        
        // Update proximity alert status
        bool was_alert = nose_state.proximity_alert;
        nose_state.proximity_alert = (distance_cm <= PROXIMITY_ALERT_CM);
        
        // Log interesting events and trigger audio responses
        if (nose_state.proximity_alert && !was_alert) {
            ESP_LOGW("goblin_nose", "PROXIMITY ALERT! Object detected at %.1f cm", distance_cm);
            
            // Trigger proximity-based goblin response
            if (distance_cm <= 5.0f) {
                // Very close - aggressive response
                speaker_play_emotional_response("angry", 0.8f);
                speaker_speak_goblin_phrase("warning");
            } else if (distance_cm <= 10.0f) {
                // Close - curious/alert response
                speaker_play_emotional_response("surprised", 0.6f);
                speaker_speak_goblin_phrase("curious");
            }
            
        } else if (!nose_state.proximity_alert && was_alert) {
            ESP_LOGI("goblin_nose", "Proximity alert cleared - object moved to %.1f cm", distance_cm);
            
            // Object moved away - relieved sound
            speaker_play_sound_by_name("goblin_grunt_yes");
        }
        
        // Periodic distance reporting (every 50 readings)
        if (nose_state.reading_count % 50 == 0) {
            const char* distance_desc;
            if (distance_cm <= PROXIMITY_ALERT_CM) {
                distance_desc = "VERY CLOSE";
            } else if (distance_cm <= CLOSE_DISTANCE_CM) {
                distance_desc = "CLOSE";
            } else if (distance_cm <= FAR_DISTANCE_CM) {
                distance_desc = "MEDIUM";
            } else {
                distance_desc = "FAR";
            }
            
            ESP_LOGI("goblin_nose", "Distance: %.1f cm (%s) - Success rate: %lu/%lu (%.1f%%)",
                     distance_cm, distance_desc,
                     nose_state.valid_readings, nose_state.reading_count,
                     (float)nose_state.valid_readings * 100.0f / nose_state.reading_count);
        }
    } else {
        ESP_LOGD("goblin_nose", "No valid sensor reading (out of range or obstacle)");
    }
}

/**
 * @brief Get the current nose sensor distance reading
 * @return Distance in cm, or -1 if no valid reading
 */
float goblin_nose_get_distance(void) {
    return nose_state.last_distance_cm;
}

/**
 * @brief Check if there's a proximity alert (object very close)
 * @return true if object is closer than proximity threshold
 */
bool goblin_nose_proximity_alert(void) {
    return nose_state.proximity_alert;
}

/**
 * @brief Get sensor statistics
 * @param total_readings Output: total number of readings attempted
 * @param valid_readings Output: number of successful readings
 * @return Current success rate as percentage
 */
float goblin_nose_get_stats(uint32_t* total_readings, uint32_t* valid_readings) {
    if (total_readings) *total_readings = nose_state.reading_count;
    if (valid_readings) *valid_readings = nose_state.valid_readings;
    
    return (nose_state.reading_count > 0) ? 
           (float)nose_state.valid_readings * 100.0f / nose_state.reading_count : 0.0f;
}
// --- End: config\bots\bot_families\goblins\head\goblin_nose.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_right_eye.src ---
// goblin_right_eye.src - Allocate shared display buffer for right eye
// Component chain: goblin_right_eye (allocate) ? goblin_eye (render) ? generic_spi_display (send & free)

#include "esp_log.h"

// Shared display buffer - allocated here, used by goblin_eye, freed by generic_spi_display
// Type: unsigned char* (cast to PixelType* inside adjustMood based on color_schema)
static unsigned char* right_display_buffer = nullptr;
static const uint32_t RIGHT_DISPLAY_WIDTH = 240;
static const uint32_t RIGHT_DISPLAY_HEIGHT = 240;
static const uint32_t RIGHT_DISPLAY_PIXELS = RIGHT_DISPLAY_WIDTH * RIGHT_DISPLAY_HEIGHT;
static const uint32_t RIGHT_DISPLAY_BYTES_RGB565 = RIGHT_DISPLAY_PIXELS * 2;  // RGB565 = 2 bytes/pixel

// Eye position (right eye relative to skull center)
struct RightEyePosition {
    int16_t x;      // +50 = right of center
    int16_t y;      // +30 = above center
    int16_t z;      // -35 = slightly back
} right_eye_position = {50, 30, -35};

esp_err_t goblin_right_eye_init(void)
{
    ESP_LOGI("goblin_right_eye", "Allocating display buffer for right eye (%u bytes)", RIGHT_DISPLAY_BYTES_RGB565);
    
    // Allocate shared buffer as unsigned char*
    // goblin_eye will cast to PixelType* (Pixel_RGB565*) based on color_schema
    right_display_buffer = (unsigned char*)malloc(RIGHT_DISPLAY_BYTES_RGB565);
    if (!right_display_buffer) {
        ESP_LOGE("goblin_right_eye", "Failed to allocate %u bytes for display buffer", RIGHT_DISPLAY_BYTES_RGB565);
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize buffer with neutral color (green iris)
    // As uint16_t: 0x0400 = RGB565 (0, 8, 0) = dark green
    uint16_t neutral = 0x0400;
    uint16_t* buffer_u16 = (uint16_t*)right_display_buffer;
    for (uint32_t i = 0; i < RIGHT_DISPLAY_PIXELS; i++) {
        buffer_u16[i] = neutral;
    }
    
    ESP_LOGI("goblin_right_eye", "Display buffer allocated (position: %d,%d,%d mm)",
             right_eye_position.x, right_eye_position.y, right_eye_position.z);
    
    return ESP_OK;
}

void goblin_right_eye_act(void)
{
    // Buffer is managed by component chain
    // goblin_eye renders into it, generic_spi_display frees it
}
// --- End: config\bots\bot_families\goblins\head\goblin_right_eye.src ---

// --- Begin: config\components\creature_specific\goblin_sinuses.src ---
// goblin_sinuses component implementation
// 3D-printed acoustic resonance chambers for natural sound amplification

#include "esp_log.h"

// Configuration variables set via use_fields
static uint16_t current_resonance_freq = 800;
static float current_amplification = 1.5f;
static uint8_t active_chambers = 3;

esp_err_t goblin_sinuses_init(void) {
    // Passive acoustic system - configuration is compile-time
    ESP_LOGI("goblin_sinuses", "Sinus cavity initialized - %d chambers, %.1fx amplification, %dHz resonance", 
             active_chambers, current_amplification, current_resonance_freq);
    
    // Physical cavity is passive - no electrical initialization needed
    // Configuration is handled through mechanical design and 3D printing
    
    return ESP_OK;
}

void goblin_sinuses_act(void) {
    // Passive acoustic system - no active processing needed
    // Sound waves naturally resonate through the 3D-printed chambers
    
    // Could add dynamic tuning in future versions with:
    // - Adjustable chamber volumes via servo-controlled baffles
    // - Variable resonance via mechanical dampers
    // - Real-time frequency analysis and optimization
}

esp_err_t goblin_sinuses_set_resonance(uint16_t frequency) {
    if (frequency < 100 || frequency > 10000) {
        ESP_LOGW("goblin_sinuses", "Resonance frequency %d out of optimal range", frequency);
        return ESP_ERR_INVALID_ARG;
    }
    
    current_resonance_freq = frequency;
    
    ESP_LOGI("goblin_sinuses", "Resonance tuned to %dHz", frequency);
    return ESP_OK;
}

esp_err_t goblin_sinuses_get_amplification(float *factor) {
    if (!factor) return ESP_ERR_INVALID_ARG;
    
    *factor = current_amplification;
    return ESP_OK;
}

esp_err_t goblin_sinuses_configure_voice(uint8_t voice_type) {
    switch (voice_type) {
        case 0: // Deep/bass voice
            current_resonance_freq = 400;
            current_amplification = 2.0f;
            break;
        case 1: // Normal voice  
            current_resonance_freq = 800;
            current_amplification = 1.5f;
            break;
        case 2: // High pitched voice
            current_resonance_freq = 1200;
            current_amplification = 1.2f;
            break;
        default:
            ESP_LOGW("goblin_sinuses", "Unknown voice type %d", voice_type);
            return ESP_ERR_INVALID_ARG;
    }
    
    // Configuration applied to static variables
    ESP_LOGI("goblin_sinuses", "Voice configured: type=%d, freq=%dHz, amp=%.1fx", 
             voice_type, current_resonance_freq, current_amplification);
    
    return ESP_OK;
}
// --- End: config\components\creature_specific\goblin_sinuses.src ---

// --- Begin: config\bots\bot_families\goblins\head\goblin_speaker.src ---
// goblin_speaker component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
esp_err_t goblin_speaker_init(void) {
    ESP_LOGI("goblin_speaker", "goblin_speaker init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void goblin_speaker_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD("goblin_speaker", "goblin_speaker act");
}
// --- End: config\bots\bot_families\goblins\head\goblin_speaker.src ---

// --- Begin: config\components\drivers\gpio_pair_driver.src ---
// gpio_pair_driver component implementation
// Debug driver for GPIO pairs - simulates HC-SR04 ultrasonic sensor timing

#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <math.h>

// Debug configuration
#define DEBUG_MODE 1  // Set to 0 for real GPIO hardware
#define SOUND_SPEED_CM_US 0.0343f  // Speed of sound: 343 m/s = 0.0343 cm/?s
#define MIN_DISTANCE_CM 2.0f
#define MAX_DISTANCE_CM 400.0f

// HC-SR04 measurement states
typedef enum {
    HC_SR04_IDLE = 0,
    HC_SR04_TRIGGERED,      // Trigger pulse sent, waiting for echo to start
    HC_SR04_MEASURING,      // Echo started, measuring duration
    HC_SR04_COMPLETE,       // Measurement done, result ready
    HC_SR04_TIMEOUT         // Measurement failed
} hc_sr04_state_t;

// GPIO pair debug simulation state
typedef struct {
    int trigger_pin;
    int echo_pin;
    bool configured;
    
    // Debug simulation state
    hc_sr04_state_t measurement_state;
    uint64_t trigger_time_us;
    uint64_t echo_start_us;
    uint32_t simulated_pulse_duration_us;
    uint32_t measurement_count;
    float current_distance_cm;
} gpio_pair_state_t;

static gpio_pair_state_t pair_state = {
    .trigger_pin = -1,
    .echo_pin = -1,
    .configured = false,
    .measurement_state = HC_SR04_IDLE,
    .trigger_time_us = 0,
    .echo_start_us = 0,
    .simulated_pulse_duration_us = 0,
    .measurement_count = 0,
    .current_distance_cm = 30.0f
};

/**
 * @brief Generate simulated distance for debug mode
 */
static float generate_simulated_distance(void) {
    pair_state.measurement_count++;
    
    // Simulate someone moving back and forth
    float time_factor = (float)pair_state.measurement_count * 0.05f;  // Slower oscillation
    float base_distance = 30.0f + 20.0f * sinf(time_factor * 0.2f);  // 10-50cm oscillation
    
    // Add noise
    float noise = ((float)(esp_random() % 1000) / 1000.0f - 0.5f) * 2.0f;  // ?1cm noise
    base_distance += noise;
    
    // Clamp to sensor range
    if (base_distance < MIN_DISTANCE_CM) base_distance = MIN_DISTANCE_CM;
    if (base_distance > MAX_DISTANCE_CM) base_distance = MAX_DISTANCE_CM;
    
    return base_distance;
}

/**
 * @brief Initialize GPIO pair driver
 */
esp_err_t gpio_pair_driver_init(void) {
    if (DEBUG_MODE) {
        ESP_LOGI("gpio_pair_driver", "GPIO pair driver init (DEBUG MODE)");
        ESP_LOGI("gpio_pair_driver", "Simulating HC-SR04 ultrasonic sensor timing");
    } else {
        ESP_LOGI("gpio_pair_driver", "GPIO pair driver init (HARDWARE MODE)");
    }
    
    pair_state.measurement_state = HC_SR04_IDLE;
    return ESP_OK;
}

/**
 * @brief Execute GPIO pair driver action
 */
void gpio_pair_driver_act(void) {
    // This driver is passive - actual work done on demand via measure functions
    ESP_LOGD("gpio_pair_driver", "GPIO pair driver act");
}

/**
 * @brief Configure GPIO pair for ultrasonic sensor
 */
esp_err_t gpio_pair_configure_ultrasonic(int trigger_pin, int echo_pin) {
    ESP_LOGI("gpio_pair_driver", "Configuring GPIO pair: trigger=%d, echo=%d", 
             trigger_pin, echo_pin);
    
    pair_state.trigger_pin = trigger_pin;
    pair_state.echo_pin = echo_pin;
    
    if (!DEBUG_MODE) {
        // Configure real GPIO pins
        gpio_config_t trigger_config = {
            .pin_bit_mask = (1ULL << trigger_pin),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        
        gpio_config_t echo_config = {
            .pin_bit_mask = (1ULL << echo_pin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        
        esp_err_t ret = gpio_config(&trigger_config);
        if (ret != ESP_OK) {
            ESP_LOGE("gpio_pair_driver", "Failed to configure trigger pin: %s", esp_err_to_name(ret));
            return ret;
        }
        
        ret = gpio_config(&echo_config);
        if (ret != ESP_OK) {
            ESP_LOGE("gpio_pair_driver", "Failed to configure echo pin: %s", esp_err_to_name(ret));
            return ret;
        }
        
        // Set trigger pin low initially
        gpio_set_level((gpio_num_t)trigger_pin, 0);
    }
    
    pair_state.configured = true;
    return ESP_OK;
}

/**
 * @brief Send trigger pulse to start ultrasonic measurement
 */
esp_err_t gpio_pair_trigger_ultrasonic(void) {
    if (!pair_state.configured) {
        ESP_LOGE("gpio_pair_driver", "GPIO pair not configured");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (pair_state.measurement_state != HC_SR04_IDLE) {
        ESP_LOGW("gpio_pair_driver", "Measurement already in progress");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (DEBUG_MODE) {
        // Debug simulation: prepare the measurement
        pair_state.current_distance_cm = generate_simulated_distance();
        
        // Simulate occasional failures (5% chance)
        if ((esp_random() % 100) < 5) {
            pair_state.measurement_state = HC_SR04_TIMEOUT;
            ESP_LOGD("gpio_pair_driver", "Simulated trigger failure");
            return ESP_ERR_TIMEOUT;
        }
        
        // Calculate pulse duration for the simulated distance
        // duration = (distance * 2) / sound_speed (round trip)
        pair_state.simulated_pulse_duration_us = (uint32_t)((pair_state.current_distance_cm * 2.0f) / SOUND_SPEED_CM_US);
        
        ESP_LOGD("gpio_pair_driver", "Debug trigger: %.1f cm -> %lu ?s pulse", 
                 pair_state.current_distance_cm, pair_state.simulated_pulse_duration_us);
    } else {
        // Real hardware: send 10?s trigger pulse
        gpio_set_level((gpio_num_t)pair_state.trigger_pin, 1);
        esp_rom_delay_us(10);
        gpio_set_level((gpio_num_t)pair_state.trigger_pin, 0);
    }
    
    pair_state.trigger_time_us = esp_timer_get_time();
    pair_state.measurement_state = HC_SR04_TRIGGERED;
    
    return ESP_OK;
}

/**
 * @brief Check echo pin status and return measurement when ready
 * Call this repeatedly after trigger until ESP_OK is returned
 */
esp_err_t gpio_pair_check_echo(uint32_t* pulse_duration_us) {
    if (!pair_state.configured) {
        return ESP_ERR_INVALID_STATE;
    }
    
    uint64_t current_time_us = esp_timer_get_time();
    
    if (DEBUG_MODE) {
        // Debug simulation timing
        switch (pair_state.measurement_state) {
            case HC_SR04_IDLE:
                return ESP_ERR_INVALID_STATE;
                
            case HC_SR04_TRIGGERED:
                // Simulate delay before echo starts (typ. 100-200?s)
                if ((current_time_us - pair_state.trigger_time_us) > 150) {
                    pair_state.echo_start_us = current_time_us;
                    pair_state.measurement_state = HC_SR04_MEASURING;
                }
                return ESP_ERR_NOT_FINISHED;
                
            case HC_SR04_MEASURING:
                // Check if simulated echo pulse is complete
                if ((current_time_us - pair_state.echo_start_us) >= pair_state.simulated_pulse_duration_us) {
                    pair_state.measurement_state = HC_SR04_COMPLETE;
                    *pulse_duration_us = pair_state.simulated_pulse_duration_us;
                    ESP_LOGD("gpio_pair_driver", "Debug measurement complete: %.1f cm", pair_state.current_distance_cm);
                    return ESP_OK;
                }
                return ESP_ERR_NOT_FINISHED;
                
            case HC_SR04_COMPLETE:
                // Measurement already complete
                *pulse_duration_us = pair_state.simulated_pulse_duration_us;
                return ESP_OK;
                
            case HC_SR04_TIMEOUT:
                return ESP_ERR_TIMEOUT;
        }
    } else {
        // Real hardware echo checking
        switch (pair_state.measurement_state) {
            case HC_SR04_TRIGGERED:
                // Wait for echo to go HIGH
                if (gpio_get_level((gpio_num_t)pair_state.echo_pin) == 1) {
                    pair_state.echo_start_us = current_time_us;
                    pair_state.measurement_state = HC_SR04_MEASURING;
                }
                // Check for timeout waiting for echo start
                if ((current_time_us - pair_state.trigger_time_us) > 30000) {
                    pair_state.measurement_state = HC_SR04_TIMEOUT;
                    return ESP_ERR_TIMEOUT;
                }
                return ESP_ERR_NOT_FINISHED;
                
            case HC_SR04_MEASURING:
                // Check if echo is still HIGH
                if (gpio_get_level((gpio_num_t)pair_state.echo_pin) == 0) {
                    // Echo went LOW - measurement complete
                    uint32_t duration = (uint32_t)(current_time_us - pair_state.echo_start_us);
                    pair_state.measurement_state = HC_SR04_COMPLETE;
                    *pulse_duration_us = duration;
                    return ESP_OK;
                }
                // Check for timeout during measurement
                if ((current_time_us - pair_state.echo_start_us) > 30000) {
                    pair_state.measurement_state = HC_SR04_TIMEOUT;
                    return ESP_ERR_TIMEOUT;
                }
                return ESP_ERR_NOT_FINISHED;
                
            case HC_SR04_COMPLETE:
                // Measurement already done
                return ESP_OK;
                
            default:
                return ESP_ERR_INVALID_STATE;
        }
    }
    
    return ESP_ERR_NOT_FINISHED;
}

/**
 * @brief Reset measurement state to idle
 */
void gpio_pair_reset_measurement(void) {
    pair_state.measurement_state = HC_SR04_IDLE;
    ESP_LOGD("gpio_pair_driver", "Measurement reset to idle");
}
// --- End: config\components\drivers\gpio_pair_driver.src ---

// --- Begin: config\components\hardware\hc_sr04.src ---
// HC-SR04 Ultrasonic Distance Sensor Hardware Component
// Uses gpio_pair_driver for pin management and protocol-correct timing

#include "esp_log.h"
// Removed: #include "components/drivers/gpio_pair_driver.hdr" - .hdr content aggregated into .hpp

// HC-SR04 pins (configured via dynamic pin assignment)
static int trigger_pin = -1;
static int echo_pin = -1;

// Sound speed constant for distance calculation
#define SOUND_SPEED_CM_US 0.0343f  // Speed of sound: 343 m/s = 0.0343 cm/us

// Measurement state
typedef enum {
    HC_SR04_SENSOR_IDLE = 0,
    HC_SR04_SENSOR_WAITING,      // Waiting for echo measurement
    HC_SR04_SENSOR_READY         // Measurement complete
} hc_sr04_sensor_state_t;

static hc_sr04_sensor_state_t sensor_state = HC_SR04_SENSOR_IDLE;
static float last_distance_cm = 0.0f;
static bool measurement_valid = false;



esp_err_t hc_sr04_init(void) {
    ESP_LOGI("hc_sr04", "Initializing HC-SR04 ultrasonic sensor");
    
    // Initialize the GPIO pair driver first
    esp_err_t ret = gpio_pair_driver_init();
    if (ret != ESP_OK) {
        ESP_LOGE("hc_sr04", "Failed to initialize GPIO pair driver: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Pin assignment happens via dynamic pin system
    // For now, using example pins - will be assigned dynamically
    trigger_pin = 12;  // Example - will be assigned dynamically
    echo_pin = 13;     // Example - will be assigned dynamically
    
    // Configure the GPIO pair for ultrasonic measurement
    ret = gpio_pair_configure_ultrasonic(trigger_pin, echo_pin);
    if (ret != ESP_OK) {
        ESP_LOGE("hc_sr04", "Failed to configure GPIO pair: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI("hc_sr04", "HC-SR04 initialized using GPIO pair (trigger: %d, echo: %d)", trigger_pin, echo_pin);
    return ESP_OK;

}



void hc_sr04_act(void) {
    uint32_t pulse_duration_us = 0;
    esp_err_t ret = ESP_ERR_NOT_FINISHED;
    
    switch (sensor_state) {
        case HC_SR04_SENSOR_IDLE:
            // Start new measurement by sending trigger pulse
            ret = gpio_pair_trigger_ultrasonic();
            if (ret == ESP_OK) {
                sensor_state = HC_SR04_SENSOR_WAITING;
                ESP_LOGD("hc_sr04", "Trigger pulse sent, waiting for echo");
            } else if (ret == ESP_ERR_TIMEOUT) {
                // Trigger failed - stay idle and try again next time
                measurement_valid = false;
                ESP_LOGD("hc_sr04", "Trigger failed, will retry");
            } else if (ret == ESP_ERR_INVALID_STATE) {
                // Measurement already in progress - shouldn't happen but handle gracefully
                ESP_LOGD("hc_sr04", "Trigger skipped - measurement already in progress");
            }
            break;
            
        case HC_SR04_SENSOR_WAITING:
            // Check if echo measurement is ready
            ret = gpio_pair_check_echo(&pulse_duration_us);
            if (ret == ESP_OK) {
                // Measurement complete - calculate distance
                last_distance_cm = (pulse_duration_us * SOUND_SPEED_CM_US) / 2.0f;
                measurement_valid = true;
                sensor_state = HC_SR04_SENSOR_READY;
                
                ESP_LOGD("hc_sr04", "Distance: %.2f cm (pulse: %lu us)", last_distance_cm, pulse_duration_us);
                
                // Reset to idle for next measurement cycle
                gpio_pair_reset_measurement();
                sensor_state = HC_SR04_SENSOR_IDLE;
                
            } else if (ret == ESP_ERR_TIMEOUT) {
                // Measurement timed out
                measurement_valid = false;
                sensor_state = HC_SR04_SENSOR_IDLE;
                gpio_pair_reset_measurement();
                ESP_LOGD("hc_sr04", "Measurement timeout");
            }
            // If ret == ESP_ERR_NOT_FINISHED, keep waiting
            break;
            
        case HC_SR04_SENSOR_READY:
            // Measurement is ready, reset to idle for next cycle
            gpio_pair_reset_measurement();
            sensor_state = HC_SR04_SENSOR_IDLE;
            break;
    }
}

/**
 * @brief Get the current distance reading in centimeters
 */
float hc_sr04_get_distance_cm(void) {
    return measurement_valid ? last_distance_cm : -1.0f;
}

/**
 * @brief Check if sensor has a valid reading
 */
bool hc_sr04_is_valid_reading(void) {
    return measurement_valid;
}
// --- End: config\components\hardware\hc_sr04.src ---

// --- Begin: config\components\interfaces\i2s_bus.src ---
// i2s_bus component implementation
// Auto-generated stub - needs actual implementation

#include "esp_log.h"
esp_err_t i2s_bus_0_init(void) {
    ESP_LOGI("i2s_bus", "i2s_bus init - STUB IMPLEMENTATION");
    // TODO: Add actual initialization code
    return ESP_OK;
}

void i2s_bus_0_act(void) {
    // TODO: Add actual action code
    // ESP_LOGD("i2s_bus", "i2s_bus act");
}
// --- End: config\components\interfaces\i2s_bus.src ---

// --- Begin: config\components\drivers\i2s_driver.src ---
// i2s_driver component implementation
// Debug implementation - streams audio data to PC via serial

#include "esp_log.h"
#include "esp_timer.h"
#include <math.h>

// Forward declarations
static float generate_goblin_waveform(float sample_time, float base_freq);

// Debug audio configuration
#define DEBUG_AUDIO_MODE 1      // Set to 0 for real I2S hardware
#define SAMPLE_RATE 44100       // 44.1kHz sample rate
#define AUDIO_BUFFER_SIZE 1024  // Samples per buffer
#define CHANNELS 1              // Mono audio

// Audio generation state
typedef struct {
    bool initialized;
    bool playing;
    uint32_t sample_count;
    float frequency_hz;         // Current tone frequency
    float amplitude;           // Current volume (0.0-1.0)
    uint64_t last_update_us;
    char sound_type[32];       // Current sound being played
} debug_audio_state_t;

static debug_audio_state_t audio_state = {
    .initialized = false,
    .playing = false,
    .sample_count = 0,
    .frequency_hz = 440.0f,    // A4 note
    .amplitude = 0.3f,         // 30% volume
    .last_update_us = 0,
    .sound_type = "idle"
};

esp_err_t i2s_driver_init(void) {
    if (DEBUG_AUDIO_MODE) {
        ESP_LOGI("i2s_driver", "I2S driver init (DEBUG AUDIO MODE)");
        ESP_LOGI("i2s_driver", "Audio will be streamed to PC via serial");
        ESP_LOGI("i2s_driver", "Sample rate: %d Hz, Channels: %d", SAMPLE_RATE, CHANNELS);
        
        // Print header for PC audio capture script
        printf("AUDIO_STREAM_START\n");
        printf("SAMPLE_RATE=%d\n", SAMPLE_RATE);
        printf("CHANNELS=%d\n", CHANNELS);
        printf("BUFFER_SIZE=%d\n", AUDIO_BUFFER_SIZE);
        printf("FORMAT=INT16\n");
        printf("AUDIO_HEADER_END\n");
        
    } else {
        ESP_LOGI("i2s_driver", "I2S driver init (HARDWARE MODE)");
        // TODO: Initialize real I2S hardware on GPIO 4,5,6
    }
    
    audio_state.initialized = true;
    audio_state.last_update_us = esp_timer_get_time();
    return ESP_OK;
}

void i2s_driver_act(void) {
    if (!audio_state.initialized) return;
    
    uint64_t current_time_us = esp_timer_get_time();
    
    // Generate audio at ~44.1kHz rate (every ~22.7us)
    if ((current_time_us - audio_state.last_update_us) >= 23) {
        
        if (DEBUG_AUDIO_MODE && audio_state.playing) {
            // Generate complex goblin audio sample
            float sample_time = (float)audio_state.sample_count / (float)SAMPLE_RATE;
            float sample_value;
            
            // Use different waveforms based on sound type
            if (strstr(audio_state.sound_type, "speech") || 
                strstr(audio_state.sound_type, "growl") || 
                strstr(audio_state.sound_type, "roar")) {
                // Complex goblin waveform for speech and vocalizations
                sample_value = generate_goblin_waveform(sample_time, audio_state.frequency_hz);
            } else {
                // Simple sine wave for system sounds
                sample_value = sinf(2.0f * M_PI * audio_state.frequency_hz * sample_time);
            }
            
            // Apply amplitude and clamp
            sample_value *= audio_state.amplitude;
            if (sample_value > 1.0f) sample_value = 1.0f;
            if (sample_value < -1.0f) sample_value = -1.0f;
            
            // Convert to 16-bit integer
            int16_t audio_sample = (int16_t)(sample_value * 32767.0f);
            
            // Stream to PC via serial (every 16 samples for better quality)
            if ((audio_state.sample_count % 16) == 0) {
                printf("AUDIO_DATA:%d\n", audio_sample);
            }
            
            audio_state.sample_count++;
        }
        
        audio_state.last_update_us = current_time_us;
    }
}

/**
 * @brief Generate complex waveform for goblin speech synthesis
 */
static float generate_goblin_waveform(float sample_time, float base_freq) {
    // Base sine wave
    float base_wave = sinf(2.0f * M_PI * base_freq * sample_time);
    
    // Add harmonics for goblin-like roughness
    float harmonic2 = 0.3f * sinf(2.0f * M_PI * base_freq * 2.0f * sample_time);
    float harmonic3 = 0.2f * sinf(2.0f * M_PI * base_freq * 3.0f * sample_time);
    
    // Add noise for growling effect
    float noise = ((float)(esp_random() % 1000) / 1000.0f - 0.5f) * 0.1f;
    
    // Add frequency modulation for warbling effect
    float fm_freq = base_freq + 20.0f * sinf(2.0f * M_PI * 3.0f * sample_time);
    float fm_wave = 0.4f * sinf(2.0f * M_PI * fm_freq * sample_time);
    
    return base_wave + harmonic2 + harmonic3 + noise + fm_wave;
}

/**
 * @brief Start playing a sound effect
 */
void i2s_driver_play_sound(const char* sound_name, float frequency, float volume) {
    ESP_LOGI("i2s_driver", "Playing sound: %s (%.1f Hz, %.1f vol)", sound_name, frequency, volume);
    
    strncpy(audio_state.sound_type, sound_name, sizeof(audio_state.sound_type) - 1);
    audio_state.frequency_hz = frequency;
    audio_state.amplitude = volume;
    audio_state.playing = true;
    audio_state.sample_count = 0;
    
    // Notify PC about sound change with enhanced info
    if (DEBUG_AUDIO_MODE) {
        printf("AUDIO_EVENT:PLAY=%s,FREQ=%.1f,VOL=%.2f,TYPE=", sound_name, frequency, volume);
        
        // Categorize sound type for PC audio processing
        if (strstr(sound_name, "speech")) {
            printf("SPEECH\n");
        } else if (strstr(sound_name, "growl") || strstr(sound_name, "roar")) {
            printf("VOCALIZATION\n");
        } else if (strstr(sound_name, "emotional")) {
            printf("EMOTION\n");
        } else if (strstr(sound_name, "proximity")) {
            printf("ALERT\n");
        } else {
            printf("EFFECT\n");
        }
    }
}

/**
 * @brief Stop audio playback
 */
void i2s_driver_stop_sound(void) {
    ESP_LOGI("i2s_driver", "Stopping audio playback");
    audio_state.playing = false;
    strcpy(audio_state.sound_type, "idle");
    
    if (DEBUG_AUDIO_MODE) {
        printf("AUDIO_EVENT:STOP\n");
    }
}
// --- End: config\components\drivers\i2s_driver.src ---

// --- Begin: config\components\hardware\ili9341.src ---
// ili9341 component implementation
// Defines display parameters via ili9341.hdr for upstream components
// Actual display I/O handled by lower-level driver (generic_spi_display)

#include "esp_log.h"
// Removed: #include "ili9341.hdr" - .hdr content aggregated into .hpp

esp_err_t ili9341_init(void) {
    ESP_LOGI("ili9341", "ili9341 display initialized - %ux%u",
             display_width, display_height);
    return ESP_OK;
}

void ili9341_act(void) {
    // No-op: display I/O handled by lower layers
}
// --- End: config\components\hardware\ili9341.src ---

// --- Begin: config\components\interfaces\max98357a_i2s_amplifier.src ---
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @file max98357a_i2s_amplifier.src
 * @brief Implementation for max98357a_i2s_amplifier component
 * 
 * TODO: Implement component initialization and action logic
 */

/**
 * @brief Initialize max98357a_i2s_amplifier
 * Performs any setup needed for this component
 */
esp_err_t max98357a_i2s_amplifier_init(void)
{
    // TODO: Implement initialization
    return ESP_OK;
}

/**
 * @brief Execute max98357a_i2s_amplifier action
 * Called periodically during subsystem dispatch
 */
void max98357a_i2s_amplifier_act(void)
{
    // TODO: Implement component behavior
}
// --- End: config\components\interfaces\max98357a_i2s_amplifier.src ---

// --- Begin: config\components\hardware\speaker.src ---
// speaker component implementation
// Debug implementation - triggers audio through i2s_driver

#include "esp_log.h"
#include "esp_timer.h"
// Removed: #include "components/drivers/i2s_driver.hdr" - .hdr content aggregated into .hpp

// Speaker state for mood-based audio
typedef struct {
    bool initialized;
    uint64_t last_sound_us;
    uint32_t sound_counter;
    char current_mood[32];
    bool audio_active;
} speaker_state_t;

static speaker_state_t speaker_state = {
    .initialized = false,
    .last_sound_us = 0,
    .sound_counter = 0,
    .current_mood = "neutral",
    .audio_active = false
};

// Enhanced goblin sound effects library
typedef struct {
    const char* name;
    float frequency_hz;
    float volume;
    uint32_t duration_ms;
    const char* description;
    const char* mood_context;
} sound_effect_t;

static const sound_effect_t sound_library[] = {
    // Basic goblin vocalizations
    {"goblin_growl_low", 120.0f, 0.5f, 2500, "Deep threatening growl", "aggressive"},
    {"goblin_growl_med", 180.0f, 0.4f, 2000, "Warning growl", "cautious"},
    {"goblin_snarl", 250.0f, 0.6f, 1200, "Angry snarl", "hostile"},
    {"goblin_hiss", 400.0f, 0.3f, 800, "Threatening hiss", "defensive"},
    
    // Goblin laughter and amusement
    {"goblin_cackle", 350.0f, 0.4f, 1800, "Evil cackling laugh", "mischievous"},
    {"goblin_chuckle", 280.0f, 0.3f, 1000, "Amused chuckle", "playful"},
    {"goblin_giggle", 450.0f, 0.2f, 600, "High-pitched giggle", "happy"},
    
    // Goblin communication
    {"goblin_grunt_yes", 200.0f, 0.3f, 500, "Affirmative grunt", "agreeable"},
    {"goblin_grunt_no", 150.0f, 0.4f, 800, "Negative grunt", "disagreeable"},
    {"goblin_question", 300.0f, 0.3f, 400, "Questioning sound", "curious"},
    {"goblin_surprise", 600.0f, 0.5f, 300, "Surprised exclamation", "startled"},
    
    // Goblin roars and calls
    {"goblin_roar_short", 180.0f, 0.7f, 1500, "Short intimidating roar", "territorial"},
    {"goblin_roar_long", 160.0f, 0.6f, 3000, "Long battle roar", "aggressive"},
    {"goblin_howl", 220.0f, 0.5f, 2200, "Mournful howl", "lonely"},
    {"goblin_screech", 800.0f, 0.4f, 600, "High-pitched screech", "alarmed"},
    
    // Environmental responses
    {"proximity_close", 1000.0f, 0.4f, 200, "Something approaching", "alert"},
    {"proximity_very_close", 1200.0f, 0.6f, 150, "Danger close", "defensive"},
    {"movement_detected", 500.0f, 0.3f, 300, "Motion sensor triggered", "attentive"},
    
    // System sounds
    {"system_boot", 440.0f, 0.3f, 1000, "System startup", "neutral"},
    {"system_error", 220.0f, 0.5f, 1500, "Error occurred", "confused"},
    {"idle_breathing", 80.0f, 0.1f, 4000, "Quiet breathing", "calm"},
    {"idle_snore", 60.0f, 0.2f, 6000, "Sleeping sounds", "sleepy"}
};
#define SOUND_LIBRARY_SIZE (sizeof(sound_library) / sizeof(sound_effect_t))

esp_err_t speaker_init(void) {
    ESP_LOGI("speaker", "Speaker hardware init");
    
    // Initialize I2S driver first
    esp_err_t ret = i2s_driver_init();
    if (ret != ESP_OK) {
        ESP_LOGE("speaker", "Failed to initialize I2S driver: %s", esp_err_to_name(ret));
        return ret;
    }
    
    speaker_state.initialized = true;
    speaker_state.last_sound_us = esp_timer_get_time();
    
    // Play boot sound
    speaker_play_sound_by_name("system_boot");
    
    ESP_LOGI("speaker", "Speaker initialized with %d sound effects", SOUND_LIBRARY_SIZE);
    return ESP_OK;
}

void speaker_act(void) {
    if (!speaker_state.initialized) return;
    
    uint64_t current_time_us = esp_timer_get_time();
    
    // Demo: Play random sounds every 10 seconds
    if ((current_time_us - speaker_state.last_sound_us) > 10000000) {  // 10 seconds
        
        // Select sound based on counter for demo purposes
        uint32_t sound_index = speaker_state.sound_counter % SOUND_LIBRARY_SIZE;
        const sound_effect_t* sound = &sound_library[sound_index];
        
        ESP_LOGI("speaker", "Demo: Playing %s", sound->name);
        i2s_driver_play_sound(sound->name, sound->frequency_hz, sound->volume);
        
        speaker_state.sound_counter++;
        speaker_state.last_sound_us = current_time_us;
        speaker_state.audio_active = true;
        
        // Schedule stop after duration
        // Note: In real implementation, this would be handled by a timer
    }
    
    // Call I2S driver to actually generate audio
    i2s_driver_act();
}

/**
 * @brief Play a specific sound by name
 */
void speaker_play_sound_by_name(const char* sound_name) {
    for (int i = 0; i < SOUND_LIBRARY_SIZE; i++) {
        if (strcmp(sound_library[i].name, sound_name) == 0) {
            const sound_effect_t* sound = &sound_library[i];
            ESP_LOGI("speaker", "Playing sound: %s", sound_name);
            i2s_driver_play_sound(sound->name, sound->frequency_hz, sound->volume);
            return;
        }
    }
    ESP_LOGW("speaker", "Sound not found: %s", sound_name);
}

/**
 * @brief Play proximity alert sound
 */
void speaker_play_proximity_alert(void) {
    speaker_play_sound_by_name("proximity_alert");
}

/**
 * @brief Play mood-based ambient sound
 */
void speaker_play_mood_sound(const char* mood) {
    strncpy(speaker_state.current_mood, mood, sizeof(speaker_state.current_mood) - 1);
    
    if (strcmp(mood, "aggressive") == 0) {
        speaker_play_sound_by_name("goblin_roar_short");
    } else if (strcmp(mood, "playful") == 0) {
        speaker_play_sound_by_name("goblin_cackle");
    } else if (strcmp(mood, "curious") == 0) {
        speaker_play_sound_by_name("goblin_question");
    } else if (strcmp(mood, "defensive") == 0) {
        speaker_play_sound_by_name("goblin_hiss");
    } else if (strcmp(mood, "happy") == 0) {
        speaker_play_sound_by_name("goblin_giggle");
    } else {
        speaker_play_sound_by_name("idle_breathing");
    }
}

/**
 * @brief Synthesize and speak goblin words/phrases
 */
void speaker_speak_goblin_phrase(const char* phrase) {
    ESP_LOGI("speaker", "Speaking goblin phrase: '%s'", phrase);
    
    // Goblin speech synthesis using phonetic mapping
    if (strcmp(phrase, "hello") == 0 || strcmp(phrase, "greetings") == 0) {
        // "Grrrak!" - Goblin greeting
        i2s_driver_play_sound("goblin_speech_greetings", 180.0f, 0.4f);
        
    } else if (strcmp(phrase, "warning") == 0 || strcmp(phrase, "danger") == 0) {
        // "Krash grok!" - Danger warning  
        i2s_driver_play_sound("goblin_speech_warning", 220.0f, 0.6f);
        
    } else if (strcmp(phrase, "attack") == 0 || strcmp(phrase, "fight") == 0) {
        // "GRAAAHHH!" - Battle cry
        i2s_driver_play_sound("goblin_speech_attack", 160.0f, 0.8f);
        
    } else if (strcmp(phrase, "retreat") == 0 || strcmp(phrase, "flee") == 0) {
        // "Grik grak grok!" - Retreat call
        i2s_driver_play_sound("goblin_speech_retreat", 300.0f, 0.5f);
        
    } else if (strcmp(phrase, "curious") == 0 || strcmp(phrase, "what") == 0) {
        // "Grok?" - Questioning
        i2s_driver_play_sound("goblin_speech_question", 350.0f, 0.3f);
        
    } else if (strcmp(phrase, "yes") == 0 || strcmp(phrase, "agree") == 0) {
        // "Grok grok!" - Agreement
        i2s_driver_play_sound("goblin_speech_yes", 200.0f, 0.4f);
        
    } else if (strcmp(phrase, "no") == 0 || strcmp(phrase, "disagree") == 0) {
        // "Grak! Grak!" - Disagreement
        i2s_driver_play_sound("goblin_speech_no", 180.0f, 0.5f);
        
    } else if (strcmp(phrase, "hungry") == 0 || strcmp(phrase, "food") == 0) {
        // "Nom nom grak!" - Hunger
        i2s_driver_play_sound("goblin_speech_hungry", 150.0f, 0.4f);
        
    } else if (strcmp(phrase, "sleep") == 0 || strcmp(phrase, "tired") == 0) {
        // "Zzzgrok..." - Sleepy
        i2s_driver_play_sound("goblin_speech_sleepy", 100.0f, 0.2f);
        
    } else {
        // Unknown phrase - generic goblin babble
        ESP_LOGW("speaker", "Unknown phrase, playing generic goblin sounds");
        i2s_driver_play_sound("goblin_speech_generic", 250.0f, 0.3f);
    }
    
    // Notify PC about speech synthesis
    printf("SPEECH_EVENT:PHRASE=%s\n", phrase);
}

/**
 * @brief Play goblin emotional response
 */
void speaker_play_emotional_response(const char* emotion, float intensity) {
    ESP_LOGI("speaker", "Emotional response: %s (intensity: %.2f)", emotion, intensity);
    
    // Adjust volume and frequency based on intensity (0.0 to 1.0)
    float volume = 0.2f + (intensity * 0.5f);  // 0.2 to 0.7 range
    
    if (strcmp(emotion, "angry") == 0) {
        float freq = 150.0f + (intensity * 100.0f);  // 150-250Hz range
        i2s_driver_play_sound("goblin_emotional_angry", freq, volume);
        
    } else if (strcmp(emotion, "happy") == 0) {
        float freq = 300.0f + (intensity * 200.0f);  // 300-500Hz range
        i2s_driver_play_sound("goblin_emotional_happy", freq, volume);
        
    } else if (strcmp(emotion, "scared") == 0) {
        float freq = 400.0f + (intensity * 400.0f);  // 400-800Hz range
        i2s_driver_play_sound("goblin_emotional_scared", freq, volume);
        
    } else if (strcmp(emotion, "surprised") == 0) {
        float freq = 500.0f + (intensity * 300.0f);  // 500-800Hz range
        i2s_driver_play_sound("goblin_emotional_surprised", freq, volume);
        
    } else if (strcmp(emotion, "sad") == 0) {
        float freq = 120.0f + (intensity * 80.0f);   // 120-200Hz range
        i2s_driver_play_sound("goblin_emotional_sad", freq, volume);
        
    } else {
        // Default neutral emotion
        i2s_driver_play_sound("goblin_emotional_neutral", 200.0f, volume);
    }
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
// Removed: #include "components/spi_display_bus.hdr" - .hdr content aggregated into .hpp

#include <stddef.h>

static constexpr size_t SPI_DISPLAY_SLOT_COUNT = 32U;
static spi_display_pinset_t spi_display_slots[SPI_DISPLAY_SLOT_COUNT];
spi_display_pinset_t cur_spi_display_pin;

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
                ESP_LOGE("spi_display_bus", "Assigned pin buffer exhausted");
                esp_system_abort("Assigned pin buffer exhausted");
            }

            assigned_pins[assigned_pins_count++] = candidate;
            return candidate;
        }
    }

    ESP_LOGE("spi_display_bus", "No assignable pins remain for SPI display bus");
    esp_system_abort("SPI display bus ran out of assignable pins");
    return -1; // Unreachable, abort will terminate execution
}

esp_err_t spi_display_bus_init(void) {
    ESP_LOGI("spi_display_bus", "=== SPI_DISPLAY_BUS_INIT STARTED ===");

    // Locate the first available slot for a display device
    size_t slot = 0;
    while (slot < SPI_DISPLAY_SLOT_COUNT && spi_display_slots[slot].cs != -1) {
        ++slot;
    }

    if (slot >= SPI_DISPLAY_SLOT_COUNT) {
        ESP_LOGE("spi_display_bus", "No remaining SPI display device slots available");
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
        ESP_LOGI("spi_display_bus", "SPI bus initialize result: %s", esp_err_to_name(ret));
        if (ret != ESP_OK) {
            ESP_LOGE("spi_display_bus", "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI("spi_display_bus",
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

    ESP_LOGI("spi_display_bus",
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
