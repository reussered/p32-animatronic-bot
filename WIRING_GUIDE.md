# ESP32-S3 to GC9A01 Display Wiring Guide
# Use this for stable connections instead of breadboard

## Dupont Jumper Method (Better than breadboard):
- Use female-to-female Dupont jumpers
- Connect directly from ESP32 pins to display pins
- No breadboard in between = more stable

## Pin Mapping (ESP32 → Display):

### Left Display:
ESP32 GPIO 34 → Display CS
ESP32 GPIO 38 → Display DC
ESP32 GPIO 39 → Display BL
ESP32 GPIO 42 → Display RST
ESP32 GPIO 35 → Display MOSI
ESP32 GPIO 37 → Display MISO
ESP32 GPIO 36 → Display SCLK
ESP32 3.3V   → Display VCC
ESP32 GND    → Display GND

### Right Display:
ESP32 GPIO 33 → Display CS
ESP32 GPIO 40 → Display DC
ESP32 GPIO 41 → Display BL
ESP32 GPIO 42 → Display RST (same as left)
ESP32 GPIO 35 → Display MOSI (same as left)
ESP32 GPIO 37 → Display MISO (same as left)
ESP32 GPIO 36 → Display SCLK (same as left)
ESP32 3.3V   → Display VCC
ESP32 GND    → Display GND

## Testing Steps:
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