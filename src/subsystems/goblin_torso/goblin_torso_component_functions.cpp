#include "subsystems/goblin_torso/goblin_torso_component_functions.hpp"
#include "core/memory/SharedMemory.hpp"
#include "with.hpp"
#include "config/shared_headers/PixelType.hpp"
#include "shared/MicrophoneData.hpp"
#include "esp_random.h"

// Auto-generated component aggregation file

// Subsystem-scoped static variables (shared across all components in this file)
static uint32_t display_width = 240;
static uint32_t display_height = 240;
static uint32_t bytes_per_pixel = 2;  // RGB565
static uint8_t* front_buffer = NULL;
static uint8_t* back_buffer = NULL;
static uint32_t display_size = 0;
static int current_row_count = 10;
