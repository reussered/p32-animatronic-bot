// Shared data types for torso and head ESP32s
#ifndef TEST_SHARED_TYPES_H
#define TEST_SHARED_TYPES_H

#include <stdint.h>
#include <string.h>

// Environment (sensor data)
typedef struct {
    uint8_t distance_cm;      // 0-255 cm
    bool target_detected;     // Object within 100cm
} Environment;

// Mood (emotional state)
typedef struct {
    int8_t happiness;         // -128 to +127
    int8_t curiosity;         // -128 to +127
    int8_t fear;              // -128 to +127
} Mood;

// Global shared memory blocks
Environment g_Envir = {255, false};
Mood g_MOOD = {0, 0, 0};

// Mesh packet for ESP-NOW
typedef struct {
    char name[32];            // "g_Envir" or "g_MOOD"
    uint8_t data[64];         // Payload
    size_t size;              // Payload size
} mesh_packet_t;

#endif // TEST_SHARED_TYPES_H
