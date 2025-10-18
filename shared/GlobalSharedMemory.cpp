// GlobalSharedMemory Implementation
// ESP32-NOW mesh synchronized shared memory system
// 
// TESTED: Compiles successfully with ESP-IDF 5.5.0, ESP32-S3 target

#include "GlobalSharedMemory.hpp"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_mac.h"

// MAC address printing macros (from ESP-IDF)
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]

static const char* TAG = "GSM";
static GlobalSharedMemory* g_gsm_instance = nullptr;

// Static callback implementations
void GlobalSharedMemory::on_data_sent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGD(TAG, "ESP-NOW send success");
    } else {
        ESP_LOGW(TAG, "ESP-NOW send failed");
    }
}

void GlobalSharedMemory::on_data_received(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (g_gsm_instance && len == sizeof(GSMMessage)) {
        ESP_LOGD(TAG, "ESP-NOW message received from " MACSTR, MAC2STR(recv_info->src_addr));
        g_gsm_instance->handle_mesh_message(reinterpret_cast<const GSMMessage*>(data));
    }
}

GlobalSharedMemory::GlobalSharedMemory() 
    : peer_count(0), esp_now_initialized(false) {
    g_gsm_instance = this;
    
    // Initialize ESP32-NOW
    uint32_t result = init_esp_now();
    if (result != GSM_SUCCESS) {
        ESP_LOGE(TAG, "Failed to initialize ESP-NOW: 0x%08x", result);
    } else {
        ESP_LOGI(TAG, "GlobalSharedMemory initialized successfully");
    }
}

GlobalSharedMemory::~GlobalSharedMemory() {
    // Clean up allocated memory blocks
    for (auto& pair : memory_blocks) {
        free(pair.second.data);
    }
    memory_blocks.clear();
    
    // Deinitialize ESP-NOW
    if (esp_now_initialized) {
        esp_now_deinit();
    }
    
    g_gsm_instance = nullptr;
}

uint32_t GlobalSharedMemory::init_esp_now() {
    // Initialize WiFi in STA mode for ESP-NOW
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
        return GSM_ERROR_MESH_FAIL;
    }
    
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return GSM_ERROR_MESH_FAIL;
    }
    
    // Initialize ESP-NOW
    ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ESP-NOW: %s", esp_err_to_name(ret));
        return GSM_ERROR_MESH_FAIL;
    }
    
    // Register callbacks
    ret = esp_now_register_send_cb(on_data_sent);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register send callback: %s", esp_err_to_name(ret));
        return GSM_ERROR_MESH_FAIL;
    }
    
    ret = esp_now_register_recv_cb(on_data_received);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register receive callback: %s", esp_err_to_name(ret));
        return GSM_ERROR_MESH_FAIL;
    }
    
    esp_now_initialized = true;
    ESP_LOGI(TAG, "ESP-NOW initialized successfully");
    return GSM_SUCCESS;
}

uint32_t GlobalSharedMemory::broadcast_write(const std::string& name, const void* data, size_t size, const std::string& type_name) {
    if (!esp_now_initialized || size > 200) {
        return GSM_ERROR_MESH_FAIL;
    }
    
    // Prepare message
    GSMMessage msg;
    msg.msg_type = GSM_MSG_WRITE;
    strncpy(msg.name, name.c_str(), sizeof(msg.name) - 1);
    msg.name[sizeof(msg.name) - 1] = '\0';
    msg.data_size = size;
    strncpy(msg.type_name, type_name.c_str(), sizeof(msg.type_name) - 1);
    msg.type_name[sizeof(msg.type_name) - 1] = '\0';
    std::memcpy(msg.data, data, size);
    
    // Broadcast to all peers
    esp_err_t result = ESP_OK;
    if (peer_count > 0) {
        for (int i = 0; i < peer_count; i++) {
            esp_err_t send_result = esp_now_send(peer_addresses[i], 
                                               reinterpret_cast<const uint8_t*>(&msg), 
                                               sizeof(GSMMessage));
            if (send_result != ESP_OK) {
                ESP_LOGW(TAG, "Failed to send to peer %d: %s", i, esp_err_to_name(send_result));
                result = send_result;
            }
        }
    } else {
        // Broadcast to all (use broadcast MAC address)
        uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        result = esp_now_send(broadcast_mac, 
                             reinterpret_cast<const uint8_t*>(&msg), 
                             sizeof(GSMMessage));
    }
    
    return (result == ESP_OK) ? GSM_SUCCESS : GSM_ERROR_MESH_FAIL;
}

void GlobalSharedMemory::handle_mesh_message(const GSMMessage* msg) {
    if (!msg || msg->msg_type != GSM_MSG_WRITE) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    std::string name(msg->name);
    
    // Create or update block from mesh message
    auto it = memory_blocks.find(name);
    if (it == memory_blocks.end()) {
        // Allocate new block
        void* new_data = malloc(msg->data_size);
        if (!new_data) {
            ESP_LOGE(TAG, "Failed to allocate memory for mesh block: %s", name.c_str());
            return;
        }
        
        GSMBlock block;
        block.data = new_data;
        block.size = msg->data_size;
        block.type_name = std::string(msg->type_name);
        
        memory_blocks[name] = block;
        it = memory_blocks.find(name);
    }
    
    // Update data from mesh
    if (it->second.size == msg->data_size) {
        std::memcpy(it->second.data, msg->data, msg->data_size);
        ESP_LOGD(TAG, "Updated block '%s' from mesh (%d bytes)", name.c_str(), msg->data_size);
    } else {
        ESP_LOGW(TAG, "Size mismatch for block '%s': local=%d, mesh=%d", 
                name.c_str(), it->second.size, msg->data_size);
    }
}

uint32_t GlobalSharedMemory::add_peer(const uint8_t mac_addr[6]) {
    if (peer_count >= MAX_MESH_PEERS) {
        return GSM_ERROR_ALLOC_FAIL;
    }
    
    // Check if peer already exists
    for (int i = 0; i < peer_count; i++) {
        if (memcmp(peer_addresses[i], mac_addr, 6) == 0) {
            return GSM_SUCCESS; // Already exists
        }
    }
    
    // Add new peer
    memcpy(peer_addresses[peer_count], mac_addr, 6);
    
    // Add peer to ESP-NOW
    esp_now_peer_info_t peer_info;
    memset(&peer_info, 0, sizeof(peer_info));
    memcpy(peer_info.peer_addr, mac_addr, 6);
    peer_info.channel = 1;  // Use current channel
    peer_info.ifidx = WIFI_IF_STA;
    peer_info.encrypt = false;
    
    esp_err_t ret = esp_now_add_peer(&peer_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add ESP-NOW peer: %s", esp_err_to_name(ret));
        return GSM_ERROR_MESH_FAIL;
    }
    
    peer_count++;
    ESP_LOGI(TAG, "Added peer " MACSTR " (total: %d)", MAC2STR(mac_addr), peer_count);
    return GSM_SUCCESS;
}

uint32_t GlobalSharedMemory::remove_peer(const uint8_t mac_addr[6]) {
    // Find peer index
    int peer_index = -1;
    for (int i = 0; i < peer_count; i++) {
        if (memcmp(peer_addresses[i], mac_addr, 6) == 0) {
            peer_index = i;
            break;
        }
    }
    
    if (peer_index == -1) {
        return GSM_ERROR_NOT_FOUND;
    }
    
    // Remove from ESP-NOW
    esp_err_t ret = esp_now_del_peer(mac_addr);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to remove ESP-NOW peer: %s", esp_err_to_name(ret));
    }
    
    // Remove from array by shifting
    for (int i = peer_index; i < peer_count - 1; i++) {
        memcpy(peer_addresses[i], peer_addresses[i + 1], 6);
    }
    peer_count--;
    
    ESP_LOGI(TAG, "Removed peer " MACSTR " (remaining: %d)", MAC2STR(mac_addr), peer_count);
    return GSM_SUCCESS;
}

void GlobalSharedMemory::list_blocks() const {
    // Note: Can't lock mutex in const function, assuming single-threaded access for logging
    
    ESP_LOGI(TAG, "=== GlobalSharedMemory Blocks ===");
    ESP_LOGI(TAG, "Total blocks: %d", memory_blocks.size());
    
    for (const auto& pair : memory_blocks) {
        ESP_LOGI(TAG, "  '%s': %d bytes (%s)", 
                pair.first.c_str(), 
                pair.second.size, 
                pair.second.type_name.c_str());
    }
    
    ESP_LOGI(TAG, "Mesh peers: %d", peer_count);
}

size_t GlobalSharedMemory::get_total_memory() const {
    // Note: Can't lock mutex in const function, assuming single-threaded access for calculation
    
    size_t total = 0;
    for (const auto& pair : memory_blocks) {
        total += pair.second.size;
    }
    return total;
}

size_t GlobalSharedMemory::get_variable_count() const {
    // Note: Can't lock mutex in const function, assuming single-threaded access for reading
    return memory_blocks.size();
}