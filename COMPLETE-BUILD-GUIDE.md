# Complete Goblin Head Build Guide
*P32 Animatronic Bot - Manufacturing & Assembly Documentation*

---

## 🎯 **BUILD OVERVIEW**

This guide shows you exactly what to 3D print and how to assemble a complete goblin head with:
- 2 Eye Displays (GC9A01 round LCDs)
- 1 Mouth Display (GC9A01 round LCD)  
- 1 Nose Sensor (HC-SR04 ultrasonic)
- 1 I2S Speaker for audio
- ESP32-S3 controller with complete wiring

---

## 📋 **REQUIRED STL FILES**

### Main Skull Structure
| File | Size | Purpose |
|------|------|---------|
| `green_goblin_skull.stl` | 1.55 MB | **Primary skull** - Complete goblin head with eye sockets, mouth cavity, sensor mount |
| `enhanced_goblin_skull.stl` | 1.92 MB | Alternative skull design with enhanced features |
| `WORKING_GOBLIN_SKULL.stl` | 1.69 MB | Verified working skull geometry |

**Choose ONE skull file** - `green_goblin_skull.stl` is recommended as the primary design.

### Component Mounts
| File | Size | Purpose |
|------|------|---------|
| `display_basic_mount.stl` | 74 KB | **Display brackets** - Secure GC9A01 displays in eye/mouth sockets |
| `speaker_basic_mount.stl` | 122 KB | **Speaker mount** - I2S speaker mounting bracket |
| `goblin_nose_sensor.stl` | 65 KB | **Sensor mount** - HC-SR04 ultrasonic sensor bracket |

### Assembly Parts
| File | Size | Purpose |
|------|------|---------|
| `goblin_head_assembly.stl` | 60 KB | Additional assembly components |

---

## 🔌 **WIRING DIAGRAM**

```
ESP32-S3-DevKitC-1 Pin Layout
                    ┌─────────────────┐
                    │  ESP32-S3       │
                    │                 │
    I2S SPEAKER ────│ GPIO 4  (BCLK)  │
    I2S SPEAKER ────│ GPIO 5  (WS)    │  
    I2S SPEAKER ────│ GPIO 6  (DATA)  │
                    │                 │
    NOSE SENSOR ────│ GPIO 9  (TRIG)  │
    NOSE SENSOR ────│ GPIO 10 (ECHO)  │
                    │                 │
    SPI DISPLAYS────│ GPIO 12 (MISO)  │ ── Shared Bus
    SPI DISPLAYS────│ GPIO 13 (MOSI)  │ ── Shared Bus
    SPI DISPLAYS────│ GPIO 14 (CLK)   │ ── Shared Bus
                    │                 │
    LEFT EYE    ────│ GPIO 15 (CS1)   │
    RIGHT EYE   ────│ GPIO 16 (CS2)   │
    MOUTH       ────│ GPIO 17 (CS3)   │
                    │                 │
                    └─────────────────┘
```

### Power Connections
- **5V**: All displays, speaker, sensor VCC
- **GND**: Common ground for all components
- **3.3V**: ESP32 logic level (displays are 5V tolerant)

---

## 🛠 **COMPONENT LIST**

### Electronics Required
| Component | Quantity | Purpose | Mounting |
|-----------|----------|---------|----------|
| ESP32-S3-DevKitC-1 | 1 | Main controller | Internal cavity |
| GC9A01 Round LCD | 3 | Eyes + Mouth displays | Display mounts |
| HC-SR04 Sensor | 1 | Distance sensing | Nose sensor mount |
| I2S Speaker | 1 | Audio output | Internal cavity |
| Jumper wires | ~20 | Connections | Cable management |

### 3D Printing Requirements
- **Material**: PLA+ recommended (PETG acceptable)
- **Layer Height**: 0.2mm standard
- **Infill**: 15-20% for structural parts
- **Supports**: Required for overhangs in skull design
- **Print Time**: ~8-12 hours total for all parts

---

## 🔧 **ASSEMBLY SEQUENCE**

### Step 1: Print Verification
1. **Load STL files** into your slicer (Cura, PrusaSlicer, etc.)
2. **Verify geometry** - You should see:
   - Goblin skull with eye sockets and facial features
   - Display mounting brackets with screw holes
   - Sensor bracket with HC-SR04 mounting posts
3. **Print all components** using settings above

### Step 2: Electronics Preparation
1. **Test all components** before installation
2. **Solder header pins** to displays if needed
3. **Prepare wire harnesses** according to wiring diagram
4. **Program ESP32-S3** with goblin firmware

### Step 3: Component Installation
1. **Install displays** in eye sockets using display mounts
2. **Mount sensor** in nose cavity using sensor bracket
3. **Route wiring** through internal channels
4. **Secure ESP32-S3** in head cavity
5. **Install speaker** for audio output

### Step 4: Final Assembly
1. **Connect all wiring** per diagram above
2. **Test each component** individually
3. **Verify complete system** operation
4. **Secure all connections** and cable management

---

## 📐 **COORDINATE SYSTEM**

The goblin head uses **3D skull-based coordinates** with:
- **Origin**: Nose center (0, 0, 0)
- **Scale**: 3.0" eye spacing baseline
- **Units**: Inches with explicit unit strings
- **Compression**: 30% vertical for goblin proportions

### Component Positions
| Component | X | Y | Z | Description |
|-----------|---|---|---|-------------|
| Left Eye | -1.5" | 0.5" | 0.8" | Left eye socket center |
| Right Eye | +1.5" | 0.5" | 0.8" | Right eye socket center |
| Mouth | 0.0" | -1.2" | -0.5" | Mouth cavity center |
| Nose Sensor | 0.0" | 0.2" | 0.0" | Nose tip sensor position |

---

## ⚡ **POWER & CONNECTIVITY**

### Power Budget
- **ESP32-S3**: ~500mA peak
- **3x GC9A01 Displays**: ~300mA total
- **HC-SR04 Sensor**: ~15mA
- **I2S Speaker**: ~200mA peak
- **Total System**: ~1A maximum (5V supply recommended)

### Interface Summary
- **SPI Bus**: Shared between all 3 displays (efficient pin usage)
- **I2S Audio**: Dedicated audio bus for speaker
- **GPIO Sensor**: Simple digital interface for ultrasonic ranging
- **Total Pins Used**: 12 of 48 available (plenty of expansion room)

---

## 🎨 **EXPECTED RESULTS**

### Physical Appearance
The assembled goblin head will feature:
- **Pronounced facial features** with goblin-like proportions
- **Two animated eyes** displaying expressions and tracking
- **Animated mouth** for speech and emotional display
- **Functional nose sensor** for proximity detection
- **Audio output** for voice, sound effects, and reactions

### Behavioral Capabilities
- **Mood-driven animations** (8 emotional states)
- **Interactive responses** to proximity detection
- **Synchronized audio/visual** expressions
- **Configurable personality** via JSON mood configs

---

## 🔍 **TROUBLESHOOTING**

### Common Issues
- **Display not working**: Check SPI wiring and CS pin assignments
- **Sensor not responding**: Verify TRIG/ECHO pin connections
- **No audio**: Check I2S wiring and speaker connections
- **Power issues**: Ensure 5V supply can handle 1A load

### Validation Steps
1. **STL geometry**: Load files in slicer to verify skull shape
2. **Component fit**: Test fit electronics in printed parts
3. **Wiring continuity**: Use multimeter to verify connections
4. **Software upload**: Verify ESP32-S3 programming successful

---

*This goblin head represents the complete P32 animatronic head subsystem. The STL files contain detailed 3D geometry that will be revealed during the printing process. The skull structure includes mounting points, cable routing, and proper proportions for all electronic components.*