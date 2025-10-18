#include "p32_component_tables.hpp"

// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

// Global loop counter - incremented by main.cpp core loop
uint64_t g_loopCount = 0;

// ============================================================================
// Initialization Table
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {
    system_core_init,
    network_monitor_init,
    display_test_init,
    testpicker_init,
    power_monitor_init,
    watchdog_init,
    serial_console_init,
    left_eye_init,
    right_eye_init,
    mouth_init,
    nose_sensor_init,
    unknown_init,
    unknown_init,
    audio_init,
    left_eye_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    system_core_act,    // [0] Core system management - initialization, health checks, error handling
    network_monitor_act,    // [1] Network connectivity monitoring - WiFi signal strength, connection status
    display_test_act,    // [2] Display GSM test integration - manages test names and coordinates display testing
    testpicker_act,    // [3] Test case picker - populates SystemTest global variable with comprehensive test cases for GC9A01 displays and system validation
    power_monitor_act,    // [4] Power monitoring - battery voltage, current consumption, remaining capacity
    watchdog_act,    // [5] Hardware watchdog timer - system health monitoring and automatic recovery
    serial_console_act,    // [6] Serial console interface - debug commands and telemetry output
    left_eye_act,    // [7] Left eye display animation
    right_eye_act,    // [8] Right eye display animation
    mouth_act,    // [9] Mouth display animation
    nose_sensor_act,    // [10] Proximity sensor monitoring
    unknown_act,    // [11] unknown component
    unknown_act,    // [12] unknown component
    audio_act,    // [13] Audio output processing
    left_eye_act     // [14] Left eye display animation - goblin variant using standard GC9A01 hardware with GSM test execution
};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
    100,    // [0] system_core - every 100 loops (1200 Hz)
    50,    // [1] network_monitor - every 50 loops (2400 Hz)
    120000,    // [2] display_test - every 120000 loops (1.0 Hz)
    240000,    // [3] testpicker - every 240000 loops (2000.0ms period)
    200,    // [4] power_monitor - every 200 loops (600.0 Hz)
    500,    // [5] watchdog - every 500 loops (240.0 Hz)
    25,    // [6] serial_console - every 25 loops (4800 Hz)
    60000,    // [7] left_eye - every 60000 loops (2.0 Hz)
    60000,    // [8] right_eye - every 60000 loops (2.0 Hz)
    36000,    // [9] mouth - every 36000 loops (3.3 Hz)
    180000,    // [10] nose_sensor - every 180000 loops (1500.0ms period)
    24000,    // [11] unknown - every 24000 loops (5.0 Hz)
    24000,    // [12] unknown - every 24000 loops (5.0 Hz)
    84000,    // [13] audio - every 84000 loops (1.4 Hz)
    5     // [14] left_eye - every 5 loops (24000 Hz)
};
