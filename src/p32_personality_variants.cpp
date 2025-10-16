#include "p32_component_config.hpp"

#ifdef ENABLE_PERSONALITY_VARIANTS
#include "p32_personality_variants.hpp"

static const char* TAG = "P32_PERSONALITY_VARIANTS";

esp_err_t p32_register_all_personalities(void) {
    ESP_LOGI(TAG, "Registering all personality variants...");
    
    esp_err_t ret;
    
    // Register goblin personality
    ret = p32_register_goblin_personality();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register goblin personality: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Register zombie personality  
    ret = p32_register_zombie_personality();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register zombie personality: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Register android personality
    ret = p32_register_android_personality();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register android personality: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "All personality variants registered successfully");
    return ESP_OK;
}

#endif // ENABLE_PERSONALITY_VARIANTS
