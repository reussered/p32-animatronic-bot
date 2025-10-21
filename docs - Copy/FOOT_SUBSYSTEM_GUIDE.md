# FOOT SUBSYSTEM CONSTRUCTION GUIDE

**P32 Animatronic Bot - Ground Contact and Tactile Interface System**

---

## OVERVIEW

The foot subsystem provides ground contact, balance feedback, and terrain adaptation for the P32 animatronic humanoid. Each foot features 5-toe articulation, pressure-sensitive contact points, and dynamic balance control. The system enables natural walking gaits, terrain adaptation, and expressive foot gestures while maintaining structural integrity under the full robot weight.

### KEY SPECIFICATIONS

- **Contact Points**: 8 pressure sensors per foot (heel, ball, 5 toes)
- **Toe Articulation**: 5DOF per foot (individual toe control)
- **Load Capacity**: 100kg total load (50kg per foot maximum)
- **Controller**: ESP32-C3 with tactile sensor processing
- **Material**: TPU flexible structure with rigid mounting points
- **Response Time**: <5ms pressure detection, 20ms toe actuation
- **Ground Clearance**: 15mm minimum for obstacle navigation

---

## ANATOMICAL ORGANIZATION

### HEEL ASSEMBLY

- **Structure**: Rigid heel cup with impact absorption
- **Sensors**: 2x FSR pressure sensors (medial/lateral)
- **Material**: PLA+ rigid shell with TPU impact padding
- **Load Distribution**: Primary contact point during heel-strike
- **Integration**: Direct connection to ankle assembly

### METATARSAL ARCH

- **Structure**: Flexible arch with embedded pressure sensors
- **Sensors**: 2x FSR sensors at ball of foot (medial/lateral)
- **Material**: TPU flexible print with embedded sensor channels
- **Function**: Weight distribution during stance phase
- **Features**: Natural arch compression for shock absorption

### TOE ASSEMBLY (5DOF)

- **Big Toe (Hallux)**: 2-segment articulation with SG90 servo
- **Four Smaller Toes**: 1-segment articulation with micro servos
- **Sensors**: FSR pressure sensor in each toe tip
- **Material**: TPU flexible joints with rigid servo mounts
- **Range**: 0° to 60° dorsiflexion per toe
- **Function**: Balance adjustment, grip enhancement, expressive gestures

---

## BILL OF MATERIALS

### MECHANICAL COMPONENTS

| Component | Quantity | Specification | Purpose |
|-----------|----------|---------------|---------|
| **TPU Filament** | 1kg spool | Shore 85A hardness | Flexible foot structure |
| **PLA+ Filament** | 500g | High-strength rigid parts | Servo mounts, heel cup |
| **Aluminum Plate** | 2 pieces | 150x80x3mm | Structural foundation |
| **Steel Ball Bearings** | 10 | 3x10x4mm | Toe joint pivots |
| **Threaded Inserts** | 20 | M3 brass inserts | TPU reinforcement |
| **Anti-Slip Pads** | 10 | Rubber 20x20mm | Ground contact surfaces |

### SERVO MOTORS

| Servo Model | Quantity | Torque | Application |
|-------------|----------|--------|-------------|
| **SG90 Micro** | 8 | 1.8kg-cm | Four smaller toes (2 servos each) |
| **MG90S Metal** | 4 | 2.5kg-cm | Big toe articulation (2 segments) |

### ELECTRONICS

| Component | Quantity | Specification | Purpose |
|-----------|----------|---------------|---------|
| **ESP32-C3** | 2 | RISC-V, WiFi, Bluetooth | Per-foot controller |
| **PCA9685** | 2 | 16-channel PWM | Servo expansion |
| **FSR Sensors** | 16 | Force Sensitive Resistor | Pressure detection |
| **ADC MUX** | 2 | CD74HC4067 (16-channel) | Sensor input expansion |
| **Op-Amp Array** | 2 | LM324 quad op-amp | FSR signal conditioning |
| **Buck Converter** | 2 | 5V to 3.3V, 2A | Logic power supply |
| **Capacitors** | 8 | 1000μF, 10V | Servo power smoothing |

### 3D PRINTED PARTS

| Part Name | Quantity | Material | Purpose |
|-----------|----------|----------|---------|
| **Foot Base** | 2 | TPU Shore 85A | Main flexible structure |
| **Heel Cup** | 2 | PLA+ rigid | Impact protection |
| **Toe Segments** | 20 | TPU Shore 85A | Flexible toe joints |
| **Servo Mounts** | 12 | PLA+ rigid | Motor mounting |
| **Sensor Housings** | 16 | TPU Shore 85A | FSR protection |
| **Controller Housing** | 2 | PLA+ rigid | Electronics protection |

### FASTENERS & HARDWARE

| Hardware | Quantity | Size | Application |
|----------|----------|------|-------------|
| **Socket Head Bolts** | 24 | M3x12mm | Servo mounting |
| **Socket Head Bolts** | 16 | M3x8mm | Sensor mounting |
| **Threaded Inserts** | 40 | M3 brass | TPU reinforcement |
| **Lock Washers** | 40 | M3 | Vibration resistance |
| **Cable Glands** | 4 | M12 waterproof | Cable protection |

---

## ELECTRICAL ARCHITECTURE

### POWER DISTRIBUTION

```
Ankle Power Interface (5V from leg)
    ├── Buck Converter → 3.3V Logic Rail (2A capacity)
    │   ├── ESP32-C3 Controller (200mA)
    │   ├── PCA9685 Servo Driver (50mA)
    │   ├── ADC Multiplexer (10mA)
    │   └── Op-Amp Array (20mA)
    └── Direct 5V Servo Rail (3A capacity)
        ├── Big Toe Servos (2x MG90S)
        └── Small Toe Servos (8x SG90)
```

### ESP32-C3 GPIO ASSIGNMENT

```c
// Foot Controller GPIO Configuration
#define FOOT_ESP32_C3_LEFT     // Left foot controller
#define FOOT_ESP32_C3_RIGHT    // Right foot controller

// I2C Bus (Shared)
#define I2C_SDA_PIN     4      // I2C data line  
#define I2C_SCL_PIN     5      // I2C clock line

// PCA9685 Servo Driver (I2C Address 0x41)
// Servo Channels:
// 0-1: Big toe (proximal, distal segments)
// 2-3: Index toe (2 servos for fine control)
// 4-5: Middle toe (2 servos for fine control)
// 6-7: Ring toe (2 servos for fine control)
// 8-9: Little toe (2 servos for fine control)

// ADC Multiplexer Control
#define MUX_S0_PIN      6      // Address bit 0
#define MUX_S1_PIN      7      // Address bit 1
#define MUX_S2_PIN      8      // Address bit 2
#define MUX_S3_PIN      9      // Address bit 3
#define MUX_ENABLE_PIN  10     // Multiplexer enable

// FSR Sensor Input
#define FSR_ANALOG_PIN  1      // ADC input for pressure sensors

// Status and Communication
#define STATUS_LED_PIN  2      // Built-in status LED
#define GROUND_LED_PIN  18     // Ground contact indicator
#define ERROR_LED_PIN   19     // Error condition LED

// Inter-Controller Communication
#define UART_TX_PIN     21     // UART to ankle controller
#define UART_RX_PIN     20     // UART from ankle controller

// Emergency and Safety
#define ESTOP_PIN       0      // Emergency stop input
#define OVERLOAD_PIN    3      // Overload detection

// Spare GPIO for expansion
#define SPARE_GPIO_11   11
#define SPARE_GPIO_12   12
#define SPARE_GPIO_13   13
```

### SENSOR MULTIPLEXING

```c
// FSR sensor channel mapping
typedef enum {
    FSR_HEEL_MEDIAL = 0,      // Inside heel contact
    FSR_HEEL_LATERAL = 1,     // Outside heel contact
    FSR_BALL_MEDIAL = 2,      // Inside ball of foot
    FSR_BALL_LATERAL = 3,     // Outside ball of foot
    FSR_TOE_BIG = 4,          // Big toe tip pressure
    FSR_TOE_INDEX = 5,        // Index toe tip
    FSR_TOE_MIDDLE = 6,       // Middle toe tip
    FSR_TOE_RING = 7,         // Ring toe tip
    FSR_TOE_LITTLE = 8,       // Little toe tip
    FSR_SPARE_9 = 9,          // Reserved for future sensors
    FSR_SPARE_10 = 10,        // Reserved
    FSR_SPARE_11 = 11,        // Reserved
    FSR_SPARE_12 = 12,        // Reserved  
    FSR_SPARE_13 = 13,        // Reserved
    FSR_SPARE_14 = 14,        // Reserved
    FSR_SPARE_15 = 15         // Reserved
} fsr_channel_t;
```

---

## 3D PRINTED COMPONENTS

### FOOT BASE DESIGN

**File**: `assets/shapes/stl/feet/foot_base.stl`

```
Specifications:
- Dimensions: 120x200x30mm (width x length x height)
- Material: TPU Shore 85A (flexible)
- Infill: 30% gyroid pattern for flexibility
- Wall Thickness: 2 perimeters for durability
- Support: Required for overhangs > 45°
- Print Time: 12 hours per foot

Features:
- Anatomically correct foot shape with natural arch
- Integrated sensor channels for FSR placement
- Flexible toe joints with embedded pivots
- Cable routing channels for servo wires
- Mounting interface for ankle assembly
- Anti-slip texture on ground contact surfaces
```

### TOE SEGMENT DESIGN

**File**: `assets/shapes/stl/feet/toe_segment.stl`

```
Specifications:
- Dimensions: 25x40x15mm per segment
- Material: TPU Shore 85A (flexible joints)
- Infill: 50% honeycomb for strength
- Wall Thickness: 3 perimeters
- Support: Supportless design
- Print Time: 2 hours per set (5 toes)

Features:
- Living hinge design for natural flexion
- Embedded ball bearing races
- Servo horn attachment points
- FSR sensor mounting cavity
- Wear-resistant contact surfaces
```

### SERVO MOUNT DESIGN

**File**: `assets/shapes/stl/feet/servo_mount.stl`

```
Specifications:
- Dimensions: Variable per servo size
- Material: PLA+ (rigid mounting)
- Infill: 100% solid for strength
- Wall Thickness: 4 perimeters
- Support: Required for mounting holes
- Print Time: 1 hour per mount

Features:
- Precise servo fit with vibration dampening
- Integrated wire management channels
- Quick-release servo installation
- Mounting points for foot base integration
- Thermal expansion compensation
```

---

## ASSEMBLY PROCEDURES

### PHASE 1: FOOT BASE PREPARATION

**Duration**: 1.5 hours per foot

1. **TPU Base Printing**
   - Print foot base with TPU Shore 85A material
   - Use 30% gyroid infill for optimal flex/strength ratio
   - Print slowly (20mm/s) for best layer adhesion
   - Allow 24 hours for TPU stress relaxation

2. **Sensor Channel Preparation**
   - Clean out FSR sensor channels with small drill bits
   - Test fit all FSR sensors in designated channels
   - Install threaded inserts in sensor mounting points
   - Apply thin layer of flexible adhesive for sensor bonding

3. **Structural Reinforcement**
   - Install aluminum plate in foot arch area
   - Use M3 bolts through threaded inserts in TPU
   - This provides rigid mounting for servo assemblies
   - Test overall structural integrity under load

### PHASE 2: TOE ARTICULATION ASSEMBLY

**Duration**: 2 hours per foot

1. **Servo Preparation**
   - Test all servos for proper operation
   - Install servo horns with thread locker
   - Route servo cables through designated channels
   - Apply vibration-dampening material to servo cases

2. **Toe Segment Installation**
   - Install ball bearings in toe joint pivots
   - Apply marine grease to bearing surfaces
   - Attach servo horns to toe segments
   - Test range of motion (0° to 60° per toe)

3. **Cable Management**
   - Route all servo cables through TPU channels
   - Use spiral cable wrap for protection
   - Install strain relief at foot base connection
   - Ensure no cables interfere with toe movement

### PHASE 3: SENSOR INTEGRATION

**Duration**: 1.5 hours per foot

1. **FSR Sensor Installation**
   - Apply thin layer of conductive adhesive to sensor backing
   - Install sensors in designated channels
   - Ensure proper contact with ground-facing surface
   - Test sensor response with known weights

2. **Signal Conditioning Setup**
   - Install op-amp array for FSR signal conditioning
   - Configure voltage divider networks for each sensor
   - Calibrate ADC multiplexer channel selection
   - Test sensor linearity across expected force range

3. **Wiring Harness Assembly**
   - Create main wiring harness from foot to ankle
   - Use waterproof connectors for environmental protection
   - Include power, data, and ground connections
   - Test continuity and signal integrity

### PHASE 4: CONTROLLER INSTALLATION

**Duration**: 1 hour per foot

1. **ESP32-C3 Setup**
   - Install controller in waterproof housing
   - Mount housing in protected area of foot structure
   - Connect power supply (3.3V logic, 5V servo)
   - Install status LEDs for operation indication

2. **Servo Driver Configuration**
   - Mount PCA9685 servo driver board
   - Connect I2C bus to ESP32-C3 controller
   - Wire servo power with individual current limiting
   - Install power smoothing capacitors

3. **Communication Interface**
   - Configure ESP-NOW for foot-to-leg communication
   - Test wireless connectivity to leg controller
   - Set up real-time sensor data transmission
   - Verify low-latency servo command response

---

## SOFTWARE ARCHITECTURE

### MAIN CONTROLLER FIRMWARE

**File**: `src/foot_controller.c`

```c
#include "esp_now.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Foot Controller State
typedef struct {
    uint8_t foot_id;              // 0=LEFT, 1=RIGHT
    bool ground_contact;          // Current contact status
    float pressure_sensors[16];   // FSR readings (N)
    float toe_angles[5];          // Toe positions (degrees)
    float total_force;            // Sum of all pressure (N)
    float center_of_pressure_x;   // CoP X coordinate (mm)
    float center_of_pressure_y;   // CoP Y coordinate (mm)
    uint32_t contact_duration;    // Time in contact (ms)
    float grip_strength;          // Toe grip force (N)
} foot_state_t;

// Global foot state
foot_state_t p32_foot_state = {0};

// Servo control initialization
void p32_foot_servo_init(void) {
    // Initialize I2C master for PCA9685
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    
    // Configure PCA9685 for toe servos
    pca9685_init(I2C_NUM_0, 0x41);
    pca9685_set_pwm_freq(50); // 50Hz for servos
}

// FSR sensor reading with multiplexer
float p32_foot_read_fsr(uint8_t channel) {
    // Set multiplexer channel
    gpio_set_level(MUX_S0_PIN, (channel >> 0) & 0x01);
    gpio_set_level(MUX_S1_PIN, (channel >> 1) & 0x01);
    gpio_set_level(MUX_S2_PIN, (channel >> 2) & 0x01);
    gpio_set_level(MUX_S3_PIN, (channel >> 3) & 0x01);
    
    // Allow settling time
    vTaskDelay(pdMS_TO_TICKS(1));
    
    // Read ADC value
    int adc_raw = adc1_get_raw(ADC1_CHANNEL_1);
    
    // Convert to force (calibrated curve)
    float voltage = (adc_raw * 3.3f) / 4095.0f;
    float force = fsr_voltage_to_force(voltage);
    
    return force;
}

// Toe control (5DOF)
void p32_foot_toe_control(uint8_t toe_index, float angle) {
    // Clamp angle to safe range
    angle = fmaxf(0.0f, fminf(60.0f, angle));
    
    // Map toe index to servo channels
    uint8_t servo_channel;
    switch (toe_index) {
        case 0: servo_channel = 0; break; // Big toe
        case 1: servo_channel = 2; break; // Index toe
        case 2: servo_channel = 4; break; // Middle toe  
        case 3: servo_channel = 6; break; // Ring toe
        case 4: servo_channel = 8; break; // Little toe
        default: return;
    }
    
    // Convert angle to PWM (1000-2000μs)
    uint16_t pwm_value = angle_to_pwm(angle, 0, 60);
    
    // Send command to PCA9685
    pca9685_set_pwm(servo_channel, pwm_value);
    
    // Update state
    p32_foot_state.toe_angles[toe_index] = angle;
}

// Ground contact detection
void p32_foot_sensor_task(void *pvParameters) {
    while (1) {
        float total_force = 0.0f;
        float moment_x = 0.0f, moment_y = 0.0f;
        
        // Read all FSR sensors
        for (int i = 0; i < 9; i++) { // 9 active sensors
            p32_foot_state.pressure_sensors[i] = p32_foot_read_fsr(i);
            total_force += p32_foot_state.pressure_sensors[i];
            
            // Calculate center of pressure
            float sensor_x = get_sensor_x_position(i);
            float sensor_y = get_sensor_y_position(i);
            moment_x += p32_foot_state.pressure_sensors[i] * sensor_x;
            moment_y += p32_foot_state.pressure_sensors[i] * sensor_y;
        }
        
        // Update foot state
        p32_foot_state.total_force = total_force;
        p32_foot_state.ground_contact = (total_force > 5.0f); // 5N threshold
        
        if (total_force > 0.1f) {
            p32_foot_state.center_of_pressure_x = moment_x / total_force;
            p32_foot_state.center_of_pressure_y = moment_y / total_force;
        }
        
        // Send sensor data to leg controller via ESP-NOW
        esp_now_send_foot_data(&p32_foot_state);
        
        vTaskDelay(pdMS_TO_TICKS(5)); // 200Hz sensor update
    }
}

// Adaptive toe grip control
void p32_foot_grip_task(void *pvParameters) {
    while (1) {
        // Receive grip commands from leg controller
        grip_command_t cmd;
        if (esp_now_receive_grip_command(&cmd)) {
            
            switch (cmd.grip_type) {
                case GRIP_BALANCE:
                    // Adjust toes for balance correction
                    balance_toe_adjustment(cmd.balance_offset);
                    break;
                    
                case GRIP_TERRAIN:
                    // Adapt to terrain contours
                    terrain_adaptation(cmd.terrain_data);
                    break;
                    
                case GRIP_EXPRESSION:
                    // Expressive toe movements
                    expressive_toe_sequence(cmd.expression_id);
                    break;
                    
                case GRIP_RELAXED:
                    // Return to neutral position
                    for (int i = 0; i < 5; i++) {
                        p32_foot_toe_control(i, 10.0f); // Slight flexion
                    }
                    break;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz grip control
    }
}

// Balance assistance through toe adjustment
void balance_toe_adjustment(float balance_offset) {
    // If falling forward, grip with toes
    if (balance_offset > 0.1f) {
        for (int i = 0; i < 5; i++) {
            p32_foot_toe_control(i, 30.0f + balance_offset * 20.0f);
        }
    }
    // If falling backward, extend toes
    else if (balance_offset < -0.1f) {
        for (int i = 0; i < 5; i++) {
            p32_foot_toe_control(i, 5.0f);
        }
    }
}

// Terrain adaptation algorithm
void terrain_adaptation(terrain_data_t terrain) {
    // Adjust individual toes based on ground contour
    for (int i = 0; i < 5; i++) {
        float ground_height = terrain.toe_ground_height[i];
        float toe_angle = 15.0f + (ground_height * 2.0f); // Adapt to surface
        toe_angle = fmaxf(0.0f, fminf(60.0f, toe_angle));
        p32_foot_toe_control(i, toe_angle);
    }
}

// ESP-NOW communication callbacks
void p32_foot_espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    if (len == sizeof(foot_command_t)) {
        foot_command_t *cmd = (foot_command_t *)data;
        
        switch (cmd->command_type) {
            case CMD_TOE_POSITION:
                p32_foot_toe_control(cmd->toe_index, cmd->angle);
                break;
            case CMD_GRIP_MODE:
                set_grip_mode(cmd->grip_mode);
                break;
            case CMD_CALIBRATE_SENSORS:
                calibrate_fsr_sensors();
                break;
            case CMD_EMERGENCY_STOP:
                emergency_stop_all_toes();
                break;
        }
    }
}

void app_main(void) {
    printf("P32 Foot Controller Starting...\n");
    
    // Initialize hardware
    gpio_install_isr_service(0);
    p32_foot_servo_init();
    setup_fsr_multiplexer();
    
    // Configure ESP-NOW mesh networking
    esp_now_init();
    esp_now_register_recv_cb(p32_foot_espnow_recv_cb);
    
    // Create tasks
    xTaskCreate(p32_foot_sensor_task, "sensors", 4096, NULL, 6, NULL);
    xTaskCreate(p32_foot_grip_task, "grip", 4096, NULL, 4, NULL);
    
    printf("Foot controller ready for ground contact.\n");
}
```

---

## CALIBRATION PROCEDURES

### FSR SENSOR CALIBRATION

1. **Zero-Force Baseline**
   - Lift foot completely off ground
   - Record all FSR sensor readings for 30 seconds
   - Calculate average baseline values
   - Store in controller EEPROM as zero reference

2. **Force Curve Calibration**
   - Apply known weights: 1kg, 5kg, 10kg, 25kg, 50kg
   - Record FSR voltage at each weight
   - Generate polynomial curve fit for each sensor
   - Validate linearity and repeatability

3. **Dynamic Calibration**
   - Test sensor response during simulated walking
   - Verify no crosstalk between adjacent sensors
   - Calibrate center-of-pressure calculations
   - Test temperature compensation if needed

### SERVO CALIBRATION

1. **Toe Range Calibration**
   ```powershell
   # Calibrate each toe individually
   ./tools/calibrate-toe-servos.ps1 -Toe BigToe -MinAngle 0 -MaxAngle 60
   ./tools/calibrate-toe-servos.ps1 -Toe IndexToe -MinAngle 0 -MaxAngle 60
   ./tools/calibrate-toe-servos.ps1 -Toe MiddleToe -MinAngle 0 -MaxAngle 60
   ./tools/calibrate-toe-servos.ps1 -Toe RingToe -MinAngle 0 -MaxAngle 60
   ./tools/calibrate-toe-servos.ps1 -Toe LittleToe -MinAngle 0 -MaxAngle 60
   ```

2. **Grip Strength Calibration**
   - Test toe grip force at various angles
   - Measure actual grip strength with force gauge
   - Generate lookup table for grip force vs angle
   - Set safe maximum grip limits

---

## TESTING PROCEDURES

### STATIC TESTING

1. **Pressure Distribution Testing**
   ```powershell
   # Test pressure sensor array
   ./tools/test-foot-pressure.ps1 -Weight 25 -Duration 60
   # Applies 25kg load for 60 seconds, maps pressure distribution
   ```

2. **Toe Articulation Testing**
   ```powershell
   # Test individual toe movement
   ./tools/test-toe-range.ps1 -AllToes -MinAngle 0 -MaxAngle 60
   # Cycles all toes through full range, checks for binding
   ```

3. **Balance Response Testing**
   ```powershell
   # Test balance assistance
   ./tools/test-balance-toes.ps1 -Disturbance Forward -Magnitude 0.2
   # Simulates forward fall, tests toe grip response
   ```

### DYNAMIC TESTING

1. **Walking Simulation**
   - Place foot on treadmill or moving platform
   - Test sensor response during heel-strike, stance, toe-off
   - Verify center-of-pressure tracking accuracy
   - Measure response time for balance corrections

2. **Terrain Adaptation**
   - Test foot on various surfaces (carpet, concrete, gravel)
   - Verify toe adaptation to surface irregularities
   - Test grip performance on inclined surfaces
   - Document maximum slope for stable contact

3. **Load Testing**
   - Progressive loading from 10kg to 75kg
   - Test structural integrity of TPU components
   - Verify sensor linearity under maximum load
   - Document any permanent deformation

---

## SAFETY CONSIDERATIONS

### STRUCTURAL SAFETY

1. **Load Limits**
   - Maximum 50kg per foot (100kg total robot weight)
   - Safety factor of 2x on all structural components
   - Mechanical stops prevent toe overextension
   - Emergency release if overload detected

2. **Material Safety**
   - TPU components tested for fatigue resistance
   - Regular inspection for stress cracks
   - Replacement schedule for wear components
   - UV-resistant materials for outdoor use

### ELECTRICAL SAFETY

```c
// Safety monitoring for foot controllers
void p32_foot_safety_task(void *pvParameters) {
    while (1) {
        // Check sensor readings for anomalies
        for (int i = 0; i < 9; i++) {
            if (p32_foot_state.pressure_sensors[i] > 500.0f) { // 500N limit
                log_safety_event("Overload detected on sensor %d", i);
                emergency_stop_all_toes();
            }
        }
        
        // Monitor servo currents
        float total_servo_current = read_total_servo_current();
        if (total_servo_current > 3.0f) { // 3A limit
            reduce_servo_power(0.7f);
            log_safety_event("High servo current: %.2fA", total_servo_current);
        }
        
        // Communication timeout check
        if (last_command_time + 2000 < millis()) {
            emergency_stop_all_toes();
            log_safety_event("Communication timeout - foot controller");
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz safety monitoring
    }
}
```

---

## MAINTENANCE SCHEDULE

### DAILY CHECKS (3 minutes per foot)

- Visual inspection of TPU components for wear
- Test ground contact detection with light pressure
- Quick toe movement check for smooth operation
- Verify all connections secure

### WEEKLY MAINTENANCE (20 minutes per foot)

- Clean pressure sensors and contact surfaces
- Lubricate toe joint bearings
- Check servo operation at full range
- Inspect cable management and strain relief

### MONTHLY MAINTENANCE (1 hour per foot)

- Recalibrate all FSR sensors
- Deep clean all components
- Replace anti-slip pads if worn
- Performance testing and optimization

### ANNUAL OVERHAUL (4 hours per foot)

- Complete disassembly and inspection
- Replace all TPU components showing wear
- Servo replacement and recalibration
- Structural integrity testing
- Documentation update

---

## INTEGRATION POINTS

### LEG SUBSYSTEM CONNECTION

- **Mechanical**: Ankle assembly provides secure mounting interface
- **Electrical**: 5V power and communication via waterproof connector
- **Software**: Receive balance commands, send ground contact data
- **Data**: Real-time pressure distribution and center-of-pressure

### FULL SYSTEM COORDINATION

- **Balance**: Foot pressure data used by torso for whole-body balance
- **Locomotion**: Ground contact timing coordinates gait cycle
- **Terrain**: Surface adaptation data shared with all subsystems
- **Safety**: Ground contact loss triggers emergency protocols

---

## TROUBLESHOOTING GUIDE

### COMMON ISSUES

**FSR Sensor Not Reading**
- Check multiplexer channel selection
- Verify sensor electrical connections
- Test with multimeter for continuity
- Replace sensor if physically damaged

**Toe Not Moving**
- Check servo power supply (5V ±0.2V)
- Verify PWM signal from PCA9685
- Test servo with direct signal
- Check for mechanical binding in joints

**Inaccurate Pressure Reading**
- Recalibrate sensor with known weights
- Check for debris on sensor surface
- Verify sensor mounting is secure
- Update calibration coefficients

**Communication Failure**
- Check ESP-NOW network configuration
- Verify power supply to controller
- Test point-to-point before mesh
- Check antenna connections

### DIAGNOSTIC TOOLS

```powershell
# Comprehensive foot diagnostics
./tools/diagnose-foot-system.ps1 -Verbose
# Tests sensors, servos, communication

# Real-time pressure visualization
./tools/visualize-foot-pressure.ps1 -Duration 60
# Shows live pressure map for 60 seconds
```

---

This completes the comprehensive foot subsystem construction guide. The system provides advanced ground contact detection, adaptive balance control, and expressive toe articulation for natural locomotion and terrain adaptation.

**Next**: Proceed to **FULL INTEGRATION GUIDE** for complete humanoid assembly and system coordination.