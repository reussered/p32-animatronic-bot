# Pin Mapping Abstraction System

## Overview
This system solves the problem of different manufacturers using different pin labels for the same electrical functions.

## File Structure

### 1. MakerNames.json
- **Purpose**: Master mapping between manufacturer labels and generic functions
- **Location**: `config/components/interfaces/MakerNames.json`
- **Contains**: Mappings for different manufacturer variants

### 2. Bus/Device Configurations (Use lowercase generic names)
- **SPI_BUS_VSPI**: Uses `data_input`, `data_output`, `clock`
- **SPI_DEVICE_X**: Uses `chip_select`, `data_command`, `reset`
- **Manufacturer Variant**: Specified in `device_config.manufacturer_variant`

### 3. Hardware Definitions
- **Purpose**: Describe electrical and physical specifications
- **Generic Pins**: Use lowercase functional names
- **Manufacturer Labels**: Listed for reference

## Your Display Mapping

**Your Display Labels → Generic Names:**
```
RST → reset
DC → data_command  
SCL → clock
SDA → data_input
CS → chip_select
VCC → power_positive
GND → power_ground
```

## Code Generation Flow

1. **Component Config** references generic pins: `"reset": 4`
2. **MakerNames.json** maps generic → manufacturer: `"reset": "RST"`
3. **Generated Code** uses manufacturer labels: `#define RST_PIN 4`
4. **Documentation** shows user-friendly wiring: "Connect ESP32 pin 4 to display RST"

## Benefits

- ✅ Same component configs work with any manufacturer
- ✅ Clear separation of electrical function vs. label
- ✅ Easy to add new manufacturers
- ✅ Generated code uses correct pin names
- ✅ Documentation matches actual hardware labels

## Usage Example

```json
// In spi_device_1.json
{
  "pins": {
    "chip_select": 15,     // Generic name
    "data_command": 2,     // Generic name  
    "reset": 4             // Generic name
  },
  "device_config": {
    "manufacturer_variant": "waveshare_gc9a01"  // Maps to your display
  }
}
```

The system automatically translates this to the correct pin labels (CS, DC, RST) for your specific display when generating code and documentation.