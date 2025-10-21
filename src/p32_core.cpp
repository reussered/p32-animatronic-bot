#include "p32_component_config.h"

#ifdef ENABLE_P32_CORE
// P32 CORE SYSTEM
#include "p32_core.h"
#include "cJSON.h"

static const char* TAG = "P32_CORE";

// Global system instance
p32_system_t g_p32_system;

// GPIO pin assignments based on goblin_full configuration
#define P32_SPI_MOSI_PIN    23
#define P32_SPI_MISO_PIN    19  
#define P32_SPI_CLK_PIN     18
#define P32_SPI_CS1_PIN     15  // Left eye
#define P32_SPI_CS2_PIN     2   // Right eye  
#define P32_SPI_CS3_PIN     4   // Mouth

#define P32_I2S_BCK_PIN     26
#define P32_I2S_WS_PIN      25
#define P32_I2S_DATA_PIN    22

esp_err_t p32_core_init(void) {
    ESP_LOGI(TAG, "Initializing P32 core system...");
    
    // Initialize system structure
    memset(&g_p32_system, 0, sizeof(p32_system_t));
    
    // Create mutex for configuration access
    g_p32_system.config_mutex = xSemaphoreCreateMutex();
    if (g_p32_system.config_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create config mutex");
        return ESP_ERR_NO_MEM;
    }
    
    // Initialize hardware interfaces
    ESP_ERROR_CHECK(p32_init_spi_bus());
    ESP_ERROR_CHECK(p32_init_i2s_bus());
    ESP_ERROR_CHECK(p32_init_gpio_interfaces());
    
    // Set system as initialized
    g_p32_system.system_initialized = true;
    g_p32_system.current_mood = MOOD_CONTENTMENT;
    
    ESP_LOGI(TAG, "P32 core system initialized successfully");
    return ESP_OK;
}

esp_err_t p32_load_bot_config(const char* bot_file) {
    ESP_LOGI(TAG, "Loading bot configuration: %s", bot_file);
    
    if (!g_p32_system.system_initialized) {
        ESP_LOGE(TAG, "System not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    // For now, load default goblin_full configuration
    // In full implementation, this would read from SPIFFS/SD card
    
    if (xSemaphoreTake(g_p32_system.config_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        // Configure default goblin_full bot
        strncpy(g_p32_system.current_bot.bot_type, "GOBLINFULL", sizeof(g_p32_system.current_bot.bot_type));
        strncpy(g_p32_system.current_bot.family_reference, "config/bots/bot_families/fantasy/goblin_family.json", 
                sizeof(g_p32_system.current_bot.family_reference));
        
        g_p32_system.current_bot.default_mood = MOOD_CURIOSITY;
        g_p32_system.current_bot.component_count = 7;
        
        // Configure left eye (SPI Device 1)
        p32_component_config_t* left_eye = &g_p32_system.current_bot.components[0];
        strncpy(left_eye->component_id, "goblineye_left", sizeof(left_eye->component_id));
        left_eye->type = COMPONENT_DISPLAY;
        left_eye->position_x = -1.5f;
        left_eye->position_y = 0.5f;
        left_eye->position_z = 0.0f;
        left_eye->gpio_pins[0] = P32_SPI_CS1_PIN;
        left_eye->pin_count = 1;
        left_eye->enabled = true;
        
        // Configure right eye (SPI Device 2)
        p32_component_config_t* right_eye = &g_p32_system.current_bot.components[1];
        strncpy(right_eye->component_id, "goblineye_right", sizeof(right_eye->component_id));
        right_eye->type = COMPONENT_DISPLAY;
        right_eye->position_x = 1.5f;
        right_eye->position_y = 0.5f;
        right_eye->position_z = 0.0f;
        right_eye->gpio_pins[0] = P32_SPI_CS2_PIN;
        right_eye->pin_count = 1;
        right_eye->enabled = true;
        
        // Configure mouth display (SPI Device 3)
        p32_component_config_t* mouth = &g_p32_system.current_bot.components[2];
        strncpy(mouth->component_id, "goblin_mouth", sizeof(mouth->component_id));
        mouth->type = COMPONENT_DISPLAY;
        mouth->position_x = 0.0f;
        mouth->position_y = -1.05f;
        mouth->position_z = 0.0f;
        mouth->gpio_pins[0] = P32_SPI_CS3_PIN;
        mouth->pin_count = 1;
        mouth->enabled = true;
        
        // Configure nose sensor (GPIO Pair 1)
        p32_component_config_t* nose = &g_p32_system.current_bot.components[3];
        strncpy(nose->component_id, "goblin_nose", sizeof(nose->component_id));
        nose->type = COMPONENT_SENSOR;
        nose->position_x = 0.0f;
        nose->position_y = 0.0f;
        nose->position_z = 0.25f;
        nose->gpio_pins[0] = 32;  // Trigger pin
        nose->gpio_pins[1] = 33;  // Echo pin
        nose->pin_count = 2;
        nose->enabled = true;
        
        // Configure speaker (I2S Device 1)
        p32_component_config_t* speaker = &g_p32_system.current_bot.components[4];
        strncpy(speaker->component_id, "goblin_speaker", sizeof(speaker->component_id));
        speaker->type = COMPONENT_SPEAKER;
        speaker->position_x = -0.5f;
        speaker->position_y = 0.5f;
        speaker->position_z = -1.0f;
        speaker->gpio_pins[0] = P32_I2S_DATA_PIN;
        speaker->pin_count = 1;
        speaker->enabled = true;
        
        g_p32_system.current_bot.initialized = true;
        
        xSemaphoreGive(g_p32_system.config_mutex);
        
        ESP_LOGI(TAG, "Loaded bot config: %s with %d components", 
                 g_p32_system.current_bot.bot_type, g_p32_system.current_bot.component_count);
        
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Failed to acquire config mutex");
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t p32_set_mood(p32_mood_t mood) {
    if (mood >= MOOD_COUNT) {
        ESP_LOGE(TAG, "Invalid mood: %d", mood);
        return ESP_ERR_INVALID_ARG;
    }
    
    if (xSemaphoreTake(g_p32_system.config_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
        p32_mood_t old_mood = g_p32_system.current_mood;
        g_p32_system.current_mood = mood;
        xSemaphoreGive(g_p32_system.config_mutex);
        
        ESP_LOGI(TAG, "Mood changed: %d -> %d", old_mood, mood);
        return ESP_OK;
    }
    
    return ESP_ERR_TIMEOUT;
}

p32_mood_t p32_get_mood(void) {
    return g_p32_system.current_mood;
}

esp_err_t p32_update_components(void) {
    if (!g_p32_system.system_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Component updates will be handled by individual subsystem tasks
    // This function serves as a coordination point for system-wide updates
    
    return ESP_OK;
}

esp_err_t p32_init_spi_bus(void) {
    ESP_LOGI(TAG, "Initializing SPI bus...");
    
    spi_bus_config_t buscfg = {
        .mosi_io_num = P32_SPI_MOSI_PIN,
        .miso_io_num = P32_SPI_MISO_PIN,
        .sclk_io_num = P32_SPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "SPI bus initialized successfully");
    return ESP_OK;
}

esp_err_t p32_init_i2s_bus(void) {
    ESP_LOGI(TAG, "Initializing I2S bus...");
    
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = P32_I2S_BCK_PIN,
        .ws_io_num = P32_I2S_WS_PIN,
        .data_out_num = P32_I2S_DATA_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    esp_err_t ret = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = i2s_set_pin(I2S_NUM_0, &pin_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pins: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "I2S bus initialized successfully");
    return ESP_OK;
}

esp_err_t p32_init_gpio_interfaces(void) {
    ESP_LOGI(TAG, "Initializing GPIO interfaces...");
    
    // Configure ultrasonic sensor pins
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << 32) | (1ULL << 33),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "GPIO interfaces initialized successfully");
    return ESP_OK;
}

void p32_log_system_state(void) {
    ESP_LOGI(TAG, "=== P32 System State ===");
    ESP_LOGI(TAG, "System Initialized: %s", g_p32_system.system_initialized ? "YES" : "NO");
    ESP_LOGI(TAG, "Current Mood: %d", g_p32_system.current_mood);
    ESP_LOGI(TAG, "Bot Type: %s", g_p32_system.current_bot.bot_type);
    ESP_LOGI(TAG, "Components: %d", g_p32_system.current_bot.component_count);
    ESP_LOGI(TAG, "=======================");
}

void p32_log_component_status(const char* component_id) {
    ESP_LOGI(TAG, "Component Status: %s", component_id);
    // Implementation for detailed component logging
}
#endif // 0 - TEMPORARILY DISABLED
