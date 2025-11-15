# P32 Goblin Full - Professional Build Guide

## Overview

This guide provides complete assembly instructions for building a P32 Goblin Full animatronic head with mood-driven behaviors. The system features three animated displays (eyes + mouth), proximity sensing, and audio output.

## Required Components

### Main Controller
- **ESP32-S3-DevKitC-1** development board
- **USB-C cable** for programming and power

### Display System  
- **3x GC9A01 Round TFT Displays** (1.28", 240x240 pixels)
  - Part sources: Adafruit #4993, SparkFun LCD-16394
  - Interface: SPI (7-pin connector)
  - Operating voltage: 3.3V (5V tolerant)

### Audio System
- **MAX98357A I2S Audio Amplifier** breakout board
- **4 3W Speaker** (40mm diameter recommended)
- Interface: I2S digital audio

### Sensors
- **HC-SR04 Ultrasonic Distance Sensor**
- Range: 2cm to 400cm
- Interface: Digital GPIO (trigger + echo)

### Power & Wiring
- **5V 2A Power Supply** with barrel jack
- **Breadboard** (830 tie points) or custom PCB
- **Jumper wires** assorted (male-male, male-female)
- **Power rails** and distribution

## GPIO Pin Assignments

The complete system uses 12 GPIO pins on the ESP32-S3:

| Function | GPIO Pin | Wire Color | Connected To |
|----------|----------|------------|--------------|
| **SPI Bus (Shared)** | | |
| SPI MISO | 12 | Blue | All displays SDO |
| SPI MOSI | 13 | Green | All displays SDA |
| SPI CLK  | 14 | Yellow | All displays SCL |
| **SPI Chip Select (Individual)** | | |
| Left Eye CS | 15 | Orange | Left display CS |
| Right Eye CS | 16 | Purple | Right display CS |
| Mouth CS | 17 | Brown | Mouth display CS |
| **I2S Audio** | | |
| I2S BCLK | 4 | Red | Audio amp BCLK |
| I2S WS | 5 | Black | Audio amp LRCK |
| I2S DATA | 6 | White | Audio amp DIN |
| **Sensor** | | |
| Sensor TRIG | 9 | Gray | HC-SR04 trigger |
| Sensor ECHO | 10 | Pink | HC-SR04 echo |

## Assembly Instructions

### Step 1: Power Distribution Setup

**Important:** All components require 5V power and common ground.

1. **Connect power rails on breadboard:**
   - Red wire: 5V supply  breadboard positive rail
   - Black wire: 5V supply ground  breadboard negative rail

2. **ESP32-S3 power connections:**
   - Breadboard positive rail  ESP32 VIN pin
   - Breadboard negative rail  ESP32 GND pin

3. **Component power distribution:**
   - Connect all display VCC pins to positive rail
   - Connect all display GND pins to negative rail
   - Connect audio amplifier VCC to positive rail
   - Connect audio amplifier GND to negative rail
   - Connect HC-SR04 VCC to positive rail
   - Connect HC-SR04 GND to negative rail

### Step 2: SPI Display Wiring

The three displays share SPI bus pins but each needs a unique chip select (CS) pin.

**Shared SPI connections (connect to ALL three displays):**
- ESP32 GPIO 12  All displays SDO pin (MISO)
- ESP32 GPIO 13  All displays SDA pin (MOSI)  
- ESP32 GPIO 14  All displays SCL pin (CLK)

**Individual chip select connections:**
- ESP32 GPIO 15  Left eye display CS pin
- ESP32 GPIO 16  Right eye display CS pin
- ESP32 GPIO 17  Mouth display CS pin

**Additional display connections:**
- Connect RES (reset) pins of all displays to ESP32 3.3V
- Connect DC (data/command) pins of all displays to ESP32 3.3V

### Step 3: I2S Audio Wiring

Connect the MAX98357A audio amplifier:

- ESP32 GPIO 4  Audio amp BCLK pin
- ESP32 GPIO 5  Audio amp LRCK (or WS) pin
- ESP32 GPIO 6  Audio amp DIN pin
- Audio amp + output  Speaker positive terminal
- Audio amp - output  Speaker negative terminal

### Step 4: Sensor Wiring

Connect the HC-SR04 ultrasonic sensor:

- ESP32 GPIO 9  HC-SR04 TRIG pin
- ESP32 GPIO 10  HC-SR04 ECHO pin
- 5V power rail  HC-SR04 VCC
- Ground rail  HC-SR04 GND

### Step 5: Physical Mounting

**Display Positioning (based on 3D skull coordinate system):**

- **Left Eye:** Position at (-1.05", +0.7", -0.35") relative to nose center
- **Right Eye:** Position at (+1.05", +0.7", -0.35") relative to nose center  
- **Mouth:** Position at (0", -1.05", 0") relative to nose center
- **Nose Sensor:** Position at (0", 0", +0.25") - slight protrusion from face
- **Speaker:** Position at (-0.5", +0.5", -1.0") - internal mounting behind displays

**Eye Spacing:** 3.0 inches between eye centers (industry standard for humanoid faces)

### Step 6: Software Installation

1. **Install Development Environment:**
   - Download and install Visual Studio Code
   - Install PlatformIO extension
   - Install Git for Windows

2. **Clone Repository:**
   ```bash
   git clone https://github.com/reussered/p32-animatronic-bot.git
   cd p32-animatronic-bot
   ```

3. **Open Project:**
   - Launch VS Code
   - Open the p32-animatronic-bot folder
   - PlatformIO should auto-detect the project

4. **Verify Configuration:**
   - Check that `src/p32_component_config.h` contains:
     ```c
     #define ENABLE_GOBLIN_COMPONENTS
     ```

### Step 7: Firmware Upload

1. **Connect ESP32-S3:**
   - Connect USB-C cable between ESP32-S3 and computer
   - System should detect COM port automatically

2. **Build Firmware:**
   ```bash
   pio run
   ```
   Expected output: Successful compilation with ~5.8% RAM usage, ~51.7% Flash usage

3. **Upload to ESP32:**
   ```bash
   pio run -t upload
   ```
   Expected output: Successful upload to detected COM port

### Step 8: System Verification

1. **Start Serial Monitor:**
   ```bash
   pio device monitor
   ```

2. **Expected Boot Sequence:**
   - Component initialization messages
   - Loop execution confirmations for all three components
   - Eye animation cycles (blink  angry_stare)
   - Distance sensor readings when objects move near sensor

3. **Functional Tests:**
   - **Visual:** All three displays show animated content
   - **Audio:** Speaker produces clear sound during startup/animations
   - **Sensor:** Distance readings change when objects approach nose area
   - **Mood System:** Colors and animations change based on proximity/time

## System Operation

### 9-Emotion Mood System

The goblin automatically cycles through 9 emotional states that affect animations and colors:

1. **FEAR** - Pale colors, rapid eye movements
2. **ANGER** - Red colors, intense staring  
3. **IRRITATION** - Orange tints, narrowed eyes
4. **HAPPINESS** - Bright colors, wide eyes
5. **CONTENTMENT** - Soft colors, relaxed expression
6. **HUNGER** - Green tints, searching movements
7. **CURIOSITY** - Blue tints, alert expression  
8. **AFFECTION** - Pink tints, gentle gaze
9. **EXCITEMENT** - Bright flashing colors, rapid movements

### Interaction Triggers

- **Proximity:** Objects near nose sensor trigger CURIOSITY
- **Extended Interaction:** Continued proximity leads to EXCITEMENT
- **Isolation:** Extended time alone leads to CONTENTMENT
- **Environmental:** Lighting and sound can influence mood transitions

## Troubleshooting

### Display Issues
- **Black screens:** Check SPI wiring and power connections
- **Partial display:** Verify individual CS pin assignments
- **Wrong colors:** Confirm RGB565 color format in firmware

### Audio Issues  
- **No sound:** Check I2S pin connections and amplifier power
- **Distorted audio:** Verify speaker impedance (4-8) and power supply capacity
- **Intermittent audio:** Check for loose connections on breadboard

### Sensor Issues
- **No distance readings:** Verify TRIG/ECHO pin assignments and 5V power
- **Fixed readings:** Check for obstructions or faulty sensor module
- **Erratic readings:** Ensure stable mounting and proper grounding

### Software Issues
- **Compilation errors:** Verify ENABLE_GOBLIN_COMPONENTS is defined
- **Upload failures:** Check USB connection and COM port detection
- **Runtime crashes:** Monitor serial output for error messages and memory issues

## Expansion Options

The current head-only configuration leaves 26+ GPIO pins available for expansion:

- **Arms:** Add servo-controlled articulated arms
- **Mobility:** Integrate wheeled or tracked base
- **Additional Sensors:** Camera, microphone, environmental sensors
- **Mesh Networking:** Add ESP32-C3 subsystem controllers
- **Full Body:** Expand to complete humanoid robot with torso and legs

## Technical Specifications

- **Controller:** ESP32-S3 (240MHz dual-core, 512KB SRAM, 8MB Flash)
- **Display Resolution:** 240x240 pixels  3 displays = 172,800 total pixels
- **Color Depth:** 16-bit RGB565 (65,536 colors per pixel)
- **Audio:** 16-bit I2S digital audio, 44.1kHz sample rate
- **Sensor Range:** 2cm to 400cm distance detection
- **Power Consumption:** ~500mA total (displays + ESP32 + audio)
- **Operating Voltage:** 5V supply, 3.3V logic levels

## Safety Notes

- **Power Supply:** Use regulated 5V supply rated for at least 2A
- **ESD Protection:** Handle ESP32-S3 with proper grounding techniques  
- **Overcurrent:** Install fuse or current-limiting resistor in power line
- **Heat Management:** Ensure adequate ventilation around ESP32-S3 and audio amplifier
- **Wire Management:** Secure all connections to prevent shorts or disconnections

## Support Resources

- **Documentation:** See `/docs` folder for detailed technical specifications
- **GitHub Issues:** Report problems at repository issue tracker
- **Community:** Join discussion forums linked in project README
- **Hardware Support:** Consult component manufacturer datasheets and forums

---

*This guide covers the complete assembly of a P32 Goblin Full animatronic head. For advanced features and full-body expansion, see additional documentation in the project repository.*