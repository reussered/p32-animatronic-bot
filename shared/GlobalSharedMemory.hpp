// GlobalSharedMemory - Template-based ESP32-NOW Mesh Synchronized Memory
// 
// Provides type-safe shared memory access across multiple ESP32 cores
// with automatic ESP32-NOW mesh synchronization for distributed processing.
// 
// TESTED: Compiles successfully with ESP-IDF 5.5.0, ESP32-S3 target
//
// Usage:
//   GlobalSharedMemory gsm;
//   gsm.write<int>("counter", &value);
//   int* ptr = gsm.find<int>("counter");
//   uint32_t result = gsm.read<int>("counter", &output);

#ifndef GLOBAL_SHARED_MEMORY_HPP
#define GLOBAL_SHARED_MEMORY_HPP

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wifi_types.h"
#include <cstring>

// Error codes for GSM operations
#define GSM_SUCCESS           0x00000000
#define GSM_ERROR_NOT_FOUND   0x00000001
#define GSM_ERROR_TYPE_SIZE   0x00000002
#define GSM_ERROR_ALLOC_FAIL  0x00000003
#define GSM_ERROR_MESH_FAIL   0x00000004
#define GSM_ERROR_INVALID_ARG 0x00000005

// Maximum peers in ESP32-NOW mesh
#define MAX_MESH_PEERS 20

// ESP32-NOW message types
#define GSM_MSG_WRITE  0x01
#define GSM_MSG_SYNC   0x02

struct GSMBlock {
    void* data;
    size_t size;
    std::string type_name;
};

struct GSMMessage {
    uint8_t msg_type;
    char name[32];
    size_t data_size;
    char type_name[32];
    uint8_t data[200];  // ESP-NOW max payload ~250 bytes
};

class GlobalSharedMemory {
private:
    std::map<std::string, GSMBlock> memory_blocks;
    std::mutex memory_mutex;
    uint8_t peer_addresses[MAX_MESH_PEERS][6];
    int peer_count;
    bool esp_now_initialized;
    
    // ESP32-NOW callback functions
    static void on_data_sent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status);
    static void on_data_received(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
    
    // Internal helper functions
    uint32_t init_esp_now();
    uint32_t broadcast_write(const std::string& name, const void* data, size_t size, const std::string& type_name);
    void handle_mesh_message(const GSMMessage* msg);
    
public:
    GlobalSharedMemory();
    ~GlobalSharedMemory();
    
    // Template member functions as specified
    template<typename T>
    uint32_t read(const std::string& name, T* output);
    
    template<typename T>
    T* find(const std::string& name);
    
    template<typename T>
    uint32_t write(const std::string& name, T* data);
    
    // Mesh management
    uint32_t add_peer(const uint8_t mac_addr[6]);
    uint32_t remove_peer(const uint8_t mac_addr[6]);
    int get_peer_count() const { return peer_count; }
    
    // Utility functions
    void list_blocks() const;
    size_t get_total_memory() const;
    size_t get_variable_count() const;
};

// Template implementations (must be in header for templates)
template<typename T>
uint32_t GlobalSharedMemory::read(const std::string& name, T* output) {
    if (!output) {
        return GSM_ERROR_INVALID_ARG;
    }
    
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    auto it = memory_blocks.find(name);
    if (it == memory_blocks.end()) {
        return GSM_ERROR_NOT_FOUND;
    }
    
    if (it->second.size != sizeof(T)) {
        return GSM_ERROR_TYPE_SIZE;
    }
    
    std::memcpy(output, it->second.data, sizeof(T));
    return GSM_SUCCESS;
}

template<typename T>
T* GlobalSharedMemory::find(const std::string& name) {
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    auto it = memory_blocks.find(name);
    if (it == memory_blocks.end()) {
        return nullptr;
    }
    
    if (it->second.size != sizeof(T)) {
        return nullptr;
    }
    
    return static_cast<T*>(it->second.data);
}

template<typename T>
uint32_t GlobalSharedMemory::write(const std::string& name, T* data) {
    if (!data) {
        return GSM_ERROR_INVALID_ARG;
    }
    
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    // Create or update local block
    auto it = memory_blocks.find(name);
    if (it == memory_blocks.end()) {
        // Allocate new block
        void* new_data = malloc(sizeof(T));
        if (!new_data) {
            return GSM_ERROR_ALLOC_FAIL;
        }
        
        GSMBlock block;
        block.data = new_data;
        block.size = sizeof(T);
        block.type_name = "unknown_type";  // RTTI disabled in ESP-IDF
        
        memory_blocks[name] = block;
        it = memory_blocks.find(name);
    }
    
    // Copy data to local block
    std::memcpy(it->second.data, data, sizeof(T));
    
    // Broadcast to mesh network
    uint32_t mesh_result = broadcast_write(name, data, sizeof(T), "unknown_type");
    if (mesh_result != GSM_SUCCESS) {
        return mesh_result;
    }
    
    return GSM_SUCCESS;
}

#endif // GLOBAL_SHARED_MEMORY_HPP