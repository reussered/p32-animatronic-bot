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
    p32_comp_system_core_init,
    p32_comp_network_monitor_init,
    p32_comp_display_test_init,
    p32_comp_testpicker_init,
    p32_comp_power_monitor_init,
    p32_comp_watchdog_init,
    p32_comp_serial_console_init,
    p32_comp_simple_core_test_init
};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
    p32_comp_system_core_act,    // [0] Core system management - initialization, health checks, error handling
    p32_comp_network_monitor_act,    // [1] Network connectivity monitoring - WiFi signal strength, connection status
    p32_comp_display_test_act,    // [2] Display GSM test integration - manages test names and coordinates display testing
    p32_comp_testpicker_act,    // [3] Test case picker - populates SystemTest global variable with comprehensive test cases for GC9A01 displays and system validation
    p32_comp_power_monitor_act,    // [4] Power monitoring - battery voltage, current consumption, remaining capacity
    p32_comp_watchdog_act,    // [5] Hardware watchdog timer - system health monitoring and automatic recovery
    p32_comp_serial_console_act,    // [6] Serial console interface - debug commands and telemetry output
    p32_comp_simple_core_test_act     // [7] Simple blink test to verify ESP32-S3 can execute code - blinks onboard LED
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
    762000     // [7] simple_core_test - every 762000 loops (6350.0ms period)
};
