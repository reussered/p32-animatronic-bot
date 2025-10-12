#pragma once

#include "p32_core.h"

// Software component function types
typedef esp_err_t (*p32_component_init_fn_t)(const p32_component_config_t* config, void** instance);
typedef esp_err_t (*p32_component_act_fn_t)(void* instance, const char* action, void* params);
typedef esp_err_t (*p32_component_cleanup_fn_t)(void* instance);

// Component action types
#define P32_ACTION_UPDATE        "update"
#define P32_ACTION_SET_MODE      "set_mode"
#define P32_ACTION_GET_VALUE     "get_value"
#define P32_ACTION_SET_PROPERTY  "set_property"
#define P32_ACTION_TRIGGER       "trigger"

// Generic action parameters structure
typedef struct {
    char key[32];
    char value[64];
    float numeric_value;
    void* data_ptr;
    size_t data_size;
} p32_action_params_t;

// Software component interface
typedef struct {
    char component_type[32];
    char component_name[64];
    p32_component_init_fn_t init_fn;
    p32_component_act_fn_t act_fn;
    p32_component_cleanup_fn_t cleanup_fn;
    void* instance;
    bool initialized;
    const p32_component_config_t* config;
} p32_software_component_t;

// Component registry system
typedef struct {
    p32_software_component_t components[P32_MAX_COMPONENTS];
    uint8_t component_count;
    bool registry_initialized;
} p32_component_registry_t;

// Component registry functions
esp_err_t p32_component_registry_init(void);
esp_err_t p32_component_register(const char* type, const char* name, 
                                  p32_component_init_fn_t init_fn,
                                  p32_component_act_fn_t act_fn,
                                  p32_component_cleanup_fn_t cleanup_fn);

// Component instance management
esp_err_t p32_component_create_instance(const char* type, const p32_component_config_t* config);
esp_err_t p32_component_send_action(const char* component_id, const char* action, p32_action_params_t* params);
esp_err_t p32_component_destroy_instance(const char* component_id);

// Component discovery and lookup
p32_software_component_t* p32_component_find_by_id(const char* component_id);
p32_software_component_t* p32_component_find_by_type(const char* component_type);
uint8_t p32_component_count_by_type(const char* component_type);

// Batch component operations
esp_err_t p32_component_init_all_from_config(void);
esp_err_t p32_component_update_all(void);
esp_err_t p32_component_shutdown_all(void);

// Built-in component registration
esp_err_t p32_register_builtin_components(void);

// Utility functions for action parameters
void p32_action_params_init(p32_action_params_t* params);
void p32_action_params_set_string(p32_action_params_t* params, const char* key, const char* value);
void p32_action_params_set_number(p32_action_params_t* params, const char* key, float value);
void p32_action_params_set_data(p32_action_params_t* params, const char* key, void* data, size_t size);