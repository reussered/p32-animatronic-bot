# Topic 5: Communication Patterns

**Purpose**: Define how components exchange data without violating isolation.

---

## Communication Types

| Type | Scope | Mechanism | Latency | Use Case |
|------|-------|-----------|---------|----------|
| **Intra-Subsystem** | Same `.cpp` file | `static` variables | 0 | Processing pipeline |
| **Inter-Subsystem** | Different ESP32s | Global Shared Memory | ~5-20ms | Cross-chip sync |
| **Prohibited** | Any | Direct function calls | N/A | Breaks isolation |

---

## 1. Intra-Subsystem Communication

**Rule**: Components in same subsystem share file scope via `static` variables.

### Pattern: Processing Pipeline

```cpp
// At top of goblin_head_component_functions.cpp (file scope)
static uint16_t* display_buffer = NULL;
static uint32_t buffer_size = 0;
static bool buffer_ready = false;

// Component A: Allocate and render
esp_err_t goblin_eye_renderer_init(void)
{
    buffer_size = 240 * 240 * 2;
    display_buffer = (uint16_t*)malloc(buffer_size);
    return (display_buffer != NULL) ? ESP_OK : ESP_ERR_NO_MEM;
}

void goblin_eye_renderer_act(void)
{
    // Render animation to display_buffer
    render_eye_frame(display_buffer);
    buffer_ready = true;
}

// Component B: Display buffer
void goblin_display_driver_act(void)
{
    if (buffer_ready && display_buffer != NULL)
    {
        send_to_display(display_buffer, buffer_size);
        buffer_ready = false;
    }
}
```

**Key Points**:
- All components see the same `display_buffer` pointer
- No `#include` needed — aggregated into same `.cpp`
- Execution order controlled by `timing.hitCount` in JSON

### Pattern: Shared State

```cpp
// File-scoped state shared by multiple components
static int32_t head_tilt_angle = 0;  // Degrees
static uint32_t blink_interval_ms = 3000;

// Servo component updates angle
void goblin_servo_act(void)
{
    set_servo_angle(head_tilt_angle);
}

// Behavior component changes angle
void goblin_behavior_act(void)
{
    Mood mood = GSM.read<Mood>();
    if (mood.curiosity > 50)
    {
        head_tilt_angle = 15;  // Tilt head when curious
    }
}
```

---

## 2. Inter-Subsystem Communication

**Rule**: Different ESP32 chips communicate **only** via `SharedMemory` (aliased as `GSM`).

### SharedMemory API

`SharedMemory` is a global singleton providing type-safe read/write:

```cpp
extern SharedMemory GSM;  // Auto-included in all components

// Read shared state
Mood mood = GSM.read<Mood>();

// Write shared state
mood.happiness = 75;
GSM.write(mood);
```

**Key behavior**:
- Reads return the current cached value (fast, no network)
- Writes update local cache **and** broadcast to all ESP32s
- Write frequency should be minimized (only on state change)

### Example: Head Reacts to Torso Sensors

```cpp
// goblin_torso subsystem (different ESP32)
void goblin_torso_sensor_act(void)
{
    Environment env = GSM.read<Environment>();
    env.proximity_cm = read_ultrasonic_sensor();
    
    // Only write when value changes significantly
    static uint32_t last_proximity = 0;
    if (abs((int)env.proximity_cm - (int)last_proximity) > 5)
    {
        GSM.write(env);  // Broadcasts to all subsystems
        last_proximity = env.proximity_cm;
    }
}

// goblin_head subsystem (different ESP32)
void goblin_head_behavior_act(void)
{
    // Read environment written by torso
    Environment env = GSM.read<Environment>();
    
    if (env.proximity_cm < 30)  // Someone close
    {
        Mood mood = GSM.read<Mood>();
        mood.curiosity = 100;
        GSM.write(mood);  // Update for all subsystems
        
        // Trigger local behavior
        trigger_curious_eyes();
    }
}
```

### Write Minimization Pattern

```cpp
// ✗ DON'T (writes every loop)
void act(void)
{
    Mood mood = GSM.read<Mood>();
    mood.timestamp = esp_timer_get_time();  // Changes every call
    GSM.write(mood);  // Broadcasts unnecessarily
}

// ✓ DO (write only on meaningful change)
void act(void)
{
    static uint8_t previous_happiness = 0;
    
    Mood mood = GSM.read<Mood>();
    uint8_t new_happiness = calculate_happiness();
    
    if (new_happiness != previous_happiness)
    {
        mood.happiness = new_happiness;
        GSM.write(mood);  // Broadcast only when changed
        previous_happiness = new_happiness;
    }
}
```

### Available Shared State Classes

Auto-included in all components (see Topic 3):

| Class | Purpose | Key Fields |
|-------|---------|------------|
| `Mood` | Emotional state | `happiness`, `curiosity`, `anger` |
| `Personality` | Character traits | `playfulness`, `aggression` |
| `Environment` | Sensor data | `proximity_cm`, `light_level`, `sound_db` |
| `BehaviorControl` | High-level commands | `current_action`, `priority` |
| `SysTest` | Diagnostics | `test_mode`, `component_status` |
| `EmergencyCoordination` | Safety | `emergency_stop`, `fault_code` |

---

## 3. Prohibited Patterns

### ✗ Direct Function Calls Between Components

```cpp
// ✗ DON'T (in goblin_eye.src)
void goblin_eye_act(void)
{
    goblin_servo_set_angle(15);  // Direct call - FORBIDDEN
}

// Why: Components in same subsystem are in same .cpp, but this
// creates hidden dependencies and breaks JSON-driven composition
```

**Fix**: Use file-scoped `static` variable:

```cpp
// ✓ DO
static int32_t desired_servo_angle = 0;  // File scope

void goblin_eye_act(void)
{
    desired_servo_angle = 15;  // Write to shared state
}

void goblin_servo_act(void)
{
    set_servo_angle(desired_servo_angle);  // Read from shared state
}
```

### ✗ Cross-Subsystem Function Calls

```cpp
// ✗ DON'T (goblin_head calling goblin_torso function)
void goblin_head_act(void)
{
    goblin_torso_get_status();  // Different ESP32 - IMPOSSIBLE
}

// Why: Different compilation units on different chips
```

**Fix**: Use GSM:

```cpp
// ✓ DO
void goblin_head_act(void)
{
    BehaviorControl control = GSM.read<BehaviorControl>();
    if (control.torso_status == TORSO_IDLE)
    {
        // React to torso state
    }
}
```

### ✗ Global Variables Without `static`

```cpp
// ✗ DON'T (in .src file)
uint16_t* display_buffer = NULL;  // Missing static - linker error

// Why: Multiple subsystems would have conflicting symbols
```

**Fix**: Always use `static` for file scope:

```cpp
// ✓ DO
static uint16_t* display_buffer = NULL;
```

---

## 4. Timing and Synchronization

### Execution Order Within Subsystem

Controlled by `timing.hitCount` in component JSON:

```json
{
  "component": "goblin_eye_renderer",
  "timing": { "hitCount": 1 }  // Runs every loop
},
{
  "component": "goblin_display_driver",
  "timing": { "hitCount": 2 }  // Runs every 2nd loop
}
```

Components execute in JSON order. Lower `hitCount` = more frequent.

### Cross-Subsystem Timing

Reads may return slightly stale data (5-20ms lag). Design for eventual consistency:

```cpp
// ✓ DO (tolerate lag)
void goblin_head_act(void)
{
    Mood mood = GSM.read<Mood>();
    if (mood.happiness > 50)
    {
        // Value may be 5-20ms old - acceptable for behavior
    }
}

// ✗ DON'T (assume instant sync)
void goblin_head_act(void)
{
    Mood mood = GSM.read<Mood>();
    mood.timestamp = esp_timer_get_time();
    GSM.write(mood);
    
    Mood verify = GSM.read<Mood>();
    assert(verify.timestamp == mood.timestamp);  // FAILS - async
}
```

---

## 5. Data Flow Examples

### Example: Eye Tracking Head Movement

**Components**: `goblin_servo` (head tilt), `goblin_eye_renderer` (eye animation)

```cpp
// File-scoped shared state
static int32_t head_tilt_angle = 0;
static bool eyes_follow_tilt = true;

// Servo component writes angle
void goblin_servo_act(void)
{
    head_tilt_angle = read_servo_position();
}

// Eye renderer reads angle
void goblin_eye_renderer_act(void)
{
    if (eyes_follow_tilt)
    {
        int32_t eye_offset = head_tilt_angle / 2;  // Eyes lag behind
        render_eyes_with_offset(eye_offset);
    }
}
```

### Example: Coordinated Behavior Across ESP32s

**Subsystems**: `goblin_head` (ESP32-S3), `goblin_torso` (ESP32-S3)

```cpp
// goblin_torso: Update mood based on sensors
void goblin_torso_sensor_act(void)
{
    Environment env = GSM.read<Environment>();
    env.proximity_cm = read_ultrasonic();
    env.light_level = read_light_sensor();
    GSM.write(env);
    
    Mood mood = GSM.read<Mood>();
    if (env.proximity_cm < 30)  // Someone close
    {
        mood.curiosity = 100;
        GSM.write(mood);
    }
}

// goblin_head: React to mood
void goblin_head_behavior_act(void)
{
    Mood mood = GSM.read<Mood>();
    if (mood.curiosity > 80)
    {
        // Wide eyes, tilt head forward
        trigger_curious_animation();
    }
}
```

---

## 6. Common Mistakes

| Mistake | Symptom | Fix |
|---------|---------|-----|
| Missing `static` on file-scope var | Linker error: "multiple definition" | Add `static` |
| Calling component function directly | Build succeeds, logic breaks | Use shared variable |
| Writing GSM every loop | ESP-NOW floods, lag | Write only on change |
| Assuming instant GSM sync | Race conditions | Design for 5-20ms latency |
| Using `extern` for inter-chip | Linker error or undefined behavior | Use GSM |

---

## Decision Tree

```
Need to share data?
├─ Same subsystem (same ESP32)?
│  └─ Use file-scoped `static` variable
│
└─ Different subsystems (different ESP32s)?
   └─ Use GSM.read<T>() / GSM.write(T)
   
Never:
- Direct function calls between components
- `extern` declarations
- Cross-subsystem includes
```

---

**Token count**: ~1,500
