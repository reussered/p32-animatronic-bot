# ESP32-S3 to GC9A01 Display Wiring Guide

##  **IMPORTANT**: Pins are now assigned DYNAMICALLY at runtime

This guide shows the PHYSICAL connections, but actual GPIO numbers are assigned automatically.

## Dupont Jumper Method (Better than breadboard)

- Use female-to-female Dupont jumpers
- Connect directly from ESP32 pins to display pins
- No breadboard in between = more stable

## Dynamic Pin Assignment System

- **Runtime Assignment**: GPIO pins are assigned during component initialization
- **No Hardcoded Pins**: Configuration uses `"DYNAMICALLY_ASSIGNED"` markers
- **Physical Connections**: The wiring below shows logical connections, not specific GPIO numbers
- **Automatic Resolution**: The system automatically assigns available pins to components

## Logical Pin Mapping (Component  Display)

 **GPIO numbers below are EXAMPLES only - actual assignment happens at runtime!**

### Left Display

ESP32 GPIO [DYNAMIC]  Display CS  (Chip Select)
ESP32 GPIO [DYNAMIC]  Display DC  (Data/Command)
ESP32 GPIO [DYNAMIC]  Display BL  (Backlight)
ESP32 GPIO [DYNAMIC]  Display RST (Reset)
ESP32 GPIO [DYNAMIC]  Display MOSI (SPI Data Out)
ESP32 GPIO [DYNAMIC]  Display MISO (SPI Data In)
ESP32 GPIO [DYNAMIC]  Display SCLK (SPI Clock)
ESP32 3.3V    Display VCC
ESP32 GND     Display GND

### Right Display

ESP32 GPIO [DYNAMIC]  Display CS  (Chip Select)
ESP32 GPIO [DYNAMIC]  Display DC  (Data/Command)
ESP32 GPIO [DYNAMIC]  Display BL  (Backlight)
ESP32 GPIO [DYNAMIC]  Display RST (Reset - shared with left display)
ESP32 GPIO [DYNAMIC]  Display MOSI (SPI Data Out - shared with left display)
ESP32 GPIO [DYNAMIC]  Display MISO (SPI Data In - shared with left display)
ESP32 GPIO [DYNAMIC]  Display SCLK (SPI Clock - shared with left display)
ESP32 3.3V    Display VCC
ESP32 GND     Display GND

## Testing Steps

1. Upload wiring_test.ino to ESP32
2. Check serial monitor for backlight tests
3. Verify displays light up when backlights are enabled
4. If backlights work, wiring is stable
5. Then try the full P32 system

## Alternative: Custom PCB

For production/stable setup:

- Design PCB with ESP32-S3 and dual GC9A01 connectors
- Eliminates all wiring issues
- Professional mounting points
