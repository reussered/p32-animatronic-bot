# P32 Goblin Head - Quick Wiring Reference Card
## Print This and Keep Near Your Workbench!

---

## GPIO Pin Assignments

```
╔══════════════════════════════════════════════════════════════╗
║                   ESP32-S3 GPIO ASSIGNMENTS                  ║
╠══════════════════════════════════════════════════════════════╣
║                     SPI BUS (SHARED)                         ║
╠══════════════════════════════════════════════════════════════╣
║  GPIO 14  │  CLK (SCK)   │  Orange  │  Both eyes            ║
║  GPIO 13  │  MOSI (SDA)  │  Yellow  │  Both eyes            ║
║  GPIO 21  │  RESET       │  White   │  Both eyes            ║
╠══════════════════════════════════════════════════════════════╣
║                      LEFT EYE (GC9A01 #1)                    ║
╠══════════════════════════════════════════════════════════════╣
║  GPIO 15  │  DC          │  Green   │  Data/Command         ║
║  GPIO 16  │  CS          │  Blue    │  Chip Select          ║
╠══════════════════════════════════════════════════════════════╣
║                     RIGHT EYE (GC9A01 #2)                    ║
╠══════════════════════════════════════════════════════════════╣
║  GPIO 17  │  DC          │  Green   │  Data/Command         ║
║  GPIO 18  │  CS          │  Blue    │  Chip Select          ║
╠══════════════════════════════════════════════════════════════╣
║                    NOSE SENSOR (HC-SR04)                     ║
╠══════════════════════════════════════════════════════════════╣
║  GPIO 19  │  TRIG        │  Purple  │  Trigger (output)     ║
║  GPIO 20  │  ECHO        │  Gray    │  Echo (input)         ║
╠══════════════════════════════════════════════════════════════╣
║                         POWER RAILS                          ║
╠══════════════════════════════════════════════════════════════╣
║  3V3      │  Power       │  Red     │  All components       ║
║  GND      │  Ground      │  Black   │  All components       ║
╚══════════════════════════════════════════════════════════════╝

Total GPIO Used: 9 pins
Total Wires: 13 (+ USB power cable)
```

---

## Display Pin Name Translation

**Your GC9A01 Displays Have These Labels**:

```
Display Pin → ESP32 Signal → Function
─────────────────────────────────────────
SCK         → SPI_CLK       → Clock
SDA         → SPI_MOSI      → Data
DC          → DATA_COMMAND  → Data/Command select
CS          → CHIP_SELECT   → Device select
RST         → RESET         → Hardware reset
VCC         → 3.3V          → Power supply
GND         → GROUND        → Ground reference
```

---

## Left Eye Wiring (7 wires)

```
Display Pin → ESP32 GPIO → Wire Color
─────────────────────────────────────
SCK         → GPIO 14     → Orange
SDA         → GPIO 13     → Yellow
DC          → GPIO 15     → Green
CS          → GPIO 16     → Blue
RST         → GPIO 21     → White
VCC         → 3.3V rail   → Red
GND         → GND rail    → Black
```

---

## Right Eye Wiring (7 wires)

```
Display Pin → ESP32 GPIO → Wire Color
─────────────────────────────────────
SCK         → GPIO 14     → Orange   (SHARED)
SDA         → GPIO 13     → Yellow   (SHARED)
DC          → GPIO 17     → Green
CS          → GPIO 18     → Blue
RST         → GPIO 21     → White    (SHARED)
VCC         → 3.3V rail   → Red
GND         → GND rail    → Black
```

---

## Nose Sensor Wiring (4 wires)

```
Sensor Pin → ESP32 GPIO → Wire Color
─────────────────────────────────────
TRIG       → GPIO 19    → Purple
ECHO       → GPIO 20    → Gray
VCC        → 3.3V rail  → Red
GND        → GND rail   → Black
```

---

## Critical Safety Notes

```
⚠️  DISPLAYS USE 3.3V ONLY - 5V WILL DESTROY THEM!
⚠️  POWER OFF BEFORE CONNECTING/DISCONNECTING
⚠️  CHECK POLARITY - VCC/GND REVERSED = DAMAGE
⚠️  MAX CURRENT: 500mA (USB 2.0 limit)
```

---

## Quick Troubleshooting

**Display Not Working?**
- Check VCC is 3.3V (NOT 5V!)
- Verify GND connected
- Confirm SCK, SDA, RST shared between both eyes
- Check DC, CS unique per eye

**Sensor Not Detecting?**
- TRIG = GPIO 19 (output)
- ECHO = GPIO 20 (input)
- Try 5V on VCC if available
- Point sensor forward (no obstructions)

**ESP32 Resetting?**
- USB cable quality (not charge-only)
- Current draw OK (~150mA total)
- No short circuits
- Breadboard connections solid

---

## Testing Checklist

```
Phase 1: Power
□ USB connected to ESP32
□ 3.3V on breadboard + rail
□ GND on breadboard - rail
□ Display backlights glow

Phase 2: Serial Output
□ Serial monitor open (115200 baud)
□ Boot messages visible
□ Component init messages

Phase 3: Displays
□ Left eye shows test pattern
□ Right eye shows test pattern
□ Both render simultaneously

Phase 4: Sensor
□ Distance readings in serial
□ Eyes react to hand movement
□ Mood system responds
```

---

## Component Layout (Top View)

```
     Left Eye         Sensor        Right Eye
     GC9A01          HC-SR04         GC9A01
     ┌─────┐         ┌─────┐         ┌─────┐
     │  O  │         │▓▓▓▓▓│         │  O  │
     └─────┘         └─────┘         └─────┘
        │               │               │
        └───────┬───────┴───────┬───────┘
                │               │
          [Breadboard Power Rails]
         + ───────────────────── 3.3V
         - ───────────────────── GND
                    │
                    │ Ribbon Cable
                    ▼
            ┌───────────────┐
            │   ESP32-S3    │
            │  DevKitC-1    │
            └───────────────┘
                    │
                 USB Cable
                    ▼
               Computer
```

---

## Quick Stats

- **Components**: 1 ESP32, 2 displays, 1 sensor
- **GPIO Used**: 8 of 45 available (18% utilization)
- **Power Draw**: ~150mA of 500mA available (30% utilization)
- **Wires**: 18 total connections
- **Cost**: ~$35 in components

---

## GPIO Available for Future Expansion

**Still Available** (35+ pins free):

- Audio amplifier (I2S: 3 pins)
- Microphones (ADC: 2 pins)
- Servos (PWM: up to 8 pins)
- LEDs (GPIO: up to 10 pins)
- Buttons/switches (GPIO: as needed)

---

## Version Info

```
Guide Version: 1.0
Date: October 15, 2025
Project: P32 Animatronic Bot
Subsystem: Goblin Head
Status: Ready for breadboard assembly
```

---

**PRINT THIS PAGE AND KEEP IT NEAR YOUR WORKBENCH!** 📄🔧
