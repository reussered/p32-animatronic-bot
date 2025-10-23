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

// Forward Declarations - Act Functions (from individual component files)

void heartbeat_act(void);
void network_monitor_act(void);

// Table size constant - ALL TABLES MUST HAVE SAME SIZE
#define TABLE_SIZE 2

#endif // P32_COMPONENT_REGISTRY_HPP