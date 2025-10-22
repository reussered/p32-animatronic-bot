#ifndef GOBLIN_EYE_HPP
#define GOBLIN_EYE_HPP

/**
 * @file goblin_eye.hpp
 * @brief Shared goblin eye processing logic with palette and mood-based rendering
 * @author P32 Animatronic Bot Project
 */

#include "core/memory/SharedMemory.hpp"
#include "Mood.hpp"
#include "../FrameProcessor.hpp"
#include <cstdint>

// Goblin eye palette - 256 colors organized by purpose
extern RGB565Pixel goblin_eye_palette[256];

// Shared eye processing variables (extern declarations)
extern uint8_t* currentFrame;
extern uint32_t current_frame_size;
extern uint32_t current_spi_device;

// Animation constants
extern const uint32_t FRAME_WIDTH;
extern const uint32_t FRAME_HEIGHT;
extern const uint32_t PIXELS_PER_FRAME;

/**
 * @brief Initialize shared goblin eye resources
 * Called once during system startup
 */
void goblin_eye_init(void);

/**
 * @brief Process current frame with mood-based color adjustments
 * Called by left/right eye components after they set up their context
 */
void goblin_eye_act(void);

/**
 * @brief Initialize the goblin eye palette with organized color ranges
 */
void init_goblin_eye_palette(void);

/**
 * @brief Apply mood-based color processing to current frame
 * Uses the pixel processing algorithm from FrameProcessor
 */
void process_frame_with_mood(void);

#endif // GOBLIN_EYE_HPP