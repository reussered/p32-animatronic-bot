# P32 Animatronic Bot - Unified Firmware Architecture

## Overview
The P32 Animatronic Bot system implements a revolutionary unified firmware architecture where **every single ESP32 controller in the system runs identical base code**. Component-specific functionality is enabled through conditional compilation using `#ifdef P32_COMP_*` preprocessor directives.

## Architectural Philosophy

### Single Codebase Principle
- **One Firmware**: All ESP32-S3 and ESP32-C3 controllers run the same firmware binary
- **Conditional Compilation**: Component-specific features enabled via `#ifdef` flags at compile time
- **Hardware Detection**: Runtime identification of connected components and interfaces
- **Modular Activation**: Only relevant code modules are compiled for each controller type

### Benefits of Unified Architecture
1. **Code Maintainability**: Single source of truth for all controller logic
2. **Testing Efficiency**: Test once, deploy everywhere with confidence
3. **Feature Consistency**: Identical mesh networking, diagnostics, and base services
4. **Development Speed**: Add features once, available across all subsystems
5. **Memory Optimization**: Only compile needed functionality for each controller

## Component Compilation Flags

### Head Subsystem
```c
#ifdef P32_COMP_GOBLIN_HEAD
    #include "goblin_head_controller.h"
    // Eye display management, facial expressions, audio playback
    #define P32_HAS_DISPLAYS 1
    #define P32_HAS_AUDIO 1
    #define P32_HAS_FACIAL_TRACKING 1
#endif
```

### Hand Controllers
```c
#ifdef P32_COMP_HAND_CONTROLLER
    #include "hand_controller.h"
    // 5-finger articulation, tactile feedback, gesture recognition
    #define P32_HAS_SERVO_CONTROL 1
    #define P32_HAS_TACTILE_SENSORS 1
    #define P32_HAS_GESTURE_LIBRARY 1
#endif
```

### Leg Controllers
```c
#ifdef P32_COMP_LEG_CONTROLLER
    #include "leg_controller.h"
    // 6DOF bipedal locomotion, balance control, gait patterns
    #define P32_HAS_LOCOMOTION 1
    #define P32_HAS_BALANCE_CONTROL 1
    #define P32_HAS_NEMA17_DRIVERS 1
#endif
```

### Foot Controllers
```c
#ifdef P32_COMP_FOOT_CONTROLLER
    #include "foot_controller.h"
    // Pressure monitoring, toe articulation, ground contact analysis
    #define P32_HAS_PRESSURE_SENSORS 1
    #define P32_HAS_TOE_CONTROL 1
    #define P32_HAS_BALANCE_FEEDBACK 1
#endif
```

### Torso Controller (Master)
```c
#ifdef P32_COMP_TORSO_MASTER
    #include "torso_controller.h"
    // System coordination, mesh master, behavior orchestration
    #define P32_IS_MESH_MASTER 1
    #define P32_HAS_BEHAVIOR_ENGINE 1
    #define P32_HAS_SYSTEM_DIAGNOSTICS 1
#endif
```

## Firmware Structure

### Core Base System (Always Compiled)
```c
// main.c - Universal entry point for all controllers
#include "esp_system.h"
#include "esp_now.h"
#include "freertos/FreeRTOS.h"

// Core services available on all controllers
#include "p32_mesh_network.h"        // ESP-NOW mesh communication
#include "p32_diagnostics.h"         // System health monitoring  
#include "p32_config_manager.h"      // JSON configuration loading
#include "p32_gpio_manager.h"        // Pin assignment and management
#include "p32_power_management.h"    // Battery and power optimization

void app_main(void) {
    // Initialize core services (identical on all controllers)
    p32_gpio_init();
    p32_mesh_network_init();
    p32_diagnostics_init();
    p32_power_management_init();
    
    // Component-specific initialization
    #ifdef P32_COMP_TORSO_MASTER
        torso_controller_init();
    #endif
    
    #ifdef P32_COMP_HAND_CONTROLLER
        hand_controller_init();
    #endif
    
    #ifdef P32_COMP_LEG_CONTROLLER
        leg_controller_init();
    #endif
    
    #ifdef P32_COMP_FOOT_CONTROLLER
        foot_controller_init();
    #endif
    
    #ifdef P32_COMP_GOBLIN_HEAD
        goblin_head_controller_init();
    #endif
    
    // Start unified task scheduler
    p32_task_scheduler_start();
}
```

### Conditional Module Loading
```c
// p32_module_loader.h - Compile-time module selection
#ifndef P32_MODULE_LOADER_H
#define P32_MODULE_LOADER_H

// Display management (only for controllers with displays)
#if defined(P32_COMP_GOBLIN_HEAD)
    #include "p32_display_manager.h"
    #define P32_HAS_DISPLAYS 1
#endif

// Servo control (hands, feet, head articulation)
#if defined(P32_COMP_HAND_CONTROLLER) || defined(P32_COMP_FOOT_CONTROLLER) || defined(P32_COMP_GOBLIN_HEAD)
    #include "p32_servo_controller.h"
    #define P32_HAS_SERVO_CONTROL 1
#endif

// High-torque motor control (legs, arms)
#if defined(P32_COMP_LEG_CONTROLLER) || defined(P32_COMP_ARM_CONTROLLER)
    #include "p32_stepper_controller.h"
    #define P32_HAS_STEPPER_MOTORS 1
#endif

// Audio playback (head subsystem)
#ifdef P32_COMP_GOBLIN_HEAD
    #include "p32_audio_manager.h"
    #define P32_HAS_AUDIO 1
#endif

// Sensor management (various subsystems)
#if defined(P32_COMP_FOOT_CONTROLLER) || defined(P32_COMP_HAND_CONTROLLER) || defined(P32_COMP_GOBLIN_HEAD)
    #include "p32_sensor_manager.h"
    #define P32_HAS_SENSORS 1
#endif

#endif // P32_MODULE_LOADER_H
```

## Build System Integration

### PlatformIO Build Environments
```ini
; platformio.ini - Unified firmware with multiple build targets

[env:torso-master]
platform = espressif32
board = esp32-s3-devkitc-1
framework = espidf
build_flags = 
    -DP32_COMP_TORSO_MASTER=1
    -DP32_IS_MESH_MASTER=1
lib_deps = ${common.lib_deps}

[env:hand-left]
platform = espressif32  
board = esp32-c3-devkitc-02
framework = espidf
build_flags = 
    -DP32_COMP_HAND_CONTROLLER=1
    -DP32_HAND_POSITION=LEFT
lib_deps = ${common.lib_deps}

[env:hand-right]
platform = espressif32
board = esp32-c3-devkitc-02  
framework = espidf
build_flags = 
    -DP32_COMP_HAND_CONTROLLER=1
    -DP32_HAND_POSITION=RIGHT
lib_deps = ${common.lib_deps}

[env:leg-left]
platform = espressif32
board = esp32-c3-devkitc-02
framework = espidf
build_flags = 
    -DP32_COMP_LEG_CONTROLLER=1
    -DP32_LEG_POSITION=LEFT
lib_deps = ${common.lib_deps}

[env:leg-right] 
platform = espressif32
board = esp32-c3-devkitc-02
framework = espidf
build_flags = 
    -DP32_COMP_LEG_CONTROLLER=1
    -DP32_LEG_POSITION=RIGHT
lib_deps = ${common.lib_deps}

[env:foot-left]
platform = espressif32
board = esp32-c3-devkitc-02
framework = espidf
build_flags = 
    -DP32_COMP_FOOT_CONTROLLER=1
    -DP32_FOOT_POSITION=LEFT
lib_deps = ${common.lib_deps}

[env:foot-right]
platform = espressif32
board = esp32-c3-devkitc-02
framework = espidf
build_flags = 
    -DP32_COMP_FOOT_CONTROLLER=1
    -DP32_FOOT_POSITION=RIGHT  
lib_deps = ${common.lib_deps}

[env:goblin-head]
platform = espressif32
board = esp32-s3-devkitc-1
framework = espidf
build_flags = 
    -DP32_COMP_GOBLIN_HEAD=1
    -DP32_HAS_PSRAM=1
lib_deps = ${common.lib_deps}

[common]
lib_deps = 
    ESP32Servo
    ArduinoJson
    FastLED
```

### Automated Build Script
```powershell
# tools\build-all-controllers.ps1 - Build unified firmware for all subsystems

Write-Host "Building P32 Unified Firmware for All Controllers..." -ForegroundColor Green

$Controllers = @(
    "torso-master",
    "hand-left", 
    "hand-right",
    "leg-left",
    "leg-right", 
    "foot-left",
    "foot-right",
    "goblin-head"
)

foreach ($controller in $Controllers) {
    Write-Host "Building $controller..." -ForegroundColor Yellow
    pio run -e $controller
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host " $controller build successful" -ForegroundColor Green
    } else {
        Write-Host " $controller build failed" -ForegroundColor Red
        exit 1
    }
}

Write-Host "All controller firmwares built successfully!" -ForegroundColor Green
```

## Memory and Performance Optimization

### Conditional Compilation Benefits
```c
// Example: Display management only compiled when needed
#ifdef P32_HAS_DISPLAYS
    static display_buffer_t display_buffers[3];  // Only allocate if displays present
    static TaskHandle_t display_task_handle;
    
    void p32_display_task(void *pvParameters) {
        // Display management code only present in head controller
    }
#endif

// Example: Servo control conditional compilation  
#ifdef P32_HAS_SERVO_CONTROL
    static servo_state_t servo_states[MAX_SERVOS];
    static void p32_servo_update_task(void *pvParameters) {
        // Servo control code only in hand/foot controllers
    }
#endif
```

### Memory Usage by Controller Type
| Controller Type | Flash Usage | RAM Usage | Features Compiled |
|---|---|---|---|
| Torso Master | ~1.2MB | ~180KB | Mesh master, behavior engine, diagnostics |  
| Hand Controller | ~800KB | ~120KB | Servo control, tactile sensors, gesture recognition |
| Leg Controller | ~900KB | ~140KB | Stepper motors, balance control, locomotion |
| Foot Controller | ~600KB | ~80KB | Pressure sensors, toe control, balance feedback |
| Head Controller | ~1.4MB | ~220KB | Displays, audio, facial tracking, mood system |

## Development Workflow

### Single Source Development
1. **Develop Feature**: Add new functionality to unified codebase
2. **Add Conditional Compilation**: Wrap feature in appropriate `#ifdef P32_COMP_*` blocks  
3. **Test Across Controllers**: Verify feature works on all relevant controller types
4. **Deploy Universally**: Single firmware update propagates to all matching controllers

### Feature Addition Example
```c
// Adding new gesture recognition feature to hand controllers
#ifdef P32_COMP_HAND_CONTROLLER
    #include "p32_gesture_recognition.h"
    
    // New gesture recognition function available on all hand controllers
    gesture_result_t p32_recognize_gesture(tactile_data_t *tactile_input) {
        // Gesture analysis implementation
        return gesture_result;
    }
    
    // Integrate into existing hand controller task  
    void hand_controller_task(void *pvParameters) {
        while (1) {
            // Existing hand control logic...
            
            // New gesture recognition integration
            tactile_data_t tactile_data;
            if (p32_read_tactile_sensors(&tactile_data)) {
                gesture_result_t gesture = p32_recognize_gesture(&tactile_data);
                if (gesture.confidence > GESTURE_THRESHOLD) {
                    p32_mesh_broadcast_gesture(gesture);
                }
            }
            
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
#endif
```

## Network Communication

### Unified Mesh Protocol
All controllers participate in the same ESP-NOW mesh network with identical protocol implementation:

```c
// p32_mesh_protocol.h - Universal mesh communication
typedef struct {
    uint8_t sender_id;
    uint8_t message_type;  
    uint32_t timestamp;
    uint8_t payload_size;
    uint8_t payload[240];  // Maximum ESP-NOW payload
    uint16_t checksum;
} p32_mesh_message_t;

// Message types understood by all controllers
#define P32_MSG_HEARTBEAT       0x01
#define P32_MSG_GESTURE         0x02  
#define P32_MSG_LOCOMOTION      0x03
#define P32_MSG_MOOD_CHANGE     0x04
#define P32_MSG_BEHAVIOR_CMD    0x05
#define P32_MSG_DIAGNOSTICS     0x06
#define P32_MSG_CONFIG_UPDATE   0x07
```

### Cross-Controller Coordination
```c
// Example: Gesture triggers coordinated response across subsystems
#ifdef P32_COMP_HAND_CONTROLLER
    // Hand controller detects wave gesture
    void on_gesture_detected(gesture_type_t gesture) {
        if (gesture == GESTURE_WAVE) {
            p32_mesh_message_t msg = {
                .message_type = P32_MSG_GESTURE,
                .payload_size = sizeof(gesture_wave_t)
            };
            memcpy(msg.payload, &gesture_data, sizeof(gesture_wave_t));
            p32_mesh_broadcast(&msg);
        }
    }
#endif

#ifdef P32_COMP_GOBLIN_HEAD  
    // Head controller responds to wave gesture
    void on_mesh_message_received(p32_mesh_message_t *msg) {
        if (msg->message_type == P32_MSG_GESTURE) {
            gesture_wave_t *wave = (gesture_wave_t*)msg->payload;
            if (wave->gesture_type == GESTURE_WAVE) {
                // Play friendly greeting animation
                p32_display_play_animation("greeting_wave.json");
                p32_audio_play_sound("hello.wav");
            }
        }
    }
#endif
```

## Diagnostic and Monitoring

### Universal Health Monitoring  
```c
// p32_diagnostics.h - System health monitoring on all controllers
typedef struct {
    uint8_t controller_type;
    uint8_t controller_id; 
    uint32_t uptime_seconds;
    uint16_t free_heap_kb;
    uint8_t cpu_usage_percent;
    uint8_t temperature_celsius;
    uint8_t component_status_flags;
    uint32_t error_count;
} p32_health_report_t;

// Health reporting task runs on all controllers
void p32_diagnostics_task(void *pvParameters) {
    while (1) {
        p32_health_report_t report;
        p32_gather_health_data(&report);
        
        // Broadcast health status to mesh network
        p32_mesh_message_t msg = {
            .message_type = P32_MSG_DIAGNOSTICS,
            .payload_size = sizeof(p32_health_report_t)
        };
        memcpy(msg.payload, &report, sizeof(p32_health_report_t));
        p32_mesh_broadcast(&msg);
        
        vTaskDelay(pdMS_TO_TICKS(5000));  // Report every 5 seconds
    }
}
```

## Configuration Management

### Runtime Configuration Discovery
```c
// p32_config_manager.h - JSON configuration loading on all controllers
void p32_config_init(void) {
    // All controllers load their configuration from JSON
    p32_config_load_base("config/p32_base_config.json");
    
    #ifdef P32_COMP_TORSO_MASTER
        p32_config_load_specific("config/torso_master_config.json");
    #endif
    
    #ifdef P32_COMP_HAND_CONTROLLER  
        #ifdef P32_HAND_POSITION_LEFT
            p32_config_load_specific("config/hand_left_config.json");
        #else
            p32_config_load_specific("config/hand_right_config.json");
        #endif
    #endif
    
    #ifdef P32_COMP_LEG_CONTROLLER
        #ifdef P32_LEG_POSITION_LEFT
            p32_config_load_specific("config/leg_left_config.json");
        #else  
            p32_config_load_specific("config/leg_right_config.json");
        #endif
    #endif
    
    // Additional controller-specific configs...
}
```

## Testing and Validation

### Unified Test Framework
```c
// tests/test_unified_firmware.c - Tests run on all controller types
void test_mesh_communication(void) {
    // Test mesh networking on all controllers
    p32_mesh_message_t test_msg = {.message_type = P32_MSG_HEARTBEAT};
    assert(p32_mesh_send(&test_msg) == ESP_OK);
}

void test_gpio_management(void) {
    // Test GPIO initialization on all controllers  
    assert(p32_gpio_init() == ESP_OK);
    
    #ifdef P32_HAS_SERVO_CONTROL
        assert(p32_servo_init() == ESP_OK);
    #endif
    
    #ifdef P32_HAS_DISPLAYS
        assert(p32_display_init() == ESP_OK); 
    #endif
}

void test_power_management(void) {
    // Test power optimization on all controllers
    p32_power_profile_t profile;
    assert(p32_power_get_status(&profile) == ESP_OK);
    assert(profile.battery_voltage > MIN_OPERATING_VOLTAGE);
}
```

## Conclusion

The unified firmware architecture provides unprecedented code maintainability and feature consistency across the entire P32 Animatronic Bot system. By using conditional compilation and modular design, we achieve:

- **Single Source of Truth**: One codebase for all controllers eliminates version conflicts
- **Efficient Development**: Add features once, deploy universally with confidence
- **Optimized Performance**: Only compile needed functionality for each controller type  
- **Consistent Behavior**: Identical mesh networking, diagnostics, and base services
- **Simplified Testing**: Test core functionality once, validate across all subsystems

This architecture enables rapid prototyping of new animatronic characters while maintaining robust, production-ready code quality across all subsystems.