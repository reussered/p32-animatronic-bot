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
esp_err_t test_head_init(void);
esp_err_t goblin_left_eye_init(void);
esp_err_t spi_bus_init(void);
esp_err_t goblin_eye_init(void);
esp_err_t gc9a01_init(void);
esp_err_t generic_spi_display_init(void);
esp_err_t goblin_right_eye_init(void);
esp_err_t spi_bus_init(void);
esp_err_t goblin_eye_init(void);
esp_err_t gc9a01_init(void);
esp_err_t generic_spi_display_init(void);

// Forward Declarations - Act Functions (from individual component files)

void heartbeat_act(void);
void network_monitor_act(void);
void test_head_act(void);
void goblin_left_eye_act(void);
void spi_bus_act(void);
void goblin_eye_act(void);
void gc9a01_act(void);
void generic_spi_display_act(void);
void goblin_right_eye_act(void);
void spi_bus_act(void);
void goblin_eye_act(void);
void gc9a01_act(void);
void generic_spi_display_act(void);

// Table size constant - ALL TABLES MUST HAVE SAME SIZE
#define TABLE_SIZE 13

#endif // P32_COMPONENT_REGISTRY_HPP