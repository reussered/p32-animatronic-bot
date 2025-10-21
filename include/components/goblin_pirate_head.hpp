/**
 * @file goblin_pirate_head.hpp
 * @brief Pirate-specific head subsystem with single eye compensation
 */

#ifndef GOBLIN_PIRATE_HEAD_HPP
#define GOBLIN_PIRATE_HEAD_HPP

#include "p32_shared_state.h"

/**
 * @brief Initialize the pirate head subsystem
 * 
 * Sets up single left eye display, enhanced audio processing,
 * and defensive scanning for missing right eye compensation.
 */
void goblin_pirate_head_init(void);

/**
 * @brief Execute pirate head coordination logic
 * 
 * Coordinates single eye display, enhanced audio, defensive scanning,
 * pirate expressions, and combat readiness behaviors.
 */
void goblin_pirate_head_act(void);

// Internal helper functions (static in implementation)
static void coordinate_single_eye_display(SharedMemory& state);
static void enhance_audio_processing(SharedMemory& state);
static void implement_defensive_scanning(SharedMemory& state);
static void manage_pirate_expressions(SharedMemory& state);
static void handle_combat_readiness(SharedMemory& state);

#endif // GOBLIN_PIRATE_HEAD_HPP