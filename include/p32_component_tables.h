#ifndef P32_COMPONENT_TABLES_H
#define P32_COMPONENT_TABLES_H

#include "esp_err.h"
#include <stdint.h>

// ============================================================================
// P32 Component Dispatch Tables
// Auto-generated from JSON bot configuration
// ============================================================================

// Component function type signatures - NO ARGUMENTS pattern
typedef esp_err_t (*init_func_t)(void);
typedef void (*act_func_t)(void);

// Table size - all three tables have same size (one entry per component)
#define COMPONENT_TABLE_SIZE 4

// ============================================================================
// Forward Declarations - Init Functions
// ============================================================================

esp_err_t heartbeat_init(void);
esp_err_t network_monitor_init(void);
esp_err_t goblin_head_init(void);
esp_err_t goblin_torso_init(void);

// ============================================================================
// Forward Declarations - Act Functions (NO ARGUMENTS)
// ============================================================================

void heartbeat_act(void);
void network_monitor_act(void);
void goblin_head_act(void);
void goblin_torso_act(void);

// ============================================================================
// Dispatch Tables
// ============================================================================

// Initialization table - called once at startup in order
extern init_func_t initTable[COMPONENT_TABLE_SIZE];

// Action table - function pointers (NO ARGUMENTS)
// Parallel to hitCountTable - both indexed by same i
extern act_func_t actTable[COMPONENT_TABLE_SIZE];

// Timing table - execution frequency control
// actTable[i] executes when g_loopCount % hitCountTable[i] == 0
extern uint32_t hitCountTable[COMPONENT_TABLE_SIZE];

#endif // P32_COMPONENT_TABLES_H