/**
 * Display Test Component Header
 * Simple GSM test integration for display components
 */

#ifndef P32_COMP_DISPLAY_TEST_HPP
#define P32_COMP_DISPLAY_TEST_HPP

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component function declarations
esp_err_t p32_comp_display_test_init(void);
void p32_comp_display_test_act(void);

// Helper function for display components to run GSM tests
bool run_display_gsm_tests(void);

#ifdef __cplusplus
}
#endif

#endif // P32_COMP_DISPLAY_TEST_HPP