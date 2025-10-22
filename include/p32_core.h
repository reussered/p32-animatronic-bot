#ifndef P32_CORE_H
#define P32_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>

// P32 Core System Types and Constants
// Essential definitions for all P32 components

// Core data types
typedef struct {
    float x, y, z;
} p32_vector3_t;

typedef struct {
    float x, y, z, w;
} p32_quaternion_t;

// Core system error codes
#define P32_ERR_INVALID_ARG     (ESP_ERR_INVALID_ARG)
#define P32_ERR_NOT_FOUND       (ESP_ERR_NOT_FOUND)
#define P32_ERR_NO_MEM          (ESP_ERR_NO_MEM)
#define P32_ERR_TIMEOUT         (ESP_ERR_TIMEOUT)

// Core system constants
#define P32_MAX_COMPONENTS      64
#define P32_MAX_SERVOS          16
#define P32_MAX_SENSORS         32

// System states
typedef enum {
    P32_STATE_INIT = 0,
    P32_STATE_READY,
    P32_STATE_ACTIVE,
    P32_STATE_ERROR,
    P32_STATE_SHUTDOWN
} p32_system_state_t;

#ifdef __cplusplus
extern "C" {
#endif

// Core function declarations
esp_err_t p32_core_init(void);
esp_err_t p32_core_start(void);
esp_err_t p32_core_stop(void);
p32_system_state_t p32_core_get_state(void);

#ifdef __cplusplus
}
#endif

#endif // P32_CORE_H