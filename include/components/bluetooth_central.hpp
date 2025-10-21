#ifndef BLUETOOTH_CENTRAL_HPP
#define BLUETOOTH_CENTRAL_HPP

/**
 * @file bluetooth_central.hpp
 * @brief Bluetooth Low Energy (BLE) central controller - mobile app connectivity
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize bluetooth_central component
 * Called once during system startup
 */
void bluetooth_central_init(void);

/**
 * @brief Execute bluetooth_central component logic
 * Called every loop iteration based on hitCount
 */
void bluetooth_central_act(void);

#endif // BLUETOOTH_CENTRAL_HPP
