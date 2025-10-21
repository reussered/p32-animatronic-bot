#ifndef SERIAL_CONSOLE_HPP
#define SERIAL_CONSOLE_HPP

/**
 * @file serial_console.hpp
 * @brief Serial console interface - debug commands and telemetry output
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize serial_console component
 * Called once during system startup
 */
void serial_console_init(void);

/**
 * @brief Execute serial_console component logic
 * Called every loop iteration based on hitCount
 */
void serial_console_act(void);

#endif // SERIAL_CONSOLE_HPP
