#include "p32_component_config.h"

#ifdef ENABLE_SIMPLE_SERIAL
// Simple Serial Monitor Component
// Provides ESP32 serial communication for debugging and logging

#include "p32_core.h"

static const char* TAG = "SIMPLE_SERIAL";

// ========================================
// SIMPLE SERIAL COMPONENT
// ========================================

esp_err_t simple_serial_init(void) {
    // Initialize UART0 for serial monitor (default ESP32 USB serial)
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    // Install UART driver
    esp_err_t ret = uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ret = uart_param_config(UART_NUM_0, &uart_config);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ret = uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ESP_LOGI(TAG, "Simple Serial: UART0 initialized at 115200 baud for serial monitor");
    ESP_LOGI(TAG, "Simple Serial: Ready for debugging output and logging");
    return ESP_OK;
}

esp_err_t simple_serial_print(const char* message) {
    if (message == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Send message to serial monitor
    int len = strlen(message);
    int bytes_written = uart_write_bytes(UART_NUM_0, message, len);
    
    if (bytes_written != len) {
        ESP_LOGW(TAG, "Simple Serial: Warning - only wrote %d of %d bytes", bytes_written, len);
        return ESP_FAIL;
    }
    
    // Add newline for readability
    uart_write_bytes(UART_NUM_0, "\n", 1);
    
    ESP_LOGD(TAG, "Simple Serial: Sent %d bytes to serial monitor", len);
    return ESP_OK;
}

esp_err_t simple_serial_printf(const char* format, ...) {
    if (format == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len >= sizeof(buffer)) {
        ESP_LOGW(TAG, "Simple Serial: Message truncated to %d characters", sizeof(buffer) - 1);
    }
    
    return simple_serial_print(buffer);
}

esp_err_t simple_serial_log_system_info(void) {
    ESP_LOGI(TAG, "=== ESP32 System Information ===");
    ESP_LOGI(TAG, "ESP-IDF Version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Chip Model: %s", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG, "Free Heap: %lu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum Free Heap: %lu bytes", esp_get_minimum_free_heap_size());
    
    // Get chip info
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "CPU Cores: %d", chip_info.cores);
    ESP_LOGI(TAG, "WiFi: %s", (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "Yes" : "No");
    ESP_LOGI(TAG, "Bluetooth: %s", (chip_info.features & CHIP_FEATURE_BT) ? "Yes" : "No");
    
    // Flash info
    uint32_t flash_size;
    esp_flash_get_size(NULL, &flash_size);
    ESP_LOGI(TAG, "Flash Size: %lu MB", flash_size / (1024 * 1024));
    
    ESP_LOGI(TAG, "=== P32 Animatronic Bot Ready ===");
    return ESP_OK;
}

// Component function pointer structure
typedef struct {
    esp_err_t (*init)(void);
    esp_err_t (*print)(const char* message);
    esp_err_t (*printf)(const char* format, ...);
    esp_err_t (*log_system_info)(void);
} simple_serial_component_t;

// Component instance with function pointers
static simple_serial_component_t serial_component = {
    .init = simple_serial_init,
    .print = simple_serial_print,
    .printf = simple_serial_printf,
    .log_system_info = simple_serial_log_system_info
};
#endif // 0 - TEMPORARILY DISABLED
