#include "core/memory/SharedMemory.hpp"
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <cstring>

static const char* TAG = "SharedMemory";

// ESP-NOW peer address (broadcast to all)
static uint8_t peer_addr[ESP_NOW_ETH_ALEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Static instance pointer for singleton
SharedMemory* SharedMemory::instance = nullptr;
bool SharedMemory::esp_now_initialized = false;

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
    
    if (!instance || len < 1) 
    {
        ESP_LOGW(TAG, "Invalid data received or no instance available");
        return;
    }
    
    // Extract type_id from first byte
    shared_type_id_t type_id = data[0];
    const uint8_t* payload = &data[1];  // Data starts after type_id
    size_t payload_size = len - 1;
    
    ESP_LOGD(TAG, "Received update for type_id %u, payload size: %zu", type_id, payload_size);
    
    // Update memory from network data
    instance->update_memory_from_network(type_id, payload, payload_size);
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
    
    esp_now_initialized = true;
    ESP_LOGI(TAG, "ESP-NOW initialized successfully");
}

void SharedMemory::espnow_broadcast(shared_type_id_t type_id, void* data, size_t size) 
{
    if (!esp_now_initialized) 
    {
        ESP_LOGD(TAG, "ESP-NOW not initialized, skipping broadcast for type_id %u", type_id);
        return;
    }
    // Create packet: type_id + data
    size_t packet_size = sizeof(shared_type_id_t) + size;
    uint8_t* packet = new uint8_t[packet_size];
    
    if (!packet) 
    {
        ESP_LOGE(TAG, "Failed to allocate packet buffer");
        return;
    }
    
    // Copy type_id and data
    packet[0] = type_id;
    if (size > 0 && data) 
    {
        memcpy(&packet[1], data, size);
    }
    
    ESP_LOGD(TAG, "Broadcasting type_id %u, data size: %zu, total packet: %zu", 
            type_id, size, packet_size);
    
    // Send to all peers (broadcast)
    esp_err_t result = esp_now_send(peer_addr, packet, packet_size);
    if (result != ESP_OK) 
    {
        ESP_LOGE(TAG, "esp_now_send failed for type_id %u: %d", type_id, result);
    }
    
    delete[] packet;
}

void SharedMemory::update_memory_from_network(shared_type_id_t type_id, const uint8_t* data, size_t size) 
{
    auto it = memory_map.find(type_id);
    
    if (it != memory_map.end()) 
    {
        // Entry exists, update in place
        memcpy(it->second, data, size);
        ESP_LOGD(TAG, "Updated existing entry type_id %u with %zu bytes", type_id, size);
    } 
    else 
    {
        // Entry doesn't exist, create new one
        uint8_t* new_mem = new uint8_t[size];
        if (new_mem) 
        {
            memcpy(new_mem, data, size);
            memory_map[type_id] = new_mem;
            ESP_LOGD(TAG, "Created new entry type_id %u with %zu bytes", type_id, size);
        } 
        else 
        {
            ESP_LOGE(TAG, "Failed to allocate memory for type_id %u", type_id);
        }
    }
}
