# P32 Animatronic Bot - Build Instructions

##  **Quick Start Build Guide**

### **Prerequisites**
- **Windows 10/11** (PowerShell 5.1+)
- **Visual Studio Code** with PlatformIO extension
- **ESP-IDF** (optional, for advanced builds)
- **Git** for version control

### ** Build Methods (Choose One)**

## **Method 1: PlatformIO (Recommended)**

### **Install PlatformIO**
```powershell
# Install VS Code PlatformIO extension
# OR install via command line:
pip install platformio
```

### **Clone and Build**
```powershell
git clone https://github.com/reussered/p32-animatronic-bot.git
cd p32-animatronic-bot

# Verify system completeness (optional)
PowerShell -ExecutionPolicy Bypass -File ".\tools\system_check.ps1"

# Build firmware
pio run

# Upload to ESP32-S3
pio run -t upload

# Monitor serial output  
pio run -t monitor
```

### **Build Configuration**
The project is configured in `platformio.ini`:
- **Target**: ESP32-S3-DevKitC-1
- **Framework**: ESP-IDF
- **Libraries**: ArduinoJson for configuration parsing
- **Features**: SPIRAM support, FreeRTOS 1000Hz

---

## **Method 2: ESP-IDF Native**

### **Install ESP-IDF**
```powershell
# Follow ESP-IDF Windows installation guide:
# https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html

# Set up environment (in ESP-IDF PowerShell)
get_idf
```

### **Build with CMake**
```powershell
cd p32-animatronic-bot

# Configure for ESP32-S3
idf.py set-target esp32s3

# Configure project (optional)
idf.py menuconfig

# Build firmware
idf.py build

# Flash and monitor
idf.py -p COM3 flash monitor
```

---

## ** Development Build Process**

### **1. Configuration Validation**
```powershell
# Verify all 30 core components exist
PowerShell -ExecutionPolicy Bypass -File ".\tools\system_check.ps1"

# Expected output:
#  16/16 Humanoid components  
#  12/12 Servo controllers
#  2/2 Hardware definitions
#  13 Goblin components
```

### **2. 3D Asset Generation (Optional)**
```powershell
# Generate mounting hardware STL files
.\tools\generate-mounting-system.ps1 -BotType goblin
.\tools\generate-stl-files.ps1 -BasicMounts -BotShells

# View generated STL files
.\tools\launch-stl-viewer.ps1 assets/shapes/stl/basic_mounts/display_mount.stl
```

### **3. Firmware Compilation**
```powershell
# Clean build
pio run -t clean

# Full build with verbose output
pio run -v

# Size analysis
pio run -t size
```

### **4. Hardware Upload**
```powershell
# Auto-detect port and upload
pio run -t upload

# Specify port manually
pio run -t upload --upload-port COM3

# Upload and immediately monitor
pio run -t upload -t monitor
```

---

## ** Expected Build Output**

### **Success Indicators:**
```
Processing esp32-s3-devkitc-1 (platform: espressif32; board: esp32-s3-devkitc-1; framework: espidf)
----------------------------------------------------------------------------------------
Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/espressif32/esp32-s3-devkitc-1.html
PLATFORM: Espressif 32 (6.4.0) > Espressif ESP32-S3-DevKitC-1
HARDWARE: ESP32S3 240MHz, 320KB RAM, 8MB Flash
DEBUG: Current (esp-builtin) On-board (esp-builtin) External (cmsis-dap, esp-bridge, esp-prog, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa)
PACKAGES: 
 - framework-espidf @ 3.50002.231204 (5.0.2)
 - tool-cmake @ 3.21.3
 - tool-ninja @ 1.9.0
 - toolchain-xtensa-esp32s3 @ 8.4.0+2021r2-patch5
LDF: Library Dependency Finder -> https://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 1 compatible libraries
Scanning dependencies...
Dependency Graph
|-- ArduinoJson @ 7.0.4
Building in release mode
Compiling .pio/build/esp32-s3-devkitc-1/src/main.cpp.o
Linking .pio/build/esp32-s3-devkitc-1/firmware.elf
Retrieving maximum program size .pio/build/esp32-s3-devkitc-1/firmware.elf
Checking size .pio/build/esp32-s3-devkitc-1/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project > esp32-s3-devkitc-1"
RAM:   [          ]   4.2% (used 13784 bytes from 327680 bytes)
Flash: [          ]   8.5% (used 699872 bytes from 8388608 bytes)
Building .pio/build/esp32-s3-devkitc-1/firmware.bin
========================= [SUCCESS] Took 45.23 seconds =========================
```

### **Memory Usage Expectations:**
- **RAM Usage**: ~4-6% (13-20KB used of 327KB)
- **Flash Usage**: ~8-15% (0.7-1.2MB used of 8MB)
- **Build Time**: 30-60 seconds first build, 5-15 seconds incremental

---

## ** Troubleshooting**

### **Common Issues:**

#### **Build Errors:**
```powershell
# Update PlatformIO
pio upgrade

# Clean everything and rebuild
pio run -t clean
pio run
```

#### **Upload Issues:**
```powershell
# Check port availability
pio device list

# Hold BOOT button during upload if needed
# Press RESET after successful upload
```

#### **Missing Components:**
```powershell
# Verify system completeness
PowerShell -ExecutionPolicy Bypass -File ".\tools\system_check.ps1"

# If components missing, check git status
git status
git pull origin main
```

---

## ** Build Variants**

### **Development Build (Default)**
```powershell
pio run -e esp32-s3-devkitc-1
```

### **Release Build (Optimized)**
```powershell
pio run -e esp32-s3-devkitc-1 --build-flag "-DRELEASE_BUILD"
```

### **Debug Build (With Serial Output)**
```powershell
pio run -e esp32-s3-devkitc-1 --build-flag "-DDEBUG_VERBOSE"
```

---

## ** System Architecture**

The build process compiles a complete animatronic system:

- **16 Humanoid Components** (full body joint control)
- **12 Bilateral Servo Controllers** (precise motor control)
- **13 Goblin Character Components** (facial expressions)
- **JSON Configuration System** (personality variants)
- **SharedMemory Networking** (distributed control)

> All subsystem communication and distributed control is performed using the SharedMemory class. Direct networking calls are not used in application code; all state sync and messaging is abstracted through SharedMemory.read() and SharedMemory.write().
- **Mood-Driven Behaviors** (AI personality system)

### **Hardware Requirements:**
- **ESP32-S3-DevKitC-1** (master controller)
- **ESP32-C3-Mini** controllers (joint-level distributed control)
- **MG996R Servos** (main joints) + **MG90S Servos** (fine control)
- **6V Power Supply** (servo power rail)
- **3D Printed Mounts** (generated from configuration)

---

## ** Next Steps After Build**

1. **Flash Firmware**: Upload to ESP32-S3 hardware
2. **Test Components**: Verify servo control and sensor input  
3. **3D Print Mounts**: Generate and print mechanical components
4. **Assembly**: Build complete animatronic system
5. **Personality Config**: Customize behavior and expressions

**Ready to build a complete animatronic robot system!** 