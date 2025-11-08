#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <string>
#include <map>
#include <cstring>
#include <memory>
#include <algorithm>
#include <cctype>

// Include shared state types
#include "Mood.hpp"
#include "Personality.hpp"
#include "../shared/Environment.hpp"
#include "../shared/MicrophoneData.hpp"

// Type ID definition - using int for type IDs
typedef int shared_type_id_t;

// Template function to get type ID for each class
// Each class must specialize this template
template<typename T>
shared_type_id_t getTypeId();

// Template specializations for known types
template<>
inline shared_type_id_t getTypeId<Environment>() {
    return 1;
}

template<>
inline shared_type_id_t getTypeId<Mood>() {
    return 2;
}

template<>
inline shared_type_id_t getTypeId<MicrophoneData>() {
    return 3;
}

template<>
inline shared_type_id_t getTypeId<Personality>() {
    return 4;
}

#ifdef ESP_PLATFORM
#ifdef CONFIG_ESP_WIFI_ESPNOW
#include <esp_now.h>
#endif
//#include <esp_wifi.h>
#include <esp_log.h>
#endif

class SharedMemory {
private:
    std::map<shared_type_id_t, void*> memory_map;
    static SharedMemory* instance;  // Singleton instance
#if defined(ESP_PLATFORM) && defined(CONFIG_ESP_WIFI_ESPNOW)
    static bool esp_now_initialized;  // Track ESP-NOW initialization
    
    static void on_data_sent(const uint8_t* mac_addr, esp_now_send_status_t status);
    static void on_data_recv(const uint8_t* mac_addr, const uint8_t* data, int len);
    void espnow_broadcast(shared_type_id_t type_id, void* data, size_t size);
#endif

    // Private constructor for singleton
    SharedMemory() 
    {
    }
    
    ~SharedMemory() 
	{
        // Free all allocated memory
        for (auto& entry : memory_map) 
		{
            delete[] static_cast<uint8_t*>(entry.second);  // Use delete[] for raw bytes
        }
    }

public:
    static SharedMemory* getInstance() {
        if (!instance) {
            instance = new SharedMemory();
        }
        return instance;
    }
    
    void init() {
#if defined(ESP_PLATFORM) && defined(CONFIG_ESP_WIFI_ESPNOW)
        espnow_init();
#endif
    }
    
#if defined(ESP_PLATFORM) && defined(CONFIG_ESP_WIFI_ESPNOW)
    void espnow_init();
#endif

    template<typename T>
    T* read() 
	{
        shared_type_id_t key = getTypeId<T>();
        auto it = memory_map.find(key);
        if (it != memory_map.end()) 
		{
            return static_cast<T*>(it->second);
        }
		else 
		{
		    // Create new instance with default constructor
            T* new_mem = new T();
            if (!new_mem) return nullptr;
            memory_map[key] = new_mem;
            
#if defined(ESP_PLATFORM) && defined(CONFIG_ESP_WIFI_ESPNOW)
            // Broadcast the new default instance to other nodes
            espnow_broadcast(key, new_mem, sizeof(T));
#endif
            return new_mem;
        }
    }

    template<typename T>
    int write() 
    {
        shared_type_id_t key = getTypeId<T>();
        auto it = memory_map.find(key);
        if (it == memory_map.end()) 
		{
            // Entry doesn't exist, this shouldn't happen in normal usage
            return -1;
        }
        
#if defined(ESP_PLATFORM) && defined(CONFIG_ESP_WIFI_ESPNOW)
        // Broadcast current data to other ESP32s
        espnow_broadcast(key, it->second, sizeof(T));
#endif
        return 0;  // Success
    }
    
    // Internal method to update memory from received ESP-NOW data
    void update_memory_from_network(shared_type_id_t type_id, const uint8_t* data, size_t size);
    
};

#define GSM (*SharedMemory::getInstance())

#endif // SHARED_MEMORY_HPP
