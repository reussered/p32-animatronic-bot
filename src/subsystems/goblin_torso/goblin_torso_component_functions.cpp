#include "subsystems/goblin_torso/goblin_torso_component_functions.hpp"
#include "core/memory/SharedMemory.hpp"
#include "with.hpp"
#include "esp_random.h"

// Shared state classes (auto-included in all components)
#include "BalanceCompensation.hpp"
#include "BehaviorControl.hpp"
#include "CollisionAvoidance.hpp"
#include "EmergencyCoordination.hpp"
#include "Environment.hpp"
#include "FrameProcessor.hpp"
#include "ManipulationControl.hpp"
#include "MicrophoneData.hpp"
#include "Mood.hpp"
#include "Personality.hpp"
#include "SensorFusion.hpp"
#include "SysTest.hpp"

// Shared type definitions (auto-included in all components)
#include "shared_headers/color_schema.hpp"
#include "shared_headers/PixelType.hpp"

// Auto-generated component aggregation file

// Subsystem-scoped static variables (shared across all components in this file)
static int display_width = 240;
static int display_height = 240;
static int bytes_per_pixel = 2;  // RGB565
static uint8_t* front_buffer = NULL;
static uint8_t* back_buffer = NULL;
static int display_size = 0;
static int current_row_count = 10;
static int max_display_height = INT_MAX;  // Min height across all displays
static char* color_schema = nullptr;
