#ifndef P32_DISPATCH_TABLES_H
#define P32_DISPATCH_TABLES_H

#include "esp_err.h"
#include <stdint.h>

// ============================================================================
// P32 Component Dispatch Tables
// Auto-generated from JSON bot configuration
// ============================================================================

// Component function type signatures - NO ARGUMENTS pattern
typedef void (*init_func_t)(void);
typedef void (*act_func_t)(void);

// Table size - all three tables have same size (one entry per component)
#define COMPONENT_TABLE_SIZE 3

// ============================================================================
// Forward Declarations - Init Functions
// ============================================================================

void goblin_left_eye_init(void);
void goblin_nose_init(void);
void goblin_right_eye_init(void);

// ============================================================================
// Forward Declarations - Act Functions (NO ARGUMENTS)
// ============================================================================

void goblin_left_eye_act(void);
void goblin_nose_act(void);
void goblin_right_eye_act(void);

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

// Global loop counter - shared across all components
extern uint64_t g_loopCount;

#endif // P32_DISPATCH_TABLES_H
