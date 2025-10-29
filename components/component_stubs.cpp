// P32 Component Stubs - Temporary implementations until real components are linked
// These will be replaced by actual archived implementations

#include "p32_component_tables.h"
#include "esp_log.h"

static const char* TAG = "STUBS";

// Stub implementations - return success, do nothing

esp_err_t p32_comp_heartbeat_init(void)
{
    ESP_LOGI(TAG, "STUB: heartbeat_init");
    return ESP_OK;
}

void p32_comp_heartbeat_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_network_monitor_init(void)
{
    ESP_LOGI(TAG, "STUB: network_monitor_init");
    return ESP_OK;
}

void p32_comp_network_monitor_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_left_eye_init(void)
{
    ESP_LOGI(TAG, "STUB: left_eye_init");
    return ESP_OK;
}

void p32_comp_left_eye_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_right_eye_init(void)
{
    ESP_LOGI(TAG, "STUB: right_eye_init");
    return ESP_OK;
}

void p32_comp_right_eye_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_nose_sensor_init(void)
{
    ESP_LOGI(TAG, "STUB: nose_sensor_init");
    return ESP_OK;
}

void p32_comp_nose_sensor_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_mouth_init(void)
{
    ESP_LOGI(TAG, "STUB: mouth_init");
    return ESP_OK;
}

void p32_comp_mouth_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_audio_init(void)
{
    ESP_LOGI(TAG, "STUB: audio_init");
    return ESP_OK;
}

void p32_comp_audio_act(void)
{
    // Silent stub
}

esp_err_t p32_comp_unknown_init(void)
{
    ESP_LOGI(TAG, "STUB: unknown_init");
    return ESP_OK;
}

void p32_comp_unknown_act(void)
{
    // Silent stub
}
