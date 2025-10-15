# P32 Goblin Head - Visual Wiring Diagram
## Breadboard Assembly Guide with ASCII Art

---

## Complete System Overview

```
                    P32 GOBLIN HEAD SUBSYSTEM
                    Breadboard Configuration
                    
    ┌──────────────────────────────────────────────────────┐
    │                                                      │
    │     GC9A01            HC-SR04           GC9A01       │
    │    LEFT EYE        NOSE SENSOR         RIGHT EYE     │
    │                                                      │
    │      ╔═══╗           ┌─────┐            ╔═══╗       │
    │      ║   ║           │▓▓ ▓▓│            ║   ║       │
    │      ║ O ║           │  ▓  │            ║ O ║       │
    │      ║   ║           │▓▓ ▓▓│            ║   ║       │
    │      ╚═══╝           └─────┘            ╚═══╝       │
    │        │                │                 │          │
    │        │ 7 wires        │ 4 wires         │ 7 wires │
    │        └────────┬───────┴────────┬────────┘          │
    │                 │                │                   │
    │           ╔═════╧════════════════╧═════╗            │
    │           ║    BREADBOARD (830-point)   ║            │
    │           ║                             ║            │
    │           ║  + RAIL ══════════════════  ║ ← 3.3V    │
    │           ║                             ║            │
    │           ║  Component Connections      ║            │
    │           ║  (Power & Signal)           ║            │
    │           ║                             ║            │
    │           ║  - RAIL ══════════════════  ║ ← GND     │
    │           ╚═════╤════════════════╤═════╝            │
    │                 │                │                   │
    │                 │ Ribbon Cable   │ Ribbon Cable     │
    │                 │ (8 signals)    │ (2 power)        │
    │                 │                │                   │
    │           ╔═════╧════════════════╧═════╗            │
    │           ║   ESP32-S3 DevKitC-1        ║            │
    │           ║                             ║            │
    │           ║   ┌─────────────────────┐   ║            │
    │           ║   │   Dual Core 240MHz  │   ║            │
    │           ║   │   45 GPIO Available │   ║            │
    │           ║   │   Using: 8 GPIO     │   ║            │
    │           ║   └─────────────────────┘   ║            │
    │           ║                             ║            │
    │           ║         USB-C Port          ║            │
    │           ╚═════════════╤═══════════════╝            │
    │                         │                            │
    │                         │ USB Cable                  │
    │                         │                            │
    │                    ┌────▼────┐                       │
    │                    │ LAPTOP  │                       │
    │                    │ OR PC   │                       │
    │                    └─────────┘                       │
    │                                                      │
    └──────────────────────────────────────────────────────┘
```

---

## Breadboard Top View (Component Placement)

```
                     FRONT VIEW
                (This faces the goblin)

    Left Side                              Right Side
    
    ┌──────┐                                ┌──────┐
    │ GC9A │ ← Left Eye                     │ GC9A │ ← Right Eye
    │  01  │    (GPIO 27 DC, 15 CS)         │  01  │    (GPIO 33 DC, 32 CS)
    │ 1.28"│                                 │ 1.28"│
    └──┬───┘                                 └───┬──┘
       │                                         │
       │          ┌──────────────┐              │
       │          │   HC-SR04    │ ← Nose       │
       │          │   Ultrasonic │    (GPIO 25 TRIG, 34 ECHO)
       │          └──────────────┘              │
       │                                         │
    ═══╪═════════════════════════════════════════╪═══
       │         BREADBOARD                      │
       │      + ────────────────────── +         │
       │                                         │
       │      a1 ── a30  |  b1 ── b30           │
       │      ...        |  ...                  │
       │      j1 ── j30  |  i1 ── i30           │
       │                                         │
       │      - ────────────────────── -         │
    ═══╧═════════════════════════════════════════╧═══
                         │
                         │ 10 wires to ESP32
                         ▼
                 ┌───────────────┐
                 │  ESP32-S3     │
                 │  DevKitC-1    │
                 │               │
                 │  ┌─────────┐  │
                 │  │USB-C    │  │ ← Power & Programming
                 │  └─────────┘  │
                 └───────────────┘
```

---

## Left Eye Detailed Connections

```
GC9A01 Display #1 (Left Eye)
─────────────────────────────

Display     Wire      Breadboard   ESP32-S3
Pin         Color     Rail         GPIO
────────────────────────────────────────────
  
 ┌─────┐
 │ SCK │ ──Orange──> + rail ──> GPIO 14 (SPI_CLK shared)
 │ SDA │ ──Yellow──> + rail ──> GPIO 13 (SPI_MOSI shared)
 │ DC  │ ──Green───> + rail ──> GPIO 27 (unique)
 │ CS  │ ──Blue────> + rail ──> GPIO 15 (unique)
 │ RST │ ──White───> + rail ──> GPIO 26 (shared)
 │ VCC │ ──Red─────> + rail ──> 3.3V
 │ GND │ ──Black───> - rail ──> GND
 └─────┘
 
  7 wires total
  3 shared (SCK, SDA, RST)
  2 unique (DC, CS)
  2 power (VCC, GND)
```

---

## Right Eye Detailed Connections

```
GC9A01 Display #2 (Right Eye)
──────────────────────────────

Display     Wire      Breadboard   ESP32-S3
Pin         Color     Rail         GPIO
────────────────────────────────────────────
  
 ┌─────┐
 │ SCK │ ──Orange──> + rail ──> GPIO 14 (SPI_CLK shared)
 │ SDA │ ──Yellow──> + rail ──> GPIO 13 (SPI_MOSI shared)
 │ DC  │ ──Green───> + rail ──> GPIO 33 (unique)
 │ CS  │ ──Blue────> + rail ──> GPIO 32 (unique)
 │ RST │ ──White───> + rail ──> GPIO 26 (shared)
 │ VCC │ ──Red─────> + rail ──> 3.3V
 │ GND │ ──Black───> - rail ──> GND
 └─────┘
 
  7 wires total
  3 shared (SCK, SDA, RST)
  2 unique (DC, CS)
  2 power (VCC, GND)
```

---

## Nose Sensor Detailed Connections

```
HC-SR04 Ultrasonic Sensor (Nose)
─────────────────────────────────

Sensor      Wire      Breadboard   ESP32-S3
Pin         Color     Rail         GPIO
────────────────────────────────────────────
  
 ┌────────┐
 │  TRIG  │ ──Purple──> + rail ──> GPIO 25 (output)
 │  ECHO  │ ──Gray────> + rail ──> GPIO 34 (input only)
 │  VCC   │ ──Red─────> + rail ──> 3.3V (or 5V if available)
 │  GND   │ ──Black───> - rail ──> GND
 └────────┘
 
  4 wires total
  2 signal (TRIG, ECHO)
  2 power (VCC, GND)
```

---

## Power Distribution Diagram

```
                     USB Power Source
                           │
                           │ 5V USB
                           ▼
                  ┌────────────────┐
                  │   ESP32-S3     │
                  │                │
                  │  ┌──────────┐  │
                  │  │ Regulator│  │
                  │  │ 5V→3.3V  │  │
                  │  └──────────┘  │
                  │                │
                  └────┬──────┬────┘
                       │      │
                    3.3V    GND
                       │      │
          ┌────────────┴──────┴────────────┐
          │    Breadboard Power Rails      │
          │                                 │
          │  + ═══════════════════════ +   │ ← 3.3V
          │  - ═══════════════════════ -   │ ← GND
          │                                 │
          └─┬──────┬──────┬──────┬──────┬──┘
            │      │      │      │      │
            │      │      │      │      │
         Left   Right  Sensor Sensor  All
         Eye    Eye    VCC    GND     GND
         VCC    VCC                   pins
         
    Total Current Draw: ~150mA
    USB 2.0 Limit: 500mA
    Headroom: 350mA (70% available)
```

---

## Signal Wire Routing

```
ESP32-S3 GPIO Layout (Simplified)
──────────────────────────────────

    LEFT SIDE                RIGHT SIDE
    
    3V3  ─────┐              GND  ─────┐
    GND  ─────┤              ...  ─────┤
    GPIO 14 ──┤ SCK          GPIO 27 ──┤ DC (L)
    GPIO 13 ──┤ MOSI         GPIO 26 ──┤ RST
    GPIO 15 ──┤ CS (L)       GPIO 25 ──┤ TRIG
    ...  ─────┤              GPIO 33 ──┤ DC (R)
    GPIO 32 ──┤ CS (R)       GPIO 34 ──┤ ECHO
    ...  ─────┤              ...  ─────┤
    
    
Routing Strategy:
──────────────────

1. Power wires (Red/Black) → Shortest path to breadboard
2. SPI bus (Orange/Yellow/White) → Bundle together
3. Unique signals (Green/Blue) → Separate from SPI
4. Sensor signals (Purple/Gray) → Avoid crossing SPI
```

---

## 3D Perspective View

```
                     SIDE VIEW
                (Looking from Left)

                    ┌─────────┐
                    │ Display │ ← Right Eye (further)
                    │ (back)  │
                    └────┬────┘
                         │
        ┌─────────┐      │
        │ Display │      │    ┌───────┐
        │ (front) │      │    │Sensor │ ← Nose (center)
        └────┬────┘      │    └───┬───┘
             │           │        │
    ═════════╪═══════════╪════════╪═════════
             │           │        │
             └───────────┴────────┘
                      │
                 Breadboard
                  (below)
                      │
               ┌──────┴──────┐
               │   ESP32-S3  │
               │             │
               └─────────────┘
```

---

## Wiring Sequence (Build Order)

```
Step 1: Power Foundation
─────────────────────────
□ ESP32 3.3V → Breadboard + rail
□ ESP32 GND → Breadboard - rail
□ Test: Multimeter shows 3.3V

Step 2: Left Eye (7 wires)
──────────────────────────
□ Display SCK → GPIO 14 (Orange)
□ Display SDA → GPIO 13 (Yellow)
□ Display DC → GPIO 27 (Green)
□ Display CS → GPIO 15 (Blue)
□ Display RST → GPIO 26 (White)
□ Display VCC → + rail (Red)
□ Display GND → - rail (Black)
□ Test: Backlight glows

Step 3: Right Eye (7 wires)
───────────────────────────
□ Display SCK → GPIO 14 (Orange) ← SHARED
□ Display SDA → GPIO 13 (Yellow) ← SHARED
□ Display DC → GPIO 33 (Green)
□ Display CS → GPIO 32 (Blue)
□ Display RST → GPIO 26 (White) ← SHARED
□ Display VCC → + rail (Red)
□ Display GND → - rail (Black)
□ Test: Both backlights glow

Step 4: Nose Sensor (4 wires)
─────────────────────────────
□ Sensor TRIG → GPIO 25 (Purple)
□ Sensor ECHO → GPIO 34 (Gray)
□ Sensor VCC → + rail (Red)
□ Sensor GND → - rail (Black)
□ Test: No smoke! 🔥

Step 5: Final Check
───────────────────
□ All wires secure
□ No loose connections
□ No short circuits
□ Components stable
□ Ready for firmware!
```

---

## Component Spacing Guide

```
Optimal Component Placement on 830-Point Breadboard
────────────────────────────────────────────────────

Row         Component         Spacing
────────────────────────────────────────────────────
1-10        Left Eye          Leave 2 rows below
11-12       [Gap]             Wire routing space
13-22       Sensor            Centered
23-24       [Gap]             Wire routing space
25-34       Right Eye         Leave 2 rows above
35-40       [Spare]           Future expansion

Power Rails: Continuous along both sides
Signal Routing: Use gaps between components
```

---

## Troubleshooting Flow Chart

```
             Power On
                │
                ▼
         ┌──────────────┐
         │ Power LED ON?│
         └──────┬───────┘
                │
         YES ───┴─── NO → Check USB cable
                │           Check ESP32
                ▼
         ┌────────────────┐
         │ Displays Glow? │
         └────────┬───────┘
              │
         YES ─┴─ NO → Check 3.3V
              │         Check VCC/GND
              ▼
    ┌────────────────────┐
    │ Serial Output OK?  │
    └────────┬───────────┘
             │
        YES ─┴─ NO → Check USB driver
             │         Check baud rate
             ▼
   ┌──────────────────────┐
   │ Displays Rendering?  │
   └────────┬─────────────┘
            │
       YES ─┴─ NO → Check SPI pins
            │         Check CS pins
            ▼
  ┌───────────────────────┐
  │ Sensor Detecting?     │
  └────────┬──────────────┘
           │
      YES ─┴─ NO → Check TRIG/ECHO
           │         Try 5V power
           ▼
    ╔═════════════╗
    ║   SUCCESS!  ║
    ╚═════════════╝
```

---

## Final Assembly Checklist

```
Physical Assembly:
□ Breadboard stable on desk
□ ESP32 positioned for easy USB access
□ Components placed per layout diagram
□ Wires organized and labeled
□ No wire stress on component pins

Electrical Connections:
□ Power rails energized (3.3V, GND)
□ All 18 signal wires connected
□ No short circuits (multimeter test)
□ Correct GPIO assignments verified

Testing Preparation:
□ Serial monitor ready (115200 baud)
□ Firmware compiled successfully
□ USB cable connected
□ Camera ready for documentation

Safety:
□ 3.3V on all display VCC pins (NOT 5V!)
□ GPIO 34 used only for input (ECHO)
□ Current draw < 500mA
□ No exposed high-voltage wires
```

---

## Next: Flash Firmware

Once wiring is complete, proceed to firmware testing:

1. **Compile test firmware** (minimal display + sensor test)
2. **Upload to ESP32-S3** via USB
3. **Watch serial output** for initialization messages
4. **Verify displays** show test patterns
5. **Test sensor** readings in serial monitor
6. **Validate mood system** responds to sensor input

**You're ready to bring your goblin to life!** 🎉👾

---

*Diagram Version: 1.0*  
*Date: October 15, 2025*  
*Print this page for visual reference during assembly*
