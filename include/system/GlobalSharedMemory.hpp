#ifndef GLOBAL_SHARED_MEMORY_HPP
#define GLOBAL_SHARED_MEMORY_HPP

#include "core/SharedMemory.hpp"

extern SharedMemory g_GlobalSharedMemory;

extern "C" {
#include "esp_err.h"
}

esp_err_t GlobalSharedMemory_init(void);
esp_err_t GlobalSharedMemory_act(void);

#endif // GLOBAL_SHARED_MEMORY_HPP
