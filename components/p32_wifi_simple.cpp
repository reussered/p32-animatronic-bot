/**
 * P32 Simple WiFi Component
 * Basic WiFi connectivity for animatronic remote control
 */

#include "p32_wifi_simple.h"

#ifdef ENABLE_SIMPLE_WIFI

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG = "P32_WIFI";

// WiFi event group
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;

// Server connection variables
static char server_ip[16] = {0};
static uint16_t server_port = 0;
static int server_socket = -1;
static uint32_t packet_counter = 0;

// Event handler for WiFi and IP events
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < P32_WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Simple WiFi initialization
esp_err_t p32_wifi_init(void) {
    ESP_LOGI(TAG, "Initializing WiFi");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Create event group
    s_wifi_event_group = xEventGroupCreate();

    // Register event handlers
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    ESP_LOGI(TAG, "WiFi initialized successfully");
    return ESP_OK;
}

// Connect to WiFi network
esp_err_t p32_wifi_connect(const char* ssid, const char* password) {
    ESP_LOGI(TAG, "Connecting to WiFi SSID: %s", ssid);

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    
    // Copy SSID and password
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // Wait for either connection or failure
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to WiFi SSID: %s", ssid);
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to WiFi SSID: %s", ssid);
        return ESP_FAIL;
    } else {
        ESP_LOGE(TAG, "Unexpected WiFi event");
        return ESP_ERR_INVALID_STATE;
    }
}

// Disconnect from WiFi
esp_err_t p32_wifi_disconnect(void) {
    ESP_LOGI(TAG, "Disconnecting from WiFi");
    return esp_wifi_disconnect();
}

// Get WiFi status
bool p32_wifi_is_connected(void) {
    EventBits_t bits = xEventGroupGetBits(s_wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}

// Start WiFi Access Point mode
esp_err_t p32_wifi_start_ap(const char* ssid, const char* password) {
    ESP_LOGI(TAG, "Starting WiFi AP: %s", ssid);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid_len = strlen(ssid),
            .channel = P32_WIFI_AP_CHANNEL,
            .max_connection = P32_WIFI_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    
    strncpy((char*)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);
    strncpy((char*)wifi_config.ap.password, password, sizeof(wifi_config.ap.password) - 1);

    if (strlen(password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP started. SSID: %s, Channel: %d", ssid, P32_WIFI_AP_CHANNEL);
    return ESP_OK;
}

// Get current WiFi status
p32_wifi_status_t p32_wifi_get_status(void) {
    if (!s_wifi_event_group) {
        return P32_WIFI_DISCONNECTED;
    }
    
    EventBits_t bits = xEventGroupGetBits(s_wifi_event_group);
    if (bits & WIFI_CONNECTED_BIT) {
        return P32_WIFI_CONNECTED;
    } else if (bits & WIFI_FAIL_BIT) {
        return P32_WIFI_ERROR;
    } else {
        return P32_WIFI_CONNECTING;
    }
}

// Initialize server connection
esp_err_t p32_server_init(const char* ip, uint16_t port) {
    if (!ip || port == 0) {
        ESP_LOGE(TAG, "Invalid server parameters");
        return ESP_ERR_INVALID_ARG;
    }
    
    strncpy(server_ip, ip, sizeof(server_ip) - 1);
    server_port = port;
    server_socket = -1;
    
    ESP_LOGI(TAG, "Server initialized: %s:%d", server_ip, server_port);
    return ESP_OK;
}

// Connect to server
esp_err_t p32_server_connect(void) {
    if (!p32_wifi_is_connected()) {
        ESP_LOGE(TAG, "WiFi not connected");
        return ESP_ERR_WIFI_NOT_CONNECT;
    }
    
    if (server_socket >= 0) {
        ESP_LOGI(TAG, "Already connected to server");
        return ESP_OK;
    }
    
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(server_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(server_port);
    
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (server_socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", server_ip, server_port);
    
    int err = connect(server_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(server_socket);
        server_socket = -1;
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "Successfully connected to server");
    return ESP_OK;
}

// Disconnect from server
esp_err_t p32_server_disconnect(void) {
    if (server_socket >= 0) {
        shutdown(server_socket, 0);
        close(server_socket);
        server_socket = -1;
        ESP_LOGI(TAG, "Disconnected from server");
    }
    return ESP_OK;
}

// Calculate checksum for packet validation
uint16_t p32_calculate_checksum(const uint8_t* data, size_t length) {
    uint32_t sum = 0;
    for (size_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return (uint16_t)(sum & 0xFFFF);
}

// Validate received packet
bool p32_validate_packet(p32_server_packet_t* packet) {
    if (!packet) return false;
    
    // Calculate checksum excluding the checksum field itself
    uint16_t calculated_checksum = p32_calculate_checksum(
        (uint8_t*)packet, 
        sizeof(p32_server_packet_t) - sizeof(packet->checksum)
    );
    
    return calculated_checksum == packet->checksum;
}

// Send packet to server
esp_err_t p32_server_send_packet(p32_server_packet_t* packet) {
    if (server_socket < 0) {
        ESP_LOGE(TAG, "Not connected to server");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (!packet) {
        ESP_LOGE(TAG, "Invalid packet");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Set packet ID and calculate checksum
    packet->packet_id = ++packet_counter;
    packet->timestamp = esp_timer_get_time() / 1000; // Convert to milliseconds
    packet->checksum = p32_calculate_checksum(
        (uint8_t*)packet, 
        sizeof(p32_server_packet_t) - sizeof(packet->checksum)
    );
    
    int err = send(server_socket, packet, sizeof(p32_server_packet_t), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return ESP_FAIL;
    }
    
    ESP_LOGD(TAG, "Packet sent: ID=%lu, CMD=0x%04X", packet->packet_id, packet->command);
    return ESP_OK;
}

// Receive packet from server
esp_err_t p32_server_receive_packet(p32_server_packet_t* packet, uint32_t timeout_ms) {
    if (server_socket < 0) {
        ESP_LOGE(TAG, "Not connected to server");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (!packet) {
        ESP_LOGE(TAG, "Invalid packet buffer");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    int len = recv(server_socket, packet, sizeof(p32_server_packet_t), 0);
    if (len < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ESP_ERR_TIMEOUT;
        }
        ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        return ESP_FAIL;
    } else if (len == 0) {
        ESP_LOGW(TAG, "Connection closed by server");
        return ESP_ERR_INVALID_STATE;
    } else if (len != sizeof(p32_server_packet_t)) {
        ESP_LOGE(TAG, "Received incomplete packet: %d bytes", len);
        return ESP_ERR_INVALID_SIZE;
    }
    
    // Validate packet
    if (!p32_validate_packet(packet)) {
        ESP_LOGE(TAG, "Invalid packet checksum");
        return ESP_ERR_INVALID_CRC;
    }
    
    ESP_LOGD(TAG, "Packet received: ID=%lu, CMD=0x%04X", packet->packet_id, packet->command);
    return ESP_OK;
}

// Send status update to server
esp_err_t p32_server_send_status(void) {
    p32_server_packet_t packet = {0};
    packet.command = P32_CMD_STATUS_REQUEST;
    packet.data_length = 0; // No data for status request
    
    return p32_server_send_packet(&packet);
}

// Send heartbeat to server
esp_err_t p32_server_send_heartbeat(void) {
    p32_server_packet_t packet = {0};
    packet.command = P32_CMD_PING;
    packet.data_length = 4;
    
    // Include system uptime in heartbeat data
    uint32_t uptime = (uint32_t)(esp_timer_get_time() / 1000000); // Convert to seconds
    memcpy(packet.data, &uptime, sizeof(uptime));
    
    return p32_server_send_packet(&packet);
}

#endif // ENABLE_SIMPLE_WIFI