#ifndef MESH_SOFTWARE_COORDINATOR_HPP
#define MESH_SOFTWARE_COORDINATOR_HPP

/**
 * @file mesh_software_coordinator.hpp
 * @brief High-level software coordination logic for distributed robot mesh network
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize mesh_software_coordinator component
 * Called once during system startup
 */
void mesh_software_coordinator_init(void);

/**
 * @brief Execute mesh_software_coordinator component logic
 * Called every loop iteration based on hitCount
 */
void mesh_software_coordinator_act(void);

#endif // MESH_SOFTWARE_COORDINATOR_HPP
