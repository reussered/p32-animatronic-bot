// Lightweight GlobalSharedMemory - Optimized for Fast Reads
// 
// Architecture:
// - Ultra-fast read() operations (just memcpy from local RAM)
// - write() operations trigger ESP-NOW broadcast to sync all ESP32s
// - No change detection - write when you know data is new
// - Fixed-size arrays instead of STL containers for minimal footprint

#ifndef GLOBAL_SHARED_MEMORY_LITE_HPP
#define GLOBAL_SHARED_MEMORY_LITE_HPP

#include <stdint.h>
#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wifi_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Configuration
#define GSM_MAX_VARIABLES    32      // Maximum shared variables
#define GSM_MAX_NAME_LEN     24      // Variable name length
#define GSM_MAX_DATA_SIZE    128     // Maximum size per variable
#define GSM_MAX_PEERS        10      // Maximum mesh peers

// Error codes
#define GSM_SUCCESS           0x00000000
#define GSM_ERROR_NOT_FOUND   0x00000001
#define GSM_ERROR_TOO_LARGE   0x00000002
#define GSM_ERROR_NO_SPACE    0x00000003
#define GSM_ERROR_MESH_FAIL   0x00000004
#define GSM_ERROR_INVALID_ARG 0x00000005

// ESP-NOW message for variable updates
struct GSMMessage {
    uint8_t msg_type;                    // Always 0x01 for GSM
    char name[GSM_MAX_NAME_LEN];         // Variable name
    uint8_t data_size;                   // Size of data
    uint8_t data[GSM_MAX_DATA_SIZE];     // Variable data
    uint32_t sequence;                   // Message sequence number
};

// Internal storage for each variable
struct GSMVariable {
    char name[GSM_MAX_NAME_LEN];
    uint8_t data[GSM_MAX_DATA_SIZE];
    uint8_t size;
    bool used;
};

class GlobalSharedMemory {
private:
    // Local storage - optimized for fast reads
    GSMVariable variables[GSM_MAX_VARIABLES];
    uint8_t variable_count;
    
    // Mesh networking
    uint8_t peers[GSM_MAX_PEERS][6];
    uint8_t peer_count;
    uint32_t sequence_counter;
    bool esp_now_initialized;
    
    // Thread safety
    SemaphoreHandle_t mutex;
    
    // ESP-NOW callbacks (must be static)
    static void on_data_sent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status);
    static void on_data_received(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
    
    // Internal methods
    uint32_t init_esp_now();
    uint32_t broadcast_message(const GSMMessage* msg);
    void handle_received_message(const GSMMessage* msg);
    int find_variable_index(const char* name);
    
public:
    GlobalSharedMemory();
    ~GlobalSharedMemory();
    
    // Core template functions - ultra-fast reads, writes trigger mesh sync
    template<typename T>
    uint32_t read(const char* name, T* output);
    
    template<typename T>
    T* find(const char* name);
    
    template<typename T>
    uint32_t write(const char* name, const T* data);
    
    // Mesh management
    uint32_t add_peer(const uint8_t mac_addr[6]);
    uint32_t remove_peer(const uint8_t mac_addr[6]);
    uint8_t get_peer_count() const { return peer_count; }
    
    // Utility
    void list_variables() const;
    uint32_t get_variable_count() const { return variable_count; }
    size_t get_memory_usage() const;
};

// Template implementations (in header for performance)

template<typename T>
uint32_t GlobalSharedMemory::read(const char* name, T* output) {
    if (!name || !output || sizeof(T) > GSM_MAX_DATA_SIZE) {
        return GSM_ERROR_INVALID_ARG;
    }
    
    // Fast read - no mutex needed for reading
    int index = find_variable_index(name);
    if (index < 0) {
        return GSM_ERROR_NOT_FOUND;
    }
    
    if (variables[index].size != sizeof(T)) {
        return GSM_ERROR_TOO_LARGE;
    }
    
    // Ultra-fast memcpy - this is the critical path
    memcpy(output, variables[index].data, sizeof(T));
    return GSM_SUCCESS;
}

template<typename T>
T* GlobalSharedMemory::find(const char* name) {
    if (!name || sizeof(T) > GSM_MAX_DATA_SIZE) {
        return nullptr;
    }
    
    // Fast read - no mutex needed for reading
    int index = find_variable_index(name);
    if (index < 0 || variables[index].size != sizeof(T)) {
        return nullptr;
    }
    
    return reinterpret_cast<T*>(variables[index].data);
}

template<typename T>
uint32_t GlobalSharedMemory::write(const char* name, const T* data) {
    if (!name || !data || sizeof(T) > GSM_MAX_DATA_SIZE) {
        return GSM_ERROR_INVALID_ARG;
    }
    
    if (strlen(name) >= GSM_MAX_NAME_LEN) {
        return GSM_ERROR_INVALID_ARG;
    }
    
    // Take mutex for writing
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return GSM_ERROR_MESH_FAIL;
    }
    
    // Find existing variable or create new one
    int index = find_variable_index(name);
    if (index < 0) {
        // Create new variable
        if (variable_count >= GSM_MAX_VARIABLES) {
            xSemaphoreGive(mutex);
            return GSM_ERROR_NO_SPACE;
        }
        
        index = variable_count++;
        strncpy(variables[index].name, name, GSM_MAX_NAME_LEN - 1);
        variables[index].name[GSM_MAX_NAME_LEN - 1] = '\0';
        variables[index].used = true;
    }
    
    // Update local data
    variables[index].size = sizeof(T);
    memcpy(variables[index].data, data, sizeof(T));
    
    // Prepare mesh message
    GSMMessage msg;
    msg.msg_type = 0x01;
    strncpy(msg.name, name, GSM_MAX_NAME_LEN - 1);
    msg.name[GSM_MAX_NAME_LEN - 1] = '\0';
    msg.data_size = sizeof(T);
    memcpy(msg.data, data, sizeof(T));
    msg.sequence = ++sequence_counter;
    
    xSemaphoreGive(mutex);
    
    // Broadcast to mesh (this is the slow part)
    return broadcast_message(&msg);
}

#endif // GLOBAL_SHARED_MEMORY_LITE_HPP