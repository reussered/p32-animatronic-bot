#ifndef TEST_HEAD_HPP
#define TEST_HEAD_HPP

#include "esp_err.h"

// P32 Component: test_head
// Test head subsystem with dual displays for hardware validation
// ESP32-S3 embedded interface

// Component initialization function - NO ARGUMENTS
esp_err_t test_head_init(void);

// Component action function - NO ARGUMENTS
void test_head_act(void);

#endif // TEST_HEAD_HPP