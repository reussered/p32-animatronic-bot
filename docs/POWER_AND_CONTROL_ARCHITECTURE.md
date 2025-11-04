# Power & Control Architecture for P32 Animatronic System

**Date**: November 4, 2025  
**Scope**: Complete electrical system design for multi-creature animatronic network  
**Key Assumptions**:
- ESP32 boards control via relays/logic signals (NOT direct motor drive)
- Multiple ESP32 chips per creature (distributed control)
- 20-30A @ 5V power budget per creature
- Servo motors: 6V standard (but 5V operation acceptable)

---

## PART 1: POWER BUDGET ANALYSIS

### Single Goblin (22 Servos - Config B Standard)

#### Current Draw Breakdown

| Servo Type | Qty | Idle (mA) | Running (mA) | Stall (mA) |
|-----------|-----|----------|-------------|-----------|
| LP (6 total) | 6 | 120 | 360 | 900 |
| MP (8 total) | 8 | 640 | 640 | 2,400 |
| HP (7 total) | 7 | 700 | 700 | 4,200 |
| **TOTAL** | **22** | **1,460** | **1,700** | **7,500** |

#### Realistic Operating Scenarios

```
SCENARIO 1: Idle (breathing, occasional blink)
  - All servos holding position or moving <10% range
  - Current draw: ~1.5A @ 5V = 7.5W
  - Duration: 80% of uptime
  
SCENARIO 2: Active animation (gesture sequence)
  - 50% of servos moving simultaneously
  - Average current: ~2.5A @ 5V = 12.5W
  - Duration: 15% of uptime
  
SCENARIO 3: Peak load (simultaneous arm reach + hand curl)
  - Multiple HP servos at stall simultaneously
  - Current draw: ~4-5A @ 5V = 20-25W
  - Duration: 5% of uptime (brief moments)

SCENARIO 4: WORST CASE (all 22 servos stall simultaneously)
  - Current draw: 7.5A @ 5V = 37.5W
  - Not realistic (not all servos stall at same time)
  - But use as upper bound for power supply sizing
```

#### Power Supply Recommendation for Single Goblin

```
For Config B (22 servos):
  Minimum: 5A @ 5V (for interactive demo mode)
  Recommended: 8A @ 5V (for continuous animation)
  Safe headroom: 10A @ 5V (accounts for simultaneous peaks)
  
For all 13 Creature Families (22 servos each):
  Minimum: 65A @ 5V
  Recommended: 104A @ 5V (8A × 13 creatures)
  Safe headroom: 130A @ 5V (10A × 13 creatures)
```

---

## PART 2: DISTRIBUTED CONTROL ARCHITECTURE

### Multi-ESP32 Approach (RECOMMENDED)

**Key Insight**: Each creature can use **multiple ESP32 boards** for distributed control:

```
Goblin Creature → 3 ESP32 Boards:

ESP32-HEAD (Controls head subsystem):
  ├─ 4× Eye servos (horizontal + vertical per eye)
  ├─ 2× Eyebrow servos
  ├─ 1× Mouth servo
  └─ Output: 7 PWM channels to servo controller
  
ESP32-ARMS (Controls arm subsystem):
  ├─ 4× Left arm servos (shoulder pitch/yaw, elbow, wrist)
  ├─ 4× Right arm servos (mirror)
  ├─ 2× Hand curl servos
  └─ Output: 10 PWM channels to servo controller
  
ESP32-BODY (Controls torso subsystem):
  ├─ 1× Torso lean servo
  ├─ 1× Torso twist servo
  ├─ 1× Breathing servo
  └─ Output: 3 PWM channels to servo controller

Communication:
  All 3 ESP32 boards connected via:
    - Shared I2C bus (for inter-board communication)
    - Single master ESP32 or central controller
    - Optional: ESP-NOW wireless mesh
```

### Benefits of Distributed Control

| Benefit | Impact |
|---------|--------|
| **Reduced PWM contention** | Each board has ample GPIO for servo control |
| **Parallel animation** | Head and arms can move independently/simultaneously |
| **Fault isolation** | If arm board fails, head still works |
| **Scalability** | Easy to add more boards for legs, torso details |
| **Reduced latency** | Local servo updates (no bus bottleneck) |
| **Modular design** | Test each subsystem independently |

---

## PART 3: ELECTRICAL CONTROL CIRCUIT

### Circuit Components (Per Goblin)

```
Power Stage (10A @ 5V):
├─ AC Mains (110-240V)
├─ Step-down transformer or switched PSU → 5V 10A
├─ Capacitor bank (electrolytic + ceramic for ripple control)
├─ Fuses (one per ESP32 board, one per servo controller)
└─ Linear regulator (for low-noise 3.3V to ESP32)

Control Stage:
├─ Master ESP32 (main controller)
│  ├─ Relay array (8 channel relay module, ~$5)
│  │  ├─ Relay 1: Power ESP32-HEAD
│  │  ├─ Relay 2: Power ESP32-ARMS
│  │  ├─ Relay 3: Power ESP32-BODY
│  │  ├─ Relay 4-8: Future expansion
│  │  └─ Control: GPIO pins D0-D7
│  │
│  ├─ I2C bus (SDA/SCL)
│  │  ├─ PCA9685 Servo Controller #1 (16 channels)
│  │  ├─ PCA9685 Servo Controller #2 (16 channels for overflow)
│  │  └─ Optional: Slave ESP32 boards via I2C
│  │
│  └─ Serial/UART
│     └─ Optional: Sensor feedback, debug logging
│
├─ Slave ESP32-HEAD
│  ├─ I2C connection to Master (SDA/SCL)
│  └─ PWM outputs: D12-D19 (8 channels)
│
├─ Slave ESP32-ARMS
│  ├─ I2C connection to Master
│  └─ PWM outputs: D12-D19 (8 channels)
│
└─ Slave ESP32-BODY
   ├─ I2C connection to Master
   └─ PWM outputs: D12-D15 (4 channels)

Servo Power Distribution:
└─ 5V 10A supply → Servo controller input
   └─ PCA9685 → 22 servo connectors
      └─ Each servo: Signal (PWM) + Power (5V) + Ground
```

### Relay Control Circuit (Per Board)

```
ESP32 GPIO D0 → Relay Control Input #1
                    ↓
              Opto-isolator (4N35)
                    ↓
              12V control voltage
                    ↓
              Relay coil (normally 12V DC)
                    ↓
              Relay switch (contacts: Common-Normally Open-Normally Closed)
                    ↓
              Contacts switch 5V servo power on/off
              
Advantage: ESP32 (3.3V logic) isolated from 5V servo power
           No backfeed through GPIO pins
           Safe simultaneous switching of multiple boards
```

### Wiring Diagram (Simplified)

```
MAINS (110V)
    ↓
[Transformer] → 5V 10A
    ↓
[Fuse] ← [Capacitor bank]
    ↓
[Linear Reg] ← [Filter]
    ↓
[3.3V to ESP32] [5V to Servo Controllers]
    ↓                    ↓
[Master ESP32]  ← [Relay Array]
  GPIO D0-D7             ↑
    ↓                    |
[I2C Bus: SDA/SCL]   [Relay coil power]
    ↓
[PCA9685 #1]
    ↓
[22 Servo Connectors]
```

---

## PART 4: POWER DISTRIBUTION FOR ALL CREATURES

### System-Level Architecture (All 13 Creatures)

```
Central Power Distribution:
├─ AC Mains (110-240V)
├─ Main Power Supply: 100A @ 5V (500W capacity)
│  └─ Switchable loads via main breakers
│
├─ Distribution Busbar
│  ├─ Positive rail (100A capacity)
│  ├─ Negative rail (100A capacity)
│  └─ Ground rail (safety)
│
├─ Creature #1 (Goblin) - 10A @ 5V
│  └─ Local fused power distribution
│
├─ Creature #2 (Bear) - 10A @ 5V
│  └─ Local fused power distribution
│
├─ Creature #3 (Elf) - 10A @ 5V
│  └─ Local fused power distribution
│
├─ [... Creatures #4-13 ...]
│
└─ Central Control Node
   ├─ Master ESP32 (orchestrates all creatures)
   ├─ I2C multiplexer (TCA9548A) for multi-bus control
   ├─ Ethernet/WiFi gateway (optional: remote monitoring)
   └─ UPS/Battery backup (5-10A for graceful shutdown)
```

### Per-Creature Power Budget

| Creature | Servos | Idle Current | Running Current | Peak Current |
|----------|--------|--------------|-----------------|--------------|
| Goblin | 22 | 1.5A | 2.5A | 5.0A |
| Bear | 20 | 1.4A | 2.3A | 4.5A |
| Elf | 22 | 1.5A | 2.5A | 5.0A |
| Orc | 20 | 1.4A | 2.3A | 4.5A |
| Cat | 18 | 1.3A | 2.2A | 4.0A |
| Wolf | 18 | 1.3A | 2.2A | 4.0A |
| Dragon | 24 | 1.7A | 2.8A | 5.5A |
| Android | 25 | 1.8A | 3.0A | 6.0A |
| Steampunk | 23 | 1.6A | 2.7A | 5.2A |
| Undead | 20 | 1.4A | 2.3A | 4.5A |
| Vampire | 22 | 1.5A | 2.5A | 5.0A |
| Humanoid | 22 | 1.5A | 2.5A | 5.0A |
| Zombie | 20 | 1.4A | 2.3A | 4.5A |
| **TOTAL** | **277** | **~19.3A** | **~33.0A** | **~65.7A** |

---

## PART 5: RELAY-BASED CONTROL SYSTEM

### Why Relays for ESP32-to-Motor Control?

**Problem**: ESP32 GPIO pins are 3.3V logic, 12mA per pin max
- Cannot directly drive servo motors (which need ~500mA-1600mA)
- Cannot safely handle inductive loads (servo startup spikes)

**Solution**: Use relays as electrically isolated switches
- ESP32 controls low-current relay coil (12mA)
- Relay contacts switch high-current servo power (1-5A)

### Relay Array Control Circuit

```
Master ESP32:
  D0 → Opto-isolator → Relay #1 (Head power on/off)
  D1 → Opto-isolator → Relay #2 (Arms power on/off)
  D2 → Opto-isolator → Relay #3 (Body power on/off)
  D3 → Opto-isolator → Relay #4 (Aux/future)
  D4 → Opto-isolator → Relay #5 (Aux/future)
  D5 → Opto-isolator → Relay #6 (Emergency shutdown)
  D6 → Opto-isolator → Relay #7 (UPS backup)
  D7 → Opto-isolator → Relay #8 (Power indicator)

Relay Coil Power: 12V (from separate, small supply or buck from 5V)
Relay Contact Power: 5V @ 10A (main servo power)
```

### Power Sequencing Strategy

```
Startup Sequence:
  1. Master ESP32 boots (powered by UPS or separate 3.3V supply)
  2. Wait 2 seconds (capacitors charge)
  3. GPIO D0 → HIGH (enable relay #1, power ESP32-HEAD)
  4. Wait 500ms (ESP32-HEAD boot)
  5. GPIO D1 → HIGH (enable relay #2, power ESP32-ARMS)
  6. Wait 500ms
  7. GPIO D2 → HIGH (enable relay #3, power ESP32-BODY)
  8. Broadcast I2C "status check" to all slave boards
  9. If all OK, set GPIO D8 → HIGH (green light indicator)
  
Shutdown Sequence:
  1. GPIO D6 → LOW (disable relay #6, remove main power)
  2. Wait 1 second
  3. GPIO D0, D1, D2 → LOW (disable all power relays)
  4. Master ESP32 enters sleep mode
  5. UPS keeps system minimal power for monitoring
```

### Safety Features

```
Emergency Stop Circuit:
  ├─ Physical pushbutton (N.O. - normally open)
  ├─ Connects to GPIO D5 (interrupt pin)
  ├─ Triggers ISR (interrupt service routine)
  ├─ ISR immediately sets GPIO D6 → LOW (relay off)
  └─ Result: All servo power cut within 10ms

Overvoltage Protection:
  ├─ Zener diode across servo power lines (5V1 Zener)
  ├─ Transient suppressors on each servo connector
  └─ Prevents backfeed from servo motors

Overcurrent Protection:
  ├─ Fuse per subsystem (main + 3 slave boards = 4 fuses)
  ├─ Main fuse: 15A (for 10A nominal)
  ├─ Slave fuses: 3A each (for slave ESP32 + local servos)
  └─ If fuse blows, that subsystem goes dark (fail-safe)

Thermal Protection:
  ├─ Temperature sensor on servo controller (optional)
  ├─ If temp > 70°C, reduce servo speed
  ├─ If temp > 85°C, disable that servo
  └─ Prevents burnout of servo motors
```

---

## PART 6: I2C BUS ARCHITECTURE FOR MULTI-ESP32

### Challenge: I2C Bus Limitations

Standard I2C:
- Supports up to 127 devices (but practical limit ~20 due to capacitance)
- Single bus requires all devices to share SDA/SCL lines
- If one device fails, entire bus can hang

### Solution: I2C Multiplexer (TCA9548A)

```
Master ESP32 (GPIO 21=SDA, 22=SCL):
  └─ I2C Multiplexer (TCA9548A)
     ├─ Channel 0: Slave ESP32-HEAD
     ├─ Channel 1: Slave ESP32-ARMS
     ├─ Channel 2: Slave ESP32-BODY
     ├─ Channel 3: Sensor array (future)
     ├─ Channel 4: Expansion
     ├─ Channel 5: Expansion
     ├─ Channel 6: Expansion
     └─ Channel 7: Expansion

Process:
  1. Master writes to TCA9548A: "Select channel 0"
  2. Master sends I2C command to ESP32-HEAD on channel 0
  3. Master writes to TCA9548A: "Select channel 1"
  4. Master sends I2C command to ESP32-ARMS on channel 1
  5. Repeat for each channel
```

### Alternative: Parallel I2C Buses

```
If TCA9548A multiplexer insufficient:

Master ESP32:
  ├─ I2C Bus #1 (GPIO 21/22):
  │  ├─ PCA9685 Servo Controller #1
  │  └─ Temperature/Humidity Sensor
  │
  ├─ I2C Bus #2 (GPIO 4/5 or Software I2C):
  │  ├─ PCA9685 Servo Controller #2
  │  └─ Expansion I/O
  │
  └─ I2C Bus #3 (GPIO 16/17 or Additional I2C):
     ├─ Slave ESP32-ARMS (optional dual-link)
     └─ Future expansion

Advantages:
  - No multiplexer bottleneck
  - Reduced contention
  - Parallel I2C transactions possible
```

---

## PART 7: SERVO CONTROLLER OPTIONS

### Option A: Centralized (Single PCA9685 x2)

```
Master ESP32 → I2C → PCA9685 #1 (16 channels) → Servos 1-16
                  → PCA9685 #2 (16 channels) → Servos 17-22 (+ spares)

Pros:
  ✓ Simple wiring (single I2C bus)
  ✓ Centralized servo timing
  ✓ Cost: ~$16 for two boards

Cons:
  ✗ Single point of failure (I2C bus)
  ✗ Limited servo response time (I2C communication delay)
  ✗ All servos must share same PWM frequency (50Hz only)
  ✗ Cascading I2C addresses complex (A0/A1/A2 pins)
```

**Cascading PCA9685 Addresses**:
```
PCA9685 boards can have different I2C addresses by changing A0/A1/A2 pins:

Board #1: A2=0, A1=0, A0=0 → Address 0x40
Board #2: A2=0, A1=0, A0=1 → Address 0x41
Board #3: A2=0, A1=1, A0=0 → Address 0x42
[... up to 8 boards with different A0/A1/A2 combinations ...]

This allows up to 8 PCA9685 boards on single I2C bus (128 servo channels)
```

### Option B: Distributed (Per-Board ESP32 PWM)

```
Master ESP32 → I2C → Slave ESP32-HEAD
                      ├─ GPIO D12-D19 (8 PWM pins) → Servos 1-8
                      └─ Local servo control logic
            → I2C → Slave ESP32-ARMS
                      ├─ GPIO D12-D19 (8 PWM pins) → Servos 9-16
                      └─ Local servo control logic
            → I2C → Slave ESP32-BODY
                      ├─ GPIO D12-D15 (4 PWM pins) → Servos 17-20
                      └─ Local servo control logic

Pros:
  ✓ Each subsystem has dedicated servo controller
  ✓ Faster response (no I2C to servo controller)
  ✓ Modular (test each board independently)
  ✓ Fault tolerance (if one board fails, others work)
  ✓ Scalable (add more boards for more servos)

Cons:
  ✗ More complex wiring (each board needs power)
  ✗ Need to load servo control firmware on 3 boards
  ✗ Cost: 3× ESP32 boards (~$15-30 each)
```

### Recommendation

**For Goblin (22 servos)**: Use **Option B (Distributed)** with 3 slave ESP32 boards
- Cost: 4 ESP32s × $8 = $32 (vs $50+ for PCA9685 cascading approach)
- Reliability: Higher (no I2C bottleneck)
- Scalability: Better (add boards as needed)
- Flexibility: Each board runs independent servo control logic

**For all 13 creatures**: Multiply by 13, 3 boards/creature
- Total: 39 slave ESP32 boards + 1 master = 40 boards × $8 = $320 (reasonable)

---

## PART 8: WIRING SPECIFICATION

### Main Power Bus

```
Gauge 8 AWG (3.3mm²) for main 100A bus:
├─ +5V red wire (runs entire length of display)
├─ GND black wire (runs entire length)
├─ GND green/bare wire (safety ground)
└─ All runs in conduit to prevent damage

Branch to Each Creature:
├─ Gauge 10 AWG (5mm²) for 10A creature branch
├─ +5V branch to local fuse (15A automotive fuse)
├─ GND branch to local star point
└─ Connected via Molex connectors (tool-free disconnect)
```

### I2C Communication Wires

```
SDA (Serial Data):
├─ Gauge 18 AWG (0.82mm²) twisted pair with GND
├─ 4.7kΩ pull-up to +3.3V (on master, not on slaves)
└─ Runs from Master ESP32 GPIO 21 to all slave boards

SCL (Serial Clock):
├─ Gauge 18 AWG (0.82mm²) twisted pair with GND
├─ 4.7kΩ pull-up to +3.3V (on master, not on slaves)
└─ Runs from Master ESP32 GPIO 22 to all slave boards

Best Practices:
├─ Use shielded twisted pair cable (STP)
├─ Shield connected to GND at master only
├─ Cable runs separate from power lines (prevent EMI)
└─ Max cable length: 5 meters (recommended, can extend with buffers)
```

### Servo Signal Wires

```
PWM Signal from ESP32:
├─ Gauge 24 AWG (0.2mm²) individual wires
├─ One per servo, runs from GPIO pin to servo signal connector
├─ Can run alongside power wires in same bundle
└─ Servo signal is low current (~5mA, 3.3V logic compatible)

Servo Power Wires (Per Servo):
├─ Gauge 22 AWG (0.34mm²) for 5V servo power
├─ Gauge 22 AWG for GND return
├─ Uses standard servo connector (JR or Futaba style)
└─ Daisy-chain servos (power all connected in parallel to +5V/GND bus)

Servo Power Header Block:
├─ Single point where all servo +5V wires connect
├─ Single point where all servo GND wires connect
├─ From PCA9685 or servo controller
├─ Use power distribution board (3-point terminal block or custom PCB)
└─ Allows easy testing/replacement of individual servos
```

---

## PART 9: COMPONENT LIST & COST

### Per-Goblin BOM (Configuration B - 22 servos)

| Item | Qty | Unit Cost | Total | Notes |
|------|-----|-----------|-------|-------|
| **Servo Motors** | | | | |
| LP servos (eye, eyebrow, thumb) | 6 | $4 | $24 | SG90 or equivalent |
| MP servos (mouth, wrist) | 8 | $5 | $40 | Slightly better torque |
| HP servos (arms, hand curl) | 7 | $6 | $42 | More power needed |
| | | **Subtotal** | **$106** | |
| **Control Electronics** | | | | |
| Master ESP32 | 1 | $8 | $8 | WROOM or similar |
| Slave ESP32 (head) | 1 | $8 | $8 | Duplicate board |
| Slave ESP32 (arms) | 1 | $8 | $8 | Duplicate board |
| Slave ESP32 (body) | 1 | $8 | $8 | Duplicate board |
| I2C Multiplexer (TCA9548A) | 1 | $3 | $3 | Optional (for expansion) |
| Relay module (8-channel) | 1 | $5 | $5 | 12V coil, 5V contacts |
| Servo connectors (JR style) | 30 | $0.20 | $6 | Pre-soldered headers |
| Wire (total) | - | - | $15 | 18/22/24 AWG assorted |
| Fuses & holders | - | - | $5 | 15A main, 3A branches |
| Capacitors, resistors | - | - | $5 | Filtering, pull-ups |
| | | **Subtotal** | **$71** | |
| **Power & Distribution** | | | | |
| 5V 8A buck converter (local) | 1 | $12 | $12 | Input 12-24V |
| 12V relay coil supply | 1 | $5 | $5 | Small buck for relays |
| Power connectors (Molex) | - | - | $8 | Daisy-chain power |
| | | **Subtotal** | **$25** | |
| **Mechanical & Misc** | | | | |
| 3D printed brackets | - | - | $15 | Servo mounting |
| Linkage rods, screws | - | - | $10 | Mechanical assembly |
| Heatsink (for servo controller) | 1 | $3 | $3 | Thermal management |
| | | **Subtotal** | **$28** | |
| **GRAND TOTAL** | | | **$230** | Per goblin animatronic |

### System-Level BOM (All 13 Creatures)

| Item | Qty | Unit Cost | Total |
|------|-----|-----------|-------|
| **Servo Motors** | 277 | $5 avg | $1,385 |
| **ESP32 Boards** | 52 | $8 | $416 |
| **Relay Modules** | 13 | $5 | $65 |
| **I2C Multiplexers** | 13 | $3 | $39 |
| **Wiring & Connectors** | - | - | $500 |
| **Central PSU** (100A @ 5V) | 1 | $200 | $200 |
| **Local PSU per creature** (8A @ 5V) | 13 | $12 | $156 |
| **Misc (fuses, caps, resistors, brackets)** | - | - | $600 |
| **TOTAL SYSTEM** | | | **$3,361** |

---

## PART 10: POWER SUPPLY SPECIFICATIONS

### Central Power Supply (for all 13 creatures)

```
Input: 110-240V AC (universal mains voltage)
Output: 5V DC @ 100A (500W)
  ├─ Ripple: < 1% (100mV peak-to-peak)
  ├─ Regulation: ±2% (4.9-5.1V under load)
  ├─ Efficiency: > 85%
  └─ Over-current protection: 120A breaker

Recommended Models:
  ├─ Mean Well RSP-500-5 ($150-180)
  ├─ EVGA 500W 80+ Modular PSU hacked to 5V ($80 + modification)
  └─ Custom multi-unit setup: 3× 30A @ 5V in parallel ($120-150)

Capacitor Bank (on PSU output):
  ├─ 4× 10,000 µF 10V electrolytic (low-ESR)
  ├─ 8× 0.1 µF ceramic (bypass, HF filtering)
  └─ Purpose: Absorb servo startup transients
```

### Local Power Supply (Per Creature)

```
Input: 12-24V (from central 100A bus, stepped down at creature)
Output: 5V DC @ 8A (40W per creature)
  ├─ Ripple: < 2% (100mV)
  ├─ Regulation: ±3%
  └─ Over-current: 10A fuse

Recommended:
  ├─ RECOM R-30.05 (10A @ 5V, compact, $12)
  ├─ Vicor BCM4135 (alternative, higher cost)
  └─ Meanwell NES-35-5 (small footprint, $15)

Capacitors (local):
  ├─ 2× 4,700 µF 10V low-ESR
  ├─ 4× 0.1 µF ceramic
  └─ 1× 1 µF tantalum (close to regulator output)
```

---

## PART 11: SAFETY & PROTECTION

### Protection Layers

```
Layer 1: Power Supply Level
  ├─ 120A circuit breaker (main AC line)
  ├─ TVS diode on 5V output (for transients)
  └─ Thermal shutdown if temp > 100°C

Layer 2: Creature Level
  ├─ 15A fuse on each creature +5V branch
  ├─ Zener diode (5V1) on servo power rail
  └─ Thermal breaker if temp > 70°C

Layer 3: Subsystem Level
  ├─ 3A fuse per ESP32 slave board
  ├─ Transient suppressor (TVS) on each servo connector
  └─ Series resistor (1Ω) on servo signal lines (dampen ringing)

Layer 4: Servo Level
  ├─ Each servo has internal mechanical stops
  ├─ Each servo has thermal protection (shuts down if hot)
  └─ Servo firmware limits max torque / current draw

Emergency Stop:
  ├─ Red pushbutton (normally open)
  ├─ Wired to Master ESP32 GPIO D5 (interrupt pin)
  ├─ Triggers immediate relay shutdown
  └─ Current cut within 10ms of button press
```

### Thermal Management

```
Servo Motor Thermal Loads:
  ├─ Each servo dissipates ~5-10W under continuous load
  ├─ 22 servos = 110-220W total thermal output
  ├─ In enclosed display case: temperature rise ~20-30°C

Solutions:
  ├─ Add ventilation holes in display enclosure
  ├─ Mount heat sink on servo controller board
  ├─ Use thermal paste between servos and mounting brackets
  ├─ Limit animation intensity during summer months
  └─ Optional: Add small 24V cooling fan (triggered by thermal sensor)

Temperature Monitoring:
  ├─ DS18B20 temperature sensor on servo bus
  ├─ Monitored by Master ESP32 every 5 seconds
  ├─ Alert if temp > 60°C, warning if > 70°C
  └─ Auto-throttle servos if > 75°C (reduce speed)
```

---

## PART 12: COMMISSIONING CHECKLIST

### Pre-Power Checklist

- [ ] All fuses installed correctly (15A main, 3A branches)
- [ ] No exposed wires or short-circuit hazards
- [ ] All connectors seated properly
- [ ] I2C pull-up resistors (4.7kΩ) installed on SDA/SCL
- [ ] Capacitor bank fully charged (1 minute with PSU disconnected)
- [ ] ESP32 firmware loaded and verified
- [ ] Relay module tested (coil lights when GPIO set HIGH)
- [ ] Emergency stop button tested (cuts power when pressed)

### Power-Up Sequence

1. [ ] PSU OFF, main breaker OFF
2. [ ] Connect AC mains to PSU
3. [ ] Turn main breaker ON (PSU should show green light)
4. [ ] Measure +5V output (should be 4.9-5.1V)
5. [ ] Measure +3.3V output (should be 3.2-3.4V)
6. [ ] Wait 30 seconds (capacitors stabilize)
7. [ ] Press reset button on Master ESP32
8. [ ] Watch for LED indicators (power, I2C, servo ready)
9. [ ] Check serial monitor for boot messages
10. [ ] Manually move each servo (command via serial)

### Functional Testing

- [ ] Eye servo #1 responds to command
- [ ] Eye servo #2 responds to command
- [ ] All 22 servos respond to individual commands
- [ ] Servo sweep test (all servos move smoothly 0°→90°→0°)
- [ ] Power draw monitoring (should match prediction)
- [ ] I2C bus communication verified
- [ ] Relay switching verified (power on/off cycles)
- [ ] Thermal stability test (run continuous animation for 10 minutes)

### Stress Testing

- [ ] Maximum current draw test (simultaneous stall of multiple servos)
- [ ] Rapid movement test (100 movements per second)
- [ ] Long-term endurance (24-hour continuous operation)
- [ ] Fail-safe test (unplug individual components, verify graceful degradation)

---

## CONCLUSION

**A complete, safe electrical system for all 13 creature animatronics requires:**

- **Central 100A @ 5V power supply** ($150-200)
- **Distributed ESP32 control** (52 boards × $8 = $416)
- **Relay-based switching** (isolated, safe, scalable)
- **Multiple protection layers** (fuses, TVS, thermal monitoring)
- **Total system cost**: ~$3,360

**Power budget realistic for 20-30A @ 5V**:
- Idle state: ~19A @ 5V = 95W
- Running state: ~33A @ 5V = 165W
- Peak state: ~65A @ 5V = 325W (brief, not sustained)

**Key advantage of distributed ESP32 approach**:
- No single point of failure
- Scalable (add more boards for more creatures)
- Modular testing (test each subsystem independently)
- Future-proof (easy to upgrade/repair individual boards)

