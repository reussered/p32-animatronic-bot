/**
 * GOBLIN_MOOD_DISPLAY_EXAMPLE.cpp
 * 
 * Quick reference: How to use mood-based display templates
 * Copy this into your goblin_head.src as a starting point
 */

#include "config/components/templates/goblin_eye_mood_display.hpp"
#include "config/components/templates/goblin_mouth_mood_display.hpp"
#include "shared/Mood.hpp"
#include "class GSM.cpp"  // For GSM.read<Mood>()

// ============================================================================
// EXAMPLE 1: Dual RGB565 Eyes (GC9A01) + RGB565 Mouth (ST7735)
// ============================================================================

static GoblinEyeMoodDisplay left_eye;
static GoblinEyeMoodDisplay right_eye;
static GoblinMouthMoodDisplay mouth;

/**
 * Initialize displays - Call once at startup
 * 
 * Configuration:
 *   Left eye:  240x240 GC9A01 RGB565  (112.5 KB)
 *   Right eye: 240x240 GC9A01 RGB565  (112.5 KB)
 *   Mouth:     160x128 ST7735 RGB565  (40 KB)
 *   Total:     265 KB (fits in 300 KB budget with 35 KB headroom)
 */
esp_err_t goblin_head_init_example1(void) {
    // Left eye: 240x240 GC9A01
    if (!left_eye.init(240, 240, RGB565_FORMAT)) {
        ESP_LOGE(TAG, "Failed to init left eye");
        return ESP_FAIL;
    }
    
    // Right eye: 240x240 GC9A01
    if (!right_eye.init(240, 240, RGB565_FORMAT)) {
        ESP_LOGE(TAG, "Failed to init right eye");
        return ESP_FAIL;
    }
    
    // Mouth: 160x128 ST7735 (no chunking needed, it's small)
    if (!mouth.init(160, 128, RGB565_FORMAT, false)) {
        ESP_LOGE(TAG, "Failed to init mouth");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Display buffers allocated: %d KB total",
             (left_eye.getBufferSize() + right_eye.getBufferSize() + 
              mouth.getChunkSize()) / 1024);
    
    return ESP_OK;
}

/**
 * Render all displays with current mood - Call in act() function every frame
 */
void goblin_head_render_example1(void) {
    // Read current mood from SharedMemory
    Mood current_mood = GSM.read<Mood>();
    
    // Render left eye: Green base color with mood applied
    left_eye.renderFrame(current_mood, 0x00AA00);
    
    // Render right eye: Same green, mood-modified
    right_eye.renderFrame(current_mood, 0x00AA00);
    
    // Render mouth: Orange base color with mood applied
    mouth.renderNextChunk(current_mood, 0xFF6600);
    mouth.resetChunkIndex();
    
    // Send to SPI displays
    // (pseudo-code, actual SPI calls depend on your driver)
    spi_driver_send_frame(SPI_DEVICE_0, left_eye.getFrameBuffer(), 
                         left_eye.getBufferSize());
    spi_driver_send_frame(SPI_DEVICE_1, right_eye.getFrameBuffer(), 
                         right_eye.getBufferSize());
    spi_driver_send_frame(SPI_DEVICE_2, mouth.getChunkBuffer(), 
                         mouth.getChunkSize());
}


// ============================================================================
// EXAMPLE 2: Dual RGB565 Eyes + RGB666 Mouth with Chunking (ILI9341)
// ============================================================================

/**
 * For larger, higher-resolution mouth display
 * 
 * Configuration:
 *   Left eye:  240x240 GC9A01 RGB565     (112.5 KB)
 *   Right eye: 240x240 GC9A01 RGB565     (112.5 KB)
 *   Mouth:     480x320 ILI9341 RGB666    (112.5 KB chunk)
 *   Total:     337.5 KB peak (tight but works)
 */
esp_err_t goblin_head_init_example2_chunked_mouth(void) {
    // Eyes same as example 1
    if (!left_eye.init(240, 240, RGB565_FORMAT)) return ESP_FAIL;
    if (!right_eye.init(240, 240, RGB565_FORMAT)) return ESP_FAIL;
    
    // Mouth: 480x320 ILI9341 with RGB666 color format
    // Chunking enabled: divide into 80-pixel-high strips (4 chunks total)
    if (!mouth.init(480, 320, RGB666_FORMAT, true, 80)) {
        ESP_LOGE(TAG, "Failed to init mouth with chunking");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Mouth configured: %d total chunks, %d KB per chunk",
             mouth.getTotalChunks(), mouth.getChunkSize() / 1024);
    
    return ESP_OK;
}

/**
 * Render with chunked mouth display
 */
void goblin_head_render_example2_chunked_mouth(void) {
    Mood current_mood = GSM.read<Mood>();
    
    // Render and send eyes first (fast, single buffer each)
    left_eye.renderFrame(current_mood, 0x00AA00);
    spi_driver_send_frame(SPI_DEVICE_0, left_eye.getFrameBuffer(), 
                         left_eye.getBufferSize());
    
    right_eye.renderFrame(current_mood, 0x00AA00);
    spi_driver_send_frame(SPI_DEVICE_1, right_eye.getFrameBuffer(), 
                         right_eye.getBufferSize());
    
    // Render mouth in chunks
    // Each chunk must be rendered sequentially
    const uint8_t* chunk_ptr = mouth.renderNextChunk(current_mood, 0xFF6600);
    uint16_t chunk_index = 0;
    
    while (chunk_ptr) {
        // Send this chunk to display driver
        // (Driver must know which vertical strip this is: chunk_index * chunk_height)
        spi_driver_send_chunk(SPI_DEVICE_2, 
                             chunk_ptr, 
                             mouth.getChunkSize(),
                             0,                           // Start X (0 for full width)
                             chunk_index * mouth.getChunkHeight(),  // Start Y
                             mouth.getWidth(),
                             mouth.getChunkHeight());
        
        chunk_ptr = mouth.renderNextChunk(current_mood, 0xFF6600);
        chunk_index++;
    }
    
    // IMPORTANT: Reset for next frame!
    mouth.resetChunkIndex();
}


// ============================================================================
// EXAMPLE 3: Animation Integration - Pupil Gaze + Expressions
// ============================================================================

/**
 * Show mood effects combined with animation
 */
void goblin_head_animated_render(void) {
    Mood current_mood = GSM.read<Mood>();
    
    // Update eye animations based on mood
    switch (current_mood.state) {
        case ANGER:
            // Angry eyes: narrowed (lower eyelid openness)
            left_eye.setEyelidOpenness(180);
            right_eye.setEyelidOpenness(180);
            // Pupils centered and small
            left_eye.setPupilPosition(120, 120);
            right_eye.setPupilPosition(120, 120);
            break;
            
        case FEAR:
            // Fearful eyes: wide open, pupils dilated
            left_eye.setEyelidOpenness(255);
            right_eye.setEyelidOpenness(255);
            left_eye.setPupilPosition(120, 100);   // Look up
            right_eye.setPupilPosition(120, 100);
            break;
            
        case CURIOSITY:
            // Curious: normal openness, pupils tracking
            left_eye.setEyelidOpenness(220);
            right_eye.setEyelidOpenness(220);
            left_eye.setPupilPosition(140, 120);   // Look right
            right_eye.setPupilPosition(140, 120);
            break;
            
        case HAPPINESS:
            // Happy: squinting eyes (lower eyelid)
            left_eye.setEyelidOpenness(150);
            right_eye.setEyelidOpenness(150);
            break;
            
        case SADNESS:
            // Sad: drooping
            left_eye.setEyelidOpenness(100);
            right_eye.setEyelidOpenness(100);
            left_eye.setPupilPosition(120, 140);   // Look down
            right_eye.setPupilPosition(120, 140);
            break;
            
        default:
            left_eye.setEyelidOpenness(220);
            right_eye.setEyelidOpenness(220);
            break;
    }
    
    // Render with mood applied
    left_eye.renderFrame(current_mood, 0x00AA00);
    right_eye.renderFrame(current_mood, 0x00AA00);
    
    // Update mouth expression
    mouth.setExpression(current_mood.intensity);
    mouth.setMouthOpen(100 + (current_mood.intensity / 2));
    
    const uint8_t* chunk = mouth.renderNextChunk(current_mood, 0xFF6600);
    while (chunk) {
        // Send chunk...
        chunk = mouth.renderNextChunk(current_mood, 0xFF6600);
    }
    mouth.resetChunkIndex();
}


// ============================================================================
// EXAMPLE 4: Customize Mood Color Palette
// ============================================================================

/**
 * If default mood colors don't match your goblin personality,
 * customize them at initialization
 */
esp_err_t goblin_head_init_custom_moods(void) {
    // Initialize displays (same as before)
    if (!left_eye.init(240, 240, RGB565_FORMAT)) return ESP_FAIL;
    if (!right_eye.init(240, 240, RGB565_FORMAT)) return ESP_FAIL;
    if (!mouth.init(160, 128, RGB565_FORMAT, false)) return ESP_FAIL;
    
    // OPTIONAL: Customize mood color mappings
    // Example: Make this goblin EXTRA angry (more red, more glow)
    
    MoodColorDelta super_angry = {
        .red_delta = 80,              // Much more red shift
        .green_delta = -50,
        .blue_delta = -50,
        .intensity = 2.0f,            // Double the effect strength
        .enable_glow = true,
        .enable_desaturation = false,
        .enable_warmth = true,
        .enable_coolness = false
    };
    
    // TODO: Apply custom mood to calculator
    // (This requires exposing mood calculator interface, which depends on implementation)
    // left_eye_calc.setMoodDelta(ANGER, super_angry);
    
    return ESP_OK;
}


// ============================================================================
// MEMORY ANALYSIS
// ============================================================================

/*
Example 1 (Dual GC9A01 + ST7735):
  Left eye:   240x240x2 = 115,200 bytes = 112.5 KB
  Right eye:  240x240x2 = 115,200 bytes = 112.5 KB
  Mouth:      160x128x2 = 40,960 bytes  = 40 KB
  ---
  Total: 271,360 bytes = 265 KB
  
  Available heap: 300 KB
  Headroom:       35 KB  GOOD

Example 2 (Dual GC9A01 + ILI9341 chunked):
  Left eye:     240x240x2 = 115,200 bytes = 112.5 KB
  Right eye:    240x240x2 = 115,200 bytes = 112.5 KB
  Mouth (1 chunk): 480x80x3 = 115,200 bytes = 112.5 KB
  ---
  Total: 345,600 bytes = 337.5 KB
  
  Available heap: 300 KB
  Over by:        37.5 KB  TIGHT
  
  Solution: Reduce chunk height to 64 pixels:
    480x64x3 = 92,160 bytes = 90 KB per chunk
    Total = 317.4 KB (still 17.4 KB over)
  
  Or drop to single large ST7735 eye + ILI9341:
    240x240x2 = 112.5 KB (1 eye)
    480x80x3  = 112.5 KB (mouth chunk)
    Total = 225 KB  SAFE
*/
