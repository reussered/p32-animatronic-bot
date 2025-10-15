# System Audit - Missing Components Report
## Comprehensive JSON Configuration Gap Analysis - October 15, 2025

---

## Executive Summary

**Purpose**: Identify all missing JSON component definitions and inconsistencies before resuming goblin skull shape work.

**Scope**: Complete system scan of all JSON files, documentation references, and subsystem configurations.

**Critical Findings**: 21 missing component definitions across 3 levels (System, Family, Bot-Specific)

**Priority**: HIGH - These gaps block code generation and firmware compilation

---

## Three-Level Component Architecture Review

### Architecture Context

Per `docs/THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md`, all functionality must be implemented as components:

1. **Level 1: System Components** - Core platform (WiFi, Mesh, Watchdog, etc.)
2. **Level 2: Family Components** - Shared behavior (Goblin personality, mood, etc.)
3. **Level 3: Bot-Specific Components** - Positioned hardware (eyes, nose, mouth, sensors)

**CRITICAL**: The architecture documentation references components that DO NOT EXIST in the JSON configs.

---

## MISSING COMPONENTS BY LEVEL

---

## 🔴 LEVEL 1: System Components (8 MISSING)

### Referenced in Documentation

From `docs/DISTRIBUTED-PROCESSING-ARCHITECTURE.md` and `config/subsystems/goblin_torso.json`:

#### 1. **system_core.json** - MISSING ❌
- **Referenced In**: `config/subsystems/goblin_torso.json` line 31
- **Expected Path**: `config/components/system/system_core.json`
- **Purpose**: Core system management (hitCount: 100)
- **Status**: File does not exist
- **Priority**: CRITICAL

#### 2. **network_monitor.json** - MISSING ❌
- **Referenced In**: `config/subsystems/goblin_torso.json` line 32
- **Expected Path**: `config/components/system/network_monitor.json`
- **Purpose**: Connectivity monitoring (hitCount: 50)
- **Status**: File does not exist
- **Priority**: CRITICAL

#### 3. **bluetooth_central.json** - MISSING ❌
- **Referenced In**: `config/subsystems/goblin_torso.json` line 35
- **Expected Path**: `config/components/system/bluetooth_central.json`
- **Purpose**: BLE coordination (hitCount: 75)
- **Status**: File does not exist
- **Priority**: MEDIUM (optional for MVP)

#### 4. **watchdog.json** - MISSING ❌
- **Referenced In**: `config/subsystems/goblin_torso.json` line 37
- **Expected Path**: `config/components/system/watchdog.json`
- **Purpose**: Health monitoring (hitCount: 500)
- **Status**: File does not exist
- **Priority**: CRITICAL

#### 5. **power_monitor.json** - MISSING ❌
- **Referenced In**: `config/subsystems/goblin_torso.json` line 38
- **Expected Path**: `config/components/system/power_monitor.json`
- **Purpose**: Voltage/current monitoring (hitCount: 200)
- **Status**: File does not exist
- **Priority**: HIGH

#### 6. **serial_console.json** - MISSING ❌
- **Referenced In**: `docs/THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md`
- **Expected Path**: `config/components/system/serial_console.json`
- **Purpose**: Debug/telemetry interface (hitCount: 25)
- **Status**: File does not exist
- **Priority**: MEDIUM (development aid)

### Existing System Components ✅

- ✅ **wifi_station.json** - EXISTS
- ✅ **esp_now_mesh.json** - EXISTS
- ✅ **telemetry_hub.json** - EXISTS

---

## 🟡 LEVEL 2: Family Components (4 MISSING)

### Referenced in Documentation

From `docs/THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md`:

#### 7. **goblin_personality.json** - MISSING ❌
- **Referenced In**: Architecture documentation, code examples
- **Expected Path**: `config/components/functional/goblin_personality.json`
- **Purpose**: Goblin family behavior traits (hitCount: 25)
- **Status**: File does not exist
- **Priority**: HIGH

#### 8. **goblin_mood.json** - MISSING ❌
- **Referenced In**: Architecture documentation
- **Expected Path**: `config/components/functional/goblin_mood.json`
- **Purpose**: 8-mood emotional state machine (hitCount: 15)
- **Status**: File does not exist
- **Priority**: HIGH

#### 9. **goblin_behavior_engine.json** - MISSING ❌
- **Referenced In**: Architecture documentation
- **Expected Path**: `config/components/functional/goblin_behavior_engine.json`
- **Purpose**: Action selection logic (hitCount: 10)
- **Status**: File does not exist
- **Priority**: HIGH

#### 10. **goblin_family.json** - INCOMPLETE ⚠️
- **Current Path**: `config/templates/families/goblin_family.json`
- **Status**: File exists BUT does not contain `family_level_components` array
- **Missing Field**: `family_level_components[]` array should reference functional components
- **Priority**: HIGH

---

## 🟢 LEVEL 3: Bot-Specific Components (9 MISSING)

### Head Subsystem Components

#### In goblin_full.json but NOT in positioned/ directory:

**Current goblin_full.json references** (lines 24-27):
```json
"positioned_components": [
  "config/components/positioned/goblin/head_components/goblin_eye_left.json",
  "config/components/positioned/goblin/head_components/goblin_eye_right.json",
  "config/components/positioned/goblin/head_components/goblin_speaker.json",
  "config/components/positioned/goblin/head_components/goblin_nose.json"
]
```

**Directory Structure Issue**:
- ✅ Files exist in `config/components/positioned/goblin/head_components/`
- ❌ But goblin_full.json references OLD PATHS (without `/goblin/head_components/` subdirectory)

#### Missing Positioned Components Referenced in goblin_head.json:

From `config/subsystems/goblin_head.json` (lines 32-38):

#### 11. **goblin_ear_left.json** - VERSION MISMATCH ⚠️
- **Current Path**: `config/components/positioned/goblin_ear_left.json` (v1.0.0, old format)
- **Better Path**: `config/components/positioned/goblin/head_components/goblin_ear_left.json`
- **Issue**: Missing `component_name` and `timing.hitCount` fields
- **Priority**: MEDIUM

#### 12. **goblin_ear_right.json** - VERSION MISMATCH ⚠️
- **Current Path**: `config/components/positioned/goblin_ear_right.json` (v1.0.0, old format)
- **Better Path**: `config/components/positioned/goblin/head_components/goblin_ear_right.json`
- **Issue**: Missing `component_name` and `timing.hitCount` fields
- **Priority**: MEDIUM

#### 13. **goblin_mouth.json** - VERSION MISMATCH ⚠️
- **Current Path**: `config/components/positioned/goblin_mouth.json` (v1.0.0, old format)
- **Better Path**: `config/components/positioned/goblin/head_components/goblin_mouth.json`
- **Issue**: Missing `template_inheritance` field (newer components have this)
- **Priority**: HIGH (removed from goblin_full.json due to hardware unavailability)

### Torso Subsystem Components

From `config/subsystems/goblin_torso.json` (lines 42-46):

#### 14. **spine_flexion_servo.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/torso_components/spine_flexion_servo.json`
- **Purpose**: Torso spine flexion servo (hitCount: 20)
- **Status**: File does not exist
- **Priority**: LOW (future expansion)

#### 15. **spine_extension_servo.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/torso_components/spine_extension_servo.json`
- **Purpose**: Torso spine extension servo (hitCount: 20)
- **Status**: File does not exist
- **Priority**: LOW (future expansion)

#### 16. **waist_rotation_servo.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/torso_components/waist_rotation_servo.json`
- **Purpose**: Torso waist rotation servo (hitCount: 20)
- **Status**: File does not exist
- **Priority**: LOW (future expansion)

#### 17. **torso_status_led.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/torso_components/torso_status_led.json`
- **Purpose**: Torso status LED (hitCount: 50)
- **Status**: File does not exist
- **Priority**: LOW (future expansion)

#### 18. **torso_speaker.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/torso_components/torso_speaker.json`
- **Purpose**: Torso speaker audio (hitCount: 15)
- **Status**: File does not exist
- **Priority**: LOW (future expansion)

### Head Subsystem - Neck Servos

From `docs/DISTRIBUTED-PROCESSING-ARCHITECTURE.md`:

#### 19. **neck_pan_servo.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/head_components/neck_pan_servo.json`
- **Purpose**: Neck pan/yaw servo (hitCount: 10)
- **Status**: File does not exist
- **Priority**: MEDIUM (future enhancement)

#### 20. **neck_tilt_servo.json** - MISSING ❌
- **Expected Path**: `config/components/positioned/goblin/head_components/neck_tilt_servo.json`
- **Purpose**: Neck tilt/pitch servo (hitCount: 10)
- **Status**: File does not exist
- **Priority**: MEDIUM (future enhancement)

---

## 🔵 HARDWARE COMPONENTS (1 INCOMPLETE)

#### 21. **directional_microphone.json** - WRONG COMPONENT TYPE ⚠️
- **Current Path**: `config/components/hardware/directional_microphone.json`
- **Issue**: File describes **directional microphone** but goblin ears reference it for **HW-496 MEMS microphones**
- **Expected**: Should be `hw496_microphone.json` or `mems_microphone.json`
- **Status**: Mismatched hardware type
- **Priority**: HIGH (hardware on hand, needs proper config)

**Action Required**: Create separate `hw496_microphone.json` for actual hardware.

---

## INCONSISTENCIES & PATH ISSUES

### 1. **Inconsistent Directory Structure**

**Issue**: Positioned components exist in multiple locations:

```
config/components/positioned/
  ├── goblineye_left.json  ❌ OLD LOCATION (no subdirectory)
  ├── goblineye_right.json ❌ OLD LOCATION
  ├── goblin_mouth.json    ❌ OLD LOCATION
  ├── goblin_nose.json     ❌ OLD LOCATION
  ├── goblin_speaker.json  ❌ OLD LOCATION
  ├── goblin_ear_left.json ❌ OLD LOCATION
  └── goblin/
      └── head_components/
          ├── goblin_eye_left.json   ✅ NEW LOCATION (v2.0.0, better format)
          ├── goblin_eye_right.json  ✅ NEW LOCATION (v1.0.0)
          ├── goblin_nose.json       ✅ NEW LOCATION (v2.0.0)
          ├── goblin_speaker.json    ✅ NEW LOCATION (v1.0.0)
          └── goblin_mouth.json      ✅ NEW LOCATION (v1.0.0)
```

**Recommendation**: Migrate ALL positioned components to organized subdirectories.

### 2. **Version Inconsistencies**

| File | Current Version | Issue |
|------|----------------|--------|
| `goblin_eye_left.json` | v2.0.0 | Has `template_inheritance` (newer format) |
| `goblin_eye_right.json` | v1.0.0 | Missing `template_inheritance` (older format) |
| `goblin_speaker.json` | v1.0.0 | Missing `template_inheritance` |
| `goblin_nose.json` | v2.0.0 | Has `template_inheritance` |

**Recommendation**: Standardize all positioned components to v2.0.0 format with `template_inheritance`.

### 3. **GPIO Pin Assignment Conflicts**

From `docs/GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md` and JSON configs:

| GPIO | Wiring Guide Assignment | JSON Config Assignment | Status |
|------|------------------------|------------------------|--------|
| 25 | HC-SR04 TRIG | ❌ Not in gpio_pair_1.json (uses GPIO 9) | MISMATCH |
| 34 | HC-SR04 ECHO | ❌ Not in gpio_pair_1.json (uses GPIO 10) | MISMATCH |
| 14 | SPI_CLK | ✅ Correct | OK |
| 13 | SPI_MOSI | ✅ Correct | OK |
| 27 | Left Eye DC | ✅ Matches spi_device_1.json | OK |
| 15 | Left Eye CS | ✅ Matches spi_device_1.json | OK |
| 33 | Right Eye DC | ✅ Matches spi_device_2.json | OK |
| 32 | Right Eye CS | ✅ Matches spi_device_2.json | OK |
| 26 | Both Displays RST | ✅ Correct | OK |

**Issue**: `config/components/interfaces/gpio_pair_1.json` uses GPIO 9/10, but wiring guide uses GPIO 25/34.

**Recommendation**: Update `gpio_pair_1.json` to match physical wiring:
```json
"pins": {
  "trig": 25,  // Change from 9 to 25
  "echo": 34   // Change from 10 to 34
}
```

### 4. **Missing Hardware Reference**

**Issue**: HW-496 microphones are physically available but have no JSON config.

From `docs/HARDWARE-TESTING-STATUS.md`:
- ✅ **HW-496 Microphones** | MEMS microphone | 2 | ✅ On hand (ears)

But `config/components/hardware/` does not contain `hw496_microphone.json`.

**Recommendation**: Create proper hardware config for HW-496.

---

## WORK REQUEST: Fix All Missing Components

---

## 🎯 TASK 1: Create Missing System Components (Priority: CRITICAL)

**Files to Create**:

### 1.1 system_core.json
```json
{
  "relative_filename": "config/components/system/system_core.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "system_core",
  "component_type": "SYSTEM_LEVEL",
  "description": "Core system management - initialization, health checks, error handling",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 100,
    "description": "Core system check every 10 seconds"
  },
  "system_requirements": {
    "min_controller": "ESP32",
    "required_features": ["FreeRTOS"],
    "memory_footprint_kb": "32",
    "cpu_usage_percent": "1-2"
  },
  "responsibilities": [
    "Initialize FreeRTOS tasks",
    "Manage system heap memory",
    "Handle critical errors",
    "Coordinate subsystem initialization",
    "Provide system status reporting"
  ]
}
```

### 1.2 network_monitor.json
```json
{
  "relative_filename": "config/components/system/network_monitor.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "network_monitor",
  "component_type": "SYSTEM_LEVEL",
  "description": "Network connectivity monitoring - WiFi signal strength, connection status",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 50,
    "description": "Network check every 5 seconds"
  },
  "system_requirements": {
    "min_controller": "ESP32",
    "required_features": ["WIFI_STA"],
    "memory_footprint_kb": "24",
    "cpu_usage_percent": "1-3"
  },
  "monitoring_metrics": {
    "wifi_rssi": true,
    "connection_quality": true,
    "packet_loss": true,
    "latency_ms": true
  },
  "responsibilities": [
    "Monitor WiFi signal strength (RSSI)",
    "Detect connection drops",
    "Report connectivity status to telemetry",
    "Trigger reconnection on failure"
  ]
}
```

### 1.3 watchdog.json
```json
{
  "relative_filename": "config/components/system/watchdog.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "watchdog",
  "component_type": "SYSTEM_LEVEL",
  "description": "Hardware watchdog timer - system health monitoring and automatic recovery",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 500,
    "description": "Watchdog heartbeat every 50 seconds"
  },
  "system_requirements": {
    "min_controller": "ESP32",
    "required_features": ["WATCHDOG_TIMER"],
    "memory_footprint_kb": "16",
    "cpu_usage_percent": "1"
  },
  "watchdog_config": {
    "timeout_seconds": 60,
    "auto_recovery": true,
    "panic_on_timeout": false
  },
  "responsibilities": [
    "Reset watchdog timer periodically",
    "Monitor system responsiveness",
    "Trigger system reboot on hang",
    "Log watchdog resets to telemetry"
  ]
}
```

### 1.4 power_monitor.json
```json
{
  "relative_filename": "config/components/system/power_monitor.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "power_monitor",
  "component_type": "SYSTEM_LEVEL",
  "description": "Power monitoring - battery voltage, current consumption, remaining capacity",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 200,
    "description": "Power check every 20 seconds"
  },
  "system_requirements": {
    "min_controller": "ESP32",
    "required_features": ["ADC"],
    "memory_footprint_kb": "20",
    "cpu_usage_percent": "1-2"
  },
  "monitoring_config": {
    "voltage_adc_pin": 36,
    "current_adc_pin": 39,
    "low_battery_threshold_mv": 3300,
    "critical_battery_threshold_mv": 3000
  },
  "responsibilities": [
    "Measure battery voltage via ADC",
    "Calculate remaining battery percentage",
    "Trigger low-power mode on low battery",
    "Report power status to telemetry"
  ]
}
```

### 1.5 bluetooth_central.json (OPTIONAL)
```json
{
  "relative_filename": "config/components/system/bluetooth_central.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "bluetooth_central",
  "component_type": "SYSTEM_LEVEL",
  "description": "Bluetooth Low Energy (BLE) central controller - mobile app connectivity",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 75,
    "description": "BLE management every 7.5 seconds"
  },
  "system_requirements": {
    "min_controller": "ESP32",
    "required_features": ["BLE_CENTRAL"],
    "memory_footprint_kb": "64",
    "cpu_usage_percent": "3-8"
  },
  "ble_config": {
    "device_name": "Goblin_Bot",
    "auto_advertise": true,
    "connection_timeout_s": 30
  },
  "responsibilities": [
    "Advertise BLE services",
    "Accept mobile app connections",
    "Handle BLE command messages",
    "Report BLE status to telemetry"
  ]
}
```

### 1.6 serial_console.json (OPTIONAL)
```json
{
  "relative_filename": "config/components/system/serial_console.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "serial_console",
  "component_type": "SYSTEM_LEVEL",
  "description": "Serial console interface - debug commands and telemetry output",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 25,
    "description": "Console check every 2.5 seconds"
  },
  "system_requirements": {
    "min_controller": "ESP32",
    "required_features": ["UART"],
    "memory_footprint_kb": "40",
    "cpu_usage_percent": "2-5"
  },
  "console_config": {
    "baud_rate": 115200,
    "command_prompt": "goblin> ",
    "echo_enabled": true
  },
  "responsibilities": [
    "Process debug commands via UART",
    "Output system logs to console",
    "Interactive component testing",
    "Real-time parameter adjustment"
  ]
}
```

---

## 🎯 TASK 2: Create Missing Family Components (Priority: HIGH)

### 2.1 goblin_personality.json
```json
{
  "relative_filename": "config/components/functional/goblin_personality.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "goblin_personality",
  "component_type": "FAMILY_LEVEL",
  "family_type": "GOBLIN",
  "description": "Goblin family personality traits - mischievous, aggressive, curious",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 25,
    "description": "Personality processing every 2.5 seconds"
  },
  "personality_traits": {
    "base_aggression": 60,
    "base_curiosity": 70,
    "base_fear": 20,
    "threat_sensitivity": "HIGH",
    "exploration_drive": "MEDIUM"
  },
  "behavior_modifiers": {
    "approach_distance_cm": 50,
    "flee_distance_cm": 20,
    "idle_movement_frequency": "FREQUENT",
    "vocalization_style": "GUTTURAL_GROWLS"
  },
  "responsibilities": [
    "Apply personality-driven mood modifications",
    "Influence behavior selection probabilities",
    "Adjust threat response thresholds",
    "Modulate vocalization patterns"
  ]
}
```

### 2.2 goblin_mood.json
```json
{
  "relative_filename": "config/components/functional/goblin_mood.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "goblin_mood",
  "component_type": "FAMILY_LEVEL",
  "family_type": "GOBLIN",
  "description": "Goblin 8-mood emotional state machine - integrates sensor inputs and personality",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 15,
    "description": "Mood update every 1.5 seconds"
  },
  "mood_components": {
    "ANGER": { "decay_rate": 5, "max_value": 127 },
    "FEAR": { "decay_rate": 3, "max_value": 127 },
    "HAPPINESS": { "decay_rate": 2, "max_value": 127 },
    "SADNESS": { "decay_rate": 2, "max_value": 127 },
    "CURIOSITY": { "decay_rate": 4, "max_value": 127 },
    "AFFECTION": { "decay_rate": 3, "max_value": 127 },
    "IRRITATION": { "decay_rate": 6, "max_value": 127 },
    "CONTENTMENT": { "decay_rate": 1, "max_value": 127 }
  },
  "responsibilities": [
    "Update 8-component mood state from sensor inputs",
    "Apply natural mood decay over time",
    "Synchronize mood state via ESP-NOW mesh",
    "Trigger animations based on mood changes"
  ]
}
```

### 2.3 goblin_behavior_engine.json
```json
{
  "relative_filename": "config/components/functional/goblin_behavior_engine.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_name": "goblin_behavior_engine",
  "component_type": "FAMILY_LEVEL",
  "family_type": "GOBLIN",
  "description": "Goblin behavior selection engine - chooses actions based on mood and personality",
  "created": "2025-10-15",
  "timing": {
    "hitCount": 10,
    "description": "Behavior decision every 1 second"
  },
  "behavior_states": {
    "IDLE": { "animation": "idle_scan", "sound": null },
    "CURIOUS": { "animation": "curious_look", "sound": "curious_chirp" },
    "AGGRESSIVE": { "animation": "angry_stare", "sound": "growl_angry" },
    "FEARFUL": { "animation": "blink", "sound": null },
    "CONTENT": { "animation": "blink", "sound": "ambient_breathing" }
  },
  "transition_thresholds": {
    "idle_to_curious": 50,
    "curious_to_aggressive": 70,
    "aggressive_to_fearful": 80,
    "any_to_content": 60
  },
  "responsibilities": [
    "Select current behavior based on mood state",
    "Trigger animations via ESP-NOW commands",
    "Select audio clips for vocalization",
    "Manage behavior state transitions"
  ]
}
```

### 2.4 Update goblin_family.json

**Add to existing file** `config/templates/families/goblin_family.json`:

```json
{
  "family_level_components": [
    "config/components/functional/goblin_personality.json",
    "config/components/functional/goblin_mood.json",
    "config/components/functional/goblin_behavior_engine.json"
  ]
}
```

---

## 🎯 TASK 3: Create Missing Hardware Component (Priority: HIGH)

### 3.1 hw496_microphone.json

Create `config/components/hardware/hw496_microphone.json`:

```json
{
  "relative_filename": "config/components/hardware/hw496_microphone.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_type": "MEMS_MICROPHONE",
  "hardware_name": "HW-496 MEMS Microphone Module",
  "description": "HW-496 MEMS microphone with MAX4466 pre-amplifier",
  "created": "2025-10-15",
  
  "hardware_specs": {
    "microphone_type": "electret_mems",
    "frequency_response": "20Hz_to_20kHz",
    "sensitivity": "adjustable_gain",
    "max_spl": "110dB_SPL",
    "signal_to_noise": "58dB"
  },
  
  "physical_properties": {
    "board_dimensions": {
      "width": "20mm",
      "length": "15mm",
      "height": "7mm"
    },
    "weight": "1.5g",
    "mounting_holes": {
      "count": 2,
      "diameter": "2mm",
      "spacing": "15mm"
    }
  },
  
  "control_interface": {
    "protocol": "analog",
    "output_type": "analog_voltage",
    "output_range": "0-3.3V",
    "adc_connection": "single_ended"
  },
  
  "power_requirements": {
    "operating_voltage": "2.4V-5V",
    "typical_voltage": "3.3V",
    "current_consumption": "4mA_typical",
    "max_current": "10mA"
  },
  
  "pin_mapping": {
    "pins": [
      {"name": "VCC", "type": "power", "voltage": "3.3V"},
      {"name": "GND", "type": "ground"},
      {"name": "OUT", "type": "analog_output", "description": "Audio signal output"}
    ]
  },
  
  "gain_adjustment": {
    "method": "potentiometer",
    "range": "25x_to_125x",
    "default": "50x"
  },
  
  "prototype_status": "available_on_hand",
  "tested": false,
  "quantity_available": 2,
  "supplier": "Generic/AliExpress",
  "notes": [
    "Physical hardware available for testing",
    "Requires ADC pin assignment for each ear",
    "Adjustable gain via onboard trimpot",
    "Can be used for sound detection or voice input"
  ]
}
```

---

## 🎯 TASK 4: Fix GPIO Pin Assignment Mismatch (Priority: HIGH)

### 4.1 Update gpio_pair_1.json

**File**: `config/components/interfaces/gpio_pair_1.json`

**Current Content** (WRONG):
```json
{
  "pins": {
    "trig": 9,
    "echo": 10
  }
}
```

**Updated Content** (CORRECT - matches wiring guide):
```json
{
  "pins": {
    "trig": 25,
    "echo": 34
  }
}
```

**Reason**: Must match physical wiring in `docs/GOBLIN-HEAD-BREADBOARD-WIRING-GUIDE.md`.

---

## 🎯 TASK 5: Update Ear Microphone Components (Priority: MEDIUM)

### 5.1 Update goblin_ear_left.json

**Current File**: `config/components/positioned/goblin_ear_left.json` (v1.0.0)

**Update to v2.0.0 format** and fix hardware reference:

```json
{
  "relative_filename": "config/components/positioned/goblin/head_components/goblin_ear_left.json",
  "version": "2.0.0",
  "author": "config/author.json",
  "component_id": "goblin_ear_left",
  
  "template_inheritance": {
    "hardware_template": "config/components/hardware/hw496_microphone.json",
    "creature_shell": "assets/shapes/scad/creature_shells/goblin/goblin_ear_left_shell.scad",
    "mount_template": "assets/shapes/scad/basic_mounts/microphone_basic_mount.scad"
  },
  
  "interface_assignment": "ADC_PIN_35",
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center",
  "position": {
    "x": "-1.5 INCH",
    "y": "0.5 INCH", 
    "z": "0.1 INCH"
  },
  "orientation": {
    "tilt": "15 DEGREES",
    "rotation": "-30 DEGREES",
    "roll": "0 DEGREES"
  },
  
  "creature_specific_config": {
    "ear_shape": "POINTED_GOBLIN",
    "gain_setting": "75x",
    "sensitivity": "HIGH"
  },
  
  "function": "left_ear_audio_input",
  "component_name": "left_ear_microphone",
  "description": "Left ear HW-496 microphone for directional audio input",
  "timing": {
    "hitCount": 20
  }
}
```

### 5.2 Update goblin_ear_right.json

Similar update for right ear (mirror of left).

---

## 🎯 TASK 6: Standardize All Positioned Components to v2.0.0 (Priority: MEDIUM)

**Files to Update**:

1. `config/components/positioned/goblin/head_components/goblin_eye_right.json` - Add `template_inheritance`
2. `config/components/positioned/goblin/head_components/goblin_speaker.json` - Add `template_inheritance`
3. `config/components/positioned/goblin/head_components/goblin_mouth.json` - Add `template_inheritance`

**Pattern**: Follow `goblin_eye_left.json` (v2.0.0) format with `template_inheritance` block.

---

## 🎯 TASK 7: Create ADC Interface Definitions (Priority: HIGH)

**Required for HW-496 microphones**:

### 7.1 Create adc_pin_35.json

```json
{
  "relative_filename": "config/components/interfaces/adc_pin_35.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "interface_id": "ADC_PIN_35",
  "interface_type": "ADC_INPUT",
  "pins": {
    "adc": 35
  },
  "adc_config": {
    "atten": "ADC_ATTEN_DB_11",
    "width": "ADC_WIDTH_BIT_12",
    "sample_rate_hz": 8000
  }
}
```

### 7.2 Create adc_pin_36.json

```json
{
  "relative_filename": "config/components/interfaces/adc_pin_36.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "interface_id": "ADC_PIN_36",
  "interface_type": "ADC_INPUT",
  "pins": {
    "adc": 36
  },
  "adc_config": {
    "atten": "ADC_ATTEN_DB_11",
    "width": "ADC_WIDTH_BIT_12",
    "sample_rate_hz": 8000
  }
}
```

---

## 🎯 TASK 8: Update goblin_full.json References (Priority: HIGH)

**Current Issue**: goblin_full.json uses old paths without subdirectories.

**File**: `config/bots/bot_families/goblins/goblin_full.json`

**Update positioned_components array**:

```json
"positioned_components": [
  "config/components/positioned/goblin/head_components/goblin_eye_left.json",
  "config/components/positioned/goblin/head_components/goblin_eye_right.json",
  "config/components/positioned/goblin/head_components/goblin_speaker.json",
  "config/components/positioned/goblin/head_components/goblin_nose.json",
  "config/components/positioned/goblin/head_components/goblin_ear_left.json",
  "config/components/positioned/goblin/head_components/goblin_ear_right.json"
]
```

**Add family_level_components array**:

```json
"family_level_components": [
  "config/components/functional/goblin_personality.json",
  "config/components/functional/goblin_mood.json",
  "config/components/functional/goblin_behavior_engine.json"
]
```

---

## SUMMARY OF WORK REQUIRED

### File Creation Count

| Category | Count | Priority |
|----------|-------|----------|
| System Components | 6 | CRITICAL (3), HIGH (1), MEDIUM (2) |
| Family Components | 3 | HIGH |
| Hardware Components | 1 | HIGH |
| Interface Components | 2 | HIGH |
| Positioned Components (updates) | 5 | MEDIUM |
| **TOTAL** | **17 files** | - |

### Time Estimate

| Task | Estimated Time |
|------|---------------|
| Task 1: System Components | 30 minutes |
| Task 2: Family Components | 30 minutes |
| Task 3: Hardware Component | 15 minutes |
| Task 4: GPIO Fix | 5 minutes |
| Task 5: Ear Updates | 20 minutes |
| Task 6: Version Standardization | 20 minutes |
| Task 7: ADC Interfaces | 10 minutes |
| Task 8: goblin_full.json Updates | 10 minutes |
| **TOTAL** | **~2.5 hours** |

---

## RECOMMENDED EXECUTION ORDER

1. **Task 4** - GPIO pin fix (5 min) - UNBLOCKS PHYSICAL TESTING
2. **Task 3** - HW-496 microphone config (15 min) - UNBLOCKS EAR COMPONENTS
3. **Task 7** - ADC interface definitions (10 min) - UNBLOCKS EAR COMPONENTS
4. **Task 5** - Update ear components (20 min) - COMPLETES EAR INTEGRATION
5. **Task 1** - System components (30 min) - UNBLOCKS TORSO SUBSYSTEM
6. **Task 2** - Family components (30 min) - UNBLOCKS BEHAVIOR SYSTEM
7. **Task 8** - goblin_full.json updates (10 min) - TIES EVERYTHING TOGETHER
8. **Task 6** - Version standardization (20 min) - CLEANUP & CONSISTENCY

---

## TESTING CHECKLIST AFTER FIXES

Once all components are created:

- [ ] All JSON files pass schema validation
- [ ] No missing file references in any JSON
- [ ] GPIO assignments match wiring guide
- [ ] All positioned components have `timing.hitCount`
- [ ] All positioned components have `component_name`
- [ ] All v2.0.0 components have `template_inheritance`
- [ ] goblin_full.json references correct paths
- [ ] System components referenced by goblin_torso.json exist
- [ ] Family components referenced by architecture exist
- [ ] Hardware components match physical inventory

---

## CONCLUSION

**Current State**: 21 missing/incomplete component definitions block code generation and firmware compilation.

**Root Cause**: Documentation-driven development created architecture specs before implementing all JSON configs.

**Impact**: Cannot generate component tables or compile firmware until gaps are filled.

**Next Steps**: 
1. Execute work request tasks 1-8 in recommended order
2. Validate all JSON files
3. Proceed with code generation
4. Resume goblin skull shape work

**Estimated Completion**: ~2.5 hours of focused work to create all missing configs.

---

**Document Status**: READY FOR EXECUTION  
**Created**: October 15, 2025  
**Priority**: HIGH - Blocks firmware development  
**Approval**: Awaiting user confirmation before proceeding
