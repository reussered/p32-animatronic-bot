# LEG SUBSYSTEM CONSTRUCTION GUIDE
**P32 Animatronic Bot - Lower Body Locomotion System**

---

## OVERVIEW

The leg subsystem provides complete lower body locomotion for the P32 animatronic humanoid. Each leg delivers 6 degrees of freedom (3DOF hip, 1DOF knee, 2DOF ankle) enabling natural walking, running, and expressive poses. The distributed control architecture uses ESP32-C3 controllers for real-time servo coordination and sensor integration.

### KEY SPECIFICATIONS
- **Degrees of Freedom**: 6DOF per leg (12DOF total bilateral)
- **Load Capacity**: 75kg total robot weight + 25kg payload
- **Controller**: ESP32-C3 per leg with ESP-NOW mesh networking
- **Servo Count**: 6 high-torque servos per leg (MG996R/MG958)
- **Walking Speed**: 0.5-2.0 meters/second with dynamic balance
- **Power Consumption**: 180W peak (both legs), 45W steady walking
- **Materials**: Aluminum extrusion frame, 3D printed joints, carbon fiber reinforcement

---

## ANATOMICAL ORGANIZATION

### HIP ASSEMBLY (3DOF)
- **Hip Flexion/Extension**: Forward/backward leg swing (-30° to +120°)
- **Hip Abduction/Adduction**: Lateral leg movement (-45° to +45°)  
- **Hip Rotation**: Internal/external rotation (-30° to +30°)
- **Servo**: 3x MG996R (8.5kg-cm torque each)
- **Controller**: Primary leg ESP32-C3 with IMU for balance

### THIGH STRUCTURE
- **Length**: 300mm (12 inches) aluminum extrusion
- **Cross-Section**: 40x40mm T-slot profile
- **Reinforcement**: Carbon fiber tube insert for bending strength
- **Integration**: Houses power distribution and communication cables
- **Mounting**: Universal joint interfaces at hip and knee

### KNEE ASSEMBLY (1DOF)
- **Knee Flexion**: 0° to 140° (full extension to deep bend)
- **Servo**: 1x MG958 (10kg-cm high-torque) with gear reduction
- **Mechanical Advantage**: 3:1 planetary gearbox for load handling
- **Safety**: Mechanical stops prevent hyperextension
- **Integration**: Embedded position encoder for precise control

### CALF STRUCTURE  
- **Length**: 250mm (10 inches) aluminum extrusion
- **Cross-Section**: 30x30mm T-slot profile (lighter than thigh)
- **Cable Management**: Internal routing for ankle servo power/signals
- **Reinforcement**: Integrated shock absorption for ground impact
- **Mounting**: Knee joint interface and ankle assembly bracket

### ANKLE ASSEMBLY (2DOF)
- **Ankle Dorsiflexion/Plantarflexion**: Toe up/down (-20° to +50°)
- **Ankle Inversion/Eversion**: Foot tilt left/right (-20° to +20°)
- **Servos**: 2x MG996R with crossed-axis mounting
- **Load Distribution**: Transfers leg forces to foot contact points
- **Sensor Integration**: IMU for ground contact detection

---

## BILL OF MATERIALS

### MECHANICAL COMPONENTS
| Component | Quantity | Specification | Purpose |
|-----------|----------|---------------|---------|
| **Aluminum Extrusion 40x40mm** | 2m | T-slot profile, 6063 alloy | Thigh structure |
| **Aluminum Extrusion 30x30mm** | 2m | T-slot profile, 6063 alloy | Calf structure |
| **Carbon Fiber Tube** | 1m | 32mm OD, 2mm wall | Thigh reinforcement |
| **Universal Joints** | 6 | Stainless steel, M8 bore | Hip/knee/ankle interfaces |
| **Planetary Gearbox** | 2 | 3:1 ratio, 8mm input shaft | Knee torque multiplication |
| **Ball Bearings** | 12 | 608ZZ (8x22x7mm) | Joint rotation support |
| **T-Slot Connectors** | 24 | M6 internal thread | Extrusion assembly |

### SERVO MOTORS
| Servo Model | Quantity | Torque | Application |
|-------------|----------|--------|-------------|
| **MG996R** | 10 | 8.5kg-cm | Hip flexion/abduction/rotation, ankle control |
| **MG958** | 2 | 10kg-cm | Knee flexion (high-load) |

### ELECTRONICS
| Component | Quantity | Specification | Purpose |
|-----------|----------|---------------|---------|
| **ESP32-C3** | 2 | RISC-V, WiFi, Bluetooth | Per-leg controller |
| **PCA9685** | 2 | 16-channel PWM | Servo expansion |
| **MPU6050** | 2 | 6-axis IMU | Balance/orientation |
| **INA219** | 2 | Current/voltage sensor | Power monitoring |
| **Buck Converter** | 2 | 12V to 6V, 10A | Servo power supply |
| **Capacitors** | 4 | 2200μF, 16V | Servo power smoothing |

### 3D PRINTED PARTS
| Part Name | Quantity | Material | Purpose |
|-----------|----------|----------|---------|
| **Hip Housing** | 2 | PLA+ (rigid) | 3DOF hip joint assembly |
| **Thigh Structure** | 2 | PLA+ (rigid) | Aluminum extrusion connectors |
| **Knee Assembly** | 2 | PETG (strong) | High-load knee joint |
| **Calf Structure** | 2 | PLA+ (rigid) | Aluminum extrusion connectors |
| **Ankle Joint** | 2 | PETG (strong) | 2DOF ankle assembly |
| **Servo Brackets** | 12 | PLA+ (rigid) | Motor mounting |

### FASTENERS & HARDWARE
| Hardware | Quantity | Size | Application |
|----------|----------|------|-------------|
| **Socket Head Bolts** | 40 | M6x25mm | T-slot assembly |
| **Socket Head Bolts** | 24 | M4x20mm | Servo mounting |  
| **Socket Head Bolts** | 16 | M3x16mm | Electronics mounting |
| **Lock Washers** | 80 | M6, M4, M3 | Vibration resistance |
| **Threaded Inserts** | 20 | M6 into plastic | 3D print reinforcement |

---

## ELECTRICAL ARCHITECTURE

### POWER DISTRIBUTION
```
Main Power Rail (12V from torso)
    ├── Buck Converter → 6V Servo Rail (10A capacity)
    │   ├── Hip Servos (3x MG996R)
    │   ├── Knee Servo (1x MG958) 
    │   └── Ankle Servos (2x MG996R)
    └── Buck Converter → 5V Logic Rail (2A capacity)
        ├── ESP32-C3 Controller (200mA)
        ├── PCA9685 Servo Driver (50mA)
        ├── MPU6050 IMU (5mA)
        └── INA219 Power Monitor (1mA)
```

### ESP32-C3 GPIO ASSIGNMENT
```c
// Leg Controller GPIO Configuration
#define LEG_ESP32_C3_LEFT    // Left leg controller
#define LEG_ESP32_C3_RIGHT   // Right leg controller

// I2C Bus (Shared)
#define I2C_SDA_PIN     4    // I2C data line
#define I2C_SCL_PIN     5    // I2C clock line

// PCA9685 Servo Driver (I2C Address 0x40)
// Servo Channels:
// 0: Hip Flexion/Extension
// 1: Hip Abduction/Adduction  
// 2: Hip Internal/External Rotation
// 3: Knee Flexion/Extension
// 4: Ankle Dorsiflexion/Plantarflexion
// 5: Ankle Inversion/Eversion

// Status LEDs
#define STATUS_LED_PIN  2    // Built-in LED for status
#define POWER_LED_PIN   18   // Power indicator LED
#define ERROR_LED_PIN   19   // Error condition LED

// Emergency Stop
#define ESTOP_PIN       0    // Emergency stop input (pull-up)

// Inter-Controller Communication
#define UART_TX_PIN     21   // UART to torso controller
#define UART_RX_PIN     20   // UART from torso controller

// Power Monitoring
#define BATTERY_SENSE_PIN 1  // ADC for battery voltage

// Spare GPIO for expansion
#define SPARE_GPIO_6    6
#define SPARE_GPIO_7    7
#define SPARE_GPIO_8    8
#define SPARE_GPIO_9    9
#define SPARE_GPIO_10   10
```

### ESP-NOW MESH INTEGRATION
```c
// ESP-NOW Network Topology
typedef struct {
    uint8_t node_id;
    char node_type[16];
    uint8_t mac_address[6];
} esp_now_node_t;

// Network Nodes
esp_now_node_t torso_controller = {
    .node_id = 0x01,
    .node_type = "TORSO_MASTER",
    .mac_address = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01}
};

esp_now_node_t left_leg_controller = {
    .node_id = 0x06,
    .node_type = "LEG_LEFT", 
    .mac_address = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x06}
};

esp_now_node_t right_leg_controller = {
    .node_id = 0x07,
    .node_type = "LEG_RIGHT",
    .mac_address = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x07}
};
```

---

## 3D PRINTED COMPONENTS

### HIP HOUSING DESIGN
**File**: `assets/shapes/stl/legs/hip_housing.stl`
```
Specifications:
- Dimensions: 120x100x80mm
- Material: PLA+ (high strength)
- Infill: 40% gyroid pattern
- Wall Thickness: 3 perimeters
- Support: Required for servo cavities
- Print Time: 8 hours per leg

Features:
- Triple servo mounting with vibration dampening
- Universal joint interfaces with sealed bearings
- Cable management channels with strain relief
- Ventilation slots for servo cooling
- Integrated mounting points for thigh extrusion
```

### KNEE ASSEMBLY DESIGN  
**File**: `assets/shapes/stl/legs/knee_assembly.stl`
```
Specifications:
- Dimensions: 100x80x60mm
- Material: PETG (high impact strength)
- Infill: 50% honeycomb pattern
- Wall Thickness: 4 perimeters
- Support: Minimal (designed for supportless printing)
- Print Time: 6 hours per leg

Features:
- Integrated planetary gearbox housing
- High-load bearing interfaces
- Mechanical stops prevent overextension
- Position encoder mounting
- Thigh-to-calf connection hardware
```

### ANKLE JOINT DESIGN
**File**: `assets/shapes/stl/legs/ankle_joint.stl`
```
Specifications:
- Dimensions: 80x60x50mm
- Material: PETG (flexibility required)
- Infill: 45% triangular pattern
- Wall Thickness: 3 perimeters  
- Support: Required for complex geometry
- Print Time: 4 hours per leg

Features:
- Dual-servo crossed-axis configuration
- IMU sensor cavity with shock mounting
- Foot attachment interface with quick-release
- Cable routing to foot pressure sensors
- Ground clearance optimization
```

---

## ASSEMBLY PROCEDURES

### PHASE 1: HIP ASSEMBLY
**Duration**: 2 hours per leg

1. **Prepare Hip Housing**
   - Clean 3D printed hip housing, remove support material
   - Install threaded inserts using soldering iron (M6 size)
   - Test fit all servo mounting points

2. **Install Hip Servos**
   - Mount MG996R servos in designated cavities
   - Use vibration dampening washers between servo and housing
   - Connect servo horns with high-strength screws
   - Verify full range of motion without binding

3. **Universal Joint Integration**
   - Install ball bearings in joint interfaces
   - Apply marine-grade grease to bearing races
   - Mount universal joints with thread locker
   - Test smooth operation under load

4. **Wiring and Testing**
   - Route servo cables through strain relief channels
   - Connect to PCA9685 servo driver (channels 0-2)
   - Test individual servo operation and range limits
   - Verify no interference between servo movements

### PHASE 2: THIGH CONSTRUCTION
**Duration**: 1.5 hours per leg

1. **Aluminum Extrusion Prep**
   - Cut 40x40mm extrusion to 300mm length
   - Deburr cut ends with files
   - Install T-slot nuts in channels for mounting hardware

2. **Carbon Fiber Reinforcement**
   - Insert 32mm carbon fiber tube inside extrusion
   - Secure with end caps and adhesive
   - This prevents bending under high loads

3. **Connection Hardware**
   - Attach 3D printed thigh structure connectors
   - Use M6 bolts with thread locker
   - Install hip and knee interface brackets
   - Test assembly alignment

### PHASE 3: KNEE INTEGRATION
**Duration**: 2.5 hours per leg

1. **Planetary Gearbox Assembly**
   - Install MG958 servo into gearbox input
   - Apply proper grease to gear trains
   - Mount gearbox in 3D printed knee housing
   - Test gear reduction and backdrive resistance

2. **Position Encoder Setup**
   - Mount rotary encoder on output shaft
   - Calibrate zero position for leg extension
   - Connect to ESP32-C3 controller via I2C
   - Verify position feedback accuracy

3. **Mechanical Safety Systems**
   - Install mechanical stops at 0° and 140°
   - Test stop engagement under servo load
   - Ensure stops cannot be damaged by full servo force
   - Document stop positions for software limits

### PHASE 4: CALF AND ANKLE
**Duration**: 1.5 hours per leg

1. **Calf Structure Assembly**
   - Cut 30x30mm extrusion to 250mm length
   - Install T-slot connectors and end brackets
   - Route ankle servo cables through extrusion
   - Mount knee-to-calf connection hardware

2. **Ankle Servo Installation**
   - Mount dual MG996R servos in crossed configuration
   - Install universal joints for 2DOF movement
   - Connect servo horns with high-strength hardware
   - Test ankle range of motion

3. **IMU and Sensor Integration**
   - Mount MPU6050 IMU in shock-resistant housing
   - Calibrate IMU orientation relative to leg axis
   - Install ground contact sensors in foot interface
   - Test sensor data acquisition and filtering

### PHASE 5: CONTROLLER INSTALLATION
**Duration**: 1 hour per leg

1. **ESP32-C3 Controller Setup**
   - Install controller in protective housing
   - Mount housing in thigh structure with vibration isolation
   - Connect power supply (12V to 5V buck converter)
   - Install status LEDs and emergency stop switch

2. **Servo Driver Configuration**
   - Mount PCA9685 servo driver board
   - Connect I2C bus to ESP32-C3 controller
   - Wire servo power rails with current limiting
   - Install power smoothing capacitors

3. **Communication Interface**
   - Configure ESP-NOW mesh networking
   - Test communication with torso master controller
   - Verify low-latency command response
   - Set up telemetry data transmission

---

## SOFTWARE ARCHITECTURE

### MAIN CONTROLLER FIRMWARE
**File**: `src/leg_controller.c`

```c
#include "esp_now.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Leg Controller Configuration
typedef struct {
    uint8_t leg_id;               // 0=LEFT, 1=RIGHT
    bool is_stance_leg;           // Weight bearing status
    float hip_angles[3];          // Flex/Abd/Rot angles
    float knee_angle;             // Knee flexion angle
    float ankle_angles[2];        // Dorsi/Invert angles
    float ground_force;           // Foot pressure (N)
    float battery_voltage;        // Power monitoring
    uint32_t step_count;          // Locomotion tracking
} leg_state_t;

// Global leg state
leg_state_t p32_leg_state = {0};

// Servo control via PCA9685
void p32_leg_servo_init(void) {
    // Initialize I2C master
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
    
    // Configure PCA9685 servo driver
    pca9685_init(I2C_NUM_0, 0x40);
    pca9685_set_pwm_freq(50); // 50Hz for servos
}

// Hip control (3DOF)
void p32_leg_hip_control(float flexion, float abduction, float rotation) {
    // Clamp angles to safe ranges
    flexion = fmaxf(-30.0f, fminf(120.0f, flexion));
    abduction = fmaxf(-45.0f, fminf(45.0f, abduction));
    rotation = fmaxf(-30.0f, fminf(30.0f, rotation));
    
    // Convert angles to servo PWM values (1000-2000μs)
    uint16_t hip_flex_pwm = angle_to_pwm(flexion, -30, 120);
    uint16_t hip_abd_pwm = angle_to_pwm(abduction, -45, 45);
    uint16_t hip_rot_pwm = angle_to_pwm(rotation, -30, 30);
    
    // Send commands to PCA9685
    pca9685_set_pwm(0, hip_flex_pwm);   // Hip flexion servo
    pca9685_set_pwm(1, hip_abd_pwm);    // Hip abduction servo
    pca9685_set_pwm(2, hip_rot_pwm);    // Hip rotation servo
    
    // Update state
    p32_leg_state.hip_angles[0] = flexion;
    p32_leg_state.hip_angles[1] = abduction;
    p32_leg_state.hip_angles[2] = rotation;
}

// Knee control (1DOF with gear reduction)
void p32_leg_knee_control(float flexion) {
    // Clamp to mechanical limits
    flexion = fmaxf(0.0f, fminf(140.0f, flexion));
    
    // Account for 3:1 gear reduction
    uint16_t knee_pwm = angle_to_pwm(flexion / 3.0f, 0, 46.7);
    
    pca9685_set_pwm(3, knee_pwm);       // Knee servo through gearbox
    p32_leg_state.knee_angle = flexion;
}

// Ankle control (2DOF)  
void p32_leg_ankle_control(float dorsiflexion, float inversion) {
    // Clamp angles to physiological limits
    dorsiflexion = fmaxf(-20.0f, fminf(50.0f, dorsiflexion));
    inversion = fmaxf(-20.0f, fminf(20.0f, inversion));
    
    uint16_t ankle_dorsi_pwm = angle_to_pwm(dorsiflexion, -20, 50);
    uint16_t ankle_inv_pwm = angle_to_pwm(inversion, -20, 20);
    
    pca9685_set_pwm(4, ankle_dorsi_pwm); // Ankle dorsiflexion
    pca9685_set_pwm(5, ankle_inv_pwm);   // Ankle inversion
    
    p32_leg_state.ankle_angles[0] = dorsiflexion;
    p32_leg_state.ankle_angles[1] = inversion;
}

// Balance and ground contact monitoring
void p32_leg_balance_task(void *pvParameters) {
    mpu6050_dev_t imu_dev;
    mpu6050_init(&imu_dev, I2C_NUM_0, 0x68);
    
    while (1) {
        // Read IMU data
        float accel_x, accel_y, accel_z;
        float gyro_x, gyro_y, gyro_z;
        mpu6050_get_accel(&imu_dev, &accel_x, &accel_y, &accel_z);
        mpu6050_get_gyro(&imu_dev, &gyro_x, &gyro_y, &gyro_z);
        
        // Detect ground contact via Z-axis acceleration spike
        if (accel_z > 15.0f) { // Ground impact threshold
            p32_leg_state.ground_force = (accel_z - 9.81f) * 7.5f; // Estimate force
        } else {
            p32_leg_state.ground_force = 0.0f; // Swing phase
        }
        
        // Send balance data to torso controller via ESP-NOW
        esp_now_send_balance_data(&p32_leg_state);
        
        vTaskDelay(pdMS_TO_TICKS(10)); // 100Hz update rate
    }
}

// Walking gait coordination
void p32_leg_walking_task(void *pvParameters) {
    float gait_phase = 0.0f;    // 0.0-1.0 gait cycle
    float stride_length = 0.4f; // 40cm stride
    float step_height = 0.1f;   // 10cm foot lift
    
    while (1) {
        // Receive gait commands from torso master
        gait_command_t cmd;
        if (esp_now_receive_gait_command(&cmd)) {
            
            // Calculate leg positions for current gait phase
            if (p32_leg_state.is_stance_leg) {
                // Stance phase: support body weight, push backward
                float hip_flex = -20.0f * sinf(gait_phase * M_PI);
                float knee_flex = 15.0f * sinf(gait_phase * M_PI);
                
                p32_leg_hip_control(hip_flex, 0, 0);
                p32_leg_knee_control(knee_flex);
                p32_leg_ankle_control(-10, 0); // Plantarflex for push-off
                
            } else {
                // Swing phase: lift leg, move forward
                float hip_flex = 60.0f * sinf(gait_phase * M_PI);
                float knee_flex = 80.0f * sinf(gait_phase * M_PI);
                
                p32_leg_hip_control(hip_flex, 0, 0);
                p32_leg_knee_control(knee_flex);
                p32_leg_ankle_control(20, 0); // Dorsiflex for clearance
            }
            
            gait_phase += cmd.gait_speed;
            if (gait_phase >= 1.0f) {
                gait_phase = 0.0f;
                p32_leg_state.step_count++;
                // Switch stance/swing phase
                p32_leg_state.is_stance_leg = !p32_leg_state.is_stance_leg;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz gait update
    }
}

// ESP-NOW communication callbacks
void p32_leg_espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    // Handle commands from torso master controller
    if (len == sizeof(leg_command_t)) {
        leg_command_t *cmd = (leg_command_t *)data;
        
        switch (cmd->command_type) {
            case CMD_WALK_FORWARD:
                start_walking_gait(cmd->speed);
                break;
            case CMD_TURN_LEFT:
                start_turning_gait(cmd->angle);
                break;
            case CMD_BALANCE_ADJUST:
                adjust_balance_compensation(cmd->balance_offset);
                break;
            case CMD_EMERGENCY_STOP:
                emergency_stop_all_servos();
                break;
        }
    }
}

void app_main(void) {
    printf("P32 Leg Controller Starting...\n");
    
    // Initialize hardware
    gpio_install_isr_service(0);
    p32_leg_servo_init();
    
    // Configure ESP-NOW mesh networking
    esp_now_init();
    esp_now_register_recv_cb(p32_leg_espnow_recv_cb);
    
    // Create tasks
    xTaskCreate(p32_leg_balance_task, "balance", 4096, NULL, 5, NULL);
    xTaskCreate(p32_leg_walking_task, "walking", 4096, NULL, 4, NULL);
    
    printf("Leg controller ready for commands.\n");
}
```

---

## WALKING GAIT ALGORITHMS

### BIPEDAL GAIT CYCLE
The leg subsystem implements a simplified bipedal walking pattern with static stability:

```c
// Gait cycle breakdown (0.0 to 1.0 phase)
typedef enum {
    GAIT_HEEL_STRIKE    = 0.0f,  // Initial foot contact
    GAIT_LOADING        = 0.1f,  // Weight transfer to stance leg
    GAIT_MIDSTANCE      = 0.3f,  // Single limb support
    GAIT_TERMINAL_STANCE = 0.5f,  // Push-off preparation
    GAIT_PRE_SWING      = 0.6f,  // Toe-off begins
    GAIT_INITIAL_SWING  = 0.7f,  // Foot clearance
    GAIT_MID_SWING      = 0.8f,  // Leg advancement
    GAIT_TERMINAL_SWING = 0.9f   // Preparation for landing
} gait_phase_t;

// Walking parameters
typedef struct {
    float stride_length;    // Forward distance per step (m)
    float stride_width;     // Lateral foot separation (m)  
    float step_frequency;   // Steps per second (Hz)
    float ground_clearance; // Foot lift height (m)
    bool dynamic_balance;   // Enable dynamic stability
} walking_params_t;
```

### BALANCE CONTROL SYSTEM
```c
// Balance control using center of mass (CoM) estimation
typedef struct {
    float com_x, com_y, com_z;     // Center of mass position
    float zmp_x, zmp_y;            // Zero moment point
    float stability_margin;         // Distance to stability boundary
    bool is_stable;                // Current stability status
} balance_state_t;

void update_balance_control(balance_state_t *balance) {
    // Calculate center of mass from leg positions and torso angle
    balance->com_x = calculate_com_position();
    
    // Estimate zero moment point from ground reaction forces
    balance->zmp_x = calculate_zmp_from_forces();
    
    // Check if ZMP is within support polygon (stable)
    balance->is_stable = (fabs(balance->zmp_x - balance->com_x) < 0.05f);
    
    // Adjust leg positions to maintain stability
    if (!balance->is_stable) {
        compensate_balance_with_legs(balance);
    }
}
```

---

## CALIBRATION PROCEDURES

### SERVO CALIBRATION
1. **Zero Position Setup**
   - Power on leg controller with all servos connected
   - Run calibration script: `./tools/calibrate-leg-servos.ps1`
   - Manually position leg in neutral stance
   - Save servo center positions to EEPROM

2. **Range of Motion Testing**
   - Test each DOF individually at 10% increments
   - Record actual achieved angles vs commanded angles
   - Generate correction lookup tables for each servo
   - Verify no mechanical binding throughout full range

3. **Load Testing**
   - Apply 50kg vertical load to foot (simulated body weight)
   - Test servo holding torque at various joint angles
   - Identify positions requiring current limiting
   - Document maximum safe operating angles under load

### IMU CALIBRATION
1. **Accelerometer Calibration**
   - Place leg assembly on level surface in 6 orientations
   - Record raw accelerometer values for each axis
   - Calculate offset and scale factors for 1g reference
   - Store calibration coefficients in controller EEPROM

2. **Gyroscope Calibration**
   - Keep leg stationary for 30 seconds, record gyro drift
   - Calculate bias values for each axis
   - Test rotation rate accuracy with known angular velocities
   - Implement temperature compensation if needed

### GROUND CONTACT CALIBRATION
1. **Force Sensor Baseline**
   - Lift leg completely off ground, record "zero" force
   - Apply known weights (5kg, 10kg, 25kg) to foot
   - Generate force calibration curve
   - Set ground contact threshold at 2kg

---

## TESTING PROCEDURES

### STATIC TESTING
1. **Joint Range Verification**
   ```powershell
   # Test hip joint ranges
   ./tools/test-leg-range.ps1 -Joint Hip -Axis Flexion -Min -30 -Max 120
   ./tools/test-leg-range.ps1 -Joint Hip -Axis Abduction -Min -45 -Max 45
   ./tools/test-leg-range.ps1 -Joint Hip -Axis Rotation -Min -30 -Max 30
   
   # Test knee and ankle
   ./tools/test-leg-range.ps1 -Joint Knee -Min 0 -Max 140
   ./tools/test-leg-range.ps1 -Joint Ankle -Axis Dorsi -Min -20 -Max 50
   ./tools/test-leg-range.ps1 -Joint Ankle -Axis Invert -Min -20 -Max 20
   ```

2. **Load Testing Protocol**
   ```powershell
   # Progressive load testing
   ./tools/test-leg-load.ps1 -StartWeight 10 -EndWeight 75 -Increment 5
   # Tests servo performance under increasing load
   ```

3. **Communication Testing**
   ```powershell  
   # ESP-NOW mesh network validation
   ./tools/test-espnow-mesh.ps1 -Nodes Torso,LeftLeg,RightLeg
   # Verifies low-latency communication between controllers
   ```

### DYNAMIC TESTING
1. **Balance Response Testing**
   - Apply external disturbances to leg assembly
   - Measure balance recovery time and stability margin
   - Test on various surface inclinations (0° to 15°)
   - Document maximum disturbance rejection capability

2. **Walking Gait Testing**
   - Start with very slow gait (0.1 steps/second)
   - Gradually increase speed up to 2.0 steps/second
   - Test forward, backward, and lateral walking
   - Measure power consumption vs walking speed

3. **Coordination Testing**
   - Test bilateral leg coordination for stable walking
   - Verify stance/swing phase transitions
   - Test turn-in-place and curved walking patterns
   - Measure gait cycle timing accuracy

---

## SAFETY CONSIDERATIONS

### MECHANICAL SAFETY
1. **Emergency Stops**
   - Hardware emergency stop button disconnects servo power
   - Software emergency stop limits servo commands to zero
   - Mechanical stops prevent joint overextension
   - All joints designed with 50% safety margin on load ratings

2. **Power Management**
   - Individual servo current limiting prevents overcurrent
   - Battery voltage monitoring prevents deep discharge
   - Thermal monitoring of servo motors and controllers
   - Automatic shutdown if any safety parameter exceeded

3. **Structural Integrity**
   - All load-bearing components rated for 2x maximum expected load
   - Regular inspection schedule for fasteners and bearings
   - Fatigue testing of 3D printed components
   - Backup support structures for critical joints

### SOFTWARE SAFETY
```c
// Safety monitoring task
void p32_leg_safety_task(void *pvParameters) {
    while (1) {
        // Check servo positions within safe ranges
        if (p32_leg_state.knee_angle > 145.0f) {
            emergency_stop_servo(3); // Knee servo
            log_safety_event("Knee overextension detected");
        }
        
        // Monitor servo currents
        float servo_current = read_servo_current();
        if (servo_current > 2.5f) { // 2.5A limit
            reduce_servo_power(0.8f); // Reduce to 80% power
            log_safety_event("High servo current detected");
        }
        
        // Check communication timeout
        if (last_command_time + 1000 < millis()) {
            emergency_stop_all_servos();
            log_safety_event("Communication timeout - stopping all motion");
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz safety checks
    }
}
```

---

## MAINTENANCE SCHEDULE

### DAILY CHECKS (5 minutes)
- Visual inspection of all mechanical connections
- Battery voltage and charge level verification
- Quick range of motion test for all joints
- Check for any unusual noises or vibrations

### WEEKLY MAINTENANCE (30 minutes)
- Torque check all critical fasteners
- Clean and lubricate joint bearings
- Inspect 3D printed parts for stress cracks
- Update and backup controller firmware

### MONTHLY MAINTENANCE (2 hours)  
- Complete servo calibration and range verification
- Deep clean all components and cable management
- Replace consumable items (bearings, bushings)
- Performance testing and gait optimization
- Document any wear patterns or degradation

### ANNUAL OVERHAUL (1 day)
- Complete disassembly and inspection
- Replace all bearings and wear components
- 3D print replacement parts as needed
- Comprehensive load testing and certification
- Update all documentation and spare parts inventory

---

## INTEGRATION POINTS

### TORSO CONNECTION
- **Mechanical**: Hip assemblies bolt directly to torso pelvis base
- **Electrical**: 12V power and communication cables route through torso
- **Software**: Receive gait commands from torso master controller
- **Data**: Send balance and ground contact data to torso for coordination

### FOOT SUBSYSTEM CONNECTION
- **Mechanical**: Ankle assembly provides mounting interface for foot
- **Electrical**: Route foot sensor cables through ankle assembly
- **Software**: Coordinate ankle position with foot sensor feedback
- **Data**: Receive ground contact and pressure distribution data

### ARM SUBSYSTEM COORDINATION
- **Balance**: Leg controllers send balance state to arms for counterbalancing
- **Walking**: Coordinate arm swing with leg gait for natural locomotion
- **Load**: Share total system weight between legs and any arm support

---

## TROUBLESHOOTING GUIDE

### COMMON ISSUES

**Servo Not Responding**
- Check power supply voltage (should be 6V ±0.2V)
- Verify I2C communication to PCA9685
- Test servo with direct PWM signal
- Replace servo if mechanically damaged

**Unstable Walking Gait**
- Recalibrate IMU sensors for accurate balance data
- Check for loose mechanical connections
- Verify bilateral leg coordination timing
- Adjust gait parameters for current load conditions

**Communication Timeouts**
- Check ESP-NOW network configuration and MAC addresses
- Verify antenna connections on all ESP32 controllers
- Test point-to-point communication before mesh network
- Ensure adequate power supply to all radio modules

**Mechanical Binding**
- Inspect joint alignment and bearing condition
- Check for debris in moving parts
- Verify correct assembly torque on all fasteners
- Lubricate joints if excessive friction detected

### DIAGNOSTIC TOOLS
```powershell
# Comprehensive leg diagnostics
./tools/diagnose-leg-system.ps1 -Verbose
# Runs complete test suite and generates detailed report

# Real-time monitoring
./tools/monitor-leg-telemetry.ps1 -Duration 300
# Displays live sensor data and servo positions for 5 minutes
```

---

This completes the comprehensive leg subsystem construction guide. The system provides robust bipedal locomotion with distributed control architecture and comprehensive safety systems. Each leg delivers 6 degrees of freedom with high-torque servos and real-time balance control for stable walking and dynamic movement.

**Next**: Proceed to **FOOT SUBSYSTEM CONSTRUCTION GUIDE** for completing the lower extremity assembly, then **FULL INTEGRATION GUIDE** for final system assembly and coordination.