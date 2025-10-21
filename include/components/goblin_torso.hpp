#ifndef GOBLIN_TORSO_HPP
#define GOBLIN_TORSO_HPP

/**
 * @file goblin_torso.hpp
 * @brief Goblin torso subsystem - Master controller with mesh networking, WiFi, and system coordination
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize goblin_torso component
 * Called once during system startup
 */
void goblin_torso_init(void);

/**
 * @brief Execute goblin_torso component logic
 * Called every loop iteration based on hitCount
 */
void goblin_torso_act(void);

#endif // GOBLIN_TORSO_HPP
