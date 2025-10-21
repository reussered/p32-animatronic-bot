// P32 Shared State - Global State Synchronized Across All Subsystems via ESP-NOW Mesh
// This file defines the global state structure that is replicated on every ESP32 subsystem
// and kept in sync via the ESP-NOW mesh network component.

#ifndef P32_SHARED_STATE_H
#define P32_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>


// Global loop counter (64-bit to prevent overflow)
// At 120,000 iterations/second: 32-bit overflows in ~10 hours, 64-bit in 4.8 million years
// Components can READ this directly, but ONLY main.c can write it
// Declared as const here to prevent accidental modification in components
extern const uint64_t g_loopCount;

#endif // P32_SHARED_STATE_H
