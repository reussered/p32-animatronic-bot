ESP32-S3 Head Controller - GPIO and Power Budget Analysis
============================================================

Controller: ESP32-S3 R8N16 (Goblin Head Primary)
Responsibilities: Dual eye displays, audio playback, facial animation, neck servos
Architecture: Config-driven component system


PART 1: GPIO PIN INVENTORY & ALLOCATION
========================================

ESP32-S3 Total Available Pins: 45 GPIO (GPIO 0-48, except 46)
Usable for user circuits: ~38-40 pins (some reserved for JTAG, flash, etc.)

Current Head Configuration Components:
  - Left eye (GC9A01 display)
  - Right eye (GC9A01 display)
  - Neck pan servo
  - Neck tilt servo
  - Speaker (I2S audio)
  - Optional: Sensors (HC-SR04, light, etc.)
  - Facial animation (10 electromagnetic actuators)


GPIO ALLOCATION STRATEGY
========================

BUS-BASED (Shared pins, multiple devices):

  SPI Bus #1 (Dual Eye Displays):
    - MOSI (SPI CLK): GPIO 35 (shared)
    - MISO (SPI MOSI): GPIO 37 (shared)
    - CLK (SPI CLK): GPIO 36 (shared)
    - CS (Eye Left): GPIO 10
    - DC (Eye Left): GPIO 11
    - RST (Eye Left): GPIO 12
    - CS (Eye Right): GPIO 8
    - DC (Eye Right): GPIO 9
    - RST (Eye Right): GPIO 39
    Subtotal SPI: 9 pins (3 shared + 6 unique)

  I2C Bus #1 (Potential expansion, GPIO expander, sensors):
    - SDA: GPIO 21
    - SCL: GPIO 22
    - Note: Can address up to 127 devices on single bus
    Subtotal I2C: 2 pins (shared bus)

  I2S Audio Bus (Speaker):
    - BCLK (Bit Clock): GPIO 14
    - LRCK (Frame Clock): GPIO 13
    - DOUT (Data Out): GPIO 11
    - Note: Configurable pins, may share with display DC if timing allows
    Subtotal I2S: 3 pins

  UART (Optional, debug/external control):
    - TX: GPIO 43
    - RX: GPIO 44
    Subtotal UART: 2 pins

DIRECT GPIO (Servos and discrete outputs):

  Neck Pan Servo (PWM):
    - Signal: GPIO 16
    - Power: 5V external (not GPIO)
    Subtotal: 1 pin

  Neck Tilt Servo (PWM):
    - Signal: GPIO 17
    - Power: 5V external
    Subtotal: 1 pin

ELECTROMAGNETIC ACTUATORS (Facial Animation):

  Via ULN2003 driver (7-channel Darlington array):
    - Channels 1-2 (Eyebrows): GPIO 15 (both eyebrows, time-multiplexed)
    - Channels 3-4 (Cheeks L/R upper): GPIO 18, GPIO 19 (or shared + PWM)
    - Channels 5-6 (Cheeks L/R lower): GPIO 20, GPIO 6 (or shared + PWM)
    - Channel 7 (unused): reserve
    Subtotal ULN2003: 5-6 pins (potential for I2C PWM expander)

  Via individual transistors (Jaws, Ears):
    - Jaw Left: GPIO 7
    - Jaw Right: GPIO 5
    - Ear Left: GPIO 4
    - Ear Right: GPIO 3
    Subtotal transistor outputs: 4 pins

  Alternative (I2C PWM expander PCA9685 for all 10):
    - All 10 actuators on PCA9685 I2C device
    - Uses only I2C bus (GPIO 21, 22 already allocated)
    - Eliminates GPIO 3-7, 15, 18-20 for expansion
    Subtotal: 0 additional pins (via I2C)

SENSORS (Optional):

  HC-SR04 Ultrasonic (range detection):
    - TRIG: GPIO 2
    - ECHO: GPIO 1
    Subtotal: 2 pins

  BMP280 Barometer/Altimeter (I2C):
    - On I2C bus (already allocated)
    - Address configurable (0x76 or 0x77)
    Subtotal: 0 additional pins

  DHT22 Temperature/Humidity (Onewire):
    - Data: GPIO 42
    Subtotal: 1 pin

LED Indicators (Status):
  - Status LED (optional, can use I2C expander): GPIO 40
  - Subtotal: 1 pin (optional)


SUMMARY: GPIO PIN USAGE
=======================

Direct GPIO Allocation (Scenario: Individual transistor drivers):
  SPI display bus: 9 pins
  I2C bus: 2 pins
  I2S audio: 3 pins
  Servo PWM: 2 pins
  Facial actuators (individual): 6 pins
  Optional sensors: 2-3 pins
  ─────────────────────
  Total used: 24-26 pins
  Available (38 usable): 12-14 pins remaining

GPIO Allocation (Scenario: I2C PWM expander for actuators):
  SPI display bus: 9 pins
  I2C bus: 2 pins (shared with PCA9685 and optional sensors)
  I2S audio: 3 pins
  Servo PWM: 2 pins
  Facial actuators: 0 pins (on I2C expander)
  Optional sensors: 0 pins (on I2C bus)
  ─────────────────────
  Total used: 16 pins
  Available: 22 pins remaining

RECOMMENDATION: Use I2C PWM expander (PCA9685) for facial animation
  - Freeing up 6+ GPIO pins
  - Enables 16 PWM channels per expander (only need 10)
  - Allows future expansion (add second PCA9685 on same I2C bus)
  - Reduces GPIO contention with display and audio


PART 2: POWER BUDGET ANALYSIS
==============================

ESP32-S3 R8N16 Supply:
  - Operating voltage: 3.3V (internal core)
  - Current draw (idle): ~5mA
  - Current draw (active, WiFi off): ~80-100mA
  - Current draw (active, WiFi on): ~200-300mA
  - External supply: 5V (regulated to 3.3V onboard)

Current System Power Consumers:

1. DUAL GC9A01 DISPLAYS (Eyes):
   
   Per display:
     - Backlight: 3.0W @ 5V (typical) = 600mA
     - LCD controller + driver: 0.5W = 100mA @ 5V
     - SPI communication overhead: minimal (~50mA @ 3.3V)
   
   Two displays:
     - Peak (both at full brightness): 3.0W + 3.0W = 6.0W = 1.2A @ 5V
     - Average (mixed brightness animation): 3-4W = 600-800mA @ 5V
     - Idle (dim): 1.5-2.0W = 300-400mA @ 5V
   
   Subtotal for displays: **6W peak, 3-4W typical**

2. SPEAKER (Audio Output):
   
   Specification: Assume small 0.5-1W speaker, I2S driven
     - Peak (loud audio): 1.0W @ 5V = 200mA
     - Average (speech): 0.3-0.5W = 60-100mA
     - Idle (no audio): 0W
   
   Note: Speaker power is often THE largest load on head controller
   (matches your suspicion about speakers being power-hungry)
   
   Subtotal for speaker: **1.0W peak, 0.3-0.5W typical**

3. ELECTROMAGNETIC FACIAL ACTUATORS:
   
   From refined spec:
     - Eyebrows: 2 × 0.25W = 0.5W
     - Cheeks (4): 4 × 0.25W = 1.0W
     - Jaws (2): 2 × 1.0W = 2.0W
     - Ears (2): 2 × 0.9W = 1.8W
     - Peak simultaneous: 5.3W
     - Average (50% duty): 2.5-3.0W
     - Idle: 0W (springs hold)
   
   Subtotal for actuators: **5.3W peak, 2.5-3.0W typical, 0W idle**

4. NECK SERVOS (Pan + Tilt):
   
   Per servo (typical hobbyist servo):
     - Stalled torque (~holding): 0.2W
     - Moving (~transit): 0.5-1.0W
     - Idle (no motion): 0.05W
   
   Two servos (pan + tilt):
     - Peak (both moving): 1.0-2.0W
     - Average (occasional movement): 0.2-0.5W
     - Idle: 0.1W
   
   Subtotal for servos: **2.0W peak, 0.2-0.5W typical**

5. ESP32-S3 PROCESSOR + PERIPHERALS:
   
   - Core processor: 0.3-0.5W (active, display rendering)
   - Onboard peripherals: 0.1-0.2W
   - Voltage regulator dissipation: 0.1-0.2W
   
   Subtotal for processor: **0.5W typical, 0.1W idle**

6. OPTIONAL SENSORS:
   
   HC-SR04 ultrasonic: 0.1-0.2W (periodic polling)
   DHT22 temperature: 0.05W (periodic polling)
   
   Subtotal for sensors: **0.1-0.2W periodic**

7. LED INDICATORS (Optional):
   
   Status LED RGB (if present): 0.1-0.5W (depending on color/brightness)
   
   Subtotal for LEDs: **0.0-0.5W (usually off)**


POWER BUDGET SUMMARY
====================

Operating Scenario A: Normal Animation (Eyes animated, speaking, servos idle)
  - Displays: 4W (mixed brightness animation)
  - Speaker: 0.3W (speech audio)
  - Actuators: 2.5W (facial expressions)
  - Servos: 0.1W (idle, holding)
  - Processor: 0.5W
  - ──────────────
  - Total: ~7.4W @ 5V (~1.5A)

Operating Scenario B: Peak Load (Eyes full bright, loud audio, all animation, servos moving)
  - Displays: 6W (full brightness)
  - Speaker: 1.0W (loud playback)
  - Actuators: 5.3W (simultaneous pulse)
  - Servos: 2.0W (both pan/tilt moving)
  - Processor: 0.5W
  - ──────────────
  - Total: ~14.8W @ 5V (~3.0A)

Operating Scenario C: Idle (Displays dim, no audio, no animation)
  - Displays: 1.5W (minimum dim)
  - Speaker: 0W
  - Actuators: 0W (springs holding)
  - Servos: 0.1W (idle)
  - Processor: 0.1W
  - ──────────────
  - Total: ~1.7W @ 5V (~0.35A)

Operating Scenario D: High-Priority Animation (Eyes bright, speech + loud, facial actuators + servos)
  - Most demanding condition (all systems active)
  - Total: ~14.8W


POWER SUPPLY RECOMMENDATIONS
============================

For Head Subsystem (assuming independent supply):

  Scenario A (Normal): 1.5A @ 5V = 7.5W supply
  Scenario B (Peak): 3.0A @ 5V = 15W supply
  Scenario C (Idle): 0.35A @ 5V = 1.75W supply

  Recommended power supply: **5V 3A-5A (15-25W)**
  
  Examples:
    - USB-C Power Delivery 5V 3A (standard phone charger): ~$10
    - 5V 5A linear supply: ~$15
    - 12V to 5V buck converter (for distributed power): ~$5
  
  Local capacitor filtering:
    - 1000µF electrolytic at supply entry (bulk energy storage)
    - 100µF ceramic at each major consumer (display, speaker, actuators)
    - Prevents brownout when speakers spike


VOLTAGE CONSIDERATIONS
=====================

5V vs 3.3V Logic Separation:

  ESP32-S3 GPIO (3.3V logic):
    - Display CS/DC/RST pins: 3.3V logic (OK, displays accept 3.3V)
    - I2C bus (SDA/SCL): 3.3V logic with pull-ups
    - I2S audio lines: 3.3V logic
    - Servo PWM signal: 3.3V logic (servos accept 3V-5V)
  
  5V Power consumers (separate rail):
    - Display backlight LED: 5V direct (no 3.3V conversion)
    - Speaker amp (if powered): 5V direct
    - Servo power: 5V direct (separate from signal)
    - Actuator coils: 5V or 9V (driven via transistor/ULN2003)

  Regulatory structure:
    - External 5V supply (USB-C PD or wall adapter)
    - 5V → 3.3V regulator (onboard ESP32 or external AMS1117)
    - Separate 5V rail for high-current loads (displays, speaker, actuators)


PART 3: COMPLETE HEAD CONTROLLER ARCHITECTURE
==============================================

Block Diagram (Logical Flow):

  [5V Supply 3-5A]
    ├─ ESP32-S3 (5V → 3.3V regulator onboard)
    │   ├─ SPI0 (GPIO 35-37, 10-12, 8-9, 39)
    │   │   ├─ Left Eye Display (GC9A01)
    │   │   └─ Right Eye Display (GC9A01)
    │   ├─ I2C0 (GPIO 21-22)
    │   │   ├─ PCA9685 PWM expander (address 0x40)
    │   │   │   ├─ Eyebrow Left (PWM ch0)
    │   │   │   ├─ Eyebrow Right (PWM ch1)
    │   │   │   ├─ Cheek L Upper (PWM ch2)
    │   │   │   ├─ Cheek L Lower (PWM ch3)
    │   │   │   ├─ Cheek R Upper (PWM ch4)
    │   │   │   ├─ Cheek R Lower (PWM ch5)
    │   │   │   ├─ Jaw Left (PWM ch6)
    │   │   │   ├─ Jaw Right (PWM ch7)
    │   │   │   ├─ Ear Left (PWM ch8)
    │   │   │   └─ Ear Right (PWM ch9)
    │   │   ├─ DHT22 temp sensor (address 0x00 onewire → GPIO 42)
    │   │   └─ BMP280 barometer (address 0x76)
    │   ├─ I2S0 (GPIO 13-14, 11 or separate pin)
    │   │   └─ Speaker DAC / Audio codec
    │   ├─ PWM/Servo outputs (GPIO 16-17)
    │   │   ├─ Neck Pan Servo
    │   │   └─ Neck Tilt Servo
    │   ├─ Analog inputs (GPIO 1-10, 32-39)
    │   │   └─ Light level sensor (ADC on GPIO 32)
    │   └─ UART debug (GPIO 43-44, optional)
    │
    └─ External Power Distribution
        ├─ 5V → Display backlight LEDs (via current limiting resistors)
        ├─ 5V → Speaker amplifier (if separate IC)
        ├─ 5V/9V → Actuator driver outputs
        │   ├─ ULN2003 (6-8 channels via GPIO or I2C PWM)
        │   └─ Discrete transistors for jaws/ears (or I2C PWM)
        ├─ 5V → Servo motors (pan + tilt)
        └─ Bulk 1000µF cap at supply entry


GPIO PIN FINAL ALLOCATION TABLE
===============================

| GPIO | Function | Level | Notes |
|------|----------|-------|-------|
| 0 | RESERVED | — | Internal use |
| 1 | ADC/Echo (HC-SR04) | 3.3V | Optional ultrasonic |
| 2 | Trigger (HC-SR04) | 3.3V | Optional ultrasonic |
| 3 | Ear Right (direct) OR Free | 3.3V | If using I2C PWM, this is free |
| 4 | Ear Left (direct) OR Free | 3.3V | If using I2C PWM, this is free |
| 5 | Jaw Right (direct) OR Free | 3.3V | If using I2C PWM, this is free |
| 6 | Cheek R Lower OR Free | 3.3V | If using I2C PWM, this is free |
| 7 | Jaw Left (direct) OR Free | 3.3V | If using I2C PWM, this is free |
| 8 | SPI1 CS (Eye Right) | 3.3V | Display chip select |
| 9 | SPI1 DC (Eye Right) | 3.3V | Display data/command |
| 10 | SPI1 CS (Eye Left) | 3.3V | Display chip select |
| 11 | SPI1 DC (Eye Left) | 3.3V | Display data/command |
| 12 | SPI1 RST (Eye Left) | 3.3V | Display reset |
| 13 | I2S LRCK (Frame clock) | 3.3V | Audio frame sync |
| 14 | I2S BCLK (Bit clock) | 3.3V | Audio bit clock |
| 15 | Eyebrows (ULN2003) OR Free | 3.3V | If using I2C PWM, this is free |
| 16 | PWM: Neck Pan Servo | 3.3V | Servo signal |
| 17 | PWM: Neck Tilt Servo | 3.3V | Servo signal |
| 18 | Cheek L Upper OR Free | 3.3V | If using I2C PWM, this is free |
| 19 | Cheek R Upper OR Free | 3.3V | If using I2C PWM, this is free |
| 20 | Cheek L Lower OR Free | 3.3V | If using I2C PWM, this is free |
| 21 | I2C SDA | 3.3V | Shared bus for PCA9685, sensors |
| 22 | I2C SCL | 3.3V | Shared bus for PCA9685, sensors |
| 23 | RESERVED | — | May be used by flash |
| 24 | RESERVED | — | May be used by flash |
| 25 | Free | 3.3V | Available for future use |
| 26 | Free | 3.3V | Available for future use |
| 27 | Free | 3.3V | Available for future use |
| 28 | Free | 3.3V | Available for future use |
| 29 | Free | 3.3V | Available for future use |
| 30 | Free | 3.3V | Available for future use |
| 31 | Free | 3.3V | Available for future use |
| 32 | ADC: Light level sensor | 3.3V | Optional brightness control |
| 33 | Free | 3.3V | Available |
| 34 | Free | 3.3V | Available |
| 35 | SPI CLK | 3.3V | Shared display clock |
| 36 | SPI MOSI | 3.3V | Shared display data |
| 37 | SPI MISO | 3.3V | Shared display data in |
| 38 | RESERVED | — | JTAG / reserved |
| 39 | SPI1 RST (Eye Right) | 3.3V | Display reset |
| 40 | LED Status (optional) | 3.3V | Optional indicator |
| 41 | RESERVED | — | JTAG / reserved |
| 42 | DHT22 OneWire | 3.3V | Temperature/humidity sensor |
| 43 | UART TX (debug) | 3.3V | Optional serial debug |
| 44 | UART RX (debug) | 3.3V | Optional serial debug |
| 45 | RESERVED | — | Strapping pin |
| 46 | RESERVED | — | Not available |
| 47 | RESERVED | — | Not available |
| 48 | RESERVED | — | Not available |


FINAL SUMMARY
=============

GPIO Pins Used (I2C Expander Strategy - RECOMMENDED):
  - Direct GPIO: 16 pins (SPI displays + I2C + I2S + servos)
  - Available: 22 pins (for future expansion)
  - Efficiency: 42% utilization

GPIO Pins Used (Direct Driver Strategy):
  - Direct GPIO: 24-26 pins
  - Available: 12-14 pins
  - Efficiency: 68% utilization

Power Budget:
  - Normal animation: 7.4W (1.5A @ 5V)
  - Peak load: 14.8W (3.0A @ 5V)
  - Recommended supply: 5V 3-5A (15-25W minimum)
  - Largest power consumer: Display backlight (~6W peak)
  - Second largest: Speaker (~1W typical)
  - Third: Facial actuators (~2.5W typical)

Expansion Headroom:
  - GPIO: 22 pins available (can add more sensors, LEDs, etc.)
  - Power: 5-10W available on 25W supply (room for upgrades)
  - I2C: Can add second PCA9685 for 16 more PWM channels
  - SPI: Can add second display or other SPI device

Bottlenecks:
  - **Power supply is primary constraint** (displays + speaker dominate)
  - GPIO is abundant (I2C strategy frees up 6+ pins)
  - Thermal: Speaker and display backlight generate heat (ensure ventilation)


DOCUMENT STATUS: Technical reference for head subsystem architecture
NOT PART OF BUILD - standalone planning document
ASCII encoding, no special characters
