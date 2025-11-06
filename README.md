# p32-animatronic-bot
ESP32-based animatronic BOT system with MOOD-driven behaviors

##  Quick Build Instructions

### **Build & Flash (PlatformIO - Recommended)**
```powershell
# Clone repository
git clone https://github.com/reussered/p32-animatronic-bot.git
cd p32-animatronic-bot

# Verify system completeness (optional)
PowerShell -ExecutionPolicy Bypass -File ".\tools\system_check.ps1"

# Build and upload firmware
pio run -t upload -t monitor
```

### **Alternative: ESP-IDF Native**
```powershell
# Set up ESP-IDF environment
get_idf

# Build and flash
idf.py set-target esp32s3
idf.py build
idf.py -p COM3 flash monitor
```

 **[Complete Build Instructions](BUILD_INSTRUCTIONS.md)** - Detailed setup, troubleshooting, and build variants

##  System Overview

Complete full-body animatronic robot system with:
- **16 Humanoid Components** (head to toe joint control)
- **12 Bilateral Servo Controllers** (distributed ESP32-C3 nodes)  
- **JSON-Driven Configuration** (personalities, hardware, behaviors)
- **Mood-Based AI Behaviors** (8 emotional states with adaptive responses)
- **3D Printable Mounting System** (auto-generated from configurations)

### **Hardware Architecture:**
- **ESP32-S3** (master controller) + **ESP32-C3** (joint controllers)
- **ESP-NOW Mesh Networking** (wireless coordination)
- **MG996R + MG90S Servos** (38 total for full humanoid)
- **Modular Design** (mix goblin head + humanoid body + custom variants)

### **Software Architecture: Pure Component System**
 **[Three-Level Component Attachment Spec](docs/THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)**

**CRITICAL**: Every feature is a component with `init()` and `act()` functions.

- **System Level**: WiFi, Serial, Core (always present)
- **Family Level**: Personality, Mood, Behavior (shared across bot family)
- **Bot-Specific Level**: Eyes, Sensors, Motors (positioned hardware)

The core loop ONLY iterates components - no application logic in `app_main()`.

### **Current Status:**  **ALL CORE COMPONENTS VERIFIED** 
Ready for hardware build and firmware deployment!
