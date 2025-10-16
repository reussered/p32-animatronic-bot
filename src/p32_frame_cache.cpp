// P32 Incremental Frame Mood Cache Implementation
#include "p32_frame_cache.hpp"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include <string.h>
#include <stdlib.h>

static const char* TAG = "FRAME_CACHE";
static frame_cache_manager_t cache_manager = {0};

esp_err_t frame_cache_init(void) {
    memset(&cache_manager, 0, sizeof(cache_manager));
    
    // Initialize with neutral mood
    memset(&cache_manager.current_mood, 0, sizeof(mood_state_t));
    
    ESP_LOGI(TAG, "Frame cache system initialized");
    return ESP_OK;
}

esp_err_t frame_cache_load_animation(const char* name, uint8_t animation_id) {
    if (animation_id >= MAX_ANIMATIONS) {
        return ESP_ERR_INVALID_ARG;
    }
    
    animation_t* anim = &cache_manager.animations[animation_id];
    strncpy(anim->name, name, sizeof(anim->name) - 1);
    anim->frame_count = 0;
    anim->is_loaded = true;
    
    ESP_LOGI(TAG, "Animation loaded: %s (ID: %d)", name, animation_id);
    return ESP_OK;
}

esp_err_t frame_cache_add_frame(uint8_t animation_id, uint8_t frame_id, 
                               const uint8_t* indexed_data, uint16_t width, uint16_t height) {
    if (animation_id >= MAX_ANIMATIONS || frame_id >= MAX_FRAMES_PER_ANIMATION) {
        return ESP_ERR_INVALID_ARG;
    }
    
    animation_t* anim = &cache_manager.animations[animation_id];
    if (!anim->is_loaded) {
        return ESP_ERR_INVALID_STATE;
    }
    
    animation_frame_t* frame = &anim->frames[frame_id];
    frame->width = width;
    frame->height = height;
    frame->cache_info.pixel_count = width * height;
    frame->cache_info.is_allocated = true; // Always allocated (static buffers)
    
    // Validate frame size
    if (frame->cache_info.pixel_count > MAX_FRAME_PIXELS) {
        ESP_LOGE(TAG, "Frame too large: %dx%d > %d pixels", width, height, MAX_FRAME_PIXELS);
        return ESP_ERR_INVALID_SIZE;
    }
    
    // Copy to static buffer - no malloc needed
    memcpy(frame->indexed_pixels, indexed_data, frame->cache_info.pixel_count);
    
    // Mark RGB565 as needing initial conversion
    memset(&frame->cache_info.last_mood, 0xFF, sizeof(mood_state_t)); // Invalid mood forces update
    
    if (frame_id >= anim->frame_count) {
        anim->frame_count = frame_id + 1;
    }
    
    ESP_LOGD(TAG, "Frame added: anim=%d frame=%d size=%dx%d", 
             animation_id, frame_id, width, height);
    return ESP_OK;
}

void frame_cache_set_mood(const mood_state_t* new_mood) {
    cache_manager.current_mood = *new_mood;
    // No pre-marking - frames check mood on-demand when displayed
    ESP_LOGV(TAG, "Mood updated");
}

esp_err_t frame_cache_get_frame_rgb565(uint8_t animation_id, uint8_t frame_id, 
                                      uint16_t** rgb565_data, uint16_t* width, uint16_t* height) {
    if (animation_id >= MAX_ANIMATIONS || frame_id >= MAX_FRAMES_PER_ANIMATION) {
        return ESP_ERR_INVALID_ARG;
    }
    
    animation_t* anim = &cache_manager.animations[animation_id];
    if (!anim->is_loaded || frame_id >= anim->frame_count) {
        return ESP_ERR_NOT_FOUND;
    }
    
    animation_frame_t* frame = &anim->frames[frame_id];
    
    // Update frame if needed (incremental processing)
    esp_err_t ret = frame_cache_update_frame_if_needed(animation_id, frame_id);
    if (ret != ESP_OK) {
        return ret;
    }
    
    *rgb565_data = frame->rgb565_pixels;
    *width = frame->width;
    *height = frame->height;
    
    cache_manager.total_cache_hits++;
    return ESP_OK;
}

esp_err_t frame_cache_update_frame_if_needed(uint8_t animation_id, uint8_t frame_id) {
    animation_frame_t* frame = &cache_manager.animations[animation_id].frames[frame_id];
    
    // Check if frame is current (always check, no pre-marking)
    if (!frame_cache_mood_changed(&frame->cache_info.last_mood, &cache_manager.current_mood)) {
        return ESP_OK; // Frame is current - use cached RGB565 data
    }
    
    // Mood changed - frame needs update but NEVER BLOCK
    // No allocation needed - buffers are static
    
    // Update frame with current mood palette (fast operation on static buffer)
    frame_cache_update_palette_for_frame(frame);
    
    // Update cache metadata AFTER successful update
    frame->cache_info.last_mood = cache_manager.current_mood;
    frame->cache_info.last_update_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    cache_manager.total_cache_misses++;
    cache_manager.frames_updated++;
    
    ESP_LOGV(TAG, "Frame updated: anim=%d frame=%d", animation_id, frame_id);
    return ESP_OK;
}

void frame_cache_update_palette_for_frame(animation_frame_t* frame) {
    // Get current mood palette (no allocation)
    const palette_color_t* active_palette = mood_palette_get_active_palette();
    
    // Convert indexed pixels to RGB565 using current mood palette
    // Fast loop on static buffers - no malloc/free
    for (uint32_t i = 0; i < frame->cache_info.pixel_count; i++) {
        uint8_t palette_index = frame->indexed_pixels[i];
        if (palette_index < PALETTE_SIZE) {
            frame->rgb565_pixels[i] = active_palette[palette_index];
        } else {
            frame->rgb565_pixels[i] = 0x0000; // Black for invalid indices
        }
    }
}

bool frame_cache_mood_changed(const mood_state_t* mood1, const mood_state_t* mood2) {
    return memcmp(mood1, mood2, sizeof(mood_state_t)) != 0;
}

esp_err_t frame_cache_allocate_frame_buffer(animation_frame_t* frame) {
    if (frame->cache_info.is_allocated) {
        return ESP_OK;
    }
    
    size_t rgb565_size = frame->cache_info.pixel_count * sizeof(uint16_t);
    frame->rgb565_pixels = heap_caps_malloc(rgb565_size, MALLOC_CAP_DMA);
    
    if (!frame->rgb565_pixels) {
        ESP_LOGE(TAG, "Failed to allocate RGB565 buffer (%d bytes)", rgb565_size);
        return ESP_ERR_NO_MEM;
    }
    
    frame->cache_info.is_allocated = true;
    ESP_LOGD(TAG, "Allocated frame buffer: %d bytes", rgb565_size);
    return ESP_OK;
}

void frame_cache_free_frame_buffer(animation_frame_t* frame) {
    if (frame->indexed_pixels) {
        free(frame->indexed_pixels);
        frame->indexed_pixels = NULL;
    }
    
    if (frame->rgb565_pixels) {
        free(frame->rgb565_pixels);
        frame->rgb565_pixels = NULL;
    }
    
    frame->cache_info.is_allocated = false;
}

void frame_cache_cleanup_unused_frames(void) {
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t freed_count = 0;
    
    for (int a = 0; a < MAX_ANIMATIONS; a++) {
        if (!cache_manager.animations[a].is_loaded) continue;
        
        for (int f = 0; f < cache_manager.animations[a].frame_count; f++) {
            animation_frame_t* frame = &cache_manager.animations[a].frames[f];
            
            // Free frames not accessed in last 30 seconds
            if (frame->cache_info.is_allocated && 
                (current_time - frame->cache_info.last_update_ms) > 30000) {
                
                if (frame->rgb565_pixels) {
                    free(frame->rgb565_pixels);
                    frame->rgb565_pixels = NULL;
                    frame->cache_info.is_allocated = false;
                    freed_count++;
                }
            }
        }
    }
    
    if (freed_count > 0) {
        ESP_LOGI(TAG, "Cleaned up %d unused frame buffers", freed_count);
    }
}

void frame_cache_print_stats(void) {
    ESP_LOGI(TAG, "=== FRAME CACHE STATISTICS ===");
    ESP_LOGI(TAG, "Cache hits: %d", cache_manager.total_cache_hits);
    ESP_LOGI(TAG, "Cache misses: %d", cache_manager.total_cache_misses);
    ESP_LOGI(TAG, "Frames updated: %d", cache_manager.frames_updated);
    ESP_LOGI(TAG, "Memory usage: %d bytes", frame_cache_get_memory_usage());
    
    float hit_rate = 0.0f;
    uint32_t total = cache_manager.total_cache_hits + cache_manager.total_cache_misses;
    if (total > 0) {
        hit_rate = (float)cache_manager.total_cache_hits / total * 100.0f;
    }
    ESP_LOGI(TAG, "Cache hit rate: %.1f%%", hit_rate);
}

void frame_cache_print_animation_info(uint8_t animation_id) {
    if (animation_id >= MAX_ANIMATIONS) return;
    
    animation_t* anim = &cache_manager.animations[animation_id];
    if (!anim->is_loaded) {
        ESP_LOGI(TAG, "Animation %d: Not loaded", animation_id);
        return;
    }
    
    ESP_LOGI(TAG, "Animation %d: %s (%d frames)", animation_id, anim->name, anim->frame_count);
    
    for (int f = 0; f < anim->frame_count; f++) {
        animation_frame_t* frame = &anim->frames[f];
        ESP_LOGI(TAG, "  Frame %d: %dx%d %s", f, frame->width, frame->height,
                 frame->cache_info.is_allocated ? "CACHED" : "UNCACHED");
    }
}

uint32_t frame_cache_get_memory_usage(void) {
    uint32_t total_bytes = 0;
    
    for (int a = 0; a < MAX_ANIMATIONS; a++) {
        if (!cache_manager.animations[a].is_loaded) continue;
        
        for (int f = 0; f < cache_manager.animations[a].frame_count; f++) {
            animation_frame_t* frame = &cache_manager.animations[a].frames[f];
            
            // Indexed pixels (always allocated)
            if (frame->indexed_pixels) {
                total_bytes += frame->cache_info.pixel_count;
            }
            
            // RGB565 pixels (allocated on demand)
            if (frame->rgb565_pixels) {
                total_bytes += frame->cache_info.pixel_count * 2;
            }
        }
    }
    
    return total_bytes;
}