#ifndef P32_COMPONENT_TABLES_HPP
#define P32_COMPONENT_TABLES_HPP

#include "esp_err.h"
#include <stdint.h>

// ============================================================================
// P32 Component Dispatch Tables
// Auto-generated from JSON bot configuration
// ============================================================================

// Component function type signatures - NO ARGUMENTS pattern
typedef esp_err_t (*init_func_t)(void);
typedef void (*act_func_t)(void);

// Table size - all three tables have same size (one entry per component)
#define COMPONENT_TABLE_SIZE 15

// ============================================================================
// Forward Declarations - Init Functions (C linkage)
// ============================================================================

extern "C" {
    esp_err_t system_core_init(void);
    esp_err_t network_monitor_init(void);
    esp_err_t display_test_init(void);
    esp_err_t testpicker_init(void);
    esp_err_t power_monitor_init(void);
    esp_err_t watchdog_init(void);
    esp_err_t serial_console_init(void);
    esp_err_t left_eye_init(void);
    esp_err_t right_eye_init(void);
    esp_err_t mouth_init(void);
    esp_err_t nose_sensor_init(void);
    esp_err_t unknown_init(void);
    esp_err_t unknown_init(void);
    esp_err_t audio_init(void);
    esp_err_t left_eye_init(void);
}

// ============================================================================
// Forward Declarations - Act Functions (NO ARGUMENTS, C linkage)
// ============================================================================

extern "C" {
    void system_core_act(void);
    void network_monitor_act(void);
    void display_test_act(void);
    void testpicker_act(void);
    void power_monitor_act(void);
    void watchdog_act(void);
    void serial_console_act(void);
    void left_eye_act(void);
    void right_eye_act(void);
    void mouth_act(void);
    void nose_sensor_act(void);
    void unknown_act(void);
    void unknown_act(void);
    void audio_act(void);
    void left_eye_act(void);
}

// ============================================================================
// Dispatch Tables
// ============================================================================

// Initialization table - called once at startup in order
extern init_func_t initTable[COMPONENT_TABLE_SIZE];

// Action table - function pointers (NO ARGUMENTS)
// Parallel to hitCountTable - both indexed by same i
extern act_func_t actTable[COMPONENT_TABLE_SIZE];

// Timing table - execution frequency control
// actTable[i] executes when g_loopCount % hitCountTable[i] == 0
extern uint32_t hitCountTable[COMPONENT_TABLE_SIZE];

#endif // P32_COMPONENT_TABLES_HPP