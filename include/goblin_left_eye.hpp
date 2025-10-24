// P32 Component Header: goblin_left_eye
// Simple test implementation for GC9A01 display

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component function declarations
esp_err_t goblin_left_eye_init(void);
void goblin_left_eye_act(void);

#ifdef __cplusplus
}
#endif