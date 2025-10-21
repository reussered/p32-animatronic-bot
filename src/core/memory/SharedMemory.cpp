#include "core/memory/SharedMemory.hpp"
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <cstring>

static const char* TAG = "SharedMemory";

// ESP-NOW peer address (broadcast to all)
static uint8_t peer_addr[ESP_NOW_ETH_ALEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Static instance pointer for callback access
SharedMemory* SharedMemory::instance = nullptr;

// Internal callback for data sent
void SharedMemory::on_data_sent(const uint8_t* mac_addr, esp_now_send_status_t status) 
{
    if (status == ESP_NOW_SEND_SUCCESS) 
    {
        ESP_LOGD(TAG, "Data sent successfully to %02x:%02x:%02x:%02x:%02x:%02x", 
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    } 
    else 
    {
        ESP_LOGW(TAG, "Failed to send data to %02x:%02x:%02x:%02x:%02x:%02x, status: %d", 
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], status);
    }
}

// Internal callback for data received
void SharedMemory::on_data_recv(const uint8_t* mac_addr, const uint8_t* data, int len) 
{
    ESP_LOGD(TAG, "Data received from %02x:%02x:%02x:%02x:%02x:%02x, len: %d", 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], len);
    
    if (!instance || len < 2) 
    {
        ESP_LOGW(TAG, "Invalid data received or no instance available");
        return;
    }
    
    // Extract null-terminated string name
    const char* name_str = reinterpret_cast<const char*>(data);
    size_t name_len = strnlen(name_str, len);
    
    if (name_len >= len - 1) 
    {
        ESP_LOGW(TAG, "Invalid packet format: missing null terminator or no data");
        return;
    }
    
    std::string name(name_str);
    const uint8_t* payload = data + name_len + 1;  // Skip past null terminator
    size_t payload_size = len - name_len - 1;
    
    ESP_LOGD(TAG, "Received update for '%s', payload size: %zu", name.c_str(), payload_size);
    
    // Update memory from network data
    instance->update_memory_from_network(name, payload, payload_size);
}

void SharedMemory::espnow_init() 
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_now_init());
    
    ESP_ERROR_CHECK(esp_now_register_send_cb([](const esp_now_send_info_t* send_info, esp_now_send_status_t status) {
        SharedMemory::on_data_sent(send_info->des_addr, status);
    }));
    ESP_ERROR_CHECK(esp_now_register_recv_cb([](const esp_now_recv_info_t* recv_info, const uint8_t* data, int len) {
        SharedMemory::on_data_recv(recv_info->src_addr, data, len);
    }));
    
    // Add broadcast peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peer_addr, ESP_NOW_ETH_ALEN);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));
    
    ESP_LOGI(TAG, "ESP-NOW initialized successfully");
}

void SharedMemory::espnow_broadcast(const std::string& name, void* data, size_t size) 
{
    // Create packet: null-terminated name + data
    size_t packet_size = name.length() + 1 + size;  // name + null + data
    uint8_t* packet = new uint8_t[packet_size];
    
    if (!packet) 
    {
        ESP_LOGE(TAG, "Failed to allocate packet buffer");
        return;
    }
    
    // Copy name and null terminator
    memcpy(packet, name.c_str(), name.length() + 1);
    
    // Copy data immediately after null terminator
    memcpy(packet + name.length() + 1, data, size);
    
    ESP_LOGD(TAG, "Broadcasting '%s', data size: %zu, total packet: %zu", 
            name.c_str(), size, packet_size);
    
    // Send to all peers (broadcast)
    esp_err_t result = esp_now_send(peer_addr, packet, packet_size);
    if (result != ESP_OK) 
    {
        ESP_LOGE(TAG, "esp_now_send failed for '%s': %d", name.c_str(), result);
    }
    
    delete[] packet;
}

void SharedMemory::update_memory_from_network(const std::string& name, const uint8_t* data, size_t size) 
{
    auto it = memory_map.find(name);
    
    if (it != memory_map.end()) 
    {
        // Entry exists, update in place
        memcpy(it->second, data, size);
        ESP_LOGD(TAG, "Updated existing entry '%s' with %zu bytes", name.c_str(), size);
    } 
    else 
    {
        // Entry doesn't exist, create new one
        uint8_t* new_mem = new uint8_t[size];
        if (new_mem) 
        {
            memcpy(new_mem, data, size);
            memory_map[name] = new_mem;
            ESP_LOGD(TAG, "Created new entry '%s' with %zu bytes", name.c_str(), size);
        } 
        else 
        {
            ESP_LOGE(TAG, "Failed to allocate memory for '%s'", name.c_str());
        }
    }
}
