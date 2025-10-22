// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

#include "p32_component_registry.hpp"

#include "heartbeat.hpp"
#include "network_monitor.hpp"
#include "goblin_head.hpp"
#include "goblin_torso.hpp"

// ============================================================================
// Initialization Table
// ============================================================================

esp_err_t (*initTable[INIT_TABLE_SIZE])(void) = {
    heartbeat_init,
    network_monitor_init,
    goblin_head_init,
    goblin_torso_init
};

// ============================================================================
// Action Table
// ============================================================================

void (*actTable[ACT_TABLE_SIZE])(void) = {
    heartbeat_act,    // [0] System heartbeat
    network_monitor_act,    // [1] Network monitoring and loop timing
    goblin_head_act,    // [2] Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
    goblin_torso_act     // [3] Goblin torso subsystem - Master controller with mesh networking, WiFi, and system coordination
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[ACT_TABLE_SIZE] = {
    1,    // [0] heartbeat - every 1 loops
    1,    // [1] network_monitor - every 1 loops
    25,    // [2] goblin_head - every 25 loops
    50     // [3] goblin_torso - every 50 loops
};
