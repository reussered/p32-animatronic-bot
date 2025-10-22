# P32 Network Architecture - Strategic Wireless Distribution

## NETWORK TOPOLOGY STRATEGY
**Selective wireless deployment to balance performance monitoring with network efficiency**

### Tiered Network Access

#### Tier 1: Primary Network Controllers (Always Wireless)
```
HEAD CONTROLLER (ESP32-S3) - Master Network Node
├── Full WiFi capability with network_monitor component
├── Receives all server commands and choreography instructions  
├── Distributes commands to subsidiary controllers via internal bus
└── Primary timing packet transmission to server

SPINAL BASE CONTROLLER (ESP32-S3) - Choreography Execution Node  
├── Full WiFi capability for sophisticated server-driven actions
├── Receives complex choreography: "dance the boogie woogie"
├── Implements server-calculated movement sequences
└── Secondary timing packet transmission for performance verification
```

#### Tier 2: Selective Wireless (Conditional Network Access)
```
MOBILITY BASE CONTROLLER (ESP32-S3) - Navigation Node
├── WiFi enabled for navigation updates and obstacle avoidance
├── Receives server-calculated path planning
├── Reports position telemetry for coordination
└── Tertiary timing packets for mobility performance

ADVANCED LIMB CONTROLLERS (ESP32-C3) - Specialized Network Access
├── WiFi enabled only for limbs requiring server-side computation
├── Example: Hand controllers for complex manipulation tasks
├── Server provides real-time inverse kinematics calculations
└── Timing packets only during active server-assisted operations
```

#### Tier 3: Bus-Only Controllers (No Direct Wireless)
```
SIMPLE LIMB CONTROLLERS (ESP32-C3) - Internal Bus Only
├── No wireless - receives commands via internal communication bus
├── Examples: Individual finger servos, basic joint actuators
├── Reduced power consumption and network overhead
└── Performance monitoring via primary controllers

SENSOR ARRAY CONTROLLERS (ESP32-C3) - Sensor Fusion Nodes
├── No wireless - reports sensor data via internal bus to head controller
├── Examples: Proximity sensors, IMU arrays, pressure sensors
├── Real-time sensor fusion processed locally
└── Critical sensor alerts forwarded through head controller
```

### JSON Component Configuration Strategy

#### Head Controller (goblin_head.json) - Full Network
```json
{
  "chip_role": "HEAD_CONTROLLER",
  "network_tier": "PRIMARY_NETWORK_NODE",
  "positioned_components": [
    "config/components/positioned/left_eye_display.json",
    "config/components/positioned/right_eye_display.json",
    "config/components/positioned/network_command_processor.json",
    "config/components/positioned/internal_bus_coordinator.json"
  ]
}
```

#### Spinal Base Controller (goblin_spinal_base.json) - Choreography Network
```json
{
  "chip_role": "SPINAL_BASE_CONTROLLER", 
  "network_tier": "CHOREOGRAPHY_EXECUTION_NODE",
  "positioned_components": [
    "config/components/positioned/choreography_processor.json",
    "config/components/positioned/movement_coordination.json",
    "config/components/positioned/posture_control.json",
    "config/components/positioned/server_command_interpreter.json"
  ]
}
```

#### Simple Hand Controller (goblin_left_hand.json) - Bus Only
```json
{
  "chip_role": "LEFT_HAND_CONTROLLER",
  "network_tier": "BUS_ONLY_NODE", 
  "positioned_components": [
    "config/components/positioned/thumb_servo.json",
    "config/components/positioned/finger_servos.json",
    "config/components/positioned/internal_bus_client.json"
    // NOTE: No network_monitor component = no wireless
  ]
}
```

### System Component Variations by Network Tier

#### Full Network System Components
```c
// Head Controller & Spinal Base Controller
system_components = [
    {"name": "heartbeat", "hitCount": 1, "description": "Chip alive indicator"},
    {"name": "network_monitor", "hitCount": 1, "description": "Server communication & timing"}
]
```

#### Bus-Only System Components  
```c
// Simple limb controllers, sensor arrays
system_components = [
    {"name": "heartbeat", "hitCount": 1, "description": "Chip alive indicator"},
    {"name": "bus_client", "hitCount": 10, "description": "Internal bus communication"}
]
```

### Server-Side Choreography Example

#### Server Command: "dance_boogie_woogie"
```json
{
  "command": "dance_boogie_woogie",
  "target_chip": "SPINAL_BASE_CONTROLLER",
  "duration_seconds": 30,
  "movement_sequence": [
    {"timestamp": 0.0, "action": "lean_left", "intensity": 0.7},
    {"timestamp": 0.5, "action": "lean_right", "intensity": 0.8},
    {"timestamp": 1.0, "action": "rotate_torso", "angle": 15},
    {"timestamp": 1.5, "action": "coordinate_arms", "pattern": "wave_motion"}
  ],
  "synchronization": {
    "head_controller": "follow_spinal_lead",
    "mobility_base": "maintain_balance"
  }
}
```

#### Spinal Base Controller Response
```c
void choreography_processor_act(void) {
    #ifdef SIMPLE_TEST
    printf("ACT[%lu]: choreography_processor - executing server sequence\n", g_loopCount);
    return;
    #endif
    
    // Receive complex choreography from server
    if (server_command_available()) {
        choreography_command_t cmd = receive_server_command();
        
        // Execute server-calculated movement sequence
        execute_movement_sequence(&cmd);
        
        // Coordinate with other controllers via internal bus
        broadcast_coordination_commands(&cmd.synchronization);
    }
}
```

### Network Performance Benefits

#### Reduced Network Traffic
- **5 chips total**: Only 2-3 with wireless (60% reduction in network packets)
- **Complex humanoid**: Only 4-6 wireless out of 15+ total chips (70% reduction)
- **Choreography on-demand**: Server communication only when executing complex behaviors

#### Enhanced Server Capabilities
- **Real-time choreography**: Server calculates complex dance sequences
- **Inverse kinematics**: Server computes joint angles for precise manipulation
- **Behavioral coordination**: Server orchestrates multi-limb synchronized actions
- **Learning integration**: Server applies ML models for adaptive behaviors

### Future Expansion Strategy

#### Smart Wireless Assignment
- **Simple components**: Always bus-only (finger servos, basic sensors)
- **Coordination components**: Conditional wireless (hand controllers for dexterity)
- **Master controllers**: Always wireless (head, spinal base, mobility)
- **Specialized systems**: Wireless when server computation required (vision processing, speech recognition)

This approach gives us **maximum flexibility** - simple operations stay local and efficient, while complex behaviors leverage server-side intelligence without overwhelming the network!