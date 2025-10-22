# PlatformIO Multi-ESP32 Variant Configuration Strategy

## 🎯 **Multi-Chip Architecture Requirements**

The P32 animatronic system requires different ESP32 variants optimized for cost and performance across subsystems:

- **ESP32-C3**: Cost-optimized single subsystems (legs, simple arms)
- **ESP32**: Standard dual-core subsystems (torso coordination, complex arms) 
- **ESP32-S3**: High-performance subsystems (head displays/audio, master controllers)
- **ESP32-S2**: Display-heavy specialized subsystems (optional)

## 📊 **Current Subsystem Mapping**

| Subsystem | Optimal Chip | GPIO Used | Memory Used | Rationale |
|-----------|--------------|-----------|-------------|-----------|
| **Goblin Head** | ESP32-S3 | 25 pins | 180KB | Dual displays + audio + sensors |
| **Goblin Torso** | ESP32-S3 | 20 pins | 120KB | Master coordination + ESP-NOW mesh |
| **Left Arm** | ESP32-C3 | 18 pins | 85KB | Basic servo control + sensors |
| **Right Arm** | ESP32-C3 | 18 pins | 85KB | Basic servo control + sensors |
| **Left Leg** | ESP32-C3 | 22 pins | 100KB | Leg+foot integration at C3 capacity |
| **Right Leg** | ESP32-C3 | 22 pins | 100KB | Leg+foot integration at C3 capacity |

## 🔧 **PlatformIO Configuration Architecture**

### **Multi-Variant Environment Strategy**

```ini
[common]
platform = espressif32
framework = espidf
lib_deps = 
    bblanchon/ArduinoJson @ ^7.0.4
monitor_speed = 115200
monitor_filters = esp32_exception_decoder
upload_speed = 921600

# Shared build flags for all variants
build_flags = 
    -DSIMPLE_TEST
    -DPROJECT_P32_ANIMATRONIC=1

[common_esp32c3]
extends = common
build_flags = 
    ${common.build_flags}
    -DESP32_CHIP_TYPE_C3=1
    -DCONFIG_ESP32C3_DEFAULT_CPU_FREQ_160=1

[common_esp32]
extends = common
build_flags = 
    ${common.build_flags}
    -DESP32_CHIP_TYPE_STANDARD=1
    -DCONFIG_ESP32_DEFAULT_CPU_FREQ_240=1

[common_esp32s3]
extends = common
build_flags = 
    ${common.build_flags}
    -DESP32_CHIP_TYPE_S3=1
    -DCONFIG_ESP32S3_SPIRAM_SUPPORT=1
    -DCONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=1

[common_esp32s2]
extends = common
build_flags = 
    ${common.build_flags}
    -DESP32_CHIP_TYPE_S2=1
    -DCONFIG_ESP32S2_SPIRAM_SUPPORT=1
```

### **Subsystem-Specific Environments**

#### **ESP32-S3 Subsystems (High Performance)**
```ini
# HEAD SUBSYSTEM - ESP32-S3 for displays, audio, sensors
[env:goblin_head]
extends = common_esp32s3
board = esp32-s3-devkitc-1
build_flags = 
    ${common_esp32s3.build_flags}
    -DSUBSYSTEM_GOBLIN_HEAD=1
    -DHEAD_DUAL_DISPLAYS=1
    -DAUDIO_I2S_ENABLED=1
upload_port = COM3

# TORSO SUBSYSTEM - ESP32-S3 for master coordination
[env:goblin_torso]
extends = common_esp32s3
board = esp32-s3-devkitc-1
build_flags = 
    ${common_esp32s3.build_flags}
    -DSUBSYSTEM_GOBLIN_TORSO=1
    -DMASTER_CONTROLLER=1
    -DESP_NOW_MESH_ENABLED=1
upload_port = COM4
```

#### **ESP32-C3 Subsystems (Cost Optimized)**
```ini
# ARM SUBSYSTEMS - ESP32-C3 for basic servo control
[env:left_arm]
extends = common_esp32c3
board = esp32-c3-devkitm-1
build_flags = 
    ${common_esp32c3.build_flags}
    -DSUBSYSTEM_LEFT_ARM=1
    -DARM_BASIC_CONTROL=1
upload_port = COM5

[env:right_arm]
extends = common_esp32c3
board = esp32-c3-devkitm-1
build_flags = 
    ${common_esp32c3.build_flags}
    -DSUBSYSTEM_RIGHT_ARM=1
    -DARM_BASIC_CONTROL=1
upload_port = COM6

# LEG SUBSYSTEMS - ESP32-C3 for leg+foot integration
[env:left_leg]
extends = common_esp32c3
board = esp32-c3-devkitm-1
build_flags = 
    ${common_esp32c3.build_flags}
    -DSUBSYSTEM_LEFT_LEG=1
    -DLEG_FOOT_INTEGRATION=1
upload_port = COM7

[env:right_leg]
extends = common_esp32c3
board = esp32-c3-devkitm-1
build_flags = 
    ${common_esp32c3.build_flags}
    -DSUBSYSTEM_RIGHT_LEG=1
    -DLEG_FOOT_INTEGRATION=1
upload_port = COM8
```

#### **ESP32 Standard Subsystems (Balanced)**
```ini
# COMPLEX ARM SUBSYSTEMS - ESP32 for advanced control
[env:left_arm_advanced]
extends = common_esp32
board = esp32dev
build_flags = 
    ${common_esp32.build_flags}
    -DSUBSYSTEM_LEFT_ARM=1
    -DARM_ADVANCED_CONTROL=1
    -DINVERSE_KINEMATICS_ENABLED=1
upload_port = COM9

[env:right_arm_advanced]
extends = common_esp32
board = esp32dev
build_flags = 
    ${common_esp32.build_flags}
    -DSUBSYSTEM_RIGHT_ARM=1
    -DARM_ADVANCED_CONTROL=1
    -DINVERSE_KINEMATICS_ENABLED=1
upload_port = COM10
```

## 🚀 **Build Commands for Multi-Variant System**

### **Build All Subsystems**
```powershell
# Build all ESP32-S3 subsystems
pio run -e goblin_head -e goblin_torso

# Build all ESP32-C3 subsystems  
pio run -e left_arm -e right_arm -e left_leg -e right_leg

# Build all ESP32 subsystems
pio run -e left_arm_advanced -e right_arm_advanced

# Build everything
pio run
```

### **Upload to Specific Subsystem**
```powershell
# Upload to head subsystem
pio run -e goblin_head -t upload

# Upload to all arm subsystems
pio run -e left_arm -e right_arm -t upload

# Upload to specific port (override COM port)
pio run -e goblin_head -t upload --upload-port COM12
```

### **Monitor Specific Subsystem**
```powershell
# Monitor head subsystem
pio device monitor --environment goblin_head

# Monitor with custom port
pio device monitor --port COM3 --baud 115200
```

## 📝 **Preprocessor Directives Strategy**

### **Chip Type Detection**
```cpp
// src/p32_shared_state.h
#ifdef ESP32_CHIP_TYPE_S3
    #define CHIP_HAS_PSRAM 1
    #define CHIP_GPIO_COUNT 45
    #define CHIP_SRAM_SIZE 512000
#elif defined(ESP32_CHIP_TYPE_C3)
    #define CHIP_HAS_PSRAM 0
    #define CHIP_GPIO_COUNT 22
    #define CHIP_SRAM_SIZE 400000
#elif defined(ESP32_CHIP_TYPE_STANDARD)
    #define CHIP_HAS_PSRAM 0
    #define CHIP_GPIO_COUNT 34
    #define CHIP_SRAM_SIZE 520000
#endif
```

### **Subsystem Feature Enablement**
```cpp
// Component selection based on chip capabilities
#if defined(ESP32_CHIP_TYPE_S3)
    #define ENABLE_DUAL_DISPLAYS 1
    #define ENABLE_ADVANCED_AUDIO 1
    #define ENABLE_COMPLEX_ALGORITHMS 1
#elif defined(ESP32_CHIP_TYPE_C3)
    #define ENABLE_BASIC_CONTROL 1
    #define ENABLE_COST_OPTIMIZATION 1
    #define MAX_SERVO_COUNT 8
#endif

// Subsystem-specific features
#ifdef SUBSYSTEM_GOBLIN_HEAD
    #define COMPONENT_LEFT_EYE_ENABLED 1
    #define COMPONENT_RIGHT_EYE_ENABLED 1
    #define COMPONENT_MOUTH_ENABLED 1
#endif

#ifdef SUBSYSTEM_LEFT_ARM
    #define COMPONENT_SHOULDER_ENABLED 1
    #define COMPONENT_ELBOW_ENABLED 1
    #ifdef ARM_ADVANCED_CONTROL
        #define COMPONENT_HAND_ENABLED 1
    #endif
#endif
```

## 🔄 **Development Workflow**

### **Phase 1: Prototype with ESP32-S3 (All Subsystems)**
```ini
# Development environment - use S3 for everything
[env:dev_all_s3]
extends = common_esp32s3
board = esp32-s3-devkitc-1
build_flags = 
    ${common_esp32s3.build_flags}
    -DDEV_MODE_ALL_SUBSYSTEMS=1
    -DENABLE_ALL_FEATURES=1
```

### **Phase 2: Optimize Individual Subsystems**
- Test each subsystem on target chip type
- Validate GPIO and memory usage
- Performance benchmarking

### **Phase 3: Production Configuration**
- Deploy cost-optimized chip selection
- Finalize pin assignments
- Production testing

## 💰 **Cost Analysis Summary**

| Configuration | Chip Count | Total Cost | Use Case |
|---------------|------------|------------|----------|
| **All ESP32-S3** | 6 chips | $48 | Development/Prototype |
| **Optimized Mix** | 2×S3 + 4×C3 | $28 | Production Optimal |
| **Maximum Cost-Opt** | 1×S3 + 5×C3 | $23 | Budget Build |

## 🛠 **Implementation Steps**

1. **Clean Up Current platformio.ini** - Remove duplications and formatting issues
2. **Implement Multi-Variant Architecture** - Add chip-specific base configurations
3. **Update Component Code** - Add preprocessor directives for chip capabilities
4. **Test Build Matrix** - Verify all environments build successfully
5. **Validate Hardware Mapping** - Ensure GPIO assignments match chip capabilities
6. **Document Build Process** - Create subsystem-specific build instructions

## 🚨 **Critical Implementation Notes**

- **Board Definitions**: Ensure correct board types for each chip variant
- **Pin Compatibility**: Validate GPIO assignments don't exceed chip limits
- **Memory Management**: Monitor RAM usage, especially on ESP32-C3
- **Communication Protocol**: ESP-NOW mesh must work across all chip types
- **Upload Ports**: Assign unique COM ports for each subsystem controller
- **Build Dependencies**: Some advanced features only available on S3 chips

---

This strategy provides a scalable, cost-optimized approach to managing multiple ESP32 variants while maintaining development flexibility and production efficiency.