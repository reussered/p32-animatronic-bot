# OBSOLETE: Pin Assignment Strategy

**NOTE**: This document describes the previous JSON-based pin assignment system. The actual implementation uses JSON files to specify pin requirements (shared vs unique counts) but assigns pins dynamically in code.

## JSON Configuration vs Code Implementation

### JSON Bus Files - Pin Requirements Specification

JSON configuration files specify **how many pins are needed** and their types (shared vs unique):

```json
{
  "bus_type": "SPI",
  "shared_pins_count": 3,  // SCLK, MISO, MOSI
  "unique_pins_per_device": 2  // CS, MOSI (if needed)
}
```

### Code Implementation - Actual Pin Assignment

The actual pin assignments are handled entirely in code:

- **Static Assignment**: ALL pins for that specific bus are assigned in bus controller `init()` functions
- **Dynamic Selection**: The `act()` functions select which pins to use for current operations
- **No JSON Pin Mapping**: JSON files don't specify which physical pins to use - that's determined by the code

## Actual Pin Assignment Implementation

### Static Pin Assignment in Bus Controllers

Pins **for each device** are assigned incrementally in the `init()` functions of bus controllers. Each call to a bus controller's `init()` function assigns pins for one specific device. The bus controller maintains a pool of assigned pins and allocates from this pool as devices are initialized. **This dynamic assignment ensures the minimum number of pins are used** - pins are only allocated when devices are actually present, not pre-allocated for hypothetical maximum configurations.

- **SPI Bus Controller**: `spi_bus_init()` assigns pins for one SPI device (CS pin, etc.), called once per SPI device
- **I2S Bus Controller**: `i2s_bus_init()` assigns pins for one I2S device (DATA pins, etc.), called once per I2S device  
- **I2C Bus Controller**: `i2c_bus_init()` assigns shared SCL/SDA pins (called once total)
- **PWM Bus Controller**: `pwm_bus_init()` assigns pins for PWM channels as needed
- **ADC Bus Controller**: `adc_bus_init()` assigns pins for ADC channels as needed
- **GPIO Bus Controllers**: `gpio_pair_init()`, `gpio_single_init()` assign GPIO pins as devices request them

### Dynamic Pin Selection in act() Functions

The `act()` functions dynamically select which subset of statically-assigned pins to use for the current bus operation:

```cpp
void spi_bus_act(void) {
    // Select current device's pins from the statically assigned pool
    current_spi_config.cs_pin = static_cs_pins[current_device_index];
    current_spi_config.mosi_pin = static_mosi_pins[current_device_index];
    // Shared pins (SCLK, MISO) are already assigned, just configure for use
}
```

### Driver Usage Pattern

Device drivers use the current pin selection configured by the bus controller:

```cpp
void spi_device_act(void) {
    // Use the pins selected for current operation
    spi_transaction(cs_pin: current_spi_config.cs_pin,
                   mosi_pin: current_spi_config.mosi_pin);
}
```### Bus Types and Pin Assignment

The system supports multiple bus interface types with different pin allocation strategies:

1. **SPI Bus** (`bus_type: "SPI"`)
   - **Static Assignment**: SCLK, MISO pins assigned once in `spi_bus_init()`
   - **Dynamic Assignment**: CS, MOSI pins assigned per device in `spi_bus_act()`
   - **Usage**: Multiple SPI devices share clock lines but have unique CS pins

2. **I2S Bus** (`bus_type: "I2S"`)
   - **Static Assignment**: BCLK, WS pins assigned once in `i2s_bus_init()`
   - **Dynamic Assignment**: DATA pins assigned per device/direction in `i2s_bus_act()`
   - **Usage**: Audio devices share timing signals but have unique data lines

3. **I2C Bus** (`bus_type: "I2C"`)
   - **Static Assignment**: SCL, SDA pins assigned once in `i2c_bus_init()`
   - **Dynamic Assignment**: Address-based device selection in `i2c_bus_act()`
   - **Usage**: All devices share SDA/SCL lines, addressed via software

4. **ADC Bus** (`bus_type: "ADC"`)
   - **Static Assignment**: ADC input pins assigned in `adc_bus_init()`
   - **Dynamic Assignment**: Channel selection in `adc_bus_act()`
   - **Usage**: Analog-to-digital conversion with channel multiplexing

5. **PWM Bus** (`bus_type: "PWM"`)
   - **Static Assignment**: PWM output pins assigned in `pwm_bus_init()`
   - **Dynamic Assignment**: Duty cycle configuration in `pwm_bus_act()`
   - **Usage**: Pulse-width modulation for servo/LED control

6. **GPIO Pairs** (`bus_type: "GPIO_PAIR"`)
   - **Static Assignment**: GPIO pin pairs assigned in `gpio_pair_init()`
   - **Dynamic Assignment**: Input/output configuration in `gpio_pair_act()`
   - **Usage**: Simple digital I/O pairs with no shared resources

7. **GPIO Single** (`bus_type: "GPIO_SINGLE"`)
   - **Static Assignment**: Single GPIO pins assigned in `gpio_single_init()`
   - **Dynamic Assignment**: Pin state control in `gpio_single_act()`
   - **Usage**: Single GPIO pin control for simple devices (LEDs, relays, etc.)

### Hardware Validation Rules

- Static pin assignments are validated at initialization time
- Dynamic pin configurations are validated before use
- Pin conflicts are detected during bus controller initialization
- GPIO availability is checked against ESP32 chip specifications
- Runtime pin validation ensures safe hardware operation

---

**OBSOLETE**: This pin assignment strategy has been replaced by the dynamic system implemented in code. Pin assignments are now handled automatically at runtime without JSON configuration.
