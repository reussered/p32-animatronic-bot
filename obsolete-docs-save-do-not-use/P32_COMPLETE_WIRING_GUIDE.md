# P32 ANIMATRONIC BOT - COMPLETE WIRING GUIDE
**ESP32-S3 Full-Body Humanoid Robot - Hardware Assembly Instructions**

---

## 🎯 **COMPLETE PIN ASSIGNMENT TABLE**

### **ESP32-S3 Master Controller - Main Board**

| GPIO | Function | Component | Wire Color | Notes |
|------|----------|-----------|------------|-------|
| **4** | I2S BCLK | Speaker Clock | **BLUE** | Shared I2S bus |
| **5** | I2S WS | Speaker Word Select | **GREEN** | Shared I2S bus |
| **6** | I2S DATA | Speaker Data | **YELLOW** | Speaker only |
| **9** | SENSOR TRIG | Nose HC-SR04 Trigger | **ORANGE** | Ultrasonic sensor |
| **10** | SENSOR ECHO | Nose HC-SR04 Echo | **PURPLE** | Ultrasonic sensor |
| **12** | SPI MISO | Display Data In | **GRAY** | Shared SPI bus |
| **13** | SPI MOSI | Display Data Out | **WHITE** | Shared SPI bus |
| **14** | SPI CLK | Display Clock | **BLACK** | Shared SPI bus |
| **15** | SPI CS1 | Left Eye Display CS | **RED** | Left eye select |
| **16** | SPI CS2 | Right Eye Display CS | **BROWN** | Right eye select |
| **17** | SPI CS3 | Mouth Display CS | **PINK** | Mouth display select |

### **Power Connections - CRITICAL**

| Connection | Voltage | Current | Wire Gauge | Color |
|------------|---------|---------|------------|-------|
| **VDD** | **5V** | 2A+ | **14 AWG** | **RED** |
| **GND** | **0V** | Return | **14 AWG** | **BLACK** |
| **3.3V** | **3.3V** | 500mA | **18 AWG** | **ORANGE** |

---

## 🔌 **COMPONENT WIRING DIAGRAMS**

### **GC9A01 Display Connections (Left Eye, Right Eye, Mouth)**

```
ESP32-S3          GC9A01 Display
GPIO 12 ────────► MISO (SDO)
GPIO 13 ────────► MOSI (SDA) 
GPIO 14 ────────► CLK (SCL)
GPIO 15 ────────► CS (Left Eye)
GPIO 16 ────────► CS (Right Eye) 
GPIO 17 ────────► CS (Mouth)
3.3V    ────────► VCC
GND     ────────► GND
```

### **I2S Speaker Connection**

```
ESP32-S3          I2S Speaker
GPIO 4  ────────► BCLK (Bit Clock)
GPIO 5  ────────► WS (Word Select)
GPIO 6  ────────► DATA (Audio Data)
5V      ────────► VCC
GND     ────────► GND
```

### **HC-SR04 Ultrasonic Sensor (Nose)**

```
ESP32-S3          HC-SR04 Sensor
GPIO 9  ────────► TRIG (Trigger)
GPIO 10 ────────► ECHO (Echo)
5V      ────────► VCC
GND     ────────► GND
```

---

## 🤖 **FULL-BODY HUMANOID EXPANSION**

### **ESP32-C3 Distributed Controllers**

| Body Part | Controller | Servos | GPIO Usage | Communication |
|-----------|------------|--------|------------|---------------|
| **Left Shoulder** | ESP32-C3 #1 | 3x MG996R | GPIO 0,1,2 | ESP-NOW Mesh |
| **Right Shoulder** | ESP32-C3 #2 | 3x MG996R | GPIO 0,1,2 | ESP-NOW Mesh |
| **Left Elbow** | ESP32-C3 #3 | 2x MG996R | GPIO 0,1 | ESP-NOW Mesh |
| **Right Elbow** | ESP32-C3 #4 | 2x MG996R | GPIO 0,1 | ESP-NOW Mesh |
| **Left Wrist** | ESP32-C3 #5 | 3x MG90S | GPIO 0,1,2 | ESP-NOW Mesh |
| **Right Wrist** | ESP32-C3 #6 | 3x MG90S | GPIO 0,1,2 | ESP-NOW Mesh |
| **Left Hip** | ESP32-C3 #7 | 3x MG996R | GPIO 0,1,2 | ESP-NOW Mesh |
| **Right Hip** | ESP32-C3 #8 | 3x MG996R | GPIO 0,1,2 | ESP-NOW Mesh |
| **Left Knee** | ESP32-C3 #9 | 2x MG996R | GPIO 0,1 | ESP-NOW Mesh |
| **Right Knee** | ESP32-C3 #10 | 2x MG996R | GPIO 0,1 | ESP-NOW Mesh |
| **Left Ankle** | ESP32-C3 #11 | 2x MG90S | GPIO 0,1 | ESP-NOW Mesh |
| **Right Ankle** | ESP32-C3 #12 | 2x MG90S | GPIO 0,1 | ESP-NOW Mesh |

### **ESP32-C3 Standard Wiring Pattern**

```
ESP32-C3-MINI      Servo/Sensor Connections
GPIO 0  ────────► Servo 1 PWM (MG996R/MG90S)
GPIO 1  ────────► Servo 2 PWM
GPIO 2  ────────► Servo 3 PWM (if applicable)
GPIO 3  ────────► Position Sensor Input
GPIO 4  ────────► Current Sensor Input  
GPIO 5  ────────► Status LED
GPIO 6  ────────► Calibration Button
GPIO 7  ────────► I2C SDA (sensor bus)
GPIO 10 ────────► I2C SCL (sensor bus)
6V      ────────► Servo VCC (Separate supply!)
GND     ────────► Common Ground
```

---

## ⚡ **POWER SYSTEM ARCHITECTURE**

### **Power Requirements**

| Component | Voltage | Current (Max) | Power |
|-----------|---------|---------------|-------|
| **ESP32-S3** | 5V | 500mA | 2.5W |
| **12x ESP32-C3** | 3.3V | 200mA each | 8W total |
| **3x GC9A01 Displays** | 3.3V | 150mA each | 1.5W total |
| **1x I2S Speaker** | 5V | 1A | 5W |
| **1x HC-SR04 Sensor** | 5V | 15mA | 0.1W |
| **38x Servos MG996R** | 6V | 2A each (stall) | **456W MAX** |
| **Total System** | Mixed | Variable | **~473W MAX** |

### **Recommended Power Supplies**

1. **Main Logic Supply**: 5V 10A (50W) - Powers ESP32s, displays, sensors
2. **Servo Supply**: 6V 80A (480W) - Powers all servo motors
3. **Backup Battery**: 12V 20Ah LiFePO4 - Mobile operation

### **Power Distribution Board Design**

```
12V Battery Input
       │
       ├─► 6V Buck Converter (80A) ──► Servo Power Rail
       │
       ├─► 5V Buck Converter (10A) ──► Logic Power Rail
       │
       └─► 3.3V LDO (2A) ──────────► Low-power sensors
```

---

## 📐 **PHYSICAL ASSEMBLY GUIDE**

### **Step 1: Head Assembly (Goblin Face)**

1. **Print 3D Components**:
   - `assets/shapes/stl/basic_mounts/display_mount.stl` (3x)
   - `assets/shapes/stl/bot_shells/goblin_eye_shells.stl` (2x)
   - `assets/shapes/stl/bot_shells/goblin_mouth_shell.stl` (1x)

2. **Install Displays**:
   - Mount GC9A01 displays in 3D printed brackets
   - Position: Left (-1.05", +0.7", -0.35"), Right (+1.05", +0.7", -0.35")
   - Mouth: (0", -1.05", 0") relative to nose center

3. **Wire Head Components**:
   - Run SPI bus (4 wires) to all displays
   - Individual CS wires to each display
   - I2S wires to speaker
   - HC-SR04 wires to nose sensor

### **Step 2: Body Frame Assembly**

1. **3D Print or Source Frame Parts**:
   - Use Poppy Humanoid STL files (open source)
   - Or print custom frame from `assets/shapes/stl/`
   - Scale for 3.0" eye spacing (goblin proportions)

2. **Install ESP32-C3 Controllers**:
   - Mount one ESP32-C3 per major joint
   - Use 3D printed controller housings
   - Ensure wireless mesh communication range

### **Step 3: Servo Installation**

1. **Joint Assignments**:
   - **MG996R (Large)**: Shoulders, hips, elbows, knees (28 total)
   - **MG90S (Small)**: Wrists, ankles, fingers (10 total)

2. **Mechanical Assembly**:
   - Follow Poppy Humanoid assembly guides
   - Replace Dynamixel servos with ESP32-controlled servos
   - Maintain same joint ranges and orientations

### **Step 4: Power System Installation**

1. **Install Power Distribution**:
   - Mount buck converters in torso
   - Run power rails throughout frame
   - Install emergency stop switches

2. **Battery Integration**:
   - Mount battery pack in lower torso
   - Balance weight distribution
   - Include charging port access

---

## 🔧 **TESTING & CALIBRATION**

### **Phase 1: Individual Component Test**

```bash
# Upload basic firmware
pio run -t upload -t monitor

# Expected output:
# "P32 Animatronic Bot System Starting..."
# "ESP32-S3 DevKit, ESP-IDF v5.x" 
# "Hardware: ESP32-S3 with 320KB RAM, 8MB Flash"
# "System heartbeat - Loop XXXX"
```

### **Phase 2: Face Component Test**

1. **Display Test**: Each GC9A01 should show test patterns
2. **Speaker Test**: I2S audio output verification  
3. **Sensor Test**: HC-SR04 distance measurements

### **Phase 3: Full Body Test**

1. **ESP-NOW Mesh**: Verify all 12 ESP32-C3 controllers connect
2. **Servo Calibration**: Set center positions and ranges
3. **Movement Test**: Basic joint articulation
4. **Integration Test**: Coordinated full-body movements

---

## 🎭 **PERSONALITY & BEHAVIOR CONFIGURATION**

Your system includes **multiple personality variants**:

- **Goblin Character**: Mischievous, curious behaviors
- **Android Companion**: Helpful, logical responses  
- **Zombie Horror**: Slow, intimidating movements
- **Custom Variants**: JSON-configurable personalities

### **Behavior Triggers**

| Sensor Input | Goblin Response | Android Response | Zombie Response |
|--------------|-----------------|------------------|-----------------|
| **Close Proximity** | Curious lean forward | Polite step back | Aggressive lunge |
| **Motion Detected** | Head tracking | Friendly greeting | Slow head turn |
| **Touch Sensor** | Playful recoil | Helpful response | Aggressive swipe |
| **Sound Input** | Ear twitching | Voice recognition | Groaning vocalization |

---

## 📋 **BILL OF MATERIALS - COMPLETE SYSTEM**

### **Electronics**

| Component | Quantity | Unit Cost | Total |
|-----------|----------|-----------|-------|
| ESP32-S3-DevKitC-1 | 1 | $15 | $15 |
| ESP32-C3-MINI-1 | 12 | $4 | $48 |
| GC9A01 Round Display | 3 | $12 | $36 |
| I2S Speaker | 1 | $8 | $8 |
| HC-SR04 Sensor | 1 | $3 | $3 |
| MG996R Servo | 28 | $8 | $224 |
| MG90S Micro Servo | 10 | $5 | $50 |
| Power Supplies | 3 | $30 | $90 |
| **Electronics Total** | | | **$474** |

### **Mechanical**

| Component | Quantity | Unit Cost | Total |
|-----------|----------|-----------|-------|
| 3D Printing Filament | 2kg | $25 | $50 |
| M3 Screws & Hardware | 1 set | $15 | $15 |
| Bearings & Joints | 1 set | $40 | $40 |
| Cables & Connectors | 1 set | $25 | $25 |
| **Mechanical Total** | | | **$130** |

### **COMPLETE SYSTEM COST: ~$604**
*(Compared to $8000+ commercial humanoid robots)*

---

## 🚀 **NEXT STEPS**

1. **Print this guide** and keep it handy during assembly
2. **Order components** using the BOM above
3. **Start with head assembly** (5 components, easier to debug)
4. **Test each subsystem** before integration
5. **Expand to full body** once head is working

## ⚠️ **SAFETY WARNINGS**

- **High Current**: Servo power supply can deliver 80+ amps
- **Moving Parts**: Servos can cause injury during testing
- **Hot Components**: Buck converters generate heat
- **Battery Safety**: LiFePO4 batteries require proper charging

---

**READY TO BUILD YOUR ANIMATRONIC ROBOT!** 🤖

*This guide covers the complete P32 Animatronic Bot system - from simple goblin face to full humanoid robot.*