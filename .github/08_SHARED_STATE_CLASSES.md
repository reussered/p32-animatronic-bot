# Topic 8: Shared State Classes

**Purpose**: Reference for SharedMemory state objects used in inter-subsystem communication.

---

## Available State Classes

All auto-included in every component (see Topic 3). Access via `GSM.read<T>()` / `GSM.write(T)`.

---

## 1. Mood

**Purpose**: Emotional state affecting behavior and animations.

```cpp
class Mood
{
public:
    uint8_t happiness;      // 0-100: Joy level
    uint8_t curiosity;      // 0-100: Interest in surroundings
    uint8_t anger;          // 0-100: Aggression level
    uint8_t fear;           // 0-100: Anxiety/caution
    uint8_t excitement;     // 0-100: Energy level
    uint64_t timestamp_us;  // Last update time
};
```

### Usage

```cpp
// Read current mood
Mood mood = GSM.read<Mood>();

// Update based on sensor input
if (proximity_cm < 30)
{
    mood.curiosity = 100;
    mood.excitement = 80;
    GSM.write(mood);
}

// Animation responds to mood
if (mood.happiness > 70)
{
    render_happy_eyes();
}
```

---

## 2. Environment

**Purpose**: Sensor data about surroundings.

```cpp
class Environment
{
public:
    uint32_t proximity_cm;      // Distance to nearest object
    uint16_t light_level;       // 0-1023: Ambient brightness
    uint16_t sound_db;          // Noise level
    int16_t temperature_c;      // Celsius * 10 (e.g., 235 = 23.5°C)
    uint8_t battery_percent;    // 0-100: Power remaining
    uint64_t timestamp_us;      // Last sensor update
};
```

### Usage

```cpp
// Sensor component writes
void sensor_act(void)
{
    Environment env = GSM.read<Environment>();
    env.proximity_cm = read_ultrasonic();
    env.light_level = read_light_sensor();
    GSM.write(env);
}

// Behavior component reads
void behavior_act(void)
{
    Environment env = GSM.read<Environment>();
    if (env.proximity_cm < 50 && env.light_level > 500)
    {
        // Someone approaching in bright light
        trigger_greeting();
    }
}
```

---

## 3. Personality

**Purpose**: Long-term character traits (rarely changes).

```cpp
class Personality
{
public:
    uint8_t playfulness;    // 0-100: Tendency toward playful behavior
    uint8_t aggression;     // 0-100: Hostile vs friendly baseline
    uint8_t curiosity;      // 0-100: Exploration tendency
    uint8_t shyness;        // 0-100: Avoidance vs engagement
    uint8_t energy;         // 0-100: Activity level baseline
};
```

### Usage

```cpp
// Set at init (rarely changed)
esp_err_t creature_init(void)
{
    Personality p = GSM.read<Personality>();
    p.playfulness = 85;   // Very playful goblin
    p.aggression = 20;    // Mostly friendly
    p.curiosity = 90;     // Highly curious
    GSM.write(p);
    return ESP_OK;
}

// Behavior uses as baseline
void behavior_act(void)
{
    Personality p = GSM.read<Personality>();
    Mood mood = GSM.read<Mood>();
    
    // Combine personality + mood
    uint8_t effective_curiosity = (p.curiosity + mood.curiosity) / 2;
    
    if (effective_curiosity > 70)
    {
        look_around();
    }
}
```

---

## 4. BehaviorControl

**Purpose**: High-level commands and state machine coordination.

```cpp
class BehaviorControl
{
public:
    uint8_t current_action;     // Action ID (enum or state)
    uint8_t priority;           // 0-255: Higher = more important
    uint8_t torso_status;       // Status code from torso
    uint8_t head_status;        // Status code from head
    bool override_active;       // Manual control active
    uint64_t action_start_us;   // When current action began
};
```

### Usage

```cpp
// Torso coordinates behavior
void torso_coordinator_act(void)
{
    BehaviorControl bc = GSM.read<BehaviorControl>();
    
    if (bc.current_action == ACTION_IDLE && new_stimulus())
    {
        bc.current_action = ACTION_INVESTIGATE;
        bc.priority = 75;
        bc.action_start_us = esp_timer_get_time();
        GSM.write(bc);
    }
}

// Head responds to coordinated action
void head_executor_act(void)
{
    BehaviorControl bc = GSM.read<BehaviorControl>();
    
    switch (bc.current_action)
    {
        case ACTION_INVESTIGATE:
            tilt_head_forward();
            widen_eyes();
            break;
        case ACTION_IDLE:
            neutral_pose();
            break;
    }
}
```

---

## 5. SysTest

**Purpose**: Diagnostics and testing modes.

```cpp
class SysTest
{
public:
    bool test_mode;             // Enable test patterns
    uint8_t test_component_id;  // Which component to test
    uint32_t test_value;        // Test parameter
    uint8_t error_code;         // Last error (0 = OK)
    uint32_t loop_count;        // Execution counter
    uint64_t boot_time_us;      // When system started
};
```

### Usage

```cpp
// Enable test mode
void enter_test_mode(void)
{
    SysTest st = GSM.read<SysTest>();
    st.test_mode = true;
    st.test_component_id = COMPONENT_DISPLAY;
    st.test_value = 0xFF0000;  // Red test pattern
    GSM.write(st);
}

// Component responds to test
void display_act(void)
{
    SysTest st = GSM.read<SysTest>();
    
    if (st.test_mode && st.test_component_id == COMPONENT_DISPLAY)
    {
        fill_display(st.test_value);  // Show test color
        return;  // Skip normal rendering
    }
    
    // Normal operation
    render_frame();
}
```

---

## 6. EmergencyCoordination

**Purpose**: Safety and fault handling across subsystems.

```cpp
class EmergencyCoordination
{
public:
    bool emergency_stop;        // Halt all motion
    uint8_t fault_code;         // Error identifier
    uint8_t fault_subsystem;    // Which chip has problem
    bool recovery_mode;         // Attempting recovery
    uint64_t fault_time_us;     // When fault occurred
};
```

### Usage

```cpp
// Detect fault and signal emergency
void motor_controller_act(void)
{
    if (motor_overcurrent_detected())
    {
        EmergencyCoordination ec = GSM.read<EmergencyCoordination>();
        ec.emergency_stop = true;
        ec.fault_code = FAULT_MOTOR_OVERCURRENT;
        ec.fault_subsystem = SUBSYSTEM_TORSO;
        ec.fault_time_us = esp_timer_get_time();
        GSM.write(ec);
        
        ESP_LOGE(TAG, "EMERGENCY: Motor overcurrent!");
    }
}

// All motion components check emergency flag
void servo_act(void)
{
    EmergencyCoordination ec = GSM.read<EmergencyCoordination>();
    
    if (ec.emergency_stop)
    {
        disable_servo();
        return;  // Don't move
    }
    
    // Normal servo control
    set_servo_angle(target_angle);
}
```

---

## 7. Additional State Classes

### BalanceCompensation
**Purpose**: Multi-legged creature balance adjustments.

**Key fields**: `pitch`, `roll`, `yaw`, `center_of_mass_x`, `center_of_mass_y`

### CollisionAvoidance
**Purpose**: Obstacle detection and path planning.

**Key fields**: `obstacle_detected`, `obstacle_distance_cm`, `safe_direction`

### SensorFusion
**Purpose**: Combined sensor data (IMU + proximity + vision).

**Key fields**: `accel_x/y/z`, `gyro_x/y/z`, `heading_deg`, `tilt_deg`

### ManipulationControl
**Purpose**: Gripper/claw coordination.

**Key fields**: `gripper_open_percent`, `grip_force`, `target_object_id`

### FrameProcessor
**Purpose**: Camera/vision processing results.

**Key fields**: `detected_objects`, `face_detected`, `motion_detected`

### MicrophoneData
**Purpose**: Audio input and voice detection.

**Key fields**: `rms_amplitude`, `frequency_peak_hz`, `voice_detected`

---

## Usage Patterns

### Pattern: Sensor → Behavior → Actuator

```cpp
// Sensor writes Environment
void sensor_act(void)
{
    Environment env = GSM.read<Environment>();
    env.proximity_cm = read_sensor();
    GSM.write(env);
}

// Behavior reads Environment, writes Mood
void behavior_act(void)
{
    Environment env = GSM.read<Environment>();
    Mood mood = GSM.read<Mood>();
    
    if (env.proximity_cm < 30)
    {
        mood.curiosity = 100;
        GSM.write(mood);
    }
}

// Actuator reads Mood
void display_act(void)
{
    Mood mood = GSM.read<Mood>();
    if (mood.curiosity > 70)
    {
        render_curious_eyes();
    }
}
```

### Pattern: Coordinated Multi-Subsystem Action

```cpp
// Torso: Initiate action
void torso_coordinator_act(void)
{
    BehaviorControl bc = GSM.read<BehaviorControl>();
    bc.current_action = ACTION_WAVE;
    bc.priority = 80;
    GSM.write(bc);
}

// Head: Respond to action (different ESP32)
void head_executor_act(void)
{
    BehaviorControl bc = GSM.read<BehaviorControl>();
    if (bc.current_action == ACTION_WAVE)
    {
        look_at_hand();
    }
}

// Arm: Respond to action (different ESP32)
void arm_executor_act(void)
{
    BehaviorControl bc = GSM.read<BehaviorControl>();
    if (bc.current_action == ACTION_WAVE)
    {
        wave_hand();
    }
}
```

---

## Best Practices

| Do | Don't |
|----|-------|
| Read when needed, cache locally | Read multiple times |
| Write only on meaningful change | Write every loop (floods network) |
| Use timestamp fields for staleness checks | Assume instant sync across ESP32s |
| Initialize state in init() | Leave fields uninitialized |
| Check emergency flags before motion | Ignore EmergencyCoordination |

---

**Token count**: ~1,500
