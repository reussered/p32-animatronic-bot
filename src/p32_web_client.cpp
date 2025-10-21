// P32 Web Client Implementation - ESP32 sends to PC server
#include "p32_web_client.hpp"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>
#include <inttypes.h>

// Forward declarations for static functions
static esp_err_t web_client_send_registration(const device_registration_t* reg);
static esp_err_t web_client_send_http_post(const animation_packet_t* packet);

static const char* TAG = "P32_WEB_CLIENT";
static web_client_config_t client_config = {0};

// WiFi event handler
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        client_config.connected = false;
        ESP_LOGI(TAG, "WiFi disconnected, retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        client_config.connected = true;
    }
}

esp_err_t web_client_init(void) {
    // Simple init with default configuration for SIMPLE_TEST mode
    return web_client_init_full("P32_GOBLIN_EYE_LEFT", "EYE", 
                               240, 240,
                               "127.0.0.1", 3000, 
                               "YourWiFi", "YourPassword");
}

esp_err_t web_client_init_full(const char* device_id, const char* display_type, 
                              uint16_t width, uint16_t height,
                              const char* pc_ip, uint16_t pc_port, 
                              const char* ssid, const char* password) {
    // Store configuration
    strncpy(client_config.device_id, device_id, sizeof(client_config.device_id) - 1);
    strncpy(client_config.display_type, display_type, sizeof(client_config.display_type) - 1);
    client_config.display_width = width;
    client_config.display_height = height;
    strncpy(client_config.pc_server_ip, pc_ip, sizeof(client_config.pc_server_ip) - 1);
    client_config.pc_server_port = pc_port;
    strncpy(client_config.wifi_ssid, ssid, sizeof(client_config.wifi_ssid) - 1);
    strncpy(client_config.wifi_password, password, sizeof(client_config.wifi_password) - 1);
    client_config.send_interval_ms = 100; // 10 FPS animation updates
    client_config.connected = false;
    client_config.registered = false;
    
    ESP_LOGI(TAG, "Web client initialized - Device: %s (%s %dx%d) -> %s:%d", 
             device_id, display_type, width, height, pc_ip, pc_port);
    
    // Print PC server setup code
    web_client_print_pc_server_code();
    
    return ESP_OK;
}

esp_err_t web_client_connect_wifi(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    
    wifi_config_t wifi_config = {0};
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    
    strcpy((char*)wifi_config.sta.ssid, client_config.wifi_ssid);
    strcpy((char*)wifi_config.sta.password, client_config.wifi_password);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "WiFi connecting to %s...", client_config.wifi_ssid);
    return ESP_OK;
}

esp_err_t web_client_register_component(const char* component_id, const char* display_type, 
                                       uint16_t width, uint16_t height,
                                       const char* bot_name, const char* component_name) {
    if (!client_config.connected) {
        return ESP_ERR_WIFI_NOT_CONNECT;
    }
    
    // Get ESP32 MAC address for unique identification
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    
    // Create registration packet for this component
    device_registration_t reg = {0};
    strncpy(reg.device_id, component_id, sizeof(reg.device_id) - 1);  // Use component-specific ID
    strncpy(reg.display_type, display_type, sizeof(reg.display_type) - 1);
    reg.display_width = width;
    reg.display_height = height;
    strncpy(reg.bot_name, bot_name, sizeof(reg.bot_name) - 1);
    strncpy(reg.component_name, component_name, sizeof(reg.component_name) - 1);
    reg.esp32_mac[0] = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | mac[3];
    reg.esp32_mac[1] = (mac[4] << 8) | mac[5];
    
    // Send registration for this specific component
    esp_err_t ret = web_client_send_registration(&reg);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Component registered: %s (%s %dx%d)", component_id, display_type, width, height);
    }
    
    return ret;
}

esp_err_t web_client_register_device(const char* bot_name, const char* component_name) {
    // Legacy function - registers the main device
    return web_client_register_component(client_config.device_id, client_config.display_type,
                                       client_config.display_width, client_config.display_height,
                                       bot_name, component_name);
}

esp_err_t web_client_send_animation_data(const eye_display_t* display, uint32_t loop_count) {
    // Legacy function using main device ID - no rate limiting
    return web_client_send_animation_data_for_component(client_config.device_id, display);
}

esp_err_t web_client_send_animation_data_for_component(const char* component_id, const eye_display_t* display) {
    if (!client_config.connected) {
        return ESP_ERR_WIFI_NOT_CONNECT;
    }
    
    // No rate limiting - send immediately when component act() function calls
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Create animation data packet for specific component
    animation_packet_t packet = {0};
    strncpy(packet.device_id, component_id, sizeof(packet.device_id) - 1);
    packet.timestamp_ms = now;
    packet.loop_count = 0; // Not used for animation data
    strncpy(packet.animation_name, display->current_animation ? display->current_animation->name : "none", sizeof(packet.animation_name) - 1);
    packet.eye_openness = display->current_frame.eye_openness;
    packet.pupil_size = display->current_frame.pupil_size;
    packet.color = display->current_frame.color;
    packet.expression = display->current_frame.expression;
    packet.animation_active = display->active;
    
    return web_client_send_http_post(&packet);
}

esp_err_t web_client_send_custom_data_for_component(const char* component_id, const char* custom_data, uint32_t loop_count) {
    if (!client_config.connected) {
        return ESP_ERR_WIFI_NOT_CONNECT;
    }
    
    // No rate limiting - send immediately when component act() function calls
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Create custom data packet for specific component
    animation_packet_t packet = {0};
    strncpy(packet.device_id, component_id, sizeof(packet.device_id) - 1);
    packet.timestamp_ms = now;
    packet.loop_count = loop_count;
    strncpy(packet.custom_data, custom_data, sizeof(packet.custom_data) - 1);
    
    return web_client_send_http_post(&packet);
}

esp_err_t web_client_send_sensor_data_for_component(const char* component_id, float sensor_value, uint32_t loop_count) {
    if (!client_config.connected) {
        return ESP_ERR_WIFI_NOT_CONNECT;
    }
    
    // No rate limiting - send immediately when component act() function calls
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
    // Create sensor data packet using custom_data field
    animation_packet_t packet = {0};
    strncpy(packet.device_id, component_id, sizeof(packet.device_id) - 1);
    packet.timestamp_ms = now;
    packet.loop_count = loop_count;
    
    // Format sensor data into custom_data field
    snprintf(packet.custom_data, sizeof(packet.custom_data), 
             "sensor_type:distance,value:%.2f,unit:cm", sensor_value);
    
    return web_client_send_http_post(&packet);
}

esp_err_t web_client_send_custom_data(const char* custom_data, uint32_t loop_count) {
    // Legacy function using main device ID
    return web_client_send_custom_data_for_component(client_config.device_id, custom_data, loop_count);
}

static esp_err_t web_client_send_registration(const device_registration_t* reg) {
    // Create JSON payload
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "device_id", reg->device_id);
    cJSON_AddStringToObject(json, "display_type", reg->display_type);
    cJSON_AddNumberToObject(json, "display_width", reg->display_width);
    cJSON_AddNumberToObject(json, "display_height", reg->display_height);
    cJSON_AddStringToObject(json, "bot_name", reg->bot_name);
    cJSON_AddStringToObject(json, "component_name", reg->component_name);
    
    // Add MAC address as hex string
    char mac_str[32];
    snprintf(mac_str, sizeof(mac_str), "%08" PRIx32 "%08" PRIx32, reg->esp32_mac[0], reg->esp32_mac[1]);
    cJSON_AddStringToObject(json, "mac_address", mac_str);
    
    char* json_string = cJSON_Print(json);
    cJSON_Delete(json);
    
    // Build registration URL
    char url[128];
    snprintf(url, sizeof(url), "http://%s:%d/api/register-device", 
             client_config.pc_server_ip, client_config.pc_server_port);
    
    // Configure HTTP client
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 3000, // Longer timeout for registration
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_string, strlen(json_string));
    
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code != 200) {
            ESP_LOGW(TAG, "Device registration failed, status: %d", status_code);
            err = ESP_FAIL;
        }
    } else {
        ESP_LOGW(TAG, "Registration HTTP error: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    free(json_string);
    
    return err;
}

static esp_err_t web_client_send_http_post(const animation_packet_t* packet) {
    // Create JSON payload
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "device_id", packet->device_id);
    cJSON_AddNumberToObject(json, "timestamp_ms", packet->timestamp_ms);
    cJSON_AddNumberToObject(json, "loop_count", packet->loop_count);
    cJSON_AddStringToObject(json, "animation_name", packet->animation_name);
    cJSON_AddNumberToObject(json, "eye_openness", packet->eye_openness);
    cJSON_AddNumberToObject(json, "pupil_size", packet->pupil_size);
    cJSON_AddNumberToObject(json, "color", packet->color);
    cJSON_AddNumberToObject(json, "expression", packet->expression);
    cJSON_AddBoolToObject(json, "animation_active", packet->animation_active);
    cJSON_AddStringToObject(json, "custom_data", packet->custom_data);
    
    char* json_string = cJSON_Print(json);
    cJSON_Delete(json);
    
    // Build animation data URL
    char url[128];
    snprintf(url, sizeof(url), "http://%s:%d/api/animation-data", 
             client_config.pc_server_ip, client_config.pc_server_port);
    
    // Configure HTTP client
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 1000,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_string, strlen(json_string));
    
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code != 200) {
            ESP_LOGW(TAG, "HTTP POST failed, status: %d", status_code);
            err = ESP_FAIL;
        }
    } else {
        ESP_LOGW(TAG, "HTTP POST error: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    free(json_string);
    
    return err;
}

bool web_client_is_connected(void) {
    return client_config.connected;
}

bool web_client_is_registered(void) {
    return client_config.registered;
}

void web_client_disconnect(void) {
    if (client_config.connected) {
        esp_wifi_disconnect();
        client_config.connected = false;
        client_config.registered = false;
    }
}

void web_client_print_pc_server_code(void) {
    ESP_LOGI(TAG, "=== PC SERVER SETUP CODE ===");
    ESP_LOGI(TAG, "Create this Node.js server on your PC:");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "// server.js - Run with: npm init -y && npm install express");
    ESP_LOGI(TAG, "// Then run: node server.js");
    ESP_LOGI(TAG, "const express = require('express');");
    ESP_LOGI(TAG, "const path = require('path');");
    ESP_LOGI(TAG, "const app = express();");
    ESP_LOGI(TAG, "const PORT = %d;", client_config.pc_server_port);
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "app.use(express.json());");
    ESP_LOGI(TAG, "app.use(express.static('public'));");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "// Store registered components and their data");
    ESP_LOGI(TAG, "let registeredComponents = {};");
    ESP_LOGI(TAG, "let componentData = {};");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "// Register ESP32 components (creates display windows)");
    ESP_LOGI(TAG, "app.post('/api/register-device', (req, res) => {");
    ESP_LOGI(TAG, "  const reg = req.body;");
    ESP_LOGI(TAG, "  registeredComponents[reg.device_id] = reg;");
    ESP_LOGI(TAG, "  console.log(`Registered: ${reg.device_id} (${reg.display_type} ${reg.display_width}x${reg.display_height})`);");
    ESP_LOGI(TAG, "  res.json({status: 'registered', component_id: reg.device_id});");
    ESP_LOGI(TAG, "});");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "// Receive ESP32 animation data");
    ESP_LOGI(TAG, "app.post('/api/animation-data', (req, res) => {");
    ESP_LOGI(TAG, "  const data = req.body;");
    ESP_LOGI(TAG, "  componentData[data.device_id] = data;");
    ESP_LOGI(TAG, "  res.json({status: 'received'});");
    ESP_LOGI(TAG, "});");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "// Serve component list and data to web page");
    ESP_LOGI(TAG, "app.get('/api/components', (req, res) => {");
    ESP_LOGI(TAG, "  res.json({");
    ESP_LOGI(TAG, "    registered: registeredComponents,");
    ESP_LOGI(TAG, "    data: componentData");
    ESP_LOGI(TAG, "  });");
    ESP_LOGI(TAG, "});");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "app.listen(PORT, () => {");
    ESP_LOGI(TAG, "  console.log(`ESP32 Multi-Component Server running on port ${PORT}`);");
    ESP_LOGI(TAG, "  console.log(`Open http://localhost:${PORT} to see component windows`);");
    ESP_LOGI(TAG, "});");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "=== Create public/index.html for multi-window display ===");
}