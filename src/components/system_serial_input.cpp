// System Serial Input Monitor - Dynamic log level control
// Press keys to change log verbosity during runtime

#include <esp_log.h>
#include <esp_err.h>
#include <driver/uart.h>

static const char* TAG = "serial_input";

esp_err_t system_serial_input_init(void)
{
    ESP_LOGI(TAG, "=== Serial Input Monitor Initialized ===");
    ESP_LOGI(TAG, "Commands:");
    ESP_LOGI(TAG, "  'v' = VERBOSE (show all logs including ESP_LOGV)");
    ESP_LOGI(TAG, "  'i' = INFO (normal operation, default)");
    ESP_LOGI(TAG, "  'w' = WARN (warnings and errors only)");
    ESP_LOGI(TAG, "  'e' = ERROR (errors only)");
    ESP_LOGI(TAG, "  'q' = QUIET (no logs)");
    ESP_LOGI(TAG, "========================================");
    
    return ESP_OK;
}

void system_serial_input_act(void)
{
    uint8_t data[128];
    int len = uart_read_bytes(UART_NUM_0, data, sizeof(data), 0);
    
    if (len > 0)
    {
        for (int i = 0; i < len; i++)
        {
            switch (data[i])
            {
                case 'v':
                case 'V':
                    esp_log_level_set("*", ESP_LOG_VERBOSE);
                    ESP_LOGI(TAG, "Log level: VERBOSE (all logs enabled)");
                    break;
                    
                case 'i':
                case 'I':
                    esp_log_level_set("*", ESP_LOG_INFO);
                    ESP_LOGI(TAG, "Log level: INFO (normal operation)");
                    break;
                    
                case 'w':
                case 'W':
                    esp_log_level_set("*", ESP_LOG_WARN);
                    ESP_LOGW(TAG, "Log level: WARN (warnings and errors only)");
                    break;
                    
                case 'e':
                case 'E':
                    esp_log_level_set("*", ESP_LOG_ERROR);
                    ESP_LOGE(TAG, "Log level: ERROR (errors only)");
                    break;
                    
                case 'q':
                case 'Q':
                    esp_log_level_set("*", ESP_LOG_NONE);
                    // This message won't show after this point
                    ESP_LOGI(TAG, "Log level: QUIET (logging disabled)");
                    break;
                    
                default:
                    // Ignore other characters (newlines, etc.)
                    break;
            }
        }
    }
}
