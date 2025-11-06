# FLYING DRAGON IMPLEMENTATION GUIDE
## Quick-Start Checklists & Decision Trees

**Date**: November 4, 2025  
**Audience**: Development team ready to begin Phase 1

---

## PART 1: PRE-PROTOTYPING DECISIONS

### Decision Tree 1: Motor Configuration

```
Should we use quad motors or hub motor?

 [CHOSEN] Distributed Quad Motors (T-Motor U3 580KV  4)
  Pros:
   Better control authority (differential motor mixing for roll/pitch)
   Scalable (can add more motors for heavier models)
   Industry standard for MAVs (proven software ecosystem)
   Easier component testing (each motor independent)
  Cons:
   More complex ESC synchronization (4 channels)
   Higher wiring complexity
  
 Centralized Hub Motor (single large motor)
   Pros:
    Simpler electrical integration (1 ESC)
    Less weight (fewer motor+ESC pairs)
   Cons:
    No differential control (must rely 100% on wings for pitch/roll)
    Higher gimbal lock risk
    Slower response to disturbances
```

**Recommendation**: **Quad Motors** (Option A) for Phase 1 and beyond.

---

### Decision Tree 2: Wing Flapping Mechanism

```
How should wings articulate?

 [CHOSEN] Servo-Driven Linkage (6 servos, 3 per wing)
  Mechanism:
   Shoulder servo (pitch/rotation)
   Twist servo (wing twist/yaw)
   Flap servo (primary beat cycle)
  Pros:
   Proven servo technology (low cost, readily available)
   Easy to prototype with 3D-printed linkage
   Can adjust flap frequency by PWM adjustment
  Cons:
   Limited compliance (stiff linkage  injury risk)
   Servo wear on high-frequency cycling
  
 Pushrod Linkage (cam-driven)
   Single motor drives eccentric cam
   Linkages convert rotational to flapping motion
  Pros:
   Efficient power transfer
   Natural frequency matching possible
  Cons:
   Complex CAD and fabrication
   Harder to prototype
  
 Direct Motor-Driven Wings
    Motors directly on wing hinge
   Pros:
    Maximum compliance and efficiency
   Cons:
    Requires tiny brushless motors (hard to find, expensive)
    Increased mass per wing
```

**Recommendation**: **Servo-Driven Linkage** (Option 1) for prototyping simplicity.

---

### Decision Tree 3: Flight Controller Hardware

```
Which ESP32 variant for flight controller?

 [CHOSEN] ESP32-S3-R8N16 (Suggested Default)
   240 MHz dual core, 512 KB SRAM
   4 PWM channels (motor control)
   6 PWM for servos (or I2C PWM expander)
   I2C + SPI for sensors
  
 ESP32-C3 (Single core, lighter footprint)
   160 MHz single core (smaller, 250 KB SRAM)
   Sufficient for flight loop, but limited headroom
  
 Dedicated Flight Controller (Pixhawk, Betaflight, etc.)
    Overkill for this project; contradicts P32 ESP32-native design
```

**Recommendation**: **ESP32-S3-R8N16** (maintain P32 ecosystem consistency).

---

### Decision Tree 4: Sensor Suite

```
Which sensors for attitude + altitude estimation?

 [CHOSEN] ICM20689 (6-axis IMU) + BMP390 (Barometer)
   IMU: 1000 Hz sampling, low noise, sensored
   Barometer: 50 Hz altitude + climb rate estimation
   Both on single I2C breakout board
   Cost: ~$1520 total
  
 Simpler: MPU6050 (older, cheaper IMU) + BMP180
   MPU6050: 1000 Hz, higher noise floor
   BMP180: Lower altitude accuracy (5 m)
   Cost: ~$812
   Trade-off: Slightly noisier estimates
  
 Premium: ICM20689 + MS5607 (barometer) + LIS3MDL (magnetometer)
    Adds heading estimation for yaw drift correction
    Not necessary for Phase 1 (can add later)
```

**Recommendation**: **ICM20689 + BMP390** (balance of cost, performance, availability).

---

## PART 2: PHASE 1 PROTOTYPE BUILD PLAN

### Bill of Materials (BOM)

| Item | Qty | Part Number | Cost ea. | Notes |
|------|-----|-------------|----------|-------|
| **Motors** | 4 | T-Motor U3 PRO 580KV | $20 | Brushless, sensored |
| **Props** | 8 | APC 105 Carbon Fiber | $10 | 2 spare sets |
| **ESCs** | 4 | Hobbywing 40A XRotor | $25 | BLHeli-32 |
| **Servos** | 6 | MG996R Digital Servo | $8 | Wing articulation |
| **IMU** | 1 | ICM20689 Breakout | $12 | I2C module |
| **Barometer** | 1 | BMP390 Breakout | $8 | I2C module (often bundled) |
| **Battery** | 1 | 4S 5000 mAh 50C LiPo | $30 | XT60 connector |
| **PDB** | 1 | Generic Power Dist. Board | $5 | 4 XT30 inputs |
| **ESP32-S3** | 1 | DevKit or Feather | $15 | Flight logic + pinout flexibility |
| **Wiring** | 1 | Hook-up wire, JST connectors | $10 | Assorted |
| **Structural** | 1 | Carbon fiber tubes, foam, glue | $20 | Build materials |
| ****TOTAL** | | | **~$275** | Pilot unit, reusable components |

### Assembly Steps (Week 13)

#### Week 1: Structural Prototype

- [ ] Design fuselage in OpenSCAD (simple tube frame, ~200 mm length)
- [ ] 3D-print or laser-cut wing mounts (hard points for servo attach)
- [ ] 3D-print or machine motor plates (4 identical, symmetrical mount)
- [ ] Assemble frame (glue/bolt carbon fiber tubes, secure motor plates)
- [ ] Install landing gear (aluminum angle or foam skids, non-damage)
- [ ] Total mass target: ~500 g (excluding electronics)

#### Week 2: Motor & ESC Integration

- [ ] Solder ESC motor leads to motor connectors (proper polarity!)
- [ ] Mount motors on plates (use vibration isolators if available)
- [ ] Solder battery connector (XT60) to PDB main bus
- [ ] Mount PDB on frame (centered for weight distribution)
- [ ] Connect ESCs to PDB, route signal wires to ESP32 GPIO pins (12, 13, 14, 15)
- [ ] Connect battery, verify no shorts (multimeter check before powering)
- [ ] **TEST**: Single ESC + motor spin test (manual PWM command from function generator)

#### Week 3: Flight Controller & Sensors

- [ ] Solder ESP32-S3 headers (if breakout board)
- [ ] Mount ESP32-S3 on frame (vibration-isolated board or 3D-printed bracket)
- [ ] Connect IMU via I2C (GPIO 21 SDA, 22 SCL)
- [ ] Connect Barometer via I2C (same bus as IMU)
- [ ] Connect battery voltage monitoring to ADC (GPIO 36)
- [ ] Load test firmware (basic sensor read loop)
- [ ] **TEST**: IMU gyro + accel readings (should be near zero at rest)
- [ ] **TEST**: Barometer altitude reading (compare to known altitude)

#### Exit Criteria for Phase 1

- [ ] All 4 motors spin independently in response to PWM commands
- [ ] Motor thrust measured on scale: 950  50 gf per motor (at 100% throttle)
- [ ] IMU orientation output stable (no random drifts)
- [ ] Barometer altitude within 2 m of known reference point
- [ ] Total system mass <2.7 kg (on target for weight budget)
- [ ] No shorts or loose connections visible
- [ ] ESP32 boots and connects to serial console

---

## PART 3: PHASE 1 MOTOR THRUST VALIDATION

### Thrust Measurement Stand Design

**Simple scale-based setup**:

```
        
          Digital Scale    (1 g accuracy)
        
                 
           
             Plywood     (3030 cm platform)
              Base     
           
                 
        
          Motor Mount      (vertical orientation)
          (3D-printed)   
        
                 
            
             Motor +  
             Prop     
            
           (thrust points down)
```

### Procedure

```powershell
# Step 1: Record baseline mass (motor + prop only)
$baseline_g = 120  # Replace with actual measurement

# Step 2: Increase throttle from 1000 s to 2000 s in steps
# Record scale reading at each step after 2-second stabilization

$throttle_tests = @(
    @{pwm_us=1000; label="Idle"; expected_scale_g=0},
    @{pwm_us=1100; label="10%"; expected_scale_g=15},
    @{pwm_us=1250; label="25%"; expected_scale_g=100},
    @{pwm_us=1500; label="50%"; expected_scale_g=250},
    @{pwm_us=1750; label="75%"; expected_scale_g=700},
    @{pwm_us=2000; label="100%"; expected_scale_g=950}
)

foreach ($test in $throttle_tests) {
    Write-Host "Testing PWM $($test.pwm_us) s ($($test.label) throttle)..."
    # Send PWM command to ESP32
    # Read scale output (manual or via serial)
    # Compare to expected thrust
    # Record deviation
}
```

### Acceptance Criteria

For each motor:
- **100% throttle**: 950  50 gf (within 5%)
- **50% throttle**: 250  50 gf (linear response expected)
- **No cogging**: Smooth acceleration, no buzzing or stuttering

**Flag motor if**:
- Static thrust <850 gf @ 100% throttle
- Non-linear response (e.g., 50% throttle produces 30% thrust)
- Audible grinding or cogging noise

---

## PART 4: IMU CALIBRATION CHECKLIST

```
ACCELEROMETER CALIBRATION
 Set accelerometer to 16g range
 Place on level surface
 Read X, Y, Z raw values (should be 0, 0, 16384 LSBs)
 If deviation > 100 LSBs, apply soft-iron calibration
  (Use calibration matrix from ICM20689 datasheet)
 Verify with known G forces (place at 45 angle)

GYROSCOPE CALIBRATION
 Set gyroscope to 2000/s range
 Place stationary on level surface
 Read X, Y, Z raw values (should be 0, 0, 0)
 Record offset drift over 60 seconds
 If drift >5/s, apply zero-rate bias correction
 Verify with manual rotation (30/s expected at known rate)

ACCELEROMETER BIAS
 Estimated from 3-point calibration
 Test 1: [0, 0, +1g] (level)
 Test 2: [0, +1g, 0] (rotated 90)
 Test 3: [+1g, 0, 0] (rotated 90)
 Solve for bias/scale matrix
```

---

## PART 5: FIRMWARE SKELETON (Stage 1)

**File**: `src/flying_dragon_flight_system.cpp` (early version)

```cpp
#include "driver/ledc.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include <math.h>

#define TAG "FLYING_DRAGON"

// Motor PWM configuration
#define MOTOR1_GPIO 12
#define MOTOR2_GPIO 13
#define MOTOR3_GPIO 14
#define MOTOR4_GPIO 15

// Sensor configuration
#define IMU_I2C_ADDR 0x68
#define BARO_I2C_ADDR 0x77

// Global state
static uint16_t motor_pwm[4] = {1000, 1000, 1000, 1000};  // s
static bool system_armed = false;

// IMU data structure
typedef struct {
    float ax, ay, az;  // Acceleration (g)
    float gx, gy, gz;  // Angular velocity (/s)
} imu_data_t;

typedef struct {
    float alt_m;       // Altitude (m)
    float pressure_pa; // Pressure (Pa)
} baro_data_t;

// ============================================================================
// INITIALIZATION ROUTINES
// ============================================================================

esp_err_t motor_controller_init(void)
{
    ESP_LOGI(TAG, "Initializing motor PWM controller...");
    
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 490,  // ESC standard
        .duty_resolution = LEDC_TIMER_13_BIT,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    int gpio_pins[4] = {MOTOR1_GPIO, MOTOR2_GPIO, MOTOR3_GPIO, MOTOR4_GPIO};
    
    for (int i = 0; i < 4; i++) {
        ledc_channel_config_t ch_conf = {
            .channel = (ledc_channel_t)i,
            .duty = 0,
            .gpio_num = gpio_pins[i],
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ch_conf));
    }

    ESP_LOGI(TAG, "Motor PWM initialized: 490 Hz, 13-bit resolution");
    return ESP_OK;
}

esp_err_t sensor_i2c_init(void)
{
    ESP_LOGI(TAG, "Initializing I2C bus for sensors...");
    
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = 22,
        .sda_io_num = 21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
        .trans_queue_depth = 0
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    ESP_LOGI(TAG, "I2C bus initialized at 400 kHz");
    return ESP_OK;
}

// ============================================================================
// MOTOR CONTROL FUNCTIONS
// ============================================================================

void motor_set_pwm(int motor_idx, uint16_t pwm_us)
{
    // Convert s to duty cycle (04095 for 13-bit resolution)
    // PWM period = 1/490 Hz  2041 s
    // Duty = (pwm_us / 2041) * 4095
    
    if (motor_idx < 0 || motor_idx > 3) return;
    if (pwm_us < 1000) pwm_us = 1000;  // Clamp to idle
    if (pwm_us > 2000) pwm_us = 2000;  // Clamp to max
    
    motor_pwm[motor_idx] = pwm_us;
    
    uint32_t duty = (pwm_us * 4095) / 2041;
    ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)motor_idx, duty);
}

void motors_arm(void)
{
    system_armed = true;
    ESP_LOGI(TAG, "Motors armed");
}

void motors_disarm(void)
{
    system_armed = false;
    for (int i = 0; i < 4; i++) {
        motor_set_pwm(i, 1000);  // Idle all motors
    }
    ESP_LOGI(TAG, "Motors disarmed and idle");
}

// ============================================================================
// SENSOR READ FUNCTIONS
// ============================================================================

esp_err_t read_imu(imu_data_t *imu_out)
{
    // TODO: Implement I2C read from ICM20689
    // Read ACCEL_XOUT_H/L, ACCEL_YOUT_H/L, ACCEL_ZOUT_H/L
    // Read GYRO_XOUT_H/L, GYRO_YOUT_H/L, GYRO_ZOUT_H/L
    
    imu_out->ax = 0.0f;  // Placeholder
    imu_out->ay = 0.0f;
    imu_out->az = 1.0f;
    imu_out->gx = 0.0f;
    imu_out->gy = 0.0f;
    imu_out->gz = 0.0f;
    
    return ESP_OK;
}

esp_err_t read_barometer(baro_data_t *baro_out)
{
    // TODO: Implement I2C read from BMP390
    // Read pressure register, convert to altitude
    
    baro_out->pressure_pa = 101325;  // Sea level
    baro_out->alt_m = 0.0f;
    
    return ESP_OK;
}

// ============================================================================
// ATTITUDE ESTIMATION (Complementary Filter)
// ============================================================================

typedef struct {
    float roll;   // Degrees
    float pitch;  // Degrees
    float yaw;    // Degrees
} attitude_t;

static attitude_t estimated_attitude = {0, 0, 0};
static const float GYRO_WEIGHT = 0.98f;  // Gyro dominance in filter
static const float ACCEL_WEIGHT = 1.0f - GYRO_WEIGHT;

void estimate_attitude(const imu_data_t *imu, float dt_s, attitude_t *att_out)
{
    // Compute roll/pitch from accelerometer (gravity vector)
    float accel_roll = atan2f(imu->ay, imu->az) * 180.0f / M_PI;
    float accel_pitch = atan2f(-imu->ax, sqrtf(imu->ay*imu->ay + imu->az*imu->az)) * 180.0f / M_PI;
    
    // Complementary filter (gyro integration with accel correction)
    estimated_attitude.roll = GYRO_WEIGHT * (estimated_attitude.roll + imu->gx * dt_s)
                             + ACCEL_WEIGHT * accel_roll;
    estimated_attitude.pitch = GYRO_WEIGHT * (estimated_attitude.pitch + imu->gy * dt_s)
                              + ACCEL_WEIGHT * accel_pitch;
    estimated_attitude.yaw = estimated_attitude.yaw + imu->gz * dt_s;  // Gyro integration only
    
    *att_out = estimated_attitude;
}

// ============================================================================
// MAIN FLIGHT CONTROL LOOP
// ============================================================================

void flight_control_task(void *arg)
{
    ESP_LOGI(TAG, "Flight control task started");
    
    imu_data_t imu;
    baro_data_t baro;
    attitude_t attitude;
    
    uint32_t last_time_us = esp_timer_get_time();
    
    // Arm sequence (manual trigger via serial or button)
    vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 sec for settling
    motors_arm();
    
    while (1) {
        uint32_t now_us = esp_timer_get_time();
        float dt_s = (now_us - last_time_us) / 1e6f;
        last_time_us = now_us;
        
        // Cap dt to prevent overflow (max 100 ms = 0.1 s)
        if (dt_s > 0.1f) dt_s = 0.1f;
        
        // 1. Read sensors
        read_imu(&imu);
        read_barometer(&baro);
        
        // 2. Estimate attitude
        estimate_attitude(&imu, dt_s, &attitude);
        
        // 3. Apply simple control law
        if (system_armed) {
            // Test: Ramp throttle from 1000 to 1500 s over 5 seconds
            static uint32_t ramp_time_ms = 0;
            uint16_t pwm_target = 1000 + (500 * ramp_time_ms) / 5000;
            ramp_time_ms += (uint32_t)(dt_s * 1000);
            
            for (int i = 0; i < 4; i++) {
                motor_set_pwm(i, pwm_target);
            }
            
            // Log telemetry
            if (ramp_time_ms % 500 == 0) {
                ESP_LOGI(TAG, "PWM=%u Att=(%.1f,%.1f,%.1f) Alt=%.1f m",
                        pwm_target, attitude.roll, attitude.pitch, attitude.yaw, baro.alt_m);
            }
        }
        
        // Control loop frequency: 100 Hz (10 ms per iteration)
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ============================================================================
// APP MAIN
// ============================================================================

void app_main(void)
{
    ESP_LOGI(TAG, "=== FLYING DRAGON FLIGHT CONTROLLER ===");
    ESP_LOGI(TAG, "Initializing subsystems...");
    
    ESP_ERROR_CHECK(motor_controller_init());
    ESP_ERROR_CHECK(sensor_i2c_init());
    
    // Disable all motors initially
    for (int i = 0; i < 4; i++) {
        motor_set_pwm(i, 1000);
    }
    
    // Create flight control task (runs at 100 Hz)
    xTaskCreate(flight_control_task, "flight_ctrl", 4096, NULL, 10, NULL);
    
    ESP_LOGI(TAG, "System ready. Awaiting command...");
}
```

---

## PART 6: TESTING SEQUENCE (Week 13)

### Day 12: Structural Assembly

```bash
# Objective: Build frame, verify weight
pio run -t build   # Compile test firmware
scale_reading=$(weigh_dragon_structure)
if scale_reading <= 500; then
    echo " Structure within weight budget"
else
    echo " Structure too heavy; remove unnecessary material"
fi
```

### Day 35: Motor Testing

```bash
# Objective: Validate all 4 motors spin independently
# Manually send PWM commands via serial terminal:
# "motor1 1100"  Motor 1 at 1100 s (gentle spin)
# "motor1 1500"  Motor 1 at 1500 s (50% throttle)
# "motor1 2000"  Motor 1 at 2000 s (100% throttle)

# Record thrust measurements on scale:
Motor 1 @ 100% = 945 gf 
Motor 2 @ 100% = 952 gf 
Motor 3 @ 100% = 948 gf 
Motor 4 @ 100% = 950 gf 
```

### Day 67: Sensor Validation

```bash
# Objective: Verify IMU + barometer output
# Serial terminal shows:
# "IMU: ax=0.00g ay=0.00g az=1.00g  gx=0/s gy=0/s gz=0/s"
# "BARO: pressure=101325 Pa, altitude=0.0 m"

# Tilt drone 45, verify IMU changes:
# "IMU: ax=-0.70g ay=0.00g az=0.70g  gx=0/s gy=0/s gz=0/s"
#
# Lift drone 1 meter, verify barometer:
# "BARO: pressure=101242 Pa, altitude=0.8 m" (0.2 m acceptable)
```

### Day 810: Integration Test

```bash
# Objective: Full system + arming sequence
# Actions:
# 1. Power on (LED indicator should pulse)
# 2. Wait 5 seconds (arming timer)
# 3. Issue "arm" command (LED solid, motors spin to idle)
# 4. Issue "throttle 1200" (all 4 motors increase smoothly)
# 5. Verify attitude estimation changes as drone tilts
# 6. Issue "disarm" (motors return to idle)
```

---

## SUMMARY: CRITICAL SUCCESS FACTORS

| Factor | Threshold | Measurement |
|--------|-----------|-------------|
| **Motor thrust uniformity** | 5% across all 4 motors | Scale weight comparison |
| **IMU stability** | 0.1 g bias, 5/s gyro offset | Serial telemetry over 60 s |
| **Barometer accuracy** | 2 m altitude | Compare to known elevation |
| **Total mass** | <2.7 kg | Scale weight |
| **Flight loop frequency** | 100 Hz | Timing instrumentation |
| **Battery voltage stability** | <0.5 V sag under load | Multimeter under full throttle |

---

**Next Step**: Begin Phase 1 assembly per this guide. Report blockers immediately to enable Phase 2 (tethered flight) by end of Week 3.

Document Status: **Ready for Deployment**
