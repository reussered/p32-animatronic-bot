#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <string>
#include <map>
#include <cstring>
#include <memory>

// ESP-NOW stub (replace with actual ESP-IDF includes and logic)
void espnow_broadcast(const std::string& name, void* data, size_t size);

class SharedMemory {
private:
    std::map<std::string, void*> memory_map;

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
            T temp{};
            write<T>(name, &temp);
            return static_cast<T*>(memory_map[name]);
        }
    }

    template<typename T>
    int write(const std::string& name, T* data) {
        auto it = memory_map.find(name);
        if (it != memory_map.end()) {
            std::memcpy(it->second, data, sizeof(T));
        } else {
            void* new_mem = malloc(sizeof(T));
            if (!new_mem) return -1;
            std::memcpy(new_mem, data, sizeof(T));
            memory_map[name] = new_mem;
        }
        // Broadcast to other ESP32s
        espnow_broadcast(name, memory_map[name], sizeof(T));
        return 0;
    }
};

#endif // SHARED_MEMORY_HPP
