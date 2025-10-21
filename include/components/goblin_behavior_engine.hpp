#ifndef GOBLIN_BEHAVIOR_ENGINE_HPP
#define GOBLIN_BEHAVIOR_ENGINE_HPP

/**
 * @file goblin_behavior_engine.hpp
 * @brief Goblin behavior selection engine - chooses actions based on mood and personality
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize goblin_behavior_engine component
 * Called once during system startup
 */
void goblin_behavior_engine_init(void);

/**
 * @brief Execute goblin_behavior_engine component logic
 * Called every loop iteration based on hitCount
 */
void goblin_behavior_engine_act(void);

#endif // GOBLIN_BEHAVIOR_ENGINE_HPP
