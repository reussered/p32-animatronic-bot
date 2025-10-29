#ifndef P32_COMPONENT_TABLE_H
#define P32_COMPONENT_TABLE_H

#include "esp_log.h"
#include "p32_display.h"
#include "p32_audio.h"
#include "p32_sensors.h"
#include "p32_mood.h"

#ifdef __cplusplus
extern "C" {
#endif

// Global loop counter
extern uint64_t loopCount;

// Component init function table - core hardware only
esp_err_t (*componentInitTable[])(void) = {
    p32_display_init,
    p32_audio_init,
    p32_sensor_init,
    p32_mood_engine_init
};

// Component loop/act function table - core hardware only
void (*componentTable[])(void) = {
    p32_display_act,
    p32_audio_act,
    p32_sensor_act,
    p32_mood_engine_act
};

// Calculate component counts
#define componentInitCount (sizeof(componentInitTable))/4
#define componentCount (sizeof(componentTable))/4

// Function declarations
void p32_init_all_components(void);
void p32_loop_all_components(void);

#ifdef __cplusplus
}
#endif

#endif // P32_COMPONENT_TABLE_H