# Component Pin Aliasing Specification

## Problem Statement

Hardware components from different manufacturers often use different pin label names for the same electrical signals. For example, a GC9A01 display module might label the SPI data pin as:
- `SDA` (Serial Data)
- `MOSI` (Master Out Slave In)
- `SDI` (Serial Data In)
- `DIN` (Data In)

All of these refer to the **same physical pin** that connects to the ESP32's SPI MOSI signal.

## Solution: Pin Aliasing in Component JSON

Component hardware JSON files now include a `pin_mapping` section that defines:
1. **Standard name** - The canonical signal name (e.g., `SPI_MOSI`)
2. **Alternate names** - All manufacturer variations that map to this signal
3. **Function description** - What the pin does
4. **Required flag** - Whether the pin must be connected

## Example: GC9A01 Display

```json
{
  "pin_mapping": {
    "description": "GC9A01 displays have varying pin labels...",
    "pins": {
      "MOSI": {
        "standard_name": "SPI_MOSI",
        "alternate_names": ["SDA", "SDI", "DIN", "MOSI"],
        "function": "SPI Master Out Slave In (Data)",
        "required": true
      },
      "CLK": {
        "standard_name": "SPI_CLK",
        "alternate_names": ["SCK", "SCLK", "CLK"],
        "function": "SPI Clock",
        "required": true
      }
    }
  }
}
```

## Pin Mapping for GC9A01 Displays

### Your Display Pinout (SCK/SDA variant)

If your GC9A01 module exposes these pins:
```
SCK  ──> ESP32 SPI_CLK   (GPIO 14)
SDA  ──> ESP32 SPI_MOSI  (GPIO 13)
DC   ──> ESP32 GPIO      (GPIO 27 for left eye, GPIO 33 for right eye)
CS   ──> ESP32 GPIO      (GPIO 15 for left eye, GPIO 32 for right eye)
RST  ──> ESP32 GPIO      (GPIO 26 - shared)
VCC  ──> 3.3V
GND  ──> GND
```

### Standard SPI Signal Mapping

| Display Pin | Alternate Names | ESP32 Signal | Function |
|-------------|-----------------|--------------|----------|
| **CLK** | SCK, SCLK, CLK | `SPI_CLK` (GPIO 14) | SPI clock signal |
| **MOSI** | SDA, SDI, DIN, MOSI | `SPI_MOSI` (GPIO 13) | Data from ESP32 to display |
| **DC** | D/C, RS, A0 | GPIO (per device) | Data/Command select |
| **CS** | SS, CE | GPIO (per device) | Chip select (active low) |
| **RST** | RESET, RES | GPIO (shared) | Hardware reset |
| **BL** | BLK, LED | GPIO or 3.3V | Backlight control (optional PWM) |
| **VCC** | VDD, 3V3, +3.3V | 3.3V rail | Power supply |
| **GND** | VSS, 0V | Ground | Ground reference |

## Usage in Code Generation

When generating component code, the code generator should:

1. **Read the positioned component** (e.g., `goblineye_left.json`)
2. **Read the hardware definition** (e.g., `gc9a01_display.json`)
3. **Look up pin mappings** from the `pin_mapping` section
4. **Match user's physical pinout** to standard names
5. **Generate correct ESP32 pin assignments**

### Example Code Generation

**Input**: User says "My display has SCK, SDA, DC, CS, RST pins"

**Code Generator**:
1. Matches `SCK` → `SPI_CLK` (from alternate_names)
2. Matches `SDA` → `SPI_MOSI` (from alternate_names)
3. Matches `DC` → `DATA_COMMAND` (exact match)
4. Matches `CS` → `SPI_CS` (exact match)
5. Matches `RST` → `RESET` (exact match)

**Generated Code**:
```c
// Left eye display configuration
#define LEFT_EYE_CLK   14  // SPI_CLK (user's "SCK" pin)
#define LEFT_EYE_MOSI  13  // SPI_MOSI (user's "SDA" pin)
#define LEFT_EYE_DC    27  // Data/Command (user's "DC" pin)
#define LEFT_EYE_CS    15  // Chip Select (user's "CS" pin)
#define LEFT_EYE_RST   26  // Reset (user's "RST" pin)
```

## Common Manufacturer Variations

### Variation 1: SCK/SDA Style (Most Common)
**Pins**: `SCK`, `SDA`, `DC`, `CS`, `RST`, `VCC`, `GND`  
**Typical**: Generic Chinese modules, eBay/AliExpress sellers  
**Your displays**: ✓ This is what you have

### Variation 2: CLK/MOSI Style (Standard SPI)
**Pins**: `CLK`, `MOSI`, `DC`, `CS`, `RST`, `BL`, `VCC`, `GND`  
**Typical**: Waveshare, Adafruit, SparkFun modules

### Variation 3: RS Style (Legacy)
**Pins**: `SCLK`, `SDI`, `RS`, `CS`, `RESET`, `LED`, `VDD`, `GND`  
**Typical**: Older TFT displays, parallel-to-SPI conversions

## Benefits

1. **Universal Compatibility**: Works with any GC9A01 display variant
2. **Self-Documenting**: Pin mappings embedded in component definition
3. **Code Generation**: Automatic translation of user's physical pinout
4. **Validation**: Can detect invalid pin names and suggest alternates
5. **Educational**: Users learn what each pin actually does

## Future Enhancements

### Automatic Pin Detection
```python
def detect_display_variant(user_pins):
    """Match user's pin names to known variations."""
    for variant in component["pin_mapping"]["common_manufacturer_variations"]:
        if set(user_pins) == set(variant["pins_exposed"]):
            return variant["description"]
    return "Custom pinout"
```

### Interactive Pin Mapper
```
$ python tools/map-display-pins.py

GC9A01 Pin Mapper
-----------------
Enter the pin labels on your display (comma-separated):
> SCK, SDA, DC, CS, RST, VCC, GND

✓ Detected variation: "Most common pinout"
✓ Pin mapping validated

ESP32 Wiring:
  SCK  (pin 1) → GPIO 14 (SPI_CLK)
  SDA  (pin 2) → GPIO 13 (SPI_MOSI)
  DC   (pin 3) → GPIO 27 (Data/Command)
  CS   (pin 4) → GPIO 15 (Chip Select)
  RST  (pin 5) → GPIO 26 (Reset - shared)
  VCC  (pin 6) → 3.3V
  GND  (pin 7) → GND
```

## Implementation Notes

### JSON Schema Addition

```json
{
  "pin_mapping": {
    "type": "object",
    "properties": {
      "description": {"type": "string"},
      "pins": {
        "type": "object",
        "patternProperties": {
          ".*": {
            "type": "object",
            "properties": {
              "standard_name": {"type": "string"},
              "alternate_names": {"type": "array", "items": {"type": "string"}},
              "function": {"type": "string"},
              "required": {"type": "boolean"}
            }
          }
        }
      },
      "common_manufacturer_variations": {
        "type": "object"
      }
    }
  }
}
```

### Validation Rules

1. **Alternate names must be unique** - No pin can appear in multiple alternate_names arrays
2. **At least one alternate name** - Every pin must have at least one alternate
3. **Standard name is canonical** - Use standard_name for all internal references
4. **Required pins enforced** - Code generation fails if required pins not mapped

## Related Documentation

📘 **[Component JSON Requirements](component-json-requirements.md)** - Base JSON structure  
📘 **[Goblin Full Interconnection Diagram](goblin-full-interconnection-diagram.md)** - Physical wiring  
📘 **[Interface GPIO Assignment Spec](interface-gpio-assignment-spec.md)** - ESP32 pin assignments

## Summary

**Problem**: Same electrical signal, different manufacturer pin labels  
**Solution**: Define all alternate names in component JSON  
**Benefit**: Universal compatibility with automatic code generation  
**Your Case**: `SCK`/`SDA` pins now correctly mapped to ESP32 SPI signals

**This makes the P32 system work with ANY GC9A01 display variant!** 🎉
