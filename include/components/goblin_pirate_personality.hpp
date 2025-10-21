/**
 * @file goblin_pirate_personality.hpp
 * @brief Pirate-specific personality engine for one-eyed goblin variants
 */

#ifndef GOBLIN_PIRATE_PERSONALITY_HPP
#define GOBLIN_PIRATE_PERSONALITY_HPP

#include "p32_shared_state.h"

/**
 * @brief Initialize the pirate personality engine
 * 
 * Sets up pirate-specific behavioral patterns, aggression levels,
 * and compensation mechanisms for missing right eye.
 */
void goblin_pirate_personality_init(void);

/**
 * @brief Execute pirate personality behavior logic
 * 
 * Applies pirate-specific mood modifiers, treasure obsession behaviors,
 * eye loss compensation, and territorial responses.
 */
void goblin_pirate_personality_act(void);

// Internal helper functions (static in implementation)
static void apply_pirate_aggression_boost(SharedMemory& state);
static void handle_treasure_obsession_behavior(SharedMemory& state);
static void implement_eye_loss_compensation(SharedMemory& state);
static void manage_pirate_mood_dynamics(SharedMemory& state);
static void execute_territorial_behaviors(SharedMemory& state);

#endif // GOBLIN_PIRATE_PERSONALITY_HPP