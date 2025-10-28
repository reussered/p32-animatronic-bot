// ============================================================================
// Goblin Head Dual Eye Display Test Harness - Header
// Tests initialization and basic functionality of dual GC9A01 displays
// ============================================================================

#ifndef DUAL_EYE_DISPLAY_TEST_HPP
#define DUAL_EYE_DISPLAY_TEST_HPP

#include "esp_err.h"

// ============================================================================
// Public Interface
// ============================================================================

/**
 * @brief Start the dual eye display test harness
 *
 * This function creates a FreeRTOS task that:
 * 1. Initializes all display components in the correct order
 * 2. Continuously executes component action functions
 * 3. Logs initialization and execution status
 *
 * The test validates:
 * - Component initialization order and success
 * - Pin assignments and hardware interface setup
 * - Component action execution timing
 * - Dual display coordination (left/right eye sync)
 *
 * @return ESP_OK on success, ESP_FAIL on task creation failure
 */
esp_err_t dual_eye_display_test_start(void);

#endif // DUAL_EYE_DISPLAY_TEST_HPP