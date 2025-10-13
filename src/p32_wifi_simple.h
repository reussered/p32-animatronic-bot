/**
 * P32 Simple WiFi Component Header
 * Basic WiFi connectivity for animatronic remote control and server communication
 */

#ifndef P32_WIFI_SIMPLE_H
#define P32_WIFI_SIMPLE_H

#include "p32_component_config.h"

#ifdef ENABLE_SIMPLE_WIFI

#include "esp_err.h"
#include <stdbool.h>

// WiFi configuration constants
#define P32_WIFI_MAX_RETRY          5
#define P32_WIFI_AP_CHANNEL         1
#define P32_WIFI_AP_MAX_CONN        4

// WiFi connection status
typedef enum {
    P32_WIFI_DISCONNECTED = 0,
    P32_WIFI_CONNECTING,
    P32_WIFI_CONNECTED,
    P32_WIFI_AP_MODE,
    P32_WIFI_ERROR
} p32_wifi_status_t;

// Server communication packet structure
typedef struct {
    uint32_t packet_id;
    uint32_t timestamp;
    uint16_t command;
    uint16_t data_length;
    uint8_t data[256];  // Max 256 bytes payload
    uint16_t checksum;
} p32_server_packet_t;

// Server commands
#define P32_CMD_PING            0x0001
#define P32_CMD_STATUS_REQUEST  0x0002
#define P32_CMD_MOOD_CHANGE     0x0003
#define P32_CMD_ANIMATION       0x0004
#define P32_CMD_SOUND           0x0005
#define P32_CMD_SHUTDOWN        0x0006
#define P32_CMD_CONFIG_UPDATE   0x0007

// WiFi function declarations
esp_err_t p32_wifi_init(void);
esp_err_t p32_wifi_connect(const char* ssid, const char* password);
esp_err_t p32_wifi_disconnect(void);
bool p32_wifi_is_connected(void);
esp_err_t p32_wifi_start_ap(const char* ssid, const char* password);
p32_wifi_status_t p32_wifi_get_status(void);

// Server communication functions
esp_err_t p32_server_init(const char* server_ip, uint16_t server_port);
esp_err_t p32_server_connect(void);
esp_err_t p32_server_disconnect(void);
esp_err_t p32_server_send_packet(p32_server_packet_t* packet);
esp_err_t p32_server_receive_packet(p32_server_packet_t* packet, uint32_t timeout_ms);
esp_err_t p32_server_send_status(void);
esp_err_t p32_server_send_heartbeat(void);

// Utility functions
uint16_t p32_calculate_checksum(const uint8_t* data, size_t length);
bool p32_validate_packet(p32_server_packet_t* packet);

#else
// Stub functions when WiFi is disabled
static inline esp_err_t p32_wifi_init(void) { return ESP_OK; }
static inline esp_err_t p32_wifi_connect(const char* ssid, const char* password) { return ESP_OK; }
static inline esp_err_t p32_wifi_disconnect(void) { return ESP_OK; }
static inline bool p32_wifi_is_connected(void) { return false; }
static inline esp_err_t p32_wifi_start_ap(const char* ssid, const char* password) { return ESP_OK; }
static inline esp_err_t p32_server_init(const char* server_ip, uint16_t server_port) { return ESP_OK; }
static inline esp_err_t p32_server_send_heartbeat(void) { return ESP_OK; }
#endif // ENABLE_SIMPLE_WIFI

#endif // P32_WIFI_SIMPLE_H