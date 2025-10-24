#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <string>
#include <map>
#include <cstring>
#include <memory>
#include <algorithm>
#include <cctype>


#ifdef ESP_PLATFORM
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_log.h>
#endif

class SharedMemory {
private:
    std::map<std::string, void*> memory_map;
    static SharedMemory* instance;  // For static callback access
    
    // Helper function to normalize key names to lowercase for case-insensitive access
    std::string normalize_key(const std::string& name) const {
        std::string normalized = name;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        return normalized;
    }
    
#ifdef ESP_PLATFORM
    static void on_data_sent(const uint8_t* mac_addr, esp_now_send_status_t status);
    static void on_data_recv(const uint8_t* mac_addr, const uint8_t* data, int len);
    void espnow_broadcast(const std::string& name, void* data, size_t size);
#endif

public:
#ifdef ESP_PLATFORM
    void espnow_init();
#endif

    SharedMemory() 
    {
        instance = this;  // Set static instance for callback access
    }
    
    ~SharedMemory() 
	{
        // Free all allocated memory
        for (auto& entry : memory_map) 
		{
            delete[] static_cast<uint8_t*>(entry.second);  // Use delete[] for raw bytes
        }
    }

    template<typename T>
    T* read(const std::string& name) 
	{
        std::string normalized_name = normalize_key(name);
        auto it = memory_map.find(normalized_name);
        if (it != memory_map.end()) 
		{
            return static_cast<T*>(it->second);
        } 
		else 
		{
		    // Create new instance with default constructor
            T* new_mem = new T();
            if (!new_mem) return nullptr;
            memory_map[normalized_name] = new_mem;
            
#ifdef ESP_PLATFORM
            // Broadcast the new default instance to other nodes
            espnow_broadcast(normalized_name, new_mem, sizeof(T));
#endif
            return new_mem;
        }
    }

    template<typename T>
    int write(const std::string& name) 
    {
        std::string normalized_name = normalize_key(name);
        auto it = memory_map.find(normalized_name);
        if (it == memory_map.end()) 
		{
            // Entry doesn't exist, this shouldn't happen in normal usage
            return -1;
        }
        
#ifdef ESP_PLATFORM
        // Broadcast current data to other ESP32s
        espnow_broadcast(normalized_name, it->second, sizeof(T));
#endif
        return 0;  // Success
    }
    
private:
    // Internal method to update memory from received ESP-NOW data
    void update_memory_from_network(const std::string& name, const uint8_t* data, size_t size);
    
};
#define GSM SharedMemory

#endif // SHARED_MEMORY_HPP
