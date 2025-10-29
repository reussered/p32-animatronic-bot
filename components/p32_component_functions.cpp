#include "p32_component_functions.hpp"

// ============================================================================
// P32 Component Function Implementations
// This file aggregates all individual component implementations
// ============================================================================

// All component implementations are included via their individual headers
// The actual function definitions are in their respective component files:
//
// Include all component implementation files for goblin_head_only configuration
#include "components/goblin_head_left_eye_only.src"
#include "goblin_full/goblin_left_eye.src"
#include "goblin_full/goblin_eye.src"
#include "components/gc9a01.src"
#include "components/spi_bus.src"
#include "components/generic_spi_display/generic_spi_display.src"
//
// This file serves as the central include point for all component functions
// that are referenced in the dispatch tables.
