#ifndef GOBLIN_PERSONALITY_HPP
#define GOBLIN_PERSONALITY_HPP

/**
 * @file goblin_personality.hpp
 * @brief Goblin family personality traits - mischievous, aggressive, curious
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize goblin_personality component
 * Called once during system startup
 */
void goblin_personality_init(void);

/**
 * @brief Execute goblin_personality component logic
 * Called every loop iteration based on hitCount
 */
void goblin_personality_act(void);

#endif // GOBLIN_PERSONALITY_HPP
