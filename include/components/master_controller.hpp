#ifndef MASTER_CONTROLLER_HPP
#define MASTER_CONTROLLER_HPP

/**
 * @file master_controller.hpp
 * @brief Master controller for coordinating all subsystems via mesh network
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize master_controller component
 * Called once during system startup
 */
void master_controller_init(void);

/**
 * @brief Execute master_controller component logic
 * Called every loop iteration based on hitCount
 */
void master_controller_act(void);

#endif // MASTER_CONTROLLER_HPP
