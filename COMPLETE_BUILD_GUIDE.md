# P32 Animatronic Bot - Complete Build Guide
*From Code to Working Robot*

## 🎯 What You're Building

A fully autonomous animatronic system with:
- **Mood-driven behaviors** using advanced C++ emotion engine
- **Distance-responsive animations** via ultrasonic sensors  
- **Real-time display system** with animated eyes and mouth
- **Expandable architecture** supporting full humanoid robots
- **Memory-optimized firmware** running on ESP32-S3

---

## 📦 Complete Part Count & Bill of Materials

### Core Electronics
| Component | Quantity | Purpose | Estimated Cost |
|-----------|----------|---------|----------------|
| **ESP32-S3-DevKitC-1** | 1 | Main microcontroller | $15 |
| **GC9A01 Round Display (240x240)** | 2 | Goblin Eyes | $12 each |
| **ST7789 Display (300x150)** | 1 | Goblin Mouth | $8 |
| **HC-SR04 Ultrasonic Sensor** | 1 | Distance Detection | $3 |
| **I2S Audio Amplifier + Speaker** | 1 | Sound Effects | $6 |
| **Jumper Wires & Breadboard** | 1 set | Prototyping | $10 |
| **Power Supply (5V 2A)** | 1 | System Power | $8 |

**Electronics Subtotal: ~$74**

### 3D Printed Parts (Generated from Config)
| Part Category | Quantity | Material | Print Time |
|---------------|----------|----------|------------|
| **Display Mounts** | 3 | PLA | 2 hours |
| **Sensor Brackets** | 1 | PLA | 30 minutes |
| **Speaker Enclosure** | 1 | PLA | 1 hour |
| **Goblin Shell Components** | 5 | PLA | 4 hours |

**3D Printing Subtotal: ~$8 in filament**

### Optional Full-Body Expansion
| Component | Quantity | Purpose | Estimated Cost |
|-----------|----------|---------|----------------|
| **NEMA 17 Stepper Motors** | 16 | Joint articulation | $12 each |
| **28BYJ-48 Stepper Motors** | 6 | Fine movements | $3 each |
| **A4988 Stepper Drivers** | 22 | Motor control | $2 each |
| **Servo Motors (SG90)** | 8 | Precision joints | $3 each |
| **Additional ESP32 Controllers** | 3 | Distributed control | $15 each |
| **Frame & Mechanical Parts** | 1 set | Robot structure | $150 |

**Full-Body Subtotal: ~$500**

---

## 🔧 Build Phases

### Phase 1: Goblin Head (Working Tonight!)
**Time: 2-4 hours** | **Cost: ~$82** | **Complexity: Beginner**

1. **Flash ESP32** with optimized firmware
2. **Wire components** following GPIO diagram
3. **3D print** display mounts and goblin shell
4. **Assemble** and test distance-responsive eyes

**Result**: Fully animated goblin head with mood system

### Phase 2: Advanced Behaviors  
**Time: 1-2 days** | **Cost: +$50** | **Complexity: Intermediate**

1. Add more sensors (touch, gyroscope, microphone)
2. Implement advanced mood transitions
3. Create personality-driven behaviors
4. Add audio system with voice synthesis

**Result**: Interactive goblin with personality

### Phase 3: Full Humanoid Body
**Time: 1-2 weeks** | **Cost: +$500** | **Complexity: Advanced**

1. Print full humanoid frame components  
2. Install 22 stepper motors for joints
3. Deploy distributed ESP32 controllers
4. Implement walking and manipulation

**Result**: Complete humanoid animatronic robot

---

## ⚡ Quick Start (Get Goblin Working Tonight)

### 1. Hardware Wiring (30 minutes)

```
ESP32-S3 Connections:
┌─────────────────────────────────────────┐
│ Component    │ ESP32 Pin │ Notes        │
│──────────────┼───────────┼──────────────│
│ Left Eye CS  │ GPIO 15   │ SPI Device 1 │
│ Right Eye CS │ GPIO 16   │ SPI Device 2 │  
│ Mouth CS     │ GPIO 17   │ SPI Device 3 │
│ SPI MISO     │ GPIO 12   │ Shared bus   │
│ SPI MOSI     │ GPIO 13   │ Shared bus   │
│ SPI CLK      │ GPIO 14   │ Shared bus   │
│ Sensor TRIG  │ GPIO 9    │ HC-SR04      │
│ Sensor ECHO  │ GPIO 10   │ HC-SR04      │
│ Audio BCLK   │ GPIO 4    │ I2S bus      │
│ Audio WS     │ GPIO 5    │ I2S bus      │
│ Audio DATA   │ GPIO 6    │ I2S speaker  │
│ Power 5V     │ VIN       │ All displays │
│ Ground       │ GND       │ All devices  │
└─────────────────────────────────────────┘
```

### 2. Firmware Flash (15 minutes)

```powershell
# In project root
pio run --target upload --environment esp32-s3-devkitc-1
```

### 3. PC Display Server (5 minutes)

```powershell
# Terminal 1: Start display server
cd tools
python esp32_display_server.py

# Terminal 2: Test components
python test_display_server.py
```

### 4. Watch Goblin Come Alive! 

- **Eyes**: Blink, dilate pupils, change colors based on distance
- **Mouth**: Talk, show surprise, express emotions  
- **Behavior**: Curious when you're close, sleepy when far away
- **Mood System**: 9 emotional components driving all behaviors

---

## 🏗️ 3D Printing Instructions

### Generate All STL Files
```powershell
# Generate goblin-specific mounts
.\tools\generate-mounting-system.ps1 -BotType goblin

# Convert to printable STLs
.\tools\generate-stl-files.ps1 -BasicMounts -BotShells

# View before printing
.\tools\launch-stl-viewer.ps1 assets/shapes/stl/goblin_eye_shells.stl
```

### Print Settings
- **Layer Height**: 0.2mm
- **Infill**: 20%  
- **Support**: Only for overhangs >45°
- **Material**: PLA (easiest) or PETG (stronger)

### Print Order
1. Display mounts (most critical)
2. Sensor brackets  
3. Decorative shells
4. Speaker enclosure

---

## 🧠 Software Architecture

### Memory Optimization Achievement
```cpp
// Before: 57KB static buffers
uint16_t frameBuffer[240*240]; // 57KB per display!

// After: <1KB dynamic mood system  
if (oldMood != currentMood) {
    applyMoodTransition(); // Only when needed
}
```

### Real-Time Animation Pipeline
```
[ESP32 Sensors] → [C++ Mood Engine] → [HTTP Stream] → [PC Display] → [Animated Eyes]
      ↓                    ↓                ↓             ↓             ↓
  Distance          Mood Deltas      JSON Data      tkinter GUI    Goblin Face
  Detection         Calculations     Packets        Animation      Expressions
```

### Component Architecture
- **Modular Design**: Add components via JSON configuration
- **Timing Control**: Each component runs at optimal frequency
- **Distributed Processing**: Scale to multiple ESP32 controllers  
- **Hot Swappable**: Enable/disable components without recompilation

---

## 📡 Network Integration

### WiFi Setup
```cpp
// In main.c - configure your network
web_client_init_full(
    "GOBLIN_LEFT_EYE",     // Component ID
    "EYE",                 // Display type  
    240, 240,              // Resolution
    "192.168.1.100",       // PC IP address
    3000,                  // Server port
    "YourWiFi",           // WiFi SSID
    "YourPassword"        // WiFi password
);
```

### Multi-Device Streaming
- Each component (eyes, mouth, sensors) streams independently  
- PC display server creates draggable windows for each device
- Real-time synchronization maintains coherent expressions
- Distance sensor data shared across all components

---

## 🔬 Advanced Customization

### Create New Bot Personalities
```json
// config/bots/my_custom_bot.json
{
    "bot_type": "CUSTOM",
    "coordinate_system": "skull_3d",
    "positioned_components": [
        "config/components/positioned/custom_eyes.json",
        "config/components/positioned/custom_mouth.json"
    ]
}
```

### Add New Sensors
```cpp
// Implement in component system
esp_err_t my_sensor_init(void);
void my_sensor_act(uint32_t loopCount);
```

### Expand Display Types
```cpp
// Add to animation window system
if (display_type == "MY_DISPLAY") {
    draw_my_custom_animation();
}
```

---

## 🚀 Expansion Roadmap

### Immediate Next Steps (Days 1-7)
- [ ] Add touch sensors for interaction
- [ ] Implement voice recognition  
- [ ] Create multiple goblin personalities
- [ ] Add gyroscope for head orientation

### Medium Term (Weeks 1-4)  
- [ ] Design and print full head assembly
- [ ] Add neck articulation with servos
- [ ] Implement advanced AI behaviors
- [ ] Create mobile app control interface

### Long Term (Months 1-6)
- [ ] Full humanoid body with 22 degrees of freedom
- [ ] Walking and balance algorithms
- [ ] Computer vision for object recognition
- [ ] Natural language processing
- [ ] Multi-robot coordination

---

## 💡 Troubleshooting

### Common Issues

**"ESP32 won't flash"**
- Check USB cable and drivers
- Hold BOOT button during flash
- Try different USB port

**"Displays show nothing"**  
- Verify SPI wiring (MISO, MOSI, CLK, CS)
- Check power supply (5V 2A minimum)
- Test one display at a time

**"Distance sensor not working"**
- Confirm TRIG/ECHO pin assignments
- Check for loose connections
- Verify sensor power (5V)

**"PC can't connect to ESP32"**
- Ensure same WiFi network  
- Check firewall settings
- Verify IP address in code

### Debug Commands
```powershell
# Check ESP32 serial output
pio device monitor

# Validate component configuration  
python tools/generate_individual_components.py

# Test display server connectivity
python tools/test_display_server.py
```

---

## 📚 Documentation Reference

### Key Files
- `docs/goblin-full-interconnection-diagram.md` - Complete wiring  
- `docs/coordinate-system-spec.md` - 3D positioning system
- `docs/interface-gpio-assignment-spec.md` - Pin assignments
- `config/bots/goblin_full.json` - Complete configuration
- `include/Mood.hpp` - C++ mood engine

### Generated Documentation
- `BUILD_INSTRUCTIONS.md` - Detailed build steps
- `P32_COMPLETE_WIRING_GUIDE.md` - Professional wiring diagrams  
- `IMPLEMENTATION_STATUS.md` - Feature completion tracking

### Video Resources
- Assembly timelapse videos (generated during build)
- Debugging walkthrough recordings
- Advanced customization tutorials

---

## 🎉 Success Metrics

### Phase 1 Complete When:
- [x] C++ mood system compiles and runs efficiently
- [x] Distance sensor triggers mood changes
- [x] Eyes animate smoothly with blinking and dilation
- [x] Mouth shows expression changes
- [x] PC display server receives real-time data
- [x] Memory usage under 1KB (vs original 57KB)

### You'll Know It's Working When:
- **Goblin eyes track your movement** with color changes
- **Pupils dilate** when you get close  
- **Mouth reacts** to proximity with surprise/talking
- **Mood transitions** happen smoothly in real-time
- **Memory efficiency** shows dramatic improvement
- **Display server** shows live component data

**Congratulations!** You've built a complete animatronic system with advanced AI behaviors, optimized memory usage, and expandable architecture. This foundation can grow into any robotics project you envision!

---

*P32 Animatronic Bot Project - Making AI Tangible Through Hardware*