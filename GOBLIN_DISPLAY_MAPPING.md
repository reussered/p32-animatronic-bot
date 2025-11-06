# Goblin Display System - Complete Mapping Reference

Display Type | Color Schema | Resolution | Bus Interface | Power | Use Case | Config File
---|---|---|---|---|---|---
GC9A01 | RGB565 | 240x240 | SPI | 0.3W | Eyes (left/right) | gc9a01.json
ST7789 | RGB565 | 240x320 | SPI | 0.25W | Rectangular TFT | st7789.json
ST7735 | RGB565 | 160x128 | SPI | 0.08W | Small rectangular | st7735.json
ILI9341 | RGB666 (18-bit) | 480x320 | SPI | 0.8W | Medium-large displays | ili9341.json
ILI9486 | RGB888 (24-bit) | 480x320 | SPI | 1.2W | Large displays | ili9486.json
ILI9481 | RGB888 (24-bit) | 800x480 | SPI/Parallel | 1.5W | XL displays | ili9481.json
ILI9488 | RGB888 (24-bit) | 480x320 | SPI/Parallel | 1.0W | High-res displays | ili9488.json
RA8875 | RGB666 (18-bit) | 800x480 | SPI | 1.15W | Large with acceleration | ra8875_tft_800x480.json
SSD1306 | MONOCHROME | 128x64 | I2C | 0.06W | Text/status only | ssd1306.json
SSD1331 | RGB565 | 96x64 | SPI | 0.20W | Micro RGB OLED | ssd1331_micro_rgb_oled.json
SSD1351 | RGB565 | 128x128 | SPI | 0.35W | Small RGB OLED | ssd1351_rgb_oled.json
GMT12864 | MONOCHROME | 128x64 | I2C | 0.05W | Simple LCD | gmt12864.json


DETAILED DISPLAY SPECIFICATIONS
==============================

## PRIMARY GOBLIN DISPLAYS (Eyes)

### GC9A01 - RECOMMENDED FOR GOBLIN EYES
- **Color Schema:** RGB565 (16-bit, 65K colors)
- **Resolution:** 240x240 pixels (perfect circular)
- **Bus:** SPI (80 MHz capable)
- **Power:** ~0.3W (backlight 0.15W + driver 0.15W)
- **Physical:** 1.28" diameter round display
- **Mounting:** Fits 26mm mounting ring
- **Frame Rate:** 60 FPS capable
- **Use Case:** Left eye, Right eye (dual displays on single SPI bus with separate CS pins)
- **Config Reference:** `config/components/hardware/gc9a01.json`

GPIO Allocation (per display):
  - SPI CLK: GPIO 35 (shared, SPI bus)
  - SPI MOSI: GPIO 37 (shared, SPI bus)
  - SPI MISO: GPIO 36 (shared, SPI bus)
  - CS (chip select): GPIO 10 (left eye), GPIO 8 (right eye)
  - DC (data/command): GPIO 11 (left eye), GPIO 9 (right eye)
  - RST (reset): GPIO 12 (left eye), GPIO 39 (right eye)

Total GPIO for dual GC9A01: 9 pins (3 shared + 6 unique)


## ALTERNATIVE DISPLAYS (if GC9A01 unavailable)

### ST7789 - Small Rectangular Alternative
- **Color Schema:** RGB565 (16-bit)
- **Resolution:** 240x320 pixels (portrait, tall)
- **Bus:** SPI (80 MHz)
- **Power:** 0.25W
- **Physical:** 2.0" display, rectangular
- **Use Case:** Portrait-oriented eye display or mouth display
- **Aspect:** Taller than wide (good for surprised/shocked expressions)

### ST7735 - Budget Rectangular
- **Color Schema:** RGB565 (16-bit)
- **Resolution:** 160x128 pixels
- **Bus:** SPI (10 MHz)
- **Power:** 0.08W (very low)
- **Physical:** 1.77" diagonal
- **Use Case:** Secondary displays, status screens
- **Advantage:** Lowest power consumption for color display


## MEDIUM-LARGE DISPLAYS (Mouth, Status)

### ILI9341 - 3.5" Professional
- **Color Schema:** RGB666 (18-bit, 262K colors)
- **Resolution:** 480x320 pixels
- **Bus:** SPI (40 MHz max)
- **Power:** 0.8W
- **Physical:** 3.5" diagonal, 76.8mm × 57.6mm
- **Use Case:** Mouth display, detailed expressions, teeth/tongue animation
- **Brightness:** 350 cd/m2
- **Frame Rate:** 60 Hz

### RA8875 - Large with Graphics Acceleration
- **Color Schema:** RGB666 (18-bit)
- **Resolution:** 800x480 pixels
- **Bus:** SPI or 8-bit parallel (SPI: 25 MHz max)
- **Power:** 1.15W
- **Physical:** 7.0" diagonal
- **Use Case:** Full head display overlay, complex animations
- **Unique Feature:** Built-in graphics acceleration (can reduce CPU load)
- **Advantage:** Parallel interface option for higher bandwidth


## OLED MICRO DISPLAYS (Accent/Status)

### SSD1331 - 0.95" RGB OLED (MICRO)
- **Color Schema:** RGB565 (16-bit, 65K colors)
- **Resolution:** 96x64 pixels
- **Bus:** SPI (25 MHz)
- **Power:** 0.20W
- **Physical:** 0.95" diagonal, 18.66mm × 11.9mm
- **Use Case:** Nostril glow, accent lighting, status indicator
- **Advantage:** OLED (true black, instant response, no backlight)

### SSD1351 - 1.27" RGB OLED (SMALL)
- **Color Schema:** RGB565 (16-bit)
- **Resolution:** 128x128 pixels
- **Bus:** SPI
- **Power:** 0.35W
- **Physical:** 1.27" diagonal, 28.35mm × 28.35mm
- **Use Case:** Secondary feature display (nostril, mouth accent)
- **Advantage:** Square, perfect for eyes if GC9A01 unavailable

### SSD1306 - 0.96" MONOCHROME OLED
- **Color Schema:** Monochrome (1-bit, black/white only)
- **Resolution:** 128x64 pixels
- **Bus:** I2C (100 kHz standard, 400 kHz fast mode)
- **Power:** 0.06W (ultra-low)
- **Physical:** 0.96" diagonal
- **Use Case:** Status display, battery level, debug info
- **Advantage:** Lowest power, I2C bus (no SPI needed)


## LARGE DISPLAYS (Full-Screen Overlay)

### ILI9486 - 4" Large Color
- **Color Schema:** RGB888 (24-bit, 16.7M colors)
- **Resolution:** 480x320 pixels
- **Bus:** SPI or 8-bit parallel
- **Power:** 1.2W
- **Physical:** 4.0" diagonal
- **Use Case:** Large decorative overlay (body projection, full-face display)

### ILI9481 - 5" XL Color
- **Color Schema:** RGB888 (24-bit)
- **Resolution:** 800x480 pixels
- **Bus:** SPI or 8-bit parallel
- **Power:** 1.5W
- **Physical:** 5.0" diagonal
- **Use Case:** Full goblin head texture/overlay (ambitious, high power)


## GOBLIN HEAD RECOMMENDED CONFIGURATION

### Standard Face Setup (Dual Eyes + Mouth)
Component | Display | Resolution | Bus | Power | GPIO Cost
---|---|---|---|---|---
Left Eye | GC9A01 | 240x240 | SPI | 0.3W | 6 (shared clock)
Right Eye | GC9A01 | 240x240 | SPI | 0.3W | 3 (separate CS/DC/RST)
Mouth | ILI9341 | 480x320 | SPI | 0.8W | 3 (separate CS/DC/RST)
Status (optional) | SSD1306 | 128x64 | I2C | 0.06W | 0 (shared I2C)
**Total** | — | — | **SPI + I2C** | **1.46W** | **12 GPIO**

### Budget Face Setup (Dual Eyes Only)
Component | Display | Resolution | Bus | Power | GPIO Cost
---|---|---|---|---|---
Left Eye | GC9A01 | 240x240 | SPI | 0.3W | 6
Right Eye | GC9A01 | 240x240 | SPI | 0.3W | 3
**Total** | — | — | **SPI** | **0.6W** | **9 GPIO**

### Premium Face Setup (Dual Eyes + Mouth + Nostril Accents)
Component | Display | Resolution | Bus | Power | GPIO Cost
---|---|---|---|---|---
Left Eye | GC9A01 | 240x240 | SPI | 0.3W | 6
Right Eye | GC9A01 | 240x240 | SPI | 0.3W | 3
Mouth | ILI9341 | 480x320 | SPI | 0.8W | 3
Left Nostril | SSD1331 | 96x64 | SPI | 0.2W | 3
Right Nostril | SSD1331 | 96x64 | SPI | 0.2W | 1 (shared CS pin with timing)
Status | SSD1306 | 128x64 | I2C | 0.06W | 0
**Total** | — | — | **SPI + I2C** | **1.86W** | **16 GPIO**


## BUS ARCHITECTURE SUMMARY

### SPI Bus (Primary, fastest)
- **Shared clock pins:** GPIO 35 (CLK), GPIO 37 (MOSI), GPIO 36 (MISO)
- **Per-device unique pins:** CS (chip select), DC (data/command), RST (reset)
- **Max devices:** 7-10 on single SPI bus (limited by unique CS pins)
- **Speed:** 80 MHz capable (GC9A01), 40 MHz typical (ILI9341)
- **Best for:** Dual eyes (fast animation), mouth display (complex graphics)

### I2C Bus (Secondary, slower but efficient)
- **Shared pins:** GPIO 21 (SDA), GPIO 22 (SCL)
- **Max devices:** 127 on single bus (address-based)
- **Speed:** 100 kHz (standard), 400 kHz (fast mode)
- **Best for:** Status displays, low-bandwidth sensors, optional extras
- **Advantage:** Only 2 GPIO pins for unlimited devices

### Parallel Bus (Rare, high bandwidth)
- **Used by:** ILI9481 (5" XL), RA8875 (with parallel option)
- **GPIO cost:** 8-16 pins for data + control
- **Advantage:** Highest bandwidth (rarely needed for goblin)
- **Disadvantage:** Expensive GPIO-wise
- **Use case:** Full-screen overlay displays (ambitious)


## COLOR SCHEMA QUICK REFERENCE

### RGB565 (16-bit, 65K colors) - STANDARD
- Red: 5 bits (0-31)
- Green: 6 bits (0-63)
- Blue: 5 bits (0-31)
- Size: 2 bytes per pixel
- Displays: GC9A01, ST7789, ST7735, SSD1331, SSD1351
- Best for: Smooth gradients, photo-realistic eyes
- Example: White = 0xFFFF, Black = 0x0000, Red = 0xF800

### RGB666 (18-bit, 262K colors) - HIGH-COLOR
- Red: 6 bits (0-63)
- Green: 6 bits (0-63)
- Blue: 6 bits (0-63)
- Size: 3 bytes per pixel (18 bits packed)
- Displays: ILI9341, RA8875
- Best for: High-quality graphics, text rendering
- Better color accuracy than RGB565

### RGB888 (24-bit, 16.7M colors) - TRUE COLOR
- Red: 8 bits (0-255)
- Green: 8 bits (0-255)
- Blue: 8 bits (0-255)
- Size: 3 bytes per pixel
- Displays: ILI9486, ILI9488, ILI9481
- Best for: Photo-realistic rendering, complex overlays
- Rarely needed for animatronic expressions

### MONOCHROME (1-bit, 2 colors) - TEXT/STATUS
- Black: 0, White: 1
- Size: 1 bit per pixel (8 pixels per byte)
- Displays: SSD1306, GMT12864
- Best for: Status text, simple debug info
- Ultra-low power, no color capability


## CONFIGURATION INTEGRATION

### How to specify display in component config:

```json
{
    "name": "goblin_left_eye",
    "hardware": {
        "display_type": "GC9A01",
        "resolution": "240x240",
        "color_schema": "RGB565",
        "bus_type": "SPI",
        "spi_device": "SPI_DEVICE_1",
        "cs_pin": "GPIO_10",
        "dc_pin": "GPIO_11",
        "rst_pin": "GPIO_12",
        "backlight_pin": "GPIO_DYNAMIC"
    }
}
```

### Example: Goblin head with standard configuration

```json
{
    "name": "goblin_head_displays",
    "displays": [
        {
            "name": "left_eye",
            "type": "GC9A01",
            "resolution": "240x240",
            "color_schema": "RGB565",
            "bus": "SPI",
            "cs_pin": "GPIO_10",
            "dc_pin": "GPIO_11",
            "rst_pin": "GPIO_12"
        },
        {
            "name": "right_eye",
            "type": "GC9A01",
            "resolution": "240x240",
            "color_schema": "RGB565",
            "bus": "SPI",
            "cs_pin": "GPIO_8",
            "dc_pin": "GPIO_9",
            "rst_pin": "GPIO_39"
        },
        {
            "name": "mouth",
            "type": "ILI9341",
            "resolution": "480x320",
            "color_schema": "RGB666",
            "bus": "SPI",
            "cs_pin": "GPIO_5",
            "dc_pin": "GPIO_6",
            "rst_pin": "GPIO_7"
        }
    ]
}
```


## POWER AND GPIO SUMMARY TABLE

Setup | Display Config | Total Power | Total GPIO | Recommended Supply | Frame Rate
---|---|---|---|---|---
Budget | 2× GC9A01 only | 0.6W | 9 | 5V 1A | 60 FPS
Standard | 2× GC9A01 + ILI9341 | 1.46W | 12 | 5V 1-2A | 60 FPS
Premium | 2× GC9A01 + ILI9341 + 2× SSD1331 + SSD1306 | 1.86W | 16 | 5V 2A | 60 FPS
XL | 2× GC9A01 + ILI9341 + RA8875 + SSD1306 | 3.0W | 15 | 5V 3A | 30 FPS


## FILE REFERENCES

All display component definitions located in:
`config/components/hardware/`

Key files:
- `gc9a01.json` - Goblin eye display (primary)
- `st7789.json` - Rectangular alternative
- `st7735.json` - Budget color display
- `ili9341.json` - Medium-large mouth display
- `ra8875.json` - Large display with acceleration
- `ssd1306.json` - Monochrome OLED (status)
- `ssd1331_micro_rgb_oled.json` - Micro RGB OLED (accents)
- `ssd1351_rgb_oled.json` - Small RGB OLED (accents)

Example multi-hardware template:
- `example_multi_hardware_eye.json` - Reference implementation


DOCUMENT STATUS: Display mapping reference for goblin head configuration
NOT PART OF BUILD SYSTEM - Standalone reference
ASCII encoding, no special characters
