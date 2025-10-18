#ifndef SIMPLE_CORE_TEST_HPP
#define SIMPLE_CORE_TEST_HPP

#include <esp_err.h>

// Component function signatures (NO ARGUMENTS pattern)
// C++ functions with C linkage for component tables
extern "C" {
    esp_err_t p32_comp_simple_core_test_init(void);
    void p32_comp_simple_core_test_act(void);
}

#endif // SIMPLE_CORE_TEST_HPP
