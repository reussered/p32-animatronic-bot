#ifndef MESH_COORDINATOR_HPP
#define MESH_COORDINATOR_HPP

/**
 * @file mesh_coordinator.hpp
 * @brief ESP-NOW mesh network master controller
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize mesh_coordinator component
 * Called once during system startup
 */
void mesh_coordinator_init(void);

/**
 * @brief Execute mesh_coordinator component logic
 * Called every loop iteration based on hitCount
 */
void mesh_coordinator_act(void);

#endif // MESH_COORDINATOR_HPP
