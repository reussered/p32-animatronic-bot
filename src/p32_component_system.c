#include "p32_component_system.h"

static const char* TAG = "P32_COMPONENTS";
static p32_component_registry_t g_component_registry = {0};

esp_err_t p32_component_registry_init(void) {
    ESP_LOGI(TAG, "Initializing component registry...");
    
    memset(&g_component_registry, 0, sizeof(p32_component_registry_t));
    g_component_registry.registry_initialized = true;
    
    // Register built-in components
    ESP_ERROR_CHECK(p32_register_builtin_components());
    
    ESP_LOGI(TAG, "Component registry initialized with %d components", 
             g_component_registry.component_count);
    
    return ESP_OK;
}

esp_err_t p32_component_register(const char* type, const char* name,
                                 p32_component_init_fn_t init_fn,
                                 p32_component_act_fn_t act_fn,
                                 p32_component_cleanup_fn_t cleanup_fn) {
    
    if (!g_component_registry.registry_initialized) {
        ESP_LOGE(TAG, "Registry not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (g_component_registry.component_count >= P32_MAX_COMPONENTS) {
        ESP_LOGE(TAG, "Component registry full");
        return ESP_ERR_NO_MEM;
    }
    
    p32_software_component_t* comp = &g_component_registry.components[g_component_registry.component_count];
    
    strncpy(comp->component_type, type, sizeof(comp->component_type) - 1);
    strncpy(comp->component_name, name, sizeof(comp->component_name) - 1);
    comp->init_fn = init_fn;
    comp->act_fn = act_fn;
    comp->cleanup_fn = cleanup_fn;
    comp->instance = NULL;
    comp->initialized = false;
    comp->config = NULL;
    
    g_component_registry.component_count++;
    
    ESP_LOGI(TAG, "Registered component: %s (%s)", name, type);
    return ESP_OK;
}

esp_err_t p32_component_create_instance(const char* type, const p32_component_config_t* config) {
    if (!type || !config) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Find component template by type
    p32_software_component_t* template = NULL;
    for (int i = 0; i < g_component_registry.component_count; i++) {
        if (strcmp(g_component_registry.components[i].component_type, type) == 0 &&
            !g_component_registry.components[i].initialized) {
            template = &g_component_registry.components[i];
            break;
        }
    }
    
    if (!template) {
        ESP_LOGE(TAG, "No available template for component type: %s", type);
        return ESP_ERR_NOT_FOUND;
    }
    
    // Initialize component instance
    esp_err_t ret = template->init_fn(config, &template->instance);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize component %s: %s", config->component_id, esp_err_to_name(ret));
        return ret;
    }
    
    template->config = config;
    template->initialized = true;
    
    ESP_LOGI(TAG, "Created instance: %s (%s)", config->component_id, type);
    return ESP_OK;
}

esp_err_t p32_component_send_action(const char* component_id, const char* action, p32_action_params_t* params) {
    p32_software_component_t* comp = p32_component_find_by_id(component_id);
    if (!comp || !comp->initialized) {
        ESP_LOGW(TAG, "Component not found or not initialized: %s", component_id);
        return ESP_ERR_NOT_FOUND;
    }
    
    if (!comp->act_fn) {
        ESP_LOGW(TAG, "Component %s has no action function", component_id);
        return ESP_ERR_NOT_SUPPORTED;
    }
    
    esp_err_t ret = comp->act_fn(comp->instance, action, params);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "Action %s failed on component %s: %s", action, component_id, esp_err_to_name(ret));
    }
    
    return ret;
}

p32_software_component_t* p32_component_find_by_id(const char* component_id) {
    if (!component_id) {
        return NULL;
    }
    
    for (int i = 0; i < g_component_registry.component_count; i++) {
        p32_software_component_t* comp = &g_component_registry.components[i];
        if (comp->initialized && comp->config && 
            strcmp(comp->config->component_id, component_id) == 0) {
            return comp;
        }
    }
    
    return NULL;
}

p32_software_component_t* p32_component_find_by_type(const char* component_type) {
    if (!component_type) {
        return NULL;
    }
    
    for (int i = 0; i < g_component_registry.component_count; i++) {
        p32_software_component_t* comp = &g_component_registry.components[i];
        if (strcmp(comp->component_type, component_type) == 0) {
            return comp;
        }
    }
    
    return NULL;
}

uint8_t p32_component_count_by_type(const char* component_type) {
    uint8_t count = 0;
    
    for (int i = 0; i < g_component_registry.component_count; i++) {
        if (strcmp(g_component_registry.components[i].component_type, component_type) == 0) {
            count++;
        }
    }
    
    return count;
}

esp_err_t p32_component_init_all_from_config(void) {
    ESP_LOGI(TAG, "Initializing all components from bot config...");
    
    extern p32_system_t g_p32_system;
    
    for (int i = 0; i < g_p32_system.current_bot.component_count; i++) {
        p32_component_config_t* config = &g_p32_system.current_bot.components[i];
        
        // Determine component type string
        const char* type_str = NULL;
        switch (config->type) {
            case COMPONENT_DISPLAY:
                type_str = "display";
                break;
            case COMPONENT_SENSOR:
                type_str = "sensor";
                break;
            case COMPONENT_SPEAKER:
                type_str = "speaker";
                break;
            case COMPONENT_LED:
                type_str = "led";
                break;
            case COMPONENT_ACTUATOR:
                type_str = "actuator";
                break;
            case COMPONENT_MICROPHONE:
                type_str = "microphone";
                break;
            default:
                ESP_LOGW(TAG, "Unknown component type: %d", config->type);
                continue;
        }
        
        esp_err_t ret = p32_component_create_instance(type_str, config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create component instance: %s", config->component_id);
            // Continue with other components
        }
    }
    
    ESP_LOGI(TAG, "Component initialization complete");
    return ESP_OK;
}

esp_err_t p32_component_update_all(void) {
    p32_action_params_t params;
    p32_action_params_init(&params);
    
    for (int i = 0; i < g_component_registry.component_count; i++) {
        p32_software_component_t* comp = &g_component_registry.components[i];
        if (comp->initialized && comp->config) {
            p32_component_send_action(comp->config->component_id, P32_ACTION_UPDATE, &params);
        }
    }
    
    return ESP_OK;
}

esp_err_t p32_component_shutdown_all(void) {
    ESP_LOGI(TAG, "Shutting down all components...");
    
    for (int i = 0; i < g_component_registry.component_count; i++) {
        p32_software_component_t* comp = &g_component_registry.components[i];
        if (comp->initialized && comp->cleanup_fn && comp->instance) {
            comp->cleanup_fn(comp->instance);
            comp->initialized = false;
            comp->instance = NULL;
        }
    }
    
    return ESP_OK;
}

// Action parameter utilities
void p32_action_params_init(p32_action_params_t* params) {
    if (params) {
        memset(params, 0, sizeof(p32_action_params_t));
    }
}

void p32_action_params_set_string(p32_action_params_t* params, const char* key, const char* value) {
    if (params && key && value) {
        strncpy(params->key, key, sizeof(params->key) - 1);
        strncpy(params->value, value, sizeof(params->value) - 1);
    }
}

void p32_action_params_set_number(p32_action_params_t* params, const char* key, float value) {
    if (params && key) {
        strncpy(params->key, key, sizeof(params->key) - 1);
        params->numeric_value = value;
    }
}

void p32_action_params_set_data(p32_action_params_t* params, const char* key, void* data, size_t size) {
    if (params && key && data) {
        strncpy(params->key, key, sizeof(params->key) - 1);
        params->data_ptr = data;
        params->data_size = size;
    }
}