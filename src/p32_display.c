#include "p32_display.h"

static const char* TAG = "P32_DISPLAY";
static p32_display_system_t g_display_system = {0};

// Mood-based color palettes (RGB565 format)
const p32_color_palette_t p32_mood_palettes[MOOD_COUNT] = {
    // MOOD_FEAR: Dark blues and blacks
    {.primary = 0x001F, .secondary = 0x0010, .highlight = 0x4A69, .background = 0x0000},
    // MOOD_ANGER: Reds and oranges  
    {.primary = 0xF800, .secondary = 0xFD20, .highlight = 0xFFE0, .background = 0x2000},
    // MOOD_IRRITATION: Yellows and oranges
    {.primary = 0xFD20, .secondary = 0xFFE0, .highlight = 0xF800, .background = 0x2100},
    // MOOD_HAPPINESS: Greens and yellows
    {.primary = 0x07E0, .secondary = 0xAFE5, .highlight = 0xFFE0, .background = 0x0200},
    // MOOD_CONTENTMENT: Soft blues and greens
    {.primary = 0x4A69, .secondary = 0x07E0, .highlight = 0xAFFF, .background = 0x0010},
    // MOOD_HUNGER: Deep reds and browns
    {.primary = 0x8000, .secondary = 0x4000, .highlight = 0xF800, .background = 0x0000},
    // MOOD_CURIOSITY: Bright blues and purples
    {.primary = 0x001F, .secondary = 0x781F, .highlight = 0xAFFF, .background = 0x0008},
    // MOOD_AFFECTION: Pinks and warm colors
    {.primary = 0xF81F, .secondary = 0xFD20, .highlight = 0xFFFF, .background = 0x2008}
};

esp_err_t p32_display_init(void) {
    ESP_LOGI(TAG, "Initializing display system...");
    
    memset(&g_display_system, 0, sizeof(p32_display_system_t));
    
    // Allocate framebuffers for each display
    g_display_system.left_eye.framebuffer = heap_caps_malloc(GC9A01_BUFFER_SIZE, MALLOC_CAP_DMA);
    g_display_system.right_eye.framebuffer = heap_caps_malloc(GC9A01_BUFFER_SIZE, MALLOC_CAP_DMA);
    g_display_system.mouth.framebuffer = heap_caps_malloc(GC9A01_BUFFER_SIZE, MALLOC_CAP_DMA);
    
    if (!g_display_system.left_eye.framebuffer || 
        !g_display_system.right_eye.framebuffer || 
        !g_display_system.mouth.framebuffer) {
        ESP_LOGE(TAG, "Failed to allocate framebuffers");
        return ESP_ERR_NO_MEM;
    }
    
    // Configure display devices from system config
    extern p32_system_t g_p32_system;
    
    // Setup left eye display (SPI Device 1)
    for (int i = 0; i < g_p32_system.current_bot.component_count; i++) {
        p32_component_config_t* comp = &g_p32_system.current_bot.components[i];
        
        if (strcmp(comp->component_id, "goblineye_left") == 0) {
            ESP_ERROR_CHECK(p32_display_setup_device(&g_display_system.left_eye, comp));
        } else if (strcmp(comp->component_id, "goblineye_right") == 0) {
            ESP_ERROR_CHECK(p32_display_setup_device(&g_display_system.right_eye, comp));
        } else if (strcmp(comp->component_id, "goblin_mouth") == 0) {
            ESP_ERROR_CHECK(p32_display_setup_device(&g_display_system.mouth, comp));
        }
    }
    
    // Clear all displays to black
    p32_display_clear(&g_display_system.left_eye, 0x0000);
    p32_display_clear(&g_display_system.right_eye, 0x0000);
    p32_display_clear(&g_display_system.mouth, 0x0000);
    
    // Set initial animations
    g_display_system.current_eye_anim = EYE_ANIM_CURIOUS_LOOK;
    g_display_system.current_mouth_anim = MOUTH_ANIM_CLOSED;
    g_display_system.displays_enabled = true;
    
    ESP_LOGI(TAG, "Display system initialized successfully");
    return ESP_OK;
}

void p32_display_act(void) {
    extern uint32_t loopCount;
    
    // Update display animations every 5 loops (about 10Hz if main loop is 50Hz)
    if (loopCount % 5 == 0) {
        if (g_display_system.displays_enabled) {
            // Update left eye
            p32_display_update(&g_display_system.left_eye);
            
            // Update right eye  
            p32_display_update(&g_display_system.right_eye);
            
            // Update mouth
            p32_display_update(&g_display_system.mouth);
        }
    }
}

esp_err_t p32_display_setup_device(p32_display_t* display, const p32_component_config_t* config) {
    ESP_LOGI(TAG, "Setting up display: %s", config->component_id);
    
    strncpy(display->component_id, config->component_id, sizeof(display->component_id));
    display->cs_pin = config->gpio_pins[0];
    display->dc_pin = 21;  // Common DC pin for all displays
    display->reset_pin = 16; // Common reset pin for all displays
    
    // Configure SPI device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10*1000*1000,  // 10 MHz
        .mode = 0,
        .spics_io_num = display->cs_pin,
        .queue_size = 7,
        .pre_cb = NULL,
        .post_cb = NULL
    };
    
    esp_err_t ret = spi_bus_add_device(SPI2_HOST, &devcfg, &display->spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device for %s: %s", config->component_id, esp_err_to_name(ret));
        return ret;
    }
    
    // Configure DC and Reset pins as outputs
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << display->dc_pin) | (1ULL << display->reset_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Reset display
    gpio_set_level(display->reset_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(display->reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    display->initialized = true;
    ESP_LOGI(TAG, "Display %s setup complete", config->component_id);
    
    return ESP_OK;
}

esp_err_t p32_display_clear(p32_display_t* display, uint16_t color) {
    if (!display->initialized || !display->framebuffer) {
        return ESP_ERR_INVALID_STATE;
    }
    
    p32_display_fill_buffer(display->framebuffer, color);
    return p32_display_update(display);
}

esp_err_t p32_display_update(p32_display_t* display) {
    if (!display->initialized || !display->framebuffer) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Send framebuffer to display via SPI
    spi_transaction_t trans = {
        .length = GC9A01_BUFFER_SIZE * 8,
        .tx_buffer = display->framebuffer
    };
    
    esp_err_t ret = spi_device_transmit(display->spi_handle, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update display %s: %s", display->component_id, esp_err_to_name(ret));
    }
    
    return ret;
}

esp_err_t p32_display_set_eye_animation(p32_eye_animation_t animation) {
    if (animation >= EYE_ANIM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_display_system.current_eye_anim = animation;
    
    // Draw animation on both eyes
    switch (animation) {
        case EYE_ANIM_BLINK:
            p32_display_draw_eye_blink(&g_display_system.left_eye);
            p32_display_draw_eye_blink(&g_display_system.right_eye);
            break;
        case EYE_ANIM_ANGRY_STARE:
            p32_display_draw_eye_angry(&g_display_system.left_eye);
            p32_display_draw_eye_angry(&g_display_system.right_eye);
            break;
        case EYE_ANIM_CURIOUS_LOOK:
            p32_display_draw_eye_curious(&g_display_system.left_eye);
            p32_display_draw_eye_curious(&g_display_system.right_eye);
            break;
        case EYE_ANIM_FEAR_WIDE:
            p32_display_draw_eye_fear(&g_display_system.left_eye);
            p32_display_draw_eye_fear(&g_display_system.right_eye);
            break;
        default:
            // Default to curious look
            p32_display_draw_eye_curious(&g_display_system.left_eye);
            p32_display_draw_eye_curious(&g_display_system.right_eye);
            break;
    }
    
    return ESP_OK;
}

esp_err_t p32_display_set_mouth_animation(p32_mouth_animation_t animation) {
    if (animation >= MOUTH_ANIM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_display_system.current_mouth_anim = animation;
    
    // Draw animation on mouth display
    switch (animation) {
        case MOUTH_ANIM_SMILE:
            p32_display_draw_mouth_smile(&g_display_system.mouth);
            break;
        case MOUTH_ANIM_FROWN:
            p32_display_draw_mouth_frown(&g_display_system.mouth);
            break;
        case MOUTH_ANIM_SNARL:
            p32_display_draw_mouth_snarl(&g_display_system.mouth);
            break;
        case MOUTH_ANIM_ROAR:
            p32_display_draw_mouth_roar(&g_display_system.mouth);
            break;
        default:
            // Default to closed mouth
            p32_display_clear(&g_display_system.mouth, 0x0000);
            break;
    }
    
    return ESP_OK;
}

esp_err_t p32_display_update_mood_animations(p32_mood_t mood) {
    if (mood >= MOOD_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Update animations based on mood
    switch (mood) {
        case MOOD_ANGER:
            p32_display_set_eye_animation(EYE_ANIM_ANGRY_STARE);
            p32_display_set_mouth_animation(MOUTH_ANIM_SNARL);
            break;
        case MOOD_FEAR:
            p32_display_set_eye_animation(EYE_ANIM_FEAR_WIDE);
            p32_display_set_mouth_animation(MOUTH_ANIM_CLOSED);
            break;
        case MOOD_HAPPINESS:
            p32_display_set_eye_animation(EYE_ANIM_CURIOUS_LOOK);
            p32_display_set_mouth_animation(MOUTH_ANIM_SMILE);
            break;
        case MOOD_CURIOSITY:
            p32_display_set_eye_animation(EYE_ANIM_CURIOUS_LOOK);
            p32_display_set_mouth_animation(MOUTH_ANIM_CLOSED);
            break;
        default:
            p32_display_set_eye_animation(EYE_ANIM_BLINK);
            p32_display_set_mouth_animation(MOUTH_ANIM_CLOSED);
            break;
    }
    
    ESP_LOGI(TAG, "Updated animations for mood: %d", mood);
    return ESP_OK;
}

// Eye animation implementations
esp_err_t p32_display_draw_eye_blink(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[p32_get_mood()];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw closed eyelid (horizontal ellipse)
    p32_display_draw_rect(display->framebuffer, 60, 115, 120, 10, palette.primary);
    
    return p32_display_update(display);
}

esp_err_t p32_display_draw_eye_angry(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[MOOD_ANGER];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw angry eye (angled narrow opening)
    p32_display_draw_circle(display->framebuffer, 120, 120, 80, palette.primary);
    p32_display_draw_circle(display->framebuffer, 120, 120, 40, palette.highlight);
    p32_display_draw_circle(display->framebuffer, 130, 110, 15, 0x0000); // Pupil
    
    return p32_display_update(display);
}

esp_err_t p32_display_draw_eye_curious(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[MOOD_CURIOSITY];
    
    // Clear to background  
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw wide curious eye
    p32_display_draw_circle(display->framebuffer, 120, 120, 90, palette.primary);
    p32_display_draw_circle(display->framebuffer, 120, 120, 60, palette.secondary);
    p32_display_draw_circle(display->framebuffer, 120, 120, 20, 0x0000); // Pupil
    
    return p32_display_update(display);
}

esp_err_t p32_display_draw_eye_fear(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[MOOD_FEAR];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw wide fearful eye
    p32_display_draw_circle(display->framebuffer, 120, 120, 100, palette.primary);
    p32_display_draw_circle(display->framebuffer, 120, 120, 70, palette.secondary);
    p32_display_draw_circle(display->framebuffer, 120, 120, 30, 0x0000); // Dilated pupil
    
    return p32_display_update(display);
}

// Mouth animation implementations
esp_err_t p32_display_draw_mouth_smile(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[MOOD_HAPPINESS];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw smile curve
    p32_display_draw_rect(display->framebuffer, 40, 140, 160, 20, palette.primary);
    p32_display_draw_rect(display->framebuffer, 60, 120, 120, 15, palette.secondary);
    
    return p32_display_update(display);
}

esp_err_t p32_display_draw_mouth_frown(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[p32_get_mood()];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw frown curve (inverted smile)
    p32_display_draw_rect(display->framebuffer, 40, 80, 160, 20, palette.primary);
    p32_display_draw_rect(display->framebuffer, 60, 100, 120, 15, palette.secondary);
    
    return p32_display_update(display);
}

esp_err_t p32_display_draw_mouth_snarl(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[MOOD_ANGER];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw snarling mouth with teeth
    p32_display_draw_rect(display->framebuffer, 30, 100, 180, 40, palette.primary);
    
    // Draw teeth
    for (int i = 0; i < 8; i++) {
        int x = 40 + (i * 20);
        p32_display_draw_rect(display->framebuffer, x, 90, 8, 20, 0xFFFF);
    }
    
    return p32_display_update(display);
}

esp_err_t p32_display_draw_mouth_roar(p32_display_t* display) {
    if (!display->framebuffer) return ESP_ERR_INVALID_STATE;
    
    p32_color_palette_t palette = p32_mood_palettes[MOOD_ANGER];
    
    // Clear to background
    p32_display_fill_buffer(display->framebuffer, palette.background);
    
    // Draw wide open roaring mouth
    p32_display_draw_circle(display->framebuffer, 120, 120, 100, palette.primary);
    p32_display_draw_circle(display->framebuffer, 120, 120, 80, 0x0000); // Black interior
    
    return p32_display_update(display);
}

// Utility functions
uint16_t p32_display_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void p32_display_draw_circle(uint16_t* buffer, int x, int y, int radius, uint16_t color) {
    // Simple circle drawing algorithm
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dy*dy <= radius*radius) {
                int px = x + dx;
                int py = y + dy;
                if (px >= 0 && px < GC9A01_WIDTH && py >= 0 && py < GC9A01_HEIGHT) {
                    buffer[py * GC9A01_WIDTH + px] = color;
                }
            }
        }
    }
}

void p32_display_draw_rect(uint16_t* buffer, int x, int y, int w, int h, uint16_t color) {
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            int px = x + dx;
            int py = y + dy;
            if (px >= 0 && px < GC9A01_WIDTH && py >= 0 && py < GC9A01_HEIGHT) {
                buffer[py * GC9A01_WIDTH + px] = color;
            }
        }
    }
}

void p32_display_fill_buffer(uint16_t* buffer, uint16_t color) {
    for (int i = 0; i < GC9A01_WIDTH * GC9A01_HEIGHT; i++) {
        buffer[i] = color;
    }
}