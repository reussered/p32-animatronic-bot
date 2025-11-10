/**
 * GOBLIN_MOOD_DISPLAY_EXAMPLE_V2.cpp
 * 
 * Correct architecture: Display specified in chain, malloc/free once, reuse buffer
 * 
 * Key changes from v1:
 * 1. Display hardware NOT stored in class (specified at init via callback)
 * 2. Single buffer: malloc once at init, reuse every frame, free at cleanup
 * 3. Color schema used ONLY to determine bytes_per_pixel
 * 4. Each render cycle: render → send → reuse buffer
 */

#include "config/components/templates/goblin_eye_mood_display_v2.hpp"
#include "config/components/templates/goblin_mouth_mood_display_v2.hpp"
#include "shared/Mood.hpp"

// ============================================================================
// EXAMPLE: Dual Eyes + Chunked Mouth
// ============================================================================

static GoblinEyeMoodDisplay left_eye;
static GoblinEyeMoodDisplay right_eye;
static GoblinMouthMoodDisplay mouth;

// Display driver callbacks (from your SPI/display driver layer)
void send_left_eye_frame(const uint8_t* buffer, uint32_t size) {
    // Call your actual SPI driver
    // spi_driver_send_frame(SPI_DEVICE_0, buffer, size);
}

void send_right_eye_frame(const uint8_t* buffer, uint32_t size) {
    // Call your actual SPI driver
    // spi_driver_send_frame(SPI_DEVICE_1, buffer, size);
}

void send_mouth_chunk(const uint8_t* buffer, uint32_t size, uint16_t start_y, uint16_t height) {
    // Call your actual display driver with chunk info
    // ili9341_send_chunk(buffer, size, 0, start_y, 480, height);
}

/**
 * Initialize: malloc buffers once
 * 
 * Memory allocation happens ONCE:
 *   Left eye:  240×240×2 = 115,200 bytes
 *   Right eye: 240×240×2 = 115,200 bytes
 *   Mouth chunk: 480×80×3 = 115,200 bytes
 * 
 * Buffers stay allocated until cleanup
 */
esp_err_t goblin_head_init(void) {
    // Left eye: 240×240 GC9A01 RGB565
    if (!left_eye.init(240, 240, RGB565_FORMAT, send_left_eye_frame)) {
        ESP_LOGE(TAG, "Failed to init left eye");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Left eye buffer: %d bytes", left_eye.getBufferSize());
    
    // Right eye: 240×240 GC9A01 RGB565
    if (!right_eye.init(240, 240, RGB565_FORMAT, send_right_eye_frame)) {
        ESP_LOGE(TAG, "Failed to init right eye");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Right eye buffer: %d bytes", right_eye.getBufferSize());
    
    // Mouth: 480×320 ILI9341 RGB666 with 80-pixel chunks
    if (!mouth.init(480, 320, RGB666_FORMAT, true, 80, send_mouth_chunk)) {
        ESP_LOGE(TAG, "Failed to init mouth");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Mouth chunk buffer: %d bytes (for %d chunks)", 
             mouth.getChunkSize(), mouth.getTotalChunks());
    
    return ESP_OK;
}

/**
 * Render all displays: use buffers, send, reuse
 * 
 * Buffer lifecycle each frame:
 * 1. renderFrame() fills buffer
 * 2. sendToDisplay() sends buffer to hardware
 * 3. Buffer immediately available for next eye/chunk
 */
void goblin_head_act(void) {
    // Read current mood from SharedMemory
    Mood current_mood = GSM.read<Mood>();
    
    // === LEFT EYE ===
    // 1. Render: fill buffer with mood-modified color
    left_eye.renderFrame(current_mood, 0x00AA00);  // Green base
    // 2. Send: buffer goes to display driver
    left_eye.sendToDisplay();
    // 3. Buffer is now free for reuse (stays allocated)
    
    // === RIGHT EYE ===
    // Same pattern: render → send → reuse
    right_eye.renderFrame(current_mood, 0x00AA00);
    right_eye.sendToDisplay();
    
    // === MOUTH (Chunked) ===
    // Render and send chunks sequentially, reusing buffer each time
    
    // Reset for this frame cycle
    mouth.resetChunkIndex();
    
    // Render and send each chunk
    while (mouth.renderNextChunk(current_mood, 0xFF6600)) {
        // Buffer now contains one chunk of the mouth
        mouth.sendChunkToDisplay();
        // Buffer reused for next chunk
    }
}

/**
 * Cleanup: free all buffers
 * 
 * Destructors called automatically when objects go out of scope:
 *   ~GoblinEyeMoodDisplay() → free(frame_buffer)
 *   ~GoblinMouthMoodDisplay() → free(chunk_buffer)
 */
void goblin_head_cleanup(void) {
    // Destructors handle free()
}

// ============================================================================
// MEMORY ANALYSIS
// ============================================================================

/*
Allocation (happens once at init):
  Left eye buffer:   240×240×2 = 115,200 bytes (112.5 KB)
  Right eye buffer:  240×240×2 = 115,200 bytes (112.5 KB)
  Mouth chunk buffer: 480×80×3 = 115,200 bytes (112.5 KB)
  ─────────────────────────────────────────────
  Total allocated:               345,600 bytes (337.5 KB)

Available RAM: 300 KB
Status: ⚠️ SLIGHTLY OVER

Solution: Use ST7735 mouth instead of ILI9341
  St7735:             160×128×2 = 40,960 bytes (40 KB)
  ─────────────────────────────────────────────
  New total:                     271,360 bytes (265 KB) ✓

OR: Reduce mouth chunk height
  480×64×3 = 92,160 bytes (90 KB)
  Total = 317.4 KB (still slightly over)

OR: Use single eye + high-res mouth
  240×240×2 = 115,200 bytes (eye)
  480×80×3 = 115,200 bytes (mouth chunk)
  ───────────────────────
  Total = 230,400 bytes (225 KB) ✓

Usage pattern each frame:
  Left eye buffer:  [FILLED] → [SENT] → [FREE]
  Right eye buffer: [FILLED] → [SENT] → [FREE]
  Mouth chunk buffer (reused 4 times):
    [FILLED CHUNK 0] → [SENT] → [FREE]
    [FILLED CHUNK 1] → [SENT] → [FREE]
    [FILLED CHUNK 2] → [SENT] → [FREE]
    [FILLED CHUNK 3] → [SENT] → [FREE]

Peak RAM: 337.5 KB (one frame buffer per eye + one chunk buffer)
Sustained: same (buffers don't grow over time)
*/

// ============================================================================
// ALTERNATIVE: All Callbacks Injected (Maximum Flexibility)
// ============================================================================

class GoblinHeadController {
private:
    GoblinEyeMoodDisplay left_eye;
    GoblinEyeMoodDisplay right_eye;
    GoblinMouthMoodDisplay mouth;
    
public:
    /**
     * Initialize with specific display driver instances
     * Each display driver specified in the dependency chain
     */
    esp_err_t init(
        DisplayDriver* left_eye_driver,
        DisplayDriver* right_eye_driver,
        DisplayDriver* mouth_driver)
    {
        if (!left_eye.init(240, 240, RGB565_FORMAT, 
            [left_eye_driver](const uint8_t* buf, uint32_t size) {
                left_eye_driver->sendFrame(buf, size);
            })) {
            return ESP_FAIL;
        }
        
        if (!right_eye.init(240, 240, RGB565_FORMAT,
            [right_eye_driver](const uint8_t* buf, uint32_t size) {
                right_eye_driver->sendFrame(buf, size);
            })) {
            return ESP_FAIL;
        }
        
        if (!mouth.init(480, 320, RGB666_FORMAT, true, 80,
            [mouth_driver](const uint8_t* buf, uint32_t size, uint16_t y, uint16_t h) {
                mouth_driver->sendChunk(buf, size, 0, y, 480, h);
            })) {
            return ESP_FAIL;
        }
        
        return ESP_OK;
    }
    
    /**
     * Render frame: all buffers reused
     */
    void renderFrame() {
        Mood mood = GSM.read<Mood>();
        
        left_eye.renderFrame(mood, 0x00AA00);
        left_eye.sendToDisplay();
        
        right_eye.renderFrame(mood, 0x00AA00);
        right_eye.sendToDisplay();
        
        mouth.resetChunkIndex();
        while (mouth.renderNextChunk(mood, 0xFF6600)) {
            mouth.sendChunkToDisplay();
        }
    }
};
