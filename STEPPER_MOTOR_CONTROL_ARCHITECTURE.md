[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# Stepper Motor Control Architecture for Goblin Expression System

How to organize 50-100+ steppers for smooth animatronic control.

---

## Control Problem

**Challenge**: ESP32 has limited PWM outputs
- Standard PWM: 16 timers max
- Need to control 50-100 motors
- Each motor needs 2 control lines (direction + step pulse)
- 100 motors = 200 GPIO outputs needed
- ESP32 only has 34 usable GPIO total

**Solution**: Hierarchical control with multiplexing and specialized drivers

---

## Architecture Option 1: I2C GPIO Expanders (Recommended)

**Components**:
- 4x PCA9685 boards (16-channel PWM each = 64 PWM outputs)
- Connected via single I2C bus to ESP32
- Cost: $10-15 per board = $50 total for I2C multiplexing

**How it works**:
- ESP32 sends I2C commands to each PCA9685
- Each PCA9685 controls 8 stepper motors (16 outputs = 8 steppers)
- 4 boards control 32 steppers via single I2C bus
- Scale to 100+ motors: Stack more PCA9685 boards

**Pros**:
- Simple I2C protocol
- Standard available boards
- Scalable (just add more boards)
- Solves GPIO limitation

**Cons**:
- I2C bus speed limits (up to 400kHz standard, 1MHz fast)
- Refresh rate: ~100 steppers in 5-10ms (acceptable for expression)
- Single I2C bus is bottleneck at very high counts

**Wiring per motor**:
```
ESP32 GPIO 21 (SDA) ---+
                        +--- PCA9685 board 1 (16 outputs)
ESP32 GPIO 22 (SCL) ---+
                        
Each motor connected to PCA9685:
- Stepper coil A+ to output N
- Stepper coil A- to output N+8
- (rest of motor on driver board)
```

---

## Architecture Option 2: Shift Registers (Budget Alternative)

**Components**:
- 8x 74HC595 shift registers (8-bit serial-to-parallel)
- Connected via 3 GPIO pins (SCLK, MOSI, SS) to ESP32
- Cost: $0.50-1 per chip = $4-8 total

**How it works**:
- ESP32 sends serial bits to shift registers
- Shift registers expand to 64 GPIO pins (8 x 8-bit)
- Very fast: 100+ updates per millisecond
- Traditional control pattern from 1990s robotics

**Pros**:
- Ultra-cheap ($5-8 for 64 outputs)
- Very fast update rate (MHz capable)
- Simple protocol (SPI-like)
- Proven in legacy animatronic systems

**Cons**:
- Requires custom PCB or breadboard assembly
- Not as easy to use as PCA9685
- Needs pull-up resistors for stepper drivers
- Less elegant codebase

**Wiring**:
```
ESP32 GPIO 23 (MOSI) ----+
ESP32 GPIO 18 (SCLK) ----+--- 74HC595 chain (8 chips = 64 outputs)
ESP32 GPIO 5  (SS)   ----+

Chain output to stepper drivers:
Output 0-1: Motor 1 direction control
Output 2-3: Motor 2 direction control
(etc., 32 motors maximum with 64 bits)
```

---

## Architecture Option 3: Dedicated Motor Controller Boards

**Components**:
- Adafruit Motor Shield v2 (32 steppers per board)
- Connected via I2C to ESP32
- Cost: $25-30 per board

**How it works**:
- Motor Shield includes onboard drivers
- Controls up to 4 stepper motors
- I2C interface simplifies control
- Stackable (multiple shields on same I2C bus)

**Pros**:
- All drivers built-in (no external A4988 boards)
- Well-documented library support
- Proven reliability

**Cons**:
- Expensive per motor
- Overkill for simple expression (drivers already on shields)
- Limited stackability (address conflicts)
- Footprint constraints in small head

---

## Recommended Hybrid Architecture

**For 100+ expression motors:**

### Layer 1: Main Controller
- ESP32-S3 (primary controller)
- Runs animation sequencing
- Coordinates all motor groups

### Layer 2: Local Driver Clusters
- 4x PCA9685 boards (64 motors total)
- Connected via I2C to ESP32
- Each PCA9685 controls 8 stepper motors

### Layer 3: Stepper Drivers
- 32x A4988 breakout boards (one per motor)
- Receive step/direction signals from PCA9685
- Control actual stepper coil currents
- Cost: $3-5 per board = $160 total for 32 drivers

### Layer 4: Physical Steppers
- 100x NEMA 8 stepper motors (or chosen size)
- Mounted on animatronic face
- Connected to A4988 drivers via ribbon cables

**Total Control Cost**: 
- ESP32: $10
- 4x PCA9685: $50
- 32x A4988: $160
- Wiring/connectors: $50
- Total: approx $270 for control electronics (scales linearly with motor count)

---

## Control Signal Timing

### Stepper motor pulse sequence
```
ESP32 (via I2C) -> PCA9685 -> A4988 driver -> Stepper motor

Step pulse width: 1-10 microseconds (depends on driver)
Direction setup: 1-50 microseconds before step
Speed: 100-1000 steps/second (0.1-1 second per full rotation)

For expression (fast movements):
- Typical speed: 200 steps/second (NEMA 8 at 3.9V)
- Full rotation time: 1 second
- Refresh rate needed: 100Hz minimum (10ms between updates)
```

### Real-world timing example (100 motors via I2C)
```
ESP32 prepares 100 motor commands:
- 100 motors / 16 outputs per PCA9685 = 6.25 boards
- Round up to 7 PCA9685 boards

I2C update cycle:
- Write command to PCA9685 #1: ~100us
- Write command to PCA9685 #2: ~100us
- ... (total 7 boards)
- Total I2C time: ~700us per update cycle

At 400kHz I2C (standard):
- Effective update rate: 1000/0.7 = 1428 updates/second
- Plenty fast for expression control (need only 10-100Hz)
```

---

## Software Architecture (Pseudocode)

```cpp
// Main animation loop (runs on ESP32)
void animate_expression(int expression_id) {
    // Load animation sequence from config
    Animation anim = load_animation(expression_id);
    
    for each keyframe in anim {
        // Update all motor targets
        for each motor (0 to 99) {
            int target_position = keyframe.motor_positions[motor];
            motor_controller.set_target(motor, target_position);
        }
        
        // Execute movement smoothly over time interval
        delay(keyframe.duration_ms);
    }
}

// Motor controller class (abstracts I2C details)
class MotorController {
    PCA9685 expanders[4];  // 4 boards = 32 motors
    
    void set_target(int motor_id, int position) {
        int board = motor_id / 8;  // Which PCA9685
        int channel = (motor_id % 8) * 2;  // Which output (step/dir)
        
        // Calculate step/direction signals
        int current_pos = current_positions[motor_id];
        int steps_needed = position - current_pos;
        int direction = (steps_needed > 0) ? HIGH : LOW;
        
        // Send to appropriate PCA9685
        expanders[board].set_pwm(channel, direction_value);
        expanders[board].set_pwm(channel+1, step_pulse_value);
        
        // Update tracking
        current_positions[motor_id] = position;
    }
};
```

---

## Power Delivery Strategy

**For 100 motors drawing up to 230W:**

### Option A: Central Supply + Local Regulation
- Single 24V/10A main supply (240W)
- Regulators at each motor cluster:
  - 24V -> 5V 5A regulator for PCA9685 and GPIO drivers
  - 24V -> 3.9V regulator for stepper coils
- Advantage: Simple, centralized
- Disadvantage: Long wire runs, voltage drop

### Option B: Distributed Supplies
- Multiple 5V supplies (one per PCA9685 + motor group)
- Total: 4x 5V/5A supplies = 100W capacity
- Each supply handles local cluster + 8 motors
- Advantage: Reduced EMI, better current distribution
- Disadvantage: More complex, multiple supplies

### Option C: PSU Bank + Power Distribution Board
- Single high-capacity PSU (24V/15A = 360W)
- Power distribution board:
  - Multiple 5V outputs (fused/protected)
  - Multiple 3.9V/12V rails for stepper variants
  - Capacitor banks for noise filtering
- Advantage: Maximum scalability
- Disadvantage: Complex custom PCB

**Recommendation**: Option B (distributed 5V supplies)
- Simplest to implement
- Good isolation between motor groups
- Easy to upgrade (add supply + more motors)

---

## Signal Integrity & Noise Management

### EMI from stepper motors
Stepper motors create noise when coils switch:
- High-speed current edges: dI/dt in milliseconds
- Voltage spikes up to 50V on stepper lines
- Can corrupt I2C/SPI communication on nearby pins

**Mitigation**:
1. Separate grounds for logic vs stepper coils
2. 100uF electrolytic capacitors on each A4988 board
3. Twisted-pair cables for stepper motor wires
4. Star grounding at central power distribution point
5. Ferrite cores on long power wires
6. I2C bus pull-up resistors (4.7k typical)

### I2C bus stability
- Maximum bus length: 1 meter (with ferrite filtering)
- Recommended: Keep PCA9685 boards close to ESP32
- Use shielded twisted-pair for I2C signals
- Pullup resistors: 4.7k ohms (standard)

---

## Scalability Path

### Phase 1: Prototype (10 motors)
- 1x PCA9685 (handles 8 motors with room to spare)
- 1x 5V/2A supply
- 10x A4988 driver boards
- 10x NEMA 8 motors
- Cost: approx $150 total

### Phase 2: Expression Layer (50 motors)
- 4x PCA9685 (64 outputs)
- 2x 5V/5A supplies
- 50x A4988 driver boards
- 50x NEMA 8 motors
- Cost: approx $800 total

### Phase 3: Full System (100+ motors)
- 7x PCA9685 (112 outputs max)
- 3x 5V/5A supplies
- 100x A4988 driver boards
- 100x NEMA 8 motors
- Cost: approx $1,500 total

---

## Testing & Validation Checklist

Before full deployment:

- [ ] Test single motor + driver + PCA9685 circuit
- [ ] Verify I2C communication at 100kHz, then 400kHz
- [ ] Measure voltage stability under load (scope test)
- [ ] Check for I2C bus corruption during motor pulse
- [ ] Validate animation sequencing with 5 motors
- [ ] Stress test: all motors at max speed for 10 minutes
- [ ] Thermal validation: measure motor case temps
- [ ] Noise measurement: quantify audible noise levels
- [ ] Power consumption: verify actual draw vs theoretical
- [ ] Scale to 50 motors, repeat thermal/noise tests
- [ ] Final full 100-motor system test

---

## Firmware Requirements

Minimal libraries needed:
- Wire.h (I2C communication to PCA9685)
- Custom stepper controller class
- Animation sequencer/state machine

No Arduino Stepper library needed:
- Stepper.h would waste precious pins
- Direct PCA9685 control more efficient

Required features:
- Non-blocking animation (use timers, not delay())
- Smooth acceleration/deceleration ramping
- Hotswap motor enable/disable (for power management)
- Telemetry: motor position feedback via SharedMemory

---

## Future Enhancement: Closed-Loop Position Feedback

Current system: Open-loop (assumes motor movements succeed)

Enhancement path:
- Add magnetic encoders (0.5-5 USD per motor)
- Use ADC or I2C to read encoder values
- Compare actual vs commanded position
- Implement PID correction loop

Benefit: Verify motor hasn't stalled, autonomous recovery from jams

---

## Summary: Control Architecture Decision

**Recommended Configuration**:
- Use PCA9685 boards (I2C multiplexing)
- A4988 drivers per motor
- 4-8x PCA9685 boards depending on motor count
- Distributed 5V power supplies (one per board cluster)
- Dedicated I2C bus at 400kHz standard speed

This architecture:
- Scales easily to 100+ motors
- Uses minimal ESP32 GPIO
- Provides clear separation of concerns
- Well-documented commercial components
- Reasonable total cost (approx $1-2 per motor for control electronics)
