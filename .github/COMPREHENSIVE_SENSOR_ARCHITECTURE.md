# Comprehensive Sensor Architecture for P32 Animatronic Bot

## Overview

This document outlines how to integrate ALL common sensor types into the goblin (and other creatures) using a unified sensor management system.

---

## Architecture Layers

### Layer 1: Hardware Interface Drivers

Located in: `config/components/drivers/`

Each protocol gets its own driver component:

```
generic_adc_sensor_driver.src        → Reads analog values from ADC pins
generic_i2c_sensor_driver.src        → I2C protocol handler
generic_spi_sensor_driver.src        → SPI protocol handler  
generic_gpio_sensor_driver.src       → Simple GPIO (digital on/off)
generic_uart_sensor_driver.src       → Serial communication
generic_pwm_sensor_driver.src        → Pulse width measurement
generic_1wire_sensor_driver.src      → Dallas 1-Wire protocol
```

### Layer 2: Sensor-Specific Wrappers

Located in: `config/components/sensors/`

Each sensor type gets a wrapper that uses the appropriate driver:

```
ANALOG:
  - light_sensor_analog.src (LDR)
  - distance_sensor_analog.src (IR distance)

I2C:
  - temperature_sensor_i2c.src (BMP280)
  - humidity_sensor_i2c.src (DHT22 via I2C)
  - accelerometer_sensor_i2c.src (MPU6050)
  - proximity_sensor_i2c.src (APDS9960)

SPI:
  - accelerometer_sensor_spi.src (LSM6DS3)

GPIO:
  - motion_sensor_gpio.src (PIR)
  - limit_switch_gpio.src (physical contact)
  - collision_detector_gpio.src (bump switch)

UART:
  - gps_sensor_uart.src
  - air_quality_sensor_uart.src

PWM:
  - distance_sensor_pwm.src (HC-SR04 ultrasonic)
  - airflow_sensor_pwm.src

1-Wire:
  - temperature_sensor_1wire.src (DS18B20)
```

### Layer 3: SharedMemory Integration

All sensor readings published to unified SharedMemory system:

```cpp
// Shared data structure
class SensorReadings {
    struct Reading {
        uint8_t sensor_id;           // Unique ID
        uint16_t raw_value;          // 0-4095 for ADC, raw ticks for PWM, etc.
        int16_t calibrated_value;    // After conversion (temp in °C, distance in cm, etc.)
        int8_t validity;             // -1=error, 0=stale, 1=fresh
        uint32_t timestamp_ms;       // When last read
        char unit[8];                // "°C", "cm", "lux", etc.
    };
};
```

### Layer 4: Creature-Specific Sensor Aggregators

Located in: `config/bots/bot_families/goblins/sensors/`

Goblin-specific sensor configuration and behavior:

```
goblin_sensor_init.src              → Register which sensors goblin has
goblin_sensor_aggregator.src        → Collect all readings
goblin_sensor_behaviors.src         → React to sensor data
```

---

## Detailed Protocol Implementation

### 1. ADC (Analog Sensors)

**Pin Configuration:**
```cpp
// ESP32-S3 ADC pins: GPIO1-GPIO10, GPIO11-GPIO20
// Each can read 0-3.3V as 0-4095 (12-bit)
// Torso uses: ADC_0=GPIO1, ADC_1=GPIO2, etc.
// Head uses: Different ADC channels
```

**Driver Implementation:**
```cpp
// generic_adc_sensor_driver.src

#include "esp_adc/adc_oneshot.h"

static adc_oneshot_unit_handle_t adc_handle = NULL;
static const adc_channel_t SENSOR_CHANNELS[] = {
    ADC_CHANNEL_0,   // GPIO1 - Temperature
    ADC_CHANNEL_1,   // GPIO2 - Light Level
    ADC_CHANNEL_2    // GPIO3 - Humidity proxy
};

esp_err_t adc_driver_init(void) {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);
    
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,  // 0-3.3V range
    };
    
    for (int i = 0; i < 3; i++) {
        adc_oneshot_config_channel(adc_handle, SENSOR_CHANNELS[i], &config);
    }
    return ESP_OK;
}

uint16_t adc_read_channel(uint8_t channel) {
    int adc_reading = 0;
    adc_oneshot_read(adc_handle, SENSOR_CHANNELS[channel], &adc_reading);
    return (uint16_t)adc_reading;
}
```

**Sensor-Specific Wrapper:**
```cpp
// light_sensor_analog.src

#include "core/memory/SharedMemory.hpp"

static const uint8_t LIGHT_SENSOR_ID = 101;
static const uint16_t LIGHT_SENSOR_CHANNEL = 0;

esp_err_t light_sensor_analog_init(void) {
    return ESP_OK;  // ADC driver handles init
}

void light_sensor_analog_act(void) {
    uint16_t raw = adc_read_channel(LIGHT_SENSOR_CHANNEL);
    
    // Convert 0-4095 to 0-1000 lux (approximate)
    uint16_t lux = (raw / 4095) * 1000;
    
    SensorReadings* readings = GSM.read<SensorReadings>();
    if (readings) {
        readings->update_reading(LIGHT_SENSOR_ID, raw);
        readings->set_calibrated(LIGHT_SENSOR_ID, lux, "lux");
        GSM.write<SensorReadings>(*readings);
    }
}
```

---

### 2. I2C (Multi-Sensor Protocol)

**Pin Configuration:**
```cpp
// ESP32-S3: SDA=GPIO21, SCL=GPIO22 (standard)
// Can chain multiple sensors on same 2 wires
// Each has unique 7-bit address
```

**Driver Implementation:**
```cpp
// generic_i2c_sensor_driver.src

#include "driver/i2c_master.h"

static i2c_master_bus_handle_t bus_handle = NULL;

esp_err_t i2c_driver_init(void) {
    i2c_master_bus_config_t i2c_mux_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = 22,
        .sda_io_num = 21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    i2c_new_master_bus(&i2c_mux_config, &bus_handle);
    return ESP_OK;
}

esp_err_t i2c_write_register(uint8_t device_addr, uint8_t reg, uint8_t data) {
    uint8_t write_buf[2] = {reg, data};
    return i2c_master_transmit(device_handle, write_buf, sizeof(write_buf), -1);
}

esp_err_t i2c_read_register(uint8_t device_addr, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t write_buf[1] = {reg};
    return i2c_master_transmit_receive(device_handle, write_buf, 1, data, len, -1);
}
```

**Sensor-Specific Wrapper (BMP280 Temperature/Pressure):**
```cpp
// temperature_sensor_i2c.src - BMP280

#include "core/memory/SharedMemory.hpp"

static const uint8_t BMP280_ADDR = 0x76;
static const uint8_t TEMP_SENSOR_ID = 102;
static const uint8_t PRESSURE_SENSOR_ID = 103;

esp_err_t temperature_sensor_i2c_init(void) {
    // Send init sequence to BMP280
    i2c_write_register(BMP280_ADDR, 0xF5, 0x14);  // Config
    i2c_write_register(BMP280_ADDR, 0xF4, 0x2F);  // Control
    return ESP_OK;
}

void temperature_sensor_i2c_act(void) {
    uint8_t data[3];
    i2c_read_register(BMP280_ADDR, 0xF7, data, 3);  // Read temp/pressure
    
    // Convert raw bytes to temperature
    int32_t raw_temp = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    int16_t temp_c = raw_temp / 256;  // Simplified conversion
    
    SensorReadings* readings = GSM.read<SensorReadings>();
    if (readings) {
        readings->update_reading(TEMP_SENSOR_ID, raw_temp);
        readings->set_calibrated(TEMP_SENSOR_ID, temp_c, "°C");
        GSM.write<SensorReadings>(*readings);
    }
}
```

---

### 3. GPIO (Digital Sensors)

**Simple On/Off Sensors:**

```cpp
// motion_sensor_gpio.src - PIR Motion Detector

static const uint8_t PIR_PIN = GPIO_NUM_35;
static const uint8_t MOTION_SENSOR_ID = 104;

esp_err_t motion_sensor_gpio_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIR_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    return ESP_OK;
}

void motion_sensor_gpio_act(void) {
    uint32_t motion = gpio_get_level(PIR_PIN);  // 0 or 1
    
    SensorReadings* readings = GSM.read<SensorReadings>();
    if (readings) {
        readings->update_reading(MOTION_SENSOR_ID, motion ? 1 : 0);
        readings->set_calibrated(MOTION_SENSOR_ID, motion, "motion");
        GSM.write<SensorReadings>(*readings);
    }
}
```

---

### 4. PWM (Pulse Width Measurement)

**Ultrasonic Distance Sensor (HC-SR04):**

```cpp
// distance_sensor_pwm.src - HC-SR04

static const uint8_t TRIGGER_PIN = GPIO_NUM_33;
static const uint8_t ECHO_PIN = GPIO_NUM_34;
static const uint8_t DISTANCE_SENSOR_ID = 105;

esp_err_t distance_sensor_pwm_init(void) {
    gpio_config_t trigger_conf = {
        .pin_bit_mask = (1ULL << TRIGGER_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&trigger_conf);
    
    gpio_config_t echo_conf = {
        .pin_bit_mask = (1ULL << ECHO_PIN),
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&echo_conf);
    return ESP_OK;
}

void distance_sensor_pwm_act(void) {
    // Send 10µs trigger pulse
    gpio_set_level(TRIGGER_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIGGER_PIN, 0);
    
    // Measure echo pulse width
    uint32_t timeout = 23200;  // 23.2ms timeout
    uint32_t duration = pulseIn(ECHO_PIN, HIGH, timeout);
    
    // Convert to cm: duration_µs * 0.034 / 2
    uint16_t distance_cm = (duration * 34) / 2000;
    
    SensorReadings* readings = GSM.read<SensorReadings>();
    if (readings) {
        readings->update_reading(DISTANCE_SENSOR_ID, duration);
        readings->set_calibrated(DISTANCE_SENSOR_ID, distance_cm, "cm");
        GSM.write<SensorReadings>(*readings);
    }
}
```

---

### 5. 1-Wire (Dallas Temperature Sensors)

```cpp
// temperature_sensor_1wire.src - DS18B20

#include "OneWire.h"
#include "DallasTemperature.h"

static const uint8_t ONE_WIRE_PIN = GPIO_NUM_32;
static const uint8_t TEMP_1WIRE_SENSOR_ID = 106;

static OneWire oneWire(ONE_WIRE_PIN);
static DallasTemperature sensors(&oneWire);

esp_err_t temperature_sensor_1wire_init(void) {
    sensors.begin();
    return ESP_OK;
}

void temperature_sensor_1wire_act(void) {
    sensors.requestTemperatures();
    float temp_c = sensors.getTempCByIndex(0);
    int16_t temp_int = (int16_t)(temp_c * 100);  // Store as int (°C * 100)
    
    SensorReadings* readings = GSM.read<SensorReadings>();
    if (readings) {
        readings->update_reading(TEMP_1WIRE_SENSOR_ID, (uint16_t)temp_c);
        readings->set_calibrated(TEMP_1WIRE_SENSOR_ID, temp_int, "°C");
        GSM.write<SensorReadings>(*readings);
    }
}
```

---

## Sensor ID Registry

```cpp
// In config/shared_headers/sensor_ids.hpp

#define SENSOR_LIGHT_AMBIENT      101   // ADC
#define SENSOR_TEMPERATURE_BMP280 102   // I2C
#define SENSOR_PRESSURE_BMP280    103   // I2C
#define SENSOR_MOTION_PIR         104   // GPIO
#define SENSOR_DISTANCE_HC_SR04   105   // PWM
#define SENSOR_TEMPERATURE_DS18B  106   // 1-Wire
#define SENSOR_ACCEL_MPU6050      107   // I2C
#define SENSOR_GYRO_MPU6050       108   // I2C
#define SENSOR_PROXIMITY_APDS9960 109   // I2C
#define SENSOR_COLLISION_BUMP     110   // GPIO
```

---

## Goblin Sensor Configuration

**File:** `config/bots/bot_families/goblins/head/goblin_sensor_config.json`

```json
{
  "goblin_sensors": {
    "head_subsystem": {
      "enabled_sensors": [
        {
          "sensor_id": 101,
          "name": "ambient_light",
          "protocol": "adc",
          "pin": 1,
          "poll_interval_ms": 100,
          "calibration": {"min": 0, "max": 4095, "scale": 1000}
        },
        {
          "sensor_id": 104,
          "name": "motion_detector",
          "protocol": "gpio",
          "pin": 35,
          "poll_interval_ms": 50,
          "threshold": 1
        },
        {
          "sensor_id": 107,
          "name": "head_tilt",
          "protocol": "i2c",
          "i2c_address": 0x68,
          "poll_interval_ms": 20
        }
      ]
    },
    "torso_subsystem": {
      "enabled_sensors": [
        {
          "sensor_id": 102,
          "name": "body_temperature",
          "protocol": "i2c",
          "i2c_address": 0x76,
          "poll_interval_ms": 1000
        },
        {
          "sensor_id": 105,
          "name": "distance_front",
          "protocol": "pwm",
          "trigger_pin": 33,
          "echo_pin": 34,
          "poll_interval_ms": 100
        }
      ]
    }
  }
}
```

---

## Sensor Behavior Integration

**File:** `config/bots/bot_families/goblins/behaviors/sensor_reactions.src`

```cpp
// Goblin reacts to sensor data

#include "core/memory/SharedMemory.hpp"
#include "Mood.hpp"

void sensor_reactions_act(void) {
    SensorReadings* readings = GSM.read<SensorReadings>();
    Mood* mood = GSM.read<Mood>();
    
    if (!readings || !mood) return;
    
    // Light level affects mood
    uint16_t light = readings->get_calibrated(101);  // 0-1000 lux
    if (light < 100) {
        mood->components[MOOD_FEAR] += 30;  // Dark = scared
    } else if (light > 800) {
        mood->components[MOOD_HAPPINESS] += 20;  // Bright = happy
    }
    
    // Motion detected = curiosity spike
    if (readings->get_calibrated(104)) {
        mood->components[MOOD_CURIOSITY] += 50;
    }
    
    // Close proximity = anxiety
    uint16_t distance = readings->get_calibrated(105);  // cm
    if (distance < 30) {
        mood->components[MOOD_IRRITATION] += 40;
    }
    
    // Head tilt affects confidence
    int16_t head_angle = readings->get_calibrated(107);  // degrees
    if (head_angle > 45) {
        mood->components[MOOD_SADNESS] += 25;  // Tilted head = sad
    }
    
    GSM.write<Mood>(*mood);
}
```

---

## Integration Summary

```
All Sensors
    ↓
Specific Drivers (ADC, I2C, GPIO, PWM, 1-Wire, SPI, UART)
    ↓
Sensor Wrappers (light_sensor, temp_sensor, motion_sensor, etc.)
    ↓
SensorReadings (SharedMemory)
    ↓
Goblin Sensor Aggregator
    ↓
Mood/Behavior System
    ↓
Servos/Display Output
```

---

## Implementation Priority

**Phase 1 (Must Have):**
- ADC driver + light sensor
- GPIO driver + motion sensor
- I2C driver + temperature sensor

**Phase 2 (Should Have):**
- PWM driver + ultrasonic distance
- Accelerometer for head tilt

**Phase 3 (Nice to Have):**
- UART for external data
- SPI for advanced sensors
- 1-Wire for distributed temperature nodes

---

This gives you a complete, unified sensor system that feeds all data into SharedMemory, which drives mood, which drives servos and displays. Everything integrated! 🎯

