#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2s.h"
#include "esp_rom_sys.h"
#include <math.h>

// Core system configuration
#define P32_TAG "P32_CORE"
#define P32_MAX_COMPONENTS 32
#define P32_MAX_JSON_SIZE 8192
#define P32_CONFIG_PATH "/spiffs/config"

// Mood system states (8 primary emotions)
typedef enum {
    MOOD_FEAR = 0,
    MOOD_ANGER,
    MOOD_IRRITATION, 
    MOOD_HAPPINESS,
    MOOD_CONTENTMENT,
    MOOD_HUNGER,
    MOOD_CURIOSITY,
    MOOD_AFFECTION,
    MOOD_COUNT
} p32_mood_t;

// Component types from JSON configuration
typedef enum {
    COMPONENT_DISPLAY = 0,
    COMPONENT_SENSOR,
    COMPONENT_SPEAKER,
    COMPONENT_LED,
    COMPONENT_ACTUATOR,
    COMPONENT_MICROPHONE,
    COMPONENT_COUNT
} p32_component_type_t;

// Interface types for GPIO mapping
typedef enum {
    INTERFACE_SPI_BUS = 0,
    INTERFACE_SPI_DEVICE,
    INTERFACE_I2S_BUS,
    INTERFACE_I2S_DEVICE,
    INTERFACE_GPIO_PAIR,
    INTERFACE_COUNT
} p32_interface_type_t;

// Component configuration structure
typedef struct {
    char component_id[32];
    p32_component_type_t type;
    char hardware_reference[64];
    char interface_assignment[32];
    float position_x;
    float position_y;
    float position_z;
    uint8_t gpio_pins[8];
    uint8_t pin_count;
    bool enabled;
} p32_component_config_t;

// Bot configuration structure
typedef struct {
    char bot_type[32];
    char family_reference[64];
    p32_mood_t default_mood;
    p32_component_config_t components[P32_MAX_COMPONENTS];
    uint8_t component_count;
    bool initialized;
} p32_bot_config_t;

// Core system state
typedef struct {
    p32_bot_config_t current_bot;
    p32_mood_t current_mood;
    bool system_initialized;
    SemaphoreHandle_t config_mutex;
    TaskHandle_t mood_task;
    TaskHandle_t animation_task;
} p32_system_t;

// Core system functions
esp_err_t p32_core_init(void);
esp_err_t p32_load_bot_config(const char* bot_file);
esp_err_t p32_set_mood(p32_mood_t mood);
p32_mood_t p32_get_mood(void);
esp_err_t p32_update_components(void);

// JSON configuration parsing
esp_err_t p32_parse_json_config(const char* json_data, p32_bot_config_t* config);
esp_err_t p32_validate_component_config(const p32_component_config_t* component);

// Component management
esp_err_t p32_init_component(const p32_component_config_t* component);
esp_err_t p32_enable_component(const char* component_id);
esp_err_t p32_disable_component(const char* component_id);

// Hardware interface initialization
esp_err_t p32_init_spi_bus(void);
esp_err_t p32_init_i2s_bus(void);
esp_err_t p32_init_gpio_interfaces(void);

// Logging and debug
void p32_log_system_state(void);
void p32_log_component_status(const char* component_id);

// Global system instance
extern p32_system_t g_p32_system;