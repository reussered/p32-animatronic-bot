#ifndef P32_COMPONENT_REGISTRY_HPP
#define P32_COMPONENT_REGISTRY_HPP

#include "esp_err.h"
#include <stdint.h>

// ============================================================================
// P32 Component Registry - Function Declarations
// Auto-generated from JSON bot configuration
// ============================================================================

// Forward Declarations - Init Functions (from individual component files)

esp_err_t heartbeat_init(void);
esp_err_t network_monitor_init(void);
esp_err_t goblin_left_eye_init(void);

// Forward Declarations - Act Functions (from individual component files)

void heartbeat_act(void);
void network_monitor_act(void);
void goblin_left_eye_act(void);

// Table size constants
#define INIT_TABLE_SIZE 3
#define ACT_TABLE_SIZE 3

#endif // P32_COMPONENT_REGISTRY_HPP