// src/components/system/serial_console.c
// Serial Console Component - Debug commands and telemetry output
// Component Type: SYSTEM_LEVEL (attached to torso subsystem)
// Timing: hitCount 25 (executes every 2.5 seconds)

#include "p32_component_config.hpp"
#include "p32_shared_state.hpp"
#include "esp_log.h"
#include "driver/uart.h"

#ifdef ENABLE_SYSTEM_COMPONENTS

static const char *TAG = "serial_console";

// Console configuration
#define CONSOLE_UART_NUM UART_NUM_0
#define CONSOLE_BAUD_RATE 115200
#define CONSOLE_PROMPT "goblin> "

// NO ARGUMENTS - Init function signature
esp_err_t serial_console_init(void) {
    ESP_LOGI(TAG, "Initializing serial console...");
    ESP_LOGI(TAG, "Baud rate: %d, Prompt: \"%s\", Echo enabled", 
             CONSOLE_BAUD_RATE, CONSOLE_PROMPT);
    
    // Configure UART parameters
    uart_config_t uart_config = {
        .baud_rate = CONSOLE_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    esp_err_t ret = uart_param_config(CONSOLE_UART_NUM, &uart_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure UART: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Install UART driver
    ret = uart_driver_install(CONSOLE_UART_NUM, 256, 0, 0, NULL, 0);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        // ESP_ERR_INVALID_STATE means already installed (OK for console)
        ESP_LOGE(TAG, "Failed to install UART driver: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Serial console initialized at loop count: %u", g_loopCount);
    
    // Print welcome banner
    printf("\n");
    printf("====================================\n");
    printf("  P32 Goblin Bot - Serial Console  \n");
    printf("====================================\n");
    printf("Type 'help' for commands\n");
    printf("%s", CONSOLE_PROMPT);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
void serial_console_act(void) {
    // hitCount: 25 → executes every 2.5 seconds
    
    // Check for incoming commands
    uint8_t data[128];
    int len = uart_read_bytes(CONSOLE_UART_NUM, data, sizeof(data) - 1, 0);
    
    if (len > 0) {
        data[len] = '\0';
        
        // Echo received data
        printf("\n");
        
        // Simple command parser (stub)
        if (strncmp((char*)data, "status", 6) == 0) {
            printf("System Status:\n");
            printf("  Uptime: %u seconds\n", g_shared_state.uptime_seconds);
            printf("  Battery: %u%%\n", g_shared_state.battery_percent);
            printf("  WiFi: %s\n", g_shared_state.wifi_connected ? "Connected" : "Disconnected");
            printf("  Loop: %u\n", g_loopCount);
        } else if (strncmp((char*)data, "help", 4) == 0) {
            printf("Available commands:\n");
            printf("  status  - Show system status\n");
            printf("  help    - Show this help\n");
        } else {
            printf("Unknown command: %s\n", data);
            printf("Type 'help' for available commands\n");
        }
        
        printf("%s", CONSOLE_PROMPT);
    }
    
    ESP_LOGD(TAG, "Console check at loop %u", g_loopCount);
}

#endif // ENABLE_SYSTEM_COMPONENTS
