// P32 Shared State - Global State Synchronized Across All Subsystems via ESP-NOW Mesh
// This file defines the global state structure that is replicated on every ESP32 subsystem
// and kept in sync via the ESP-NOW mesh network component.

#ifndef P32_SHARED_STATE_H
#define P32_SHARED_STATE_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// ESP32 CHIP VARIANT DETECTION AND CAPABILITIES
// =============================================================================

// Chip type detection based on build flags
#ifdef ESP32_CHIP_TYPE_S3
    #define CHIP_HAS_PSRAM 1
    #define CHIP_GPIO_COUNT 45
    #define CHIP_SRAM_SIZE 512000
    #define CHIP_CPU_CORES 2
    #define CHIP_MAX_FREQ_MHZ 240
    #define CHIP_VARIANT_NAME "ESP32-S3"
    #define CHIP_ARCHITECTURE "Xtensa"
#elif defined(ESP32_CHIP_TYPE_C3)
    #define CHIP_HAS_PSRAM 0
    #define CHIP_GPIO_COUNT 22
    #define CHIP_SRAM_SIZE 400000
    #define CHIP_CPU_CORES 1
    #define CHIP_MAX_FREQ_MHZ 160
    #define CHIP_VARIANT_NAME "ESP32-C3"
    #define CHIP_ARCHITECTURE "RISC-V"
#elif defined(ESP32_CHIP_TYPE_STANDARD)
    #define CHIP_HAS_PSRAM 0
    #define CHIP_GPIO_COUNT 34
    #define CHIP_SRAM_SIZE 520000
    #define CHIP_CPU_CORES 2
    #define CHIP_MAX_FREQ_MHZ 240
    #define CHIP_VARIANT_NAME "ESP32"
    #define CHIP_ARCHITECTURE "Xtensa"
#elif defined(ESP32_CHIP_TYPE_S2)
    #define CHIP_HAS_PSRAM 1
    #define CHIP_GPIO_COUNT 43
    #define CHIP_SRAM_SIZE 320000
    #define CHIP_CPU_CORES 1
    #define CHIP_MAX_FREQ_MHZ 240
    #define CHIP_VARIANT_NAME "ESP32-S2"
    #define CHIP_ARCHITECTURE "Xtensa"
#else
    #error "ESP32 chip type not defined! Add -DESP32_CHIP_TYPE_xxx to build flags"
#endif

// =============================================================================
// FEATURE ENABLEMENT BASED ON CHIP CAPABILITIES
// =============================================================================

// High-performance features (requires ESP32-S3)
#if defined(ESP32_CHIP_TYPE_S3)
    #define ENABLE_DUAL_DISPLAYS 1
    #define ENABLE_ADVANCED_AUDIO 1
    #define ENABLE_COMPLEX_ALGORITHMS 1
    #define ENABLE_PSRAM_BUFFERS 1
    #define MAX_COMPONENT_COUNT 32
    #define MAX_SERVO_COUNT 16
#endif

// Cost-optimized features (ESP32-C3)
#if defined(ESP32_CHIP_TYPE_C3)
    #define ENABLE_BASIC_CONTROL 1
    #define ENABLE_COST_OPTIMIZATION 1
    #define MAX_COMPONENT_COUNT 8
    #define MAX_SERVO_COUNT 8
    #define MEMORY_CONSERVATION_MODE 1
#endif

// Standard features (ESP32)
#if defined(ESP32_CHIP_TYPE_STANDARD)
    #define ENABLE_DUAL_CORE_PROCESSING 1
    #define MAX_COMPONENT_COUNT 16
    #define MAX_SERVO_COUNT 12
#endif

// =============================================================================
// SUBSYSTEM FEATURE DETECTION
// =============================================================================

// Head subsystem features
#ifdef SUBSYSTEM_GOBLIN_HEAD
    #define COMPONENT_COUNT_EXPECTED 6  // left_eye, right_eye, mouth, nose, ears
    #ifdef HEAD_DUAL_DISPLAYS
        #define SPI_DEVICE_COUNT 3  // 2 eyes + 1 mouth
    #endif
#endif

// Torso subsystem features  
#ifdef SUBSYSTEM_GOBLIN_TORSO
    #define COMPONENT_COUNT_EXPECTED 4  // power, mesh, coordination, spine
    #ifdef MASTER_CONTROLLER
        #define ESP_NOW_MESH_MASTER 1
        #define ENABLE_SYSTEM_TELEMETRY 1
    #endif
#endif

// Arm subsystem features
#if defined(SUBSYSTEM_LEFT_ARM) || defined(SUBSYSTEM_RIGHT_ARM)
    #ifdef ARM_BASIC_CONTROL
        #define COMPONENT_COUNT_EXPECTED 2  // shoulder, elbow
    #endif
    #ifdef ARM_ADVANCED_CONTROL
        #define COMPONENT_COUNT_EXPECTED 3  // shoulder, elbow, hand
        #define ENABLE_INVERSE_KINEMATICS 1
    #endif
#endif

// Leg subsystem features
#if defined(SUBSYSTEM_LEFT_LEG) || defined(SUBSYSTEM_RIGHT_LEG)
    #ifdef LEG_FOOT_INTEGRATION
        #define COMPONENT_COUNT_EXPECTED 4  // hip, knee, ankle, foot
    #endif
#endif

// =============================================================================
// GLOBAL SHARED STATE
// =============================================================================

// Global loop counter (64-bit to prevent overflow)
// At 120,000 iterations/second: 32-bit overflows in ~10 hours, 64-bit in 4.8 million years
// Components can READ this directly, but ONLY main.c can write it
// Declared as const here to prevent accidental modification in components
extern const uint64_t g_loopCount;

#endif // P32_SHARED_STATE_H
