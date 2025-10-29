#ifndef P32_PERSONALITY_VARIANTS_H
#define P32_PERSONALITY_VARIANTS_H

#include "p32_personality_component.h"

#ifdef __cplusplus
extern "C" {
#endif

// Goblin Personality Functions
esp_err_t p32_goblin_personality_init(const p32_component_config_t* config, void** instance);
esp_err_t p32_goblin_personality_act(void* instance, const char* action, void* params);
esp_err_t p32_goblin_process_sensor_event(p32_personality_instance_t* personality, 
                                          p32_sensor_event_for_personality_t* event);
esp_err_t p32_register_goblin_personality(void);

// Zombie Personality Functions
esp_err_t p32_zombie_personality_init(const p32_component_config_t* config, void** instance);
esp_err_t p32_zombie_personality_act(void* instance, const char* action, void* params);
esp_err_t p32_zombie_process_sensor_event(p32_personality_instance_t* personality, 
                                          p32_sensor_event_for_personality_t* event);
esp_err_t p32_register_zombie_personality(void);

// Android Personality Functions  
esp_err_t p32_android_personality_init(const p32_component_config_t* config, void** instance);
esp_err_t p32_android_personality_act(void* instance, const char* action, void* params);
esp_err_t p32_android_process_sensor_event(p32_personality_instance_t* personality, 
                                           p32_sensor_event_for_personality_t* event);
esp_err_t p32_register_android_personality(void);

// Convenience function to register all personality variants
esp_err_t p32_register_all_personalities(void);

#ifdef __cplusplus
}
#endif

#endif // P32_PERSONALITY_VARIANTS_H