#ifndef P32_GPIO_PINS_H
#define P32_GPIO_PINS_H

#include "driver/gpio.h"

/**
 * P32 Animatronic Bot - Centralized GPIO Pin Definitions
 * =====================================================
 * 
 * This is the SINGLE AUTHORITATIVE SOURCE for all GPIO pin assignments.
 * ALL other files must #include this file and use these definitions.
 * 
 * DO NOT hardcode GPIO numbers anywhere else in the codebase!
 * 
 * Pin assignments are based on ESP32-S3-DevKitC-1 board layout.
 */

// ============================================================================
// SPI BUS - DISPLAY SYSTEM 
// ============================================================================

// SPI Bus - Shared pins for all devices on the bus
#define P32_SPI_BUS_MOSI_PIN    GPIO_NUM_13     // Master Out Slave In (Data)
#define P32_SPI_BUS_CLK_PIN     GPIO_NUM_14     // SPI Clock
#define P32_SPI_BUS_DC_PIN      GPIO_NUM_2      // Data/Command select
#define P32_SPI_BUS_RST_PIN     GPIO_NUM_21     // Reset

// SPI Device Chip Select pins (one per device)
#define P32_SPI_DEVICE_1_CS     GPIO_NUM_15     // SPI Device 1 - Left Eye
#define P32_SPI_DEVICE_2_CS     GPIO_NUM_16     // SPI Device 2 - Right Eye
#define P32_SPI_DEVICE_3_CS     GPIO_NUM_17     // SPI Device 3 - Mouth

// Device assignments
#define P32_LEFT_EYE_CS_PIN     P32_SPI_DEVICE_1_CS    // Left Eye uses SPI Device 1
#define P32_RIGHT_EYE_CS_PIN    P32_SPI_DEVICE_2_CS    // Right Eye uses SPI Device 2
#define P32_MOUTH_CS_PIN        P32_SPI_DEVICE_3_CS    // Mouth uses SPI Device 3

// ============================================================================
// I2S AUDIO SYSTEM
// ============================================================================

#define P32_I2S_BCLK_PIN        GPIO_NUM_4      // Bit Clock
#define P32_I2S_WS_PIN          GPIO_NUM_5      // Word Select (LRCK)
#define P32_I2S_DATA_PIN        GPIO_NUM_6      // Data Output

// ============================================================================
// SENSORS
// ============================================================================

#define P32_NOSE_SENSOR_TRIG    GPIO_NUM_9      // HC-SR04 Trigger
#define P32_NOSE_SENSOR_ECHO    GPIO_NUM_10     // HC-SR04 Echo

// ============================================================================
// STATUS/DEBUG
// ============================================================================

#define P32_STATUS_LED_PIN      GPIO_NUM_48     // RGB LED (ESP32-S3 onboard)
#define P32_DEBUG_LED_PIN       GPIO_NUM_2      // Fallback LED if RGB not available

// ============================================================================
// PIN VALIDATION MACROS
// ============================================================================

// Validate that pins don't conflict
#if (P32_SPI_BUS_MOSI_PIN == P32_I2S_BCLK_PIN) || \
    (P32_SPI_BUS_CLK_PIN == P32_I2S_WS_PIN) || \
    (P32_SPI_BUS_DC_PIN == P32_DEBUG_LED_PIN)
    #error "GPIO pin conflict detected! Check pin assignments."
#endif

// ============================================================================
// PIN USAGE SUMMARY (for documentation)
// ============================================================================
/*
 * Total GPIO Usage: 10 pins
 * 
 * SPI Displays (7 pins total):
 *   Shared Bus (4 pins):
 *     - GPIO 13: MOSI (data to all displays)
 *     - GPIO 14: CLK (clock to all displays)
 *     - GPIO 2:  DC (data/command to all displays)
 *     - GPIO 21: RST (reset to all displays)
 *   Individual CS (3 pins):
 *     - GPIO 15: CS Left Eye
 *     - GPIO 16: CS Right Eye  
 *     - GPIO 17: CS Mouth
 * 
 * I2S Audio (3 pins):
 *   - GPIO 4: BCLK
 *   - GPIO 5: WS
 *   - GPIO 6: DATA
 * 
 * Sensors (2 pins):
 *   - GPIO 9:  Nose Trigger
 *   - GPIO 10: Nose Echo
 * 
 * Status (1 pin):
 *   - GPIO 48: Status LED
 */

#endif // P32_GPIO_PINS_H