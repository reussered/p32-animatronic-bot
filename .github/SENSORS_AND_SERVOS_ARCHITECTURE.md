# Sensors & Servos Architecture Plan

## Current State

### Existing Servo Components
- **Micro servos**: `servo_sg90_micro.src` (stub)
- **Standard servos**: Multiple motor-specific implementations
- **Positions**: Shoulder, elbow, wrist, hip, ankle, finger, spine, waist, tail, etc.
- **Status**: Mostly stubs (TODO comments, no real implementation)

### Existing Sensor Components  
- **Creature-specific**: `goblin_scent_sensor`, `goblin_shadow_sensors`, `bear_ice_sensors`, `cat_nose_sensor`
- **Types**: Scent/chemical sensors, IR/shadow sensors, temperature sensors, etc.
- **Status**: Mostly stubs (TODO comments, no real implementation)

### Current Architecture Issues
- No unified servo control interface (each servo is independent stub)
- No unified sensor reading interface
- No data sharing between sensors and servo actuators
- No feedback loops (servos don't know sensor state)
- No PWM/timing coordination
- SharedMemory not used for sensor readings or servo commands

---

## Proposed Architecture

### Layer 1: Hardware Abstraction (drivers/)

#### Generic PWM Servo Driver
**File**: `config/components/drivers/generic_pwm_servo_driver.src`

Handles:
- PWM signal generation (50Hz for servos = 20ms period)
- Duty cycle calculation (angle 0° = 1ms, 90° = 1.5ms, 180° = 2ms)
- Pin management (dynamic assignment from pin configuration)
- Frequency tuning per servo type

```cpp
esp_err_t generic_pwm_servo_init(void);
void generic_pwm_servo_act(void);

// Core function
esp_err_t servo_set_angle(uint8_t servo_id, uint16_t angle_degrees);
```

#### Generic ADC/I2C Sensor Driver
**File**: `config/components/drivers/generic_sensor_reader_driver.src`

Handles:
- ADC channel configuration (12-bit resolution, 0-3.3V)
- I2C polling (for digital sensors)
- Calibration/offset storage
- Value smoothing/filtering

```cpp
esp_err_t generic_sensor_reader_init(void);
void generic_sensor_reader_act(void);

// Core function
uint16_t sensor_read_raw(uint8_t sensor_id);
```

### Layer 2: Shared Data Structures

#### Servo State Class
**File**: `shared/ServoState.hpp`

```cpp
class ServoState {
public:
    struct ServoChannel {
        uint8_t id;              // 0-255
        uint16_t target_angle;   // 0-180 degrees (or wider range)
        uint16_t current_angle;  // Read back position
        uint8_t speed;           // 1-255 (speed units, not standard)
        bool enabled;            // Enable/disable flag
    };
    
    static const uint8_t MAX_SERVOS = 32;
    ServoChannel servos[MAX_SERVOS];
    uint8_t active_count;
    
    // Methods
    esp_err_t set_servo(uint8_t id, uint16_t angle, uint8_t speed);
    uint16_t get_servo_angle(uint8_t id) const;
    void clear(void);
};
```

#### Sensor Reading Class
**File**: `shared/SensorReadings.hpp`

```cpp
class SensorReadings {
public:
    struct SensorValue {
        uint8_t id;              // 0-255
        uint16_t raw_value;      // 0-4095 (12-bit ADC) or 0-1023 (scaled)
        int16_t calibrated_value; // After offset/gain applied
        uint32_t timestamp_ms;   // Last read time
        bool valid;              // Data freshness flag
    };
    
    static const uint8_t MAX_SENSORS = 64;
    SensorValue sensors[MAX_SENSORS];
    uint8_t active_count;
    
    // Methods
    esp_err_t register_sensor(uint8_t id, const char* type_name);
    void update_reading(uint8_t id, uint16_t raw_value);
    int16_t get_calibrated(uint8_t id) const;
};
```

### Layer 3: SharedMemory Registration

Register both as GSM types:

```cpp
// In SharedMemory.hpp or initialization
static_assert(Mood::typeID == 2, "Type ID 2 reserved for Mood");
static_assert(Personality::typeID == 4, "Type ID 4 reserved for Personality");

// New type IDs (need to decide actual IDs)
static_assert(ServoState::typeID == 5, "Type ID 5 reserved for ServoState");
static_assert(SensorReadings::typeID == 6, "Type ID 6 reserved for SensorReadings");
```

Usage:
```cpp
// In servo component (torso):
void my_servo_act(void) {
    ServoState* state = GSM.read<ServoState>();
    if (state && state->servos[0].enabled) {
        servo_set_angle(state->servos[0].id, state->servos[0].target_angle);
    }
}

// In sensor component (torso or local):
void my_sensor_act(void) {
    uint16_t raw = sensor_read_raw(SENSOR_ID_TEMPERATURE);
    SensorReadings* readings = GSM.read<SensorReadings>();
    readings->update_reading(SENSOR_ID_TEMPERATURE, raw);
    GSM.write<SensorReadings>(*readings);
}
```

### Layer 4: Creature-Specific Components

#### Goblin Servo Manager
**File**: `config/bots/bot_families/goblins/torso/goblin_servo_manager.src`

```cpp
// Maps goblin joints to servo IDs
static const uint8_t GOBLIN_SERVO_LEFT_SHOULDER = 0;
static const uint8_t GOBLIN_SERVO_RIGHT_SHOULDER = 1;
static const uint8_t GOBLIN_SERVO_WAIST = 2;
// ... etc

esp_err_t goblin_servo_manager_init(void) {
    // Register which servo IDs are used by this goblin
    // Set initial angles (neutral pose)
    return ESP_OK;
}

void goblin_servo_manager_act(void) {
    // Read mood from GSM
    // Convert mood components to servo movements
    // Example: anger → shoulder raise, tension in waist
    // Write servo commands to ServoState via GSM
}
```

#### Goblin Sensor Aggregator
**File**: `config/bots/bot_families/goblins/head/goblin_sensor_aggregator.src`

```cpp
// Maps goblin sensors to sensor IDs
static const uint8_t GOBLIN_SENSOR_LEFT_IR = 10;
static const uint8_t GOBLIN_SENSOR_RIGHT_IR = 11;
static const uint8_t GOBLIN_SENSOR_TEMPERATURE = 12;
// ... etc

esp_err_t goblin_sensor_aggregator_init(void) {
    // Calibrate sensors (store offsets)
    return ESP_OK;
}

void goblin_sensor_aggregator_act(void) {
    // Read raw sensor values
    // Update SensorReadings in GSM
    // Optionally trigger behaviors based on sensor thresholds
}
```

---

## Implementation Strategy

### Phase 1: Foundation (Low Priority, Can Do Later)
1. Create `ServoState.hpp` and `SensorReadings.hpp` shared classes
2. Register in SharedMemory with type IDs
3. Create generic driver stubs (empty for now)

### Phase 2: Servo Control (Medium Priority)
1. Implement `generic_pwm_servo_driver.src`:
   - Calculate duty cycles from angles
   - Handle PWM pin setup
   - Test with one servo (e.g., shoulder)
2. Create goblin servo manager that reads/writes to SharedMemory
3. Test mood → servo movement feedback loop

### Phase 3: Sensor Reading (Medium Priority)
1. Implement `generic_sensor_reader_driver.src`:
   - ADC initialization and polling
   - Calibration storage
   - Value filtering
2. Create goblin sensor aggregator that reads/writes to SharedMemory
3. Test sensor reading and data flow to head

### Phase 4: Integration (High Priority After Above)
1. Coordinate servo timing (don't update all at once)
2. Create feedback loops (sensors → mood → servos)
3. Add safety limits (servo angle bounds, max speed)
4. Creature-specific behaviors (each family has different servo count/config)

---

## Design Questions to Decide

### Q1: Servo Angle Representation
Options:
- **0-180 degrees** (standard servo range)
- **0-255 units** (easier to pack, loses precision)
- **Signed ±90 degrees** (allows bidirectional motion specification)

**Recommendation**: Use 0-180 degrees (human-readable), convert to duty cycle at driver level

### Q2: Servo Update Frequency
Options:
- **Update every frame** (if 50Hz, servos get 50 updates/sec)
- **Update every N frames** (reduce I2C/SPI traffic)
- **Only update if changed** (reduces unnecessary PWM adjustments)

**Recommendation**: Only update if changed (servo already holds position, no need for constant updates)

### Q3: Sensor Filtering
Options:
- **No filtering** (raw values)
- **Moving average** (last N readings)
- **Exponential moving average** (weighted recent > old)
- **Kalman filter** (if high precision needed)

**Recommendation**: Start with moving average (simple, effective)

### Q4: Feedback Loop: Who Drives Servos?
Options:
- **Mood-based** (mood intensity → servo angle)
- **Behavior-based** (behavior rules → servo angle)
- **Remote command** (host sends servo angles)
- **Sensor-based** (sensor reading → servo reaction)

**Recommendation**: All of the above eventually, start with mood-based (already have Mood system)

### Q5: Multi-Creature Support
Options:
- **One struct per creature type** (Goblin vs Bear vs Cat servos defined separately)
- **Generic mapping** (all creatures use same ServoState, different ID assignments)

**Recommendation**: Generic mapping (ServoState used by all creatures, each registers its own IDs)

---

## Data Flow Diagram

```
SENSORS                    PROCESSING                      SERVOS
═══════                    ══════════                      ══════

Physical           Generic Sensor         SensorReadings      Mood/
Sensors       →    Reader Driver    →     (SharedMemory) →   Behavior
(ADC, I2C)        (.src stub)              (Type ID: 6)       Logic
                   
                                          ↓
                                          
                                    Creature Sensor
                                    Aggregator
                                    (goblin_sensor_aggregator.src)
                                    
                                          ↓
                                          
Physical           Generic PWM           ServoState     ←    Creature
Servos       ←     Servo Driver    ←     (SharedMemory)       Servo
(GPIO PWM)        (.src stub)           (Type ID: 5)         Manager
                   
                                        ↑
                                        
                                    Creature Servo
                                    Manager
                                    (goblin_servo_manager.src)
```

---

## Key Files to Create/Update

### New Files (Must Create)
- `shared/ServoState.hpp`
- `shared/SensorReadings.hpp`
- `config/components/drivers/generic_pwm_servo_driver.src`
- `config/components/drivers/generic_sensor_reader_driver.src`
- `config/bots/bot_families/goblins/torso/goblin_servo_manager.src`
- `config/bots/bot_families/goblins/head/goblin_sensor_aggregator.src`

### Files to Update
- `include/core/memory/SharedMemory.hpp` (register new type IDs)
- Creature-specific JSON configs (register servo/sensor IDs)

### Files to Replace (Current Stubs)
- `config/components/hardware/servo_sg90_micro.src` (actual PWM logic)
- Individual sensor .src files (actual ADC/I2C reading)

---

## Success Criteria

✅ **Phase 1 Complete When**:
- ServoState and SensorReadings classes compile
- Both registered in SharedMemory
- Generic drivers are callable (even if empty)

✅ **Phase 2 Complete When**:
- One servo successfully accepts angle commands via SharedMemory
- Servo moves in response to mood changes
- Angle read-back works

✅ **Phase 3 Complete When**:
- One sensor successfully reads values
- Values appear in SensorReadings
- Sensor aggregator updates SharedMemory

✅ **Phase 4 Complete When**:
- Full loop: Sensor → Mood → Servo works
- Multiple servos coordinate without conflicts
- Creature-specific behaviors work

---

## Next Steps

### Immediate (Choose One)
1. **Option A**: Start with servo architecture (more predictable, fewer variables)
2. **Option B**: Start with sensor architecture (foundation for feedback loops)
3. **Option C**: Do both in parallel (if resources allow)

### Recommendation
Start with **Option A (Servos)** because:
- Simpler feedback (angle in, movement out)
- Already have Mood system to drive servo angles
- Can test with visible physical results
- Sensors are useless without something to do with their data

---

## Questions for You

1. **Servo scope**: How many servos do you need to control? (Just goblins or all creatures?)
2. **Sensor scope**: How many sensors? Which types (ADC, I2C, GPIO)?
3. **Feedback priority**: Do you want mood → servo movement first, or sensor → behavior first?
4. **Hardware constraints**: Any PWM pin limitations? I2C/ADC availability on head vs torso?

