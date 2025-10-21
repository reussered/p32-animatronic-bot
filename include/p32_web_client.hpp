// P32 Web Client - Sends animation data to PC web server
// ESP32 acts as client, PC hosts the display server

#ifndef P32_WEB_CLIENT_H
#define P32_WEB_CLIENT_H

#include "esp_err.h"
#include "p32_eye_display.h"
#include <stdint.h>

// Web client configuration
typedef struct {
    char pc_server_ip[16];          // Your PC's IP address
    uint16_t pc_server_port;        // Your PC's web server port (e.g., 3000)
    char wifi_ssid[32];             // WiFi network name
    char wifi_password[64];         // WiFi password
    char device_id[32];             // Unique device identifier (e.g., "GOBLIN_LEFT_EYE")
    char display_type[16];          // "EYE", "MOUTH", "SENSOR", "DEBUG"
    uint16_t display_width;         // Display window width (e.g., 200)
    uint16_t display_height;        // Display window height (e.g., 200)
    bool connected;                 // Connection status
    bool registered;                // Registered with PC server
    uint32_t send_interval_ms;      // How often to send updates
    uint32_t last_send_time;        // Last transmission time
} web_client_config_t;

// Device registration packet (sent at init)
typedef struct {
    char device_id[32];             // Unique device identifier
    char display_type[16];          // "EYE", "MOUTH", "SENSOR", "DEBUG"
    uint16_t display_width;         // Requested window width
    uint16_t display_height;        // Requested window height
    char bot_name[32];              // "GOBLIN_FULL", "CYCLOPS", etc.
    char component_name[32];        // "LEFT_EYE", "RIGHT_EYE", "MOUTH", etc.
    uint32_t esp32_mac[2];          // ESP32 MAC address for identification
} device_registration_t;

// Animation data packet to send to PC
typedef struct {
    char device_id[32];             // Device identifier (matches registration)
    uint32_t timestamp_ms;          // ESP32 timestamp
    uint32_t loop_count;            // Main loop counter
    char animation_name[32];        // Current animation
    float eye_openness;             // 0.0 - 1.0 (for eyes)
    float pupil_size;               // 0.0 - 1.0 (for eyes)
    uint32_t color;                 // RGB color (0xRRGGBB)
    int expression;                 // Expression type
    bool animation_active;          // Is animation running?
    char custom_data[128];          // For mouth shapes, sensor readings, etc.
} animation_packet_t;

// Function declarations
esp_err_t web_client_init(void); // Simple init with default config
esp_err_t web_client_init_full(const char* device_id, const char* display_type, 
                              uint16_t width, uint16_t height,
                              const char* pc_ip, uint16_t pc_port, 
                              const char* ssid, const char* password);
esp_err_t web_client_connect_wifi(void);
esp_err_t web_client_register_device(const char* bot_name, const char* component_name);
esp_err_t web_client_register_component(const char* component_id, const char* display_type, 
                                       uint16_t width, uint16_t height,
                                       const char* bot_name, const char* component_name);
esp_err_t web_client_send_animation_data_for_component(const char* component_id, const eye_display_t* display);
esp_err_t web_client_send_sensor_data_for_component(const char* component_id, float sensor_value, uint32_t loop_count);
esp_err_t web_client_send_custom_data_for_component(const char* component_id, const char* custom_data, uint32_t loop_count);
esp_err_t web_client_send_animation_data(const eye_display_t* display, uint32_t loop_count);
esp_err_t web_client_send_custom_data(const char* custom_data, uint32_t loop_count);
bool web_client_is_connected(void);
bool web_client_is_registered(void);
void web_client_disconnect(void);

// PC Server setup helper
void web_client_print_pc_server_code(void);

#endif // P32_WEB_CLIENT_H