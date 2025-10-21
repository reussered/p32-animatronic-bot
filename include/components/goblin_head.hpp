#ifndef GOBLIN_HEAD_HPP
#define GOBLIN_HEAD_HPP

/**
 * @file goblin_head.hpp
 * @brief Goblin head subsystem with facial expressions, sensors, and audio output - Focus on display/audio processing
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize goblin_head component
 * Called once during system startup
 */
void goblin_head_init(void);

/**
 * @brief Execute goblin_head component logic
 * Called every loop iteration based on hitCount
 */
void goblin_head_act(void);

#endif // GOBLIN_HEAD_HPP
