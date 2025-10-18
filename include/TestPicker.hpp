/**
 * TestPicker Component Header
 * 
 * Purpose: Adds comprehensive test cases to the SystemTest global variable
 * Author: reussered
 * Created: 2025-10-18
 */

#pragma once

#include "esp_err.h"
#include <stdbool.h>

// SystemTest structure definition (should match the one in display_test component)
typedef struct {
    char test_names[10][64];  // Array of test name strings
    int test_count;
    bool tests_added;
} SystemTest;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Component: TestPicker
 * Function: TestPicker_init
 * Purpose: Initialize test picker and populate SystemTest with comprehensive test cases
 */
esp_err_t TestPicker_init(void);

/**
 * Component: TestPicker  
 * Function: TestPicker_act
 * Purpose: Monitor test execution status and provide test management
 */
esp_err_t TestPicker_act(void);

/**
 * Utility functions for test management
 */
int TestPicker_get_test_count(void);
const char* TestPicker_get_test_name(int index);
bool TestPicker_tests_ready(void);

#ifdef __cplusplus
}
#endif