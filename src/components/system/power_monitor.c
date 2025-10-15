// src/components/system/power_monitor.c
// Power Monitor Component - Battery voltage and current monitoring
// Component Type: SYSTEM_LEVEL (attached to torso subsystem)
// Timing: hitCount 200 (executes every 20 seconds)

#include "p32_component_config.h"
#include "p32_shared_state.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#ifdef ENABLE_SYSTEM_COMPONENTS

static const char *TAG = "power_monitor";
static adc_oneshot_unit_handle_t s_adc_handle = NULL;
static adc_cali_handle_t s_adc_cali_handle = NULL;

// Voltage divider for battery monitoring (adjust based on hardware)
#define VOLTAGE_DIVIDER_RATIO 2.0f  // 2:1 divider (R1=10k, R2=10k)

// Battery voltage thresholds (mV)
#define BATTERY_LOW_MV 3300
#define BATTERY_CRITICAL_MV 3000

// NO ARGUMENTS - Init function signature
esp_err_t power_monitor_init(void) {
    ESP_LOGI(TAG, "Initializing power monitor...");
    ESP_LOGI(TAG, "Battery voltage monitoring on GPIO 36");
    ESP_LOGI(TAG, "Thresholds: Low=%dmV, Critical=%dmV", BATTERY_LOW_MV, BATTERY_CRITICAL_MV);
    
    // Configure ADC for battery voltage monitoring
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    
    esp_err_t ret = adc_oneshot_new_unit(&init_config, &s_adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure ADC channel for GPIO 36 (ADC1_CH0)
    adc_oneshot_chan_cfg_t chan_config = {
        .atten = ADC_ATTEN_DB_11,  // 0-3.3V range
        .bitwidth = ADC_BITWIDTH_12,
    };
    
    ret = adc_oneshot_config_channel(s_adc_handle, ADC_CHANNEL_0, &chan_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC channel: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Initialize calibration
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };
    
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &s_adc_cali_handle);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Calibration not available, using raw values");
    }
    
    ESP_LOGI(TAG, "Power monitor initialized at loop count: %u", g_loopCount);
    
    return ESP_OK;
}

// NO ARGUMENTS - Act function signature
void power_monitor_act(void) {
    // hitCount: 200 → executes every 20 seconds
    
    if (s_adc_handle == NULL) {
        return;
    }
    
    // Read battery voltage
    int adc_raw = 0;
    esp_err_t ret = adc_oneshot_read(s_adc_handle, ADC_CHANNEL_0, &adc_raw);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to read ADC: %s", esp_err_to_name(ret));
        return;
    }
    
    // Convert to voltage (mV)
    int voltage_mv = 0;
    if (s_adc_cali_handle != NULL) {
        adc_cali_raw_to_voltage(s_adc_cali_handle, adc_raw, &voltage_mv);
    } else {
        // Rough conversion without calibration
        voltage_mv = (adc_raw * 3300) / 4095;
    }
    
    // Apply voltage divider correction
    voltage_mv = (int)(voltage_mv * VOLTAGE_DIVIDER_RATIO);
    
    // Convert to battery percentage (rough LiPo curve: 3.0V=0%, 4.2V=100%)
    int battery_percent = ((voltage_mv - 3000) * 100) / 1200;
    if (battery_percent < 0) battery_percent = 0;
    if (battery_percent > 100) battery_percent = 100;
    
    // Update shared state
    g_shared_state.battery_percent = (uint8_t)battery_percent;
    
    ESP_LOGD(TAG, "Power status at loop %u: %dmV (%d%%)",
             g_loopCount, voltage_mv, battery_percent);
    
    // Warn on low battery
    if (voltage_mv < BATTERY_CRITICAL_MV) {
        ESP_LOGE(TAG, "CRITICAL BATTERY: %dmV", voltage_mv);
    } else if (voltage_mv < BATTERY_LOW_MV) {
        ESP_LOGW(TAG, "Low battery: %dmV", voltage_mv);
    }
}

#endif // ENABLE_SYSTEM_COMPONENTS
