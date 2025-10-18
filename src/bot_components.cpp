#include "p32_component_tables.hpp"
#include "esp_log.h"
// #include "components/testing_framework.hpp"  // Will implement later for Halloween goblin!

// Positioned component implementations - C++
// Generated from JSON bot configuration

// Forward declaration for actual hardware initialization
extern "C" esp_err_t goblin_eye_left_init(void);

namespace {
    constexpr const char* TAG_LEFT_EYE = "LEFT_EYE";
}

extern "C" esp_err_t p32_comp_left_eye_init(void)
{
    // Testing framework will be implemented later - for now, always do real hardware init
    ESP_LOGI(TAG_LEFT_EYE, "Calling actual hardware initialization...");
    return goblin_eye_left_init(); // Call the real SPI initialization function!
}

extern "C" void p32_comp_left_eye_act(void)
{
    // Component: left_eye - Left eye display animation
    // Timing: Execute every 60000 loops
    
    // Testing framework will be implemented later - for now, always do real hardware action
    // Call the actual hardware-specific goblin eye component
    extern void goblin_eye_left_act(void);
    goblin_eye_left_act();
}
