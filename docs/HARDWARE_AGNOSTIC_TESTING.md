# Hardware-Agnostic Test Specifications

This document defines test specifications that are **hardware-agnostic** but have **hardware-specific implementations**.

## Architecture Pattern

```
Test Specification (What to test)    →    Hardware Implementation (How to test)
├── "display_colors"                 →    ESP32: SPI RGB565 patterns
├── "animation_smooth"               →    ESP32: SPI transfer timing  
├── "eye_tracking"                   →    ESP32: Servo motor control
└── "mock_sensors"                   →    ESP32: Override g_shared_state
```

## Core Test Specifications

### Display Tests
- **`"display_colors"`** - Test color accuracy and display functionality
  - ESP32-S3: Direct SPI RGB565 color patterns to GC9A01
  - Raspberry Pi: Framebuffer RGB888 color patterns  
  - Simulator: OpenGL color quads

- **`"display_brightness"`** - Test brightness control
  - ESP32-S3: PWM backlight control via GPIO
  - Raspberry Pi: /sys/class/backlight controls
  - Simulator: Fragment shader intensity

### Animation Tests  
- **`"animation_smooth"`** - Test frame rate and smoothness
  - ESP32-S3: Measure SPI transfer timing with esp_timer
  - Raspberry Pi: Measure DRM/KMS vsync timing
  - Simulator: OpenGL frame timing queries

- **`"animation_blink"`** - Test basic blink animation
  - ESP32-S3: Verify eye_display_t state transitions
  - Raspberry Pi: Verify animation state machine
  - Simulator: Verify virtual eye state

### Hardware Tests
- **`"spi_bus"`** - Test SPI communication
  - ESP32-S3: SPI loopback tests, transfer validation
  - Raspberry Pi: /dev/spidev read/write tests
  - Simulator: Mock SPI transaction logging

- **`"gpio_pins"`** - Test GPIO functionality  
  - ESP32-S3: gpio_set_level/gpio_get_level validation
  - Raspberry Pi: /sys/class/gpio manipulation
  - Simulator: Virtual GPIO state tracking

### Sensor Tests
- **`"mock_sensors"`** - Use synthetic sensor data
  - ESP32-S3: Override g_shared_state.distance_cm
  - Raspberry Pi: Override shared memory sensor values
  - Simulator: Generate synthetic sensor patterns

- **`"proximity_sensor"`** - Test distance measurement
  - ESP32-S3: Validate HC-SR04 ultrasonic readings
  - Raspberry Pi: Validate I2C ToF sensor readings  
  - Simulator: Synthetic distance simulation

## Test Control Examples

```cpp
// Start hardware-agnostic test
g_TestFramework.start_test("display_colors");

// Components check for tests (hardware-agnostic)
if (IS_TEST_ACTIVE("display_colors")) {
    // Hardware-specific implementation
    #ifdef ESP32_PLATFORM
        gc9a01_render_test_pattern(0xF800); // ESP32: SPI red pattern
    #elif RASPBERRY_PI  
        framebuffer_fill_color(0xFF0000);   // RPi: Framebuffer red
    #elif SIMULATOR
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Simulator: OpenGL red
    #endif
}
```

## Benefits

1. **Hardware Portability**: Same test names work across platforms
2. **Implementation Flexibility**: Each platform optimizes for its hardware
3. **Test Consistency**: Same logical tests, different physical implementations
4. **Debugging Clarity**: Know exactly which test is running and how
5. **Modular Testing**: Enable/disable specific test categories dynamically

## Usage

```cpp
// Enable specific tests
g_TestFramework.start_test("display_colors");
g_TestFramework.start_test("animation_smooth");

// Check if testing
if (g_TestFramework.test_display()) {
    // Hardware-specific display testing code
}

// Disable all tests (return to production)
g_TestFramework.clear_all_tests();
```