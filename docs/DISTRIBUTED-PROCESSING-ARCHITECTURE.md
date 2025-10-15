# Distributed System Architecture - Processing Load Distribution

## Strategic Component Allocation

### Design Philosophy

**CRITICAL**: System-level components (WiFi, Mesh, Telemetry) are attached to the **torso subsystem** because:

1. **Head has high real-time processing demands** (displays, audio)
2. **Torso has processing headroom** for system coordination
3. **Separation of concerns** - Head focuses on expression, Torso manages system

## Processing Load Distribution

### Torso (ESP32-S3 Master Coordinator)

```
┌─────────────────────────────────────────────────────────────────┐
│                TORSO SUBSYSTEM - Master Coordinator             │
│                     ESP32-S3 DevKitC-1                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  SYSTEM-LEVEL COMPONENTS (Attached Here):                      │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ • esp_now_mesh (hitCount: 10) - Mesh master             │  │
│  │ • wifi_station (hitCount: 50) - WiFi management         │  │
│  │ • telemetry_hub (hitCount: 75) - MQTT publishing        │  │
│  │ • network_monitor (hitCount: 50) - Connectivity         │  │
│  │ • bluetooth_central (hitCount: 75) - BLE coordination   │  │
│  │ • system_core (hitCount: 100) - Core services           │  │
│  │ • watchdog (hitCount: 500) - Health monitoring          │  │
│  │ • power_monitor (hitCount: 200) - Voltage/current       │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  HARDWARE COMPONENTS (Positioned):                              │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ • spine_flexion_servo (hitCount: 20)                     │  │
│  │ • spine_extension_servo (hitCount: 20)                   │  │
│  │ • waist_rotation_servo (hitCount: 20)                    │  │
│  │ • torso_status_led (hitCount: 50)                        │  │
│  │ • torso_speaker (hitCount: 15)                           │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  CPU UTILIZATION ESTIMATE:                                      │
│  ├─ System Tasks:    25% (WiFi, Mesh, Telemetry)              │
│  ├─ Servo Control:   10% (3 servos @ 50 Hz)                   │
│  ├─ Coordination:    15% (Behavior planning)                   │
│  └─ TOTAL:          ~50%  ✓ ADEQUATE HEADROOM                 │
│                                                                 │
│  RESPONSIBILITIES:                                              │
│  • Mesh network master (coordinate 5+ slave nodes)            │
│  • WiFi connectivity and internet access                       │
│  • MQTT telemetry aggregation and publishing                   │
│  • System-wide command routing                                 │
│  • High-level behavior planning                                │
│  • Balance and posture control                                 │
│  • Power distribution monitoring                               │
│  • Watchdog and health checks                                  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Head (ESP32-S3 High-Priority Slave)

```
┌─────────────────────────────────────────────────────────────────┐
│            HEAD SUBSYSTEM - High-Priority Slave                 │
│                     ESP32-S3 DevKitC-1                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  NO SYSTEM-LEVEL COMPONENTS (Clean Separation)                  │
│                                                                 │
│  HARDWARE COMPONENTS (Positioned - High Frequency):             │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ • left_eye (hitCount: 5) - GC9A01 @ 60 FPS capable       │  │
│  │ • right_eye (hitCount: 5) - GC9A01 @ 60 FPS capable      │  │
│  │ • mouth (hitCount: 3) - GC9A01 @ 100 FPS capable         │  │
│  │ • nose_sensor (hitCount: 15) - HC-SR04 ultrasonic        │  │
│  │ • speaker (hitCount: 7) - I2S audio output               │  │
│  │ • neck_pan_servo (hitCount: 10)                           │  │
│  │ • neck_tilt_servo (hitCount: 10)                          │  │
│  │ • ear_left_led (hitCount: 20)                             │  │
│  │ • ear_right_led (hitCount: 20)                            │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  CPU UTILIZATION ESTIMATE:                                      │
│  ├─ Display Rendering:  40% (2x GC9A01 @ 60 FPS)              │
│  ├─ Audio Processing:   20% (I2S mixing)                       │
│  ├─ Sensor Polling:     10% (HC-SR04 filtering)                │
│  ├─ Servo Control:       5% (2 neck servos)                    │
│  └─ TOTAL:             ~75%  ⚠️ HIGH BUT ACCEPTABLE            │
│                                                                 │
│  EXCLUDED RESPONSIBILITIES (Handled by Torso):                  │
│  ✗ WiFi management                                             │
│  ✗ Mesh network coordination                                   │
│  ✗ Telemetry aggregation                                       │
│  ✗ System-wide behavior planning                               │
│                                                                 │
│  FOCUSED RESPONSIBILITIES:                                      │
│  • Dual display rendering (eyes + mouth)                       │
│  • Real-time facial expression animation                       │
│  • I2S audio playback and mixing                               │
│  • Proximity sensor data acquisition                            │
│  • Mood-driven visual effects                                  │
│  • Neck articulation control                                   │
│  • Respond to mesh commands from torso                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Arm/Leg Subsystems (ESP32-C3 Slaves)

```
┌─────────────────────────────────────────────────────────────────┐
│        ARM/LEG SUBSYSTEMS - Medium-Priority Slaves              │
│                      ESP32-C3 Mini                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  NO SYSTEM-LEVEL COMPONENTS (Lightweight Slaves)                │
│                                                                 │
│  HARDWARE COMPONENTS (7 DOF Arm Example):                       │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ • shoulder_flexion_servo (hitCount: 10)                   │  │
│  │ • shoulder_abduction_servo (hitCount: 10)                 │  │
│  │ • shoulder_rotation_servo (hitCount: 10)                  │  │
│  │ • elbow_flexion_servo (hitCount: 10)                      │  │
│  │ • forearm_rotation_servo (hitCount: 10)                   │  │
│  │ • wrist_flexion_servo (hitCount: 10)                      │  │
│  │ • wrist_rotation_servo (hitCount: 10)                     │  │
│  │ • arm_status_led (hitCount: 50)                           │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
│  CPU UTILIZATION ESTIMATE:                                      │
│  ├─ Servo Control:   30% (7 servos @ 100 Hz)                  │
│  ├─ Position Updates: 20% (Inverse kinematics)                │
│  ├─ Mesh Response:   10% (Command processing)                  │
│  └─ TOTAL:          ~60%  ✓ ADEQUATE FOR ESP32-C3             │
│                                                                 │
│  FOCUSED RESPONSIBILITIES:                                      │
│  • Multi-DOF servo coordination                                │
│  • Inverse kinematics calculations                             │
│  • Position interpolation and smoothing                        │
│  • Joint limit enforcement                                     │
│  • Respond to mesh commands from torso                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Mesh Network Architecture

```
                    ┌──────────────────────────────────┐
                    │    TORSO ESP32-S3 MASTER         │
                    │                                  │
                    │  System Components:              │
                    │  • WiFi Station → Internet       │
                    │  • ESP-NOW Mesh Master           │
                    │  • MQTT Telemetry Hub            │
                    │  • Bluetooth Central             │
                    │  • System Core Services          │
                    │                                  │
                    │  Mesh Role: COORDINATOR          │
                    │  Priority: SYSTEM                │
                    └──────────────────────────────────┘
                                  │
                    ESP-NOW Mesh Network (Channel 6)
                                  │
        ┌─────────────────────────┼─────────────────────────┐
        │                         │                         │
        ▼                         ▼                         ▼
┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐
│  HEAD ESP32-S3   │    │ ARM_L ESP32-C3   │    │ ARM_R ESP32-C3   │
│                  │    │                  │    │                  │
│  Role: SLAVE     │    │  Role: SLAVE     │    │  Role: SLAVE     │
│  Priority: HIGH  │    │  Priority: MED   │    │  Priority: MED   │
│                  │    │                  │    │                  │
│  Focus:          │    │  Focus:          │    │  Focus:          │
│  • Display       │    │  • 7-DOF Arm     │    │  • 7-DOF Arm     │
│  • Audio         │    │  • IK Solver     │    │  • IK Solver     │
│  • Sensors       │    │  • Servo Ctrl    │    │  • Servo Ctrl    │
│  • Neck Servos   │    │                  │    │                  │
└──────────────────┘    └──────────────────┘    └──────────────────┘

        ┌─────────────────────────┐
        │                         │
        ▼                         ▼
┌──────────────────┐    ┌──────────────────┐
│ LEG_L ESP32-C3   │    │ LEG_R ESP32-C3   │
│                  │    │                  │
│  Role: SLAVE     │    │  Role: SLAVE     │
│  Priority: MED   │    │  Priority: MED   │
│                  │    │                  │
│  Focus:          │    │  Focus:          │
│  • 6-DOF Leg     │    │  • 6-DOF Leg     │
│  • IK Solver     │    │  • IK Solver     │
│  • Balance Ctrl  │    │  • Balance Ctrl  │
└──────────────────┘    └──────────────────┘
```

## Communication Flow Examples

### Example 1: Remote Command via MQTT

```
[Mobile App] → [WiFi] → [MQTT Broker]
                              ↓
                    [Torso: telemetry_hub component]
                              ↓
                    [Command Router: Parse JSON]
                              ↓
                    ┌─────────┴─────────┐
                    ▼                   ▼
            [Head: "smile"]    [Arm_L: "wave"]
                    ↓                   ↓
          [ESP-NOW Mesh]      [ESP-NOW Mesh]
                    ↓                   ↓
        [Head: Execute smile]  [Arm_L: Execute wave]
```

### Example 2: Sensor-Driven Reaction

```
[Head: nose_sensor detects object @ 30cm]
              ↓
    [Head: Local processing]
              ↓
    [ESP-NOW: Alert to Torso]
              ↓
[Torso: Behavior planner decides "curious"]
              ↓
    [ESP-NOW Broadcast to ALL nodes]
              ↓
    ┌─────────┼─────────┐
    ▼         ▼         ▼
[Head:    [Arm_L:    [Arm_R:
 Wide      Reach      Reach
 eyes]     forward]   forward]
```

### Example 3: System Health Monitoring

```
[Torso: watchdog component (every 5 sec)]
                    ↓
        [ESP-NOW: Health check broadcast]
                    ↓
    ┌───────────────┼───────────────┐
    ▼               ▼               ▼
[Head:         [Arm_L:         [Arm_R:
 Respond       Respond         Respond
 OK]           OK]             OK]
    │               │               │
    └───────────────┼───────────────┘
                    ▼
    [Torso: Aggregate health status]
                    ↓
    [Torso: Publish to MQTT]
                    ↓
        [Cloud Dashboard: Display]
```

## Component Attachment Decision Tree

```
┌──────────────────────────────────────────────────────┐
│  "Where should this component be attached?"          │
└──────────────────────────────────────────────────────┘
                        │
                        ▼
            ┌───────────────────────┐
            │ Is it SYSTEM-LEVEL?   │
            │ (WiFi, Mesh, Telemetry)│
            └───────────────────────┘
                │               │
               YES              NO
                │               │
                ▼               ▼
    ┌────────────────────┐  ┌──────────────────────┐
    │ Attach to TORSO    │  │ Is it high-frequency │
    │ subsystem_level_   │  │ real-time?           │
    │ components[]       │  │ (Displays, Audio)    │
    └────────────────────┘  └──────────────────────┘
                                    │               │
                                   YES              NO
                                    │               │
                                    ▼               ▼
                        ┌────────────────────┐  ┌──────────────────┐
                        │ Attach to HEAD     │  │ Attach to        │
                        │ positioned_        │  │ appropriate      │
                        │ components[]       │  │ subsystem        │
                        │ (hitCount: 3-10)   │  │ (hitCount: 10-30)│
                        └────────────────────┘  └──────────────────┘
```

## Performance Optimization Strategy

### Load Balancing Principles

1. **System tasks → Torso** (Master coordinator has headroom)
2. **Real-time rendering → Head** (Dedicated high-frequency processing)
3. **Servo control → Local subsystem** (Minimize mesh latency)
4. **Telemetry aggregation → Torso** (Central collection point)
5. **Behavior planning → Torso** (High-level coordination)

### hitCount Tuning Guidelines

| Component Location | Task Type | hitCount | Rationale |
|-------------------|-----------|----------|-----------|
| Torso System | WiFi Status | 50 | 5-second check sufficient |
| Torso System | Mesh Coordination | 10 | 1-second node management |
| Torso System | Telemetry Publish | 75 | 7.5-second MQTT cycle |
| Head Hardware | Eye Display | 5 | 500ms = 2 FPS (sufficient for expressions) |
| Head Hardware | Mouth Display | 3 | 300ms = 3.3 FPS (smooth speech) |
| Head Hardware | Audio Output | 7 | 700ms audio chunk updates |
| Arm Hardware | Servo Control | 10 | 1-second position updates (smooth motion) |

### CPU Budgeting

```
Total Loop Period: 100ms (10 Hz base rate)

Torso ESP32-S3 (240 MHz dual-core):
├─ Core 0: System tasks (WiFi, Mesh, Telemetry) = 25%
├─ Core 1: Servo control + Coordination = 25%
└─ Total: 50% utilized, 50% headroom ✓

Head ESP32-S3 (240 MHz dual-core):
├─ Core 0: Display rendering (SPI) = 40%
├─ Core 1: Audio processing (I2S) + Sensors = 35%
└─ Total: 75% utilized, 25% headroom (acceptable)

Arm ESP32-C3 (160 MHz single-core):
├─ Core 0: 7x Servo control + IK solver = 60%
└─ Total: 60% utilized, 40% headroom ✓
```

## Summary

**Key Design Decisions**:

1. ✅ **System components attached to torso** - Torso has processing headroom
2. ✅ **Head focuses on real-time rendering** - No system overhead
3. ✅ **Strategic load distribution** - Each subsystem optimized for its role
4. ✅ **Mesh architecture** - Torso master coordinates all slaves
5. ✅ **Component isolation** - Clean separation of concerns

**Benefits**:

- **Head performance maximized** - 75% CPU for displays/audio
- **Torso has expansion capacity** - 50% headroom for future features
- **System reliability** - Watchdog and health monitoring centralized
- **Network efficiency** - Single WiFi/mesh coordinator
- **Scalability** - Easy to add new ESP32-C3 slave nodes

📘 **[Complete Component Hierarchy](HIERARCHICAL-COMPONENT-COMPOSITION-SPEC.md)**
