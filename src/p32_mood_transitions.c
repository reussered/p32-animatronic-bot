// P32 Delta-Based Mood Transition System Implementation
// Real-time mood effects without frame buffering - Memory efficient!

#include "p32_mood_transitions.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static const char* TAG = "MOOD_TRANSITIONS";
static mood_transition_manager_t transition_manager = {0};

esp_err_t mood_transitions_init(void) {
    memset(&transition_manager, 0, sizeof(transition_manager));
    
    // Initialize with neutral mood
    memset(&transition_manager.current_mood, 0, sizeof(mood_state_t));
    memset(&transition_manager.previous_mood, 0, sizeof(mood_state_t));
    
    transition_manager.global_overshoot = 1.0f;  // Normal response
    transition_manager.nervousness_mode = false;
    
    ESP_LOGI(TAG, "Delta-based mood transition system initialized (<%dKB memory)", 
             sizeof(transition_manager) / 1024);
    return ESP_OK;
}

esp_err_t mood_transitions_set_new_mood(const mood_state_t* new_mood) {
    if (!new_mood) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Store previous mood for delta calculation
    memcpy(&transition_manager.previous_mood, &transition_manager.current_mood, sizeof(mood_state_t));
    
    // Calculate deltas for each mood type
    mood_transitions_calculate_deltas(&transition_manager.previous_mood, new_mood);
    
    // Update current mood
    memcpy(&transition_manager.current_mood, new_mood, sizeof(mood_state_t));
    
    transition_manager.total_transitions++;
    
    ESP_LOGD(TAG, "Mood transition calculated - %d active deltas", transition_manager.active_delta_count);
    return ESP_OK;
}

void mood_transitions_calculate_deltas(const mood_state_t* old_mood, const mood_state_t* new_mood) {
    transition_manager.active_delta_count = 0;
    uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
    
    // Calculate delta for each mood type
    for (int i = 0; i < MOOD_TYPE_COUNT; i++) {
        int16_t old_intensity = 0;
        int16_t new_intensity = 0;
        
        // Extract intensities based on mood type
        switch (i) {
            case MOOD_ANGER:
                old_intensity = old_mood->anger;
                new_intensity = new_mood->anger;
                break;
            case MOOD_FEAR:
                old_intensity = old_mood->fear;
                new_intensity = new_mood->fear;
                break;
            case MOOD_HAPPINESS:
                old_intensity = old_mood->happiness;
                new_intensity = new_mood->happiness;
                break;
            case MOOD_SADNESS:
                old_intensity = old_mood->sadness;
                new_intensity = new_mood->sadness;
                break;
            case MOOD_CURIOSITY:
                old_intensity = old_mood->curiosity;
                new_intensity = new_mood->curiosity;
                break;
            case MOOD_AFFECTION:
                old_intensity = old_mood->affection;
                new_intensity = new_mood->affection;
                break;
            case MOOD_IRRITATION:
                old_intensity = old_mood->irritation;
                new_intensity = new_mood->irritation;
                break;
            case MOOD_CONTENTMENT:
                old_intensity = old_mood->contentment;
                new_intensity = new_mood->contentment;
                break;
        }
        
        int16_t delta = new_intensity - old_intensity;
        
        // Only create delta entry if there's a significant change
        if (abs(delta) > 10) {  // Threshold to avoid micro-changes
            mood_delta_t* mood_delta = &transition_manager.active_deltas[transition_manager.active_delta_count];
            
            mood_delta->mood_type = (mood_type_t)i;
            mood_delta->old_intensity = old_intensity;
            mood_delta->new_intensity = new_intensity;
            mood_delta->delta = delta;
            mood_delta->reverse_on_decrease = (delta < 0);  // Reverse if decreasing
            mood_delta->transition_start_ms = current_time;
            mood_delta->transition_duration_ms = 500;  // 500ms transition time
            
            // Calculate overshoot multiplier
            float base_overshoot = transition_manager.global_overshoot;
            
            // Large deltas get more overshoot for dramatic effect
            if (abs(delta) > 200) {
                base_overshoot *= 1.5f;  // 50% more overshoot for big changes
            }
            
            // Nervousness mode adds random variation
            if (transition_manager.nervousness_mode) {
                float random_factor = 0.8f + 0.4f * ((float)(current_time % 1000) / 1000.0f);  // 0.8 - 1.2
                base_overshoot *= random_factor;
                transition_manager.overshoot_events++;
            }
            
            mood_delta->overshoot_multiplier = base_overshoot;
            
            transition_manager.active_delta_count++;
            
            ESP_LOGD(TAG, "Delta created: %s %+d (%.2fx overshoot)", 
                     mood_type_names[i], delta, base_overshoot);
        }
    }
}

bool mood_transitions_has_active_deltas(void) {
    return transition_manager.active_delta_count > 0;
}

uint16_t mood_transitions_apply_to_pixel(uint8_t palette_index, mood_type_t primary_mood) {
    // Start with base palette color
    const palette_color_t* base_palette = mood_palette_get_neutral_palette();
    uint16_t base_color = base_palette[palette_index];
    
    // If no active transitions, return base color
    if (transition_manager.active_delta_count == 0) {
        return base_color;
    }
    
    // Apply all active mood deltas to this pixel
    uint16_t final_color = base_color;
    
    for (int i = 0; i < transition_manager.active_delta_count; i++) {
        mood_delta_t* delta = &transition_manager.active_deltas[i];
        
        // Calculate transition progress (0.0 - 1.0)
        uint32_t current_time = (uint32_t)(esp_timer_get_time() / 1000);
        uint32_t elapsed = current_time - delta->transition_start_ms;
        float progress = (float)elapsed / (float)delta->transition_duration_ms;
        
        if (progress > 1.0f) {
            progress = 1.0f;  // Clamp to complete
        }
        
        // Apply overshoot curve
        float effect_strength = progress * delta->overshoot_multiplier;
        
        // Reverse calculation if mood is decreasing
        if (delta->reverse_on_decrease) {
            effect_strength = -effect_strength;  // Subtract the effect
        }
        
        // Get mood-specific palette and blend
        const palette_color_t* mood_palette = mood_palette_get_palette_for_mood(delta->mood_type);
        uint16_t mood_color = mood_palette[palette_index];
        
        // Blend based on effect strength
        final_color = mood_palette_blend_colors(final_color, mood_color, effect_strength);
    }
    
    return final_color;
}

esp_err_t mood_transitions_render_frame_realtime(const realtime_frame_t* frame, uint16_t* output_buffer) {
    if (!frame || !output_buffer) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Process each pixel on-demand - no memory allocation!
    for (uint16_t i = 0; i < frame->pixel_count; i++) {
        uint8_t palette_index = frame->indexed_pixels[i];
        
        // Apply mood transitions to this pixel
        output_buffer[i] = mood_transitions_apply_to_pixel(palette_index, MOOD_ANGER);  // TODO: detect primary mood
    }
    
    return ESP_OK;
}

void mood_transitions_set_global_overshoot(float multiplier) {
    transition_manager.global_overshoot = multiplier;
    ESP_LOGI(TAG, "Global overshoot set to %.2f", multiplier);
}

void mood_transitions_enable_nervousness(bool enable) {
    transition_manager.nervousness_mode = enable;
    ESP_LOGI(TAG, "Nervousness mode %s", enable ? "enabled" : "disabled");
}

void mood_transitions_add_internal_tension(mood_type_t mood, float tension_level) {
    // Add a small overshoot to simulate internal emotional pressure
    for (int i = 0; i < transition_manager.active_delta_count; i++) {
        if (transition_manager.active_deltas[i].mood_type == mood) {
            transition_manager.active_deltas[i].overshoot_multiplier += tension_level;
            ESP_LOGD(TAG, "Added internal tension %.2f to %s", tension_level, mood_type_names[mood]);
            break;
        }
    }
}

uint16_t mood_transitions_reverse_pixel_change(uint8_t palette_index, const mood_delta_t* delta) {
    // Calculate the reverse transformation instead of storing the original!
    const palette_color_t* base_palette = mood_palette_get_neutral_palette();
    const palette_color_t* mood_palette = mood_palette_get_palette_for_mood(delta->mood_type);
    
    uint16_t base_color = base_palette[palette_index];
    uint16_t mood_color = mood_palette[palette_index];
    
    // Reverse the blend by the same amount it was applied
    float reverse_strength = -delta->overshoot_multiplier;
    
    return mood_palette_blend_colors(mood_color, base_color, reverse_strength);
}

void mood_transitions_reverse_mood_change(mood_type_t mood_type, int16_t reverse_amount) {
    // Find the active delta for this mood and reduce it
    for (int i = 0; i < transition_manager.active_delta_count; i++) {
        if (transition_manager.active_deltas[i].mood_type == mood_type) {
            transition_manager.active_deltas[i].delta -= reverse_amount;
            ESP_LOGD(TAG, "Reversed %s by %d", mood_type_names[mood_type], reverse_amount);
            
            // Remove delta if it's now negligible
            if (abs(transition_manager.active_deltas[i].delta) < 10) {
                // Shift remaining deltas down
                for (int j = i; j < transition_manager.active_delta_count - 1; j++) {
                    memcpy(&transition_manager.active_deltas[j], &transition_manager.active_deltas[j + 1], sizeof(mood_delta_t));
                }
                transition_manager.active_delta_count--;
                ESP_LOGD(TAG, "Removed completed delta for %s", mood_type_names[mood_type]);
            }
            break;
        }
    }
}

void mood_transitions_print_stats(void) {
    ESP_LOGI(TAG, "=== MOOD TRANSITION STATS ===");
    ESP_LOGI(TAG, "Total transitions: %lu", transition_manager.total_transitions);
    ESP_LOGI(TAG, "Active deltas: %d", transition_manager.active_delta_count);
    ESP_LOGI(TAG, "Overshoot events: %lu", transition_manager.overshoot_events);
    ESP_LOGI(TAG, "Global overshoot: %.2f", transition_manager.global_overshoot);
    ESP_LOGI(TAG, "Nervousness mode: %s", transition_manager.nervousness_mode ? "ON" : "OFF");
    
    for (int i = 0; i < transition_manager.active_delta_count; i++) {
        mood_delta_t* delta = &transition_manager.active_deltas[i];
        ESP_LOGI(TAG, "  Delta %d: %s %+d (%.2fx)", 
                 i, mood_type_names[delta->mood_type], delta->delta, delta->overshoot_multiplier);
    }
}

uint32_t mood_transitions_get_memory_usage(void) {
    // This system uses almost no memory compared to 57KB frame buffers!
    return sizeof(transition_manager);  // Should be < 1KB
}