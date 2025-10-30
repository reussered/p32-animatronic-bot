# Dynamic Pin Assignment System
## Overview
The P32 Animatronic Bot now uses a dynamic pin assignment system where GPIO pins are assigned at runtime during component initialization, rather than at build time. This allows multiple components to share the same assignable pin arrays without conflicts.
## Key Principles
### Static Pin Assignments
- **Hardware-First Design**: Pin assignments must reflect actual physical chip wiring
- **No Runtime Changes**: Once assigned during init(), pins remain assigned for program lifetime
- **Failure on Changes**: If pin assignments change, the system has failed - assignments should be stable
- **Physical Reality**: Deallocating pins makes no conceptual sense - hardware connections are fixed
### Runtime Assignment Process
- **Init-Time Only**: All pin assignments happen during component initialization
- **Round-Robin Execution**: Components assign pins in the order their init() functions execute
- **Conflict Prevention**: `assigned_pins` array prevents multiple components from claiming same pin
- **Permanent Assignment**: Assigned pins are never released - they represent physical hardware connections
### 3. Component Implementation Pattern
#### Init Function
```cpp
esp_err_t component_init(void) {
    // Assign pin dynamically
    assigned_pin = assign_pin(gpio_assignable, gpio_assignable_count);
    if (assigned_pin == -1) {
        return ESP_FAIL; // No pins available
    }
    // Configure the assigned pin
    // ... pin configuration code ...
    return ESP_OK;
}
```
#### Act Function
```cpp
void component_act(void) {
    // Use the pin assigned during init
    if (assigned_pin != -1) {
        // ... use assigned_pin for I/O operations ...
    }
}
```
## Benefits
1. **Conflict Prevention**: Multiple components can use the same assignable arrays without pin conflicts
2. **Runtime Flexibility**: Pin assignments happen during initialization, allowing dynamic configuration
3. **Round-Robin Execution**: Components assign pins when their init() functions run, respecting execution order
4. **No Duplicate Removal**: Dispatch tables maintain all components since each needs unique pin assignments
## Bus Type Arrays
Each controller provides assignable pin arrays for different bus types:
- `spi_assignable[]` - Pins assignable to SPI bus
- `i2c_assignable[]` - Pins assignable to I2C bus
- `i2s_assignable[]` - Pins assignable to I2S bus
- `adc_assignable[]` - Pins with ADC capability
- `pwm_assignable[]` - Pins assignable to PWM
- `gpio_assignable[]` - All assignable GPIO pins
## Example: Dynamic GPIO Driver
See `config/components/dynamic_gpio_driver.src` for a complete example that:
1. Assigns a GPIO pin during `init()`
2. Uses the assigned pin during `act()` to toggle an LED
3. Demonstrates proper error handling for pin assignment failures
## Controller Compatibility

The system works with all ESP32 controllers:
- ESP32-S3-DevKitC-1 (39 assignable GPIO pins exposed on headers)
- ESP32-S3-R8N16 (24 assignable pins)
- ESP32-S3-R2N2 (24 assignable pins)
- ESP32-C3 (16 assignable pins)

Each controller generates its own pin assignment header with appropriate pin arrays and `assigned_pins` tracking.
