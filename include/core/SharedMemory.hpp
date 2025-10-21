#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <string>
#include <map>
#include <cstring>
#include <memory>


#ifdef ESP_PLATFORM
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_log.h>
#endif

class SharedMemory {
private:
    std::map<std::string, void*> memory_map;

class SharedMemory {
private:
    std::map<std::string, void*> memory_map;
#ifdef ESP_PLATFORM
    static void on_data_sent(const uint8_t* mac_addr, esp_now_send_status_t status);
    static void on_data_recv(const uint8_t* mac_addr, const uint8_t* data, int len);
    void espnow_broadcast(const std::string& name, void* data, size_t size);
#endif

public:
#ifdef ESP_PLATFORM
    void espnow_init();
#endif

public:
    SharedMemory() = default;
    ~SharedMemory() {
        // Free all allocated memory
        for (auto& entry : memory_map) {
            free(entry.second);
        }
    }

    template<typename T>
    T* read(const std::string& name) {
        auto it = memory_map.find(name);
        if (it != memory_map.end()) {
            return static_cast<T*>(it->second);
        } else {
            write<T>(name );
            return static_cast<T*>(memory_map[name]);
        }
    }

    template<typename T>
    int write(const std::string& name ) {
        auto it = memory_map.find(name);
        void* new_mem;
        if (it == memory_map.end()) {
            new_mem = new T();
            if (!new_mem) return -1;
            memory_map[name] = new_mem;
        }
        else
        {
            new_mem = it->second;
        }


        // Broadcast to other ESP32s
        espnow_broadcast(name, new_mem, sizeof(T));
        return ESP_OK;
    }
#endif
};

#endif // SHARED_MEMORY_HPP
