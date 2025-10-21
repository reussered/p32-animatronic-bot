#include "core/SharedMemory.hpp"
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <cstring>

static const char* TAG = "SharedMemory";

// ESP-NOW peer address (example, should be set up per network)
static uint8_t peer_addr[ESP_NOW_ETH_ALEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Internal callback for data sent
void SharedMemory::on_data_sent(const uint8_t* mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Data sent to %02x:%02x:%02x:%02x:%02x:%02x, status: %d", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], status);
}

// Internal callback for data received
void SharedMemory::on_data_recv(const uint8_t* mac_addr, const uint8_t* data, int len) {
    ESP_LOGI(TAG, "Data received from %02x:%02x:%02x:%02x:%02x:%02x, len: %d", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], len);
    // Optionally: parse and update memory_map
}

void SharedMemory::espnow_init() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config_default()));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb([](const uint8_t* mac_addr, esp_now_send_status_t status) {
        SharedMemory::on_data_sent(mac_addr, status);
    }));
    ESP_ERROR_CHECK(esp_now_register_recv_cb([](const uint8_t* mac_addr, const uint8_t* data, int len) {
        SharedMemory::on_data_recv(mac_addr, data, len);
    }));
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peer_addr, ESP_NOW_ETH_ALEN);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));
}

void SharedMemory::espnow_broadcast(const std::string& name, void* data, size_t size) {
    // Send data to all peers
    esp_err_t result = esp_now_send(peer_addr, reinterpret_cast<uint8_t*>(data), size);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "esp_now_send failed: %d", result);
    }
}
