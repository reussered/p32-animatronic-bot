#ifndef GOBLIN_MOOD_HPP
#define GOBLIN_MOOD_HPP

/**
 * @file goblin_mood.hpp
 * @brief Goblin 8-mood emotional state machine - integrates sensor inputs and personality
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"


// Component-specific includes would go here

/**
 * @brief Initialize goblin_mood component
 * Called once during system startup
 */
void goblin_mood_init(void);

/**
 * @brief Execute goblin_mood component logic
 * Called every loop iteration based on hitCount
 */
void goblin_mood_act(void);

#endif // GOBLIN_MOOD_HPP
