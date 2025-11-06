# Flying Dragons Bothome - Directory Tree

**Generated**: November 5, 2025  
**Status**:  Flattened & Organized

---

## Visual Directory Structure

```
F:\GitHub\p32-animatronic-bot\config\bots\bot_families\dragons\
(BOTHOME - Canonical Dragon Family Location)

  flying_dragon_full.json                 [2.1 KB] Master bot definition
    Defines: 2 subsystems (head, flight_controller), 11 total components
    Mass: 2560g | Performance: 15 min flight, 15 m/s max, 50 m ceiling

  README.md                               [25 KB] Integration guide & assembly
    Contents: 4-phase build procedures, mass budgets, assembly checklist

  INDEX.md                                [30 KB] JSON reference & specs
    Contents: Component definitions, motor mixing, part list, verification

  QUICK_REFERENCE.md                      [10 KB] Quick lookup card
    Contents: Specs, pin assignments, file locations, performance envelope

  BOTHOME_STRUCTURE.md                    [~15 KB] Organizational guide
    Contents: Detailed part folder organization, data flow, integration workflow

  REORGANIZATION_SUMMARY.md               [~12 KB] Migration documentation
    Contents: Before/after structure, migration steps, next phases

  head/                                   [Sensory/Display]
     dragon_nostril_left.json
     dragon_nostril_right.json
     [flying_dragon_left_eye config]     [To be organized]
     [flying_dragon_right_eye config]    [To be organized]
     [flying_dragon_mouth config]        [To be organized]
     [flying_dragon_speaker config]      [To be organized]

  wings/                                  [Wing Articulation]
     [flying_dragon_wing_servo_controller config]  [To be organized]

  body/                                   [Structural Components]
     body_segment/
        [body structure configs]

  motors/                                 [Propulsion System]
     [flying_dragon_motor_controller references]
    [Motor mixing & thrust tables]

  sensors/                                [Sensor Fusion & Perception]
     [flying_dragon_sensor_fusion references]
    Complementary filter logic
    Calibration parameters

  power/                                  [Battery & Safety]
     [flying_dragon_flight_safety references]
    Battery monitoring (6S LiPo: 25.2V  11V cutoff)
    Watchdog timer (500 ms)

  control/                                [Flight Logic & Behavior]
     [flying_dragon_behavior_sequencer references]
    8-state flight machine (IDLE, ARMING, HOVER, CLIMB, etc.)
    PID controller gains (100 Hz attitude loop)

  hardware_templates/                     [Procurement Reference - READ ONLY]
     T_MOTOR_U3_580KV.json               [Motor: 580 KV, 950 gf, 55g]
     HOBBYWING_40A_ESC.json              [ESC: 40A continuous, BLHeli-32]
     MG996R_SERVO.json                   [Servo: 11 kgcm, 50 Hz, 55g]
     ICM20689_BMP390.json                [IMU+Barometer: 1kHz/50Hz]

  legs/                                   [Empty - Future Variants]
    [Reserved for quadruped variants]

  feet/                                   [Empty - Future Variants]
    [Reserved for foot articulation]

  dragon_ancient_fire_001.json            [Variant: Fire Dragon]
  dragon_frost_guardian_001.json          [Variant: Frost Dragon]
  dragon_hatchling_001.json               [Variant: Baby Dragon]

  FLYING_DRAGONS_ORGANIZATION.md          [Previous org doc - reference]
```

---

## File Count & Size Summary

```
Directory Structure:

 Root level (Master & Docs):    6 files    ~100 KB
 head/                          2 files    ~0.5 KB  (+ 4 to organize)
 wings/                          files     KB    (to organize)
 body/                          1 subdir    KB    (populated)
 body_segment/                   files     KB    (structure folder)
 motors/                         files     KB    (to organize)
 sensors/                        files     KB    (to organize)
 power/                          files     KB    (to organize)
 control/                        files     KB    (to organize)
 hardware_templates/            4 files    ~4.4 KB (procurement ref)
 legs/                           files     KB    (empty - future)
 feet/                           files     KB    (empty - future)

 TOTAL:                         13+ files  ~105 KB
```

---

## Part-Based Organization Map

### **Head** (Sensory/Display)
```
head/
 Component: flying_dragon_left_eye<GC9A01>
    240240 IPS color display, SPI interface
 Component: flying_dragon_right_eye<GC9A01>
    240240 IPS color display, SPI interface
 Component: flying_dragon_mouth<GC9A01>
    240135 IPS display, SPI interface
 Component: flying_dragon_speaker<MINISPKR>
    Audio output, I2S interface
 Component: flying_dragon_nostril_left<RGB_LED>
    Status indicator, GPIO control
 Component: flying_dragon_nostril_right<RGB_LED>
     Status indicator, GPIO control
```

### **Wings** (Articulation)
```
wings/
 Component: flying_dragon_wing_servo_controller<MG996R_SERVO>
    Motor 1 (Front-Left wing): Servo 1, 2, 3 (3 DoF)
    Motor 2 (Front-Right wing): Servo 4, 5, 6 (3 DoF)
    Flap frequency: 3 Hz (independent from motor control)
    Phase locking: Synchronized flapping
    Servo specs: 11 kgcm torque, 50 Hz PWM, 55g each
```

### **Motors** (Propulsion)
```
motors/
 Component: flying_dragon_motor_controller<T_MOTOR_U3_580KV>
    Motor 1 (Front-Left): 580 KV, 950 gf hover thrust
    Motor 2 (Front-Right): 580 KV, 950 gf hover thrust
    Motor 3 (Back-Left): 580 KV, 950 gf hover thrust
    Motor 4 (Back-Right): 580 KV, 950 gf hover thrust
    Total hover thrust: 3800 gf (1.5 mass budget)
    ESCs: 4 Hobbywing 40A continuous
    Throttle at hover: 50%
    Motor mixing: Quadcopter X-frame
```

### **Sensors** (Fusion & Perception)
```
sensors/
 Component: flying_dragon_sensor_fusion<ICM20689_BMP390>
    IMU: ICM20689 (1000 Hz update rate)
       Accel: 16g, 3-axis
       Gyro: 2000/s, 3-axis
       Output: Roll, Pitch, Yaw, Accel_X/Y/Z
    Barometer: BMP390 (50 Hz update rate)
       Altitude: 1m accuracy
       Output: Height, vertical_velocity, pressure
    Fusion: Complementary filter (100 Hz loop)
```

### **Power** (Battery & Safety)
```
power/
 Component: flying_dragon_flight_safety<WATCHDOG_BATTERY_MONITOR>
    Battery monitoring:
       Cell count: 6S LiPo
       Max voltage: 25.2V (6  4.2V)
       Nominal: 22.2V (6  3.7V)
       Cutoff: 11V (emergency descent triggered)
    Watchdog timer:
        Period: 500 ms heartbeat
        Timeout action: Emergency descent (0.5 m/s down)
```

### **Control** (Flight Logic)
```
control/
 Component: flying_dragon_behavior_sequencer<ESP32_S3_FLIGHT_LOGIC>
    Flight states:
       IDLE: Zero throttle, system checks
       ARMING: Startup test, motor spin-up
       HOVER: Altitude hold, stable attitude
       CLIMB: Vertical ascent (max 2 m/s)
       CRUISE: Horizontal flight (5 m/s nominal)
       MANEUVER: Agile flight (up to 15 m/s)
       LANDING: Controlled descent (0.5 m/s)
       EMERGENCY_DESCENT: Failsafe descent (0.5 m/s)
    Control loops:
       Position loop: 1 Hz (outer, slow)
       Velocity loop: 10 Hz (middle)
       Attitude loop: 100 Hz (inner, fast)
       Wing flap: 3 Hz (independent async)
    Mood modulation:
        Happy: 2 wing flap, 1.2 max speed
        Alert: 1.5 wing flap, 1 speed
        Calm: 0.5 wing flap, 0.5 speed
        Fearful: 3 wing flap, emergency descent triggered
```

---

## Subsystem Architecture (from flying_dragon_full.json)

### **Subsystem 1: flying_dragon_head**
```
flying_dragon_head (Sensory/Display Subsystem)
 Component: flying_dragon_left_eye<GC9A01>
 Component: flying_dragon_right_eye<GC9A01>
 Component: flying_dragon_mouth<GC9A01>
 Component: flying_dragon_speaker<MINISPKR>
 Component: flying_dragon_nostril_left<RGB_LED>
 Component: flying_dragon_nostril_right<RGB_LED>

Purpose: Display expressions, play audio, provide status feedback
Timing: Eyes at 1 (high frequency), mouth/speaker at variable
Communication: Shared memory reads for mood/state
```

### **Subsystem 2: flying_dragon_flight_controller**
```
flying_dragon_flight_controller (Propulsion/Flight Subsystem)
 Component: flying_dragon_sensor_fusion<ICM20689_BMP390>
 Component: flying_dragon_flight_safety<WATCHDOG_BATTERY_MONITOR>
 Component: flying_dragon_motor_controller<T_MOTOR_U3_580KV>
 Component: flying_dragon_wing_servo_controller<MG996R_SERVO>
 Component: flying_dragon_behavior_sequencer<ESP32_S3_FLIGHT_LOGIC>

Purpose: Maintain stable flight, manage propulsion, execute flight behaviors
Timing: Sensors at 100 Hz, control at 100 Hz, wing flap at 3 Hz
Communication: Shared memory for flight telemetry, battery status, mood
```

---

## Hardware Component Location Map

### **Source of Truth: config/hardware/**
```
config/hardware/
 flying_dragon_motor_controller.json      [PRIMARY - 4 motors + ESCs]
 flying_dragon_wing_servo_controller.json [PRIMARY - 6 servos]
 flying_dragon_sensor_fusion.json         [PRIMARY - IMU + barometer]
 flying_dragon_flight_safety.json         [PRIMARY - Watchdog + battery]
 flying_dragon_behavior_sequencer.json    [PRIMARY - Flight logic]

These are the AUTHORITATIVE hardware specifications.
Used by component system during compilation.
```

### **Reference: bothome/hardware_templates/**
```
bothome/hardware_templates/
 T_MOTOR_U3_580KV.json                   [REFERENCE - Procurement specs]
 HOBBYWING_40A_ESC.json                  [REFERENCE - Procurement specs]
 MG996R_SERVO.json                       [REFERENCE - Procurement specs]
 ICM20689_BMP390.json                    [REFERENCE - Procurement specs]

These are READ-ONLY procurement reference documents.
Used for sourcing parts and validating specifications.
```

### **Subsystem Integration: config/components/subsystem/**
```
config/components/subsystem/
 flying_dragon_flight_system.json         [INTEGRATOR - Combines all 5 flight components]
     Ensures execution order: sensors  safety  motors  wings  behavior
     Validates component dependencies
     Single compilation unit for flight controller
```

---

## Data Flow Diagram

```

 BOTHOME: flying_dragon_full.json                                
 (Master Bot Definition - Composition Layer)                     
                                                                   
  Subsystem 1: flying_dragon_head                                
   6 display/audio/LED components for expression               
                                                                   
  Subsystem 2: flying_dragon_flight_controller                   
   5 propulsion/control components for flight                  

                            
                             References components defined in:
                              - Part folders (head/, wings/, etc.)
                              - config/hardware/ (hardware specs)
                              - config/components/ (behavior logic)
                            
                            

 CONFIG/HARDWARE/: Hardware Specifications (Primary)             
 (Source of Truth for All Hardware Definitions)                  
                                                                   
  flying_dragon_motor_controller.json                          
    4 T-Motor U3 580KV + 4 Hobbywing 40A ESCs             
                                                                  
  flying_dragon_wing_servo_controller.json                     
    6 MG996R servos (3 per wing, 3 DoF each)               
                                                                  
  flying_dragon_sensor_fusion.json                             
    1 ICM20689 (1kHz IMU) + 1 BMP390 (50Hz barometer)     
                                                                  
  flying_dragon_flight_safety.json                             
    Battery monitor (6S LiPo) + Watchdog timer (500ms)       
                                                                  
  flying_dragon_behavior_sequencer.json                        
     8-state flight machine, PID gains, mood modulation       

                            
                             Referenced by:
                              - Component system build
                              - Subsystem integration
                              - Runtime configuration
                            
                            

 CONFIG/COMPONENTS/SUBSYSTEM/: Integration Layer                 
                                                                   
  flying_dragon_flight_system.json                             
    Combines: sensor_fusion  safety  motor  wings  behavior
    Execution order: 100 Hz sensors, 100 Hz control, 3 Hz wing
    Single compilation unit for flight controller             

                            
                            
         
          FIRMWARE COMPILATION & FLASHING      
          (Via PlatformIO + ESP-IDF)          
                                               
          Target: ESP32-S3 Dev Board           
          Output: Binary firmware              
         
```

---

## Navigation Guide

### **Starting Points**

**"I want to assemble a flying dragon"**  START
```
flying_dragon_full.json
    
README.md (4-phase assembly guide)
    
QUICK_REFERENCE.md (pin assignments, specs)
    
Part folders (head/, wings/, motors/, etc.)
```

**"I need hardware specifications"**  START
```
hardware_templates/ (procurement reference)
    
flying_dragon_full.json (bot composition)
    
config/hardware/ (authoritative specs)
```

**"I want to modify flight behavior"**  START
```
flying_dragon_full.json (subsystem structure)
    
control/ folder (behavior/state machine)
    
config/hardware/flying_dragon_behavior_sequencer.json
    
config/components/subsystem/flying_dragon_flight_system.json
```

**"I'm creating a new dragon variant"**  START
```
flying_dragon_full.json (copy as template)
    
Create: dragon_{family}_{name}_{id}.json
    
Reference existing subsystems or create new ones
    
Place in bothome root with other variants
```

---

## Quick Statistics

| Metric | Value |
|--------|-------|
| **Total Part Folders** | 8 (head, wings, body, motors, sensors, power, control, legs, feet) |
| **Master Bot Definition** | 1 (flying_dragon_full.json) |
| **Documentation Files** | 4 (README, INDEX, QUICK_REFERENCE, BOTHOME_STRUCTURE) |
| **Hardware Templates** | 4 (T-Motor, ESC, Servo, IMU+Barometer) |
| **Subsystems** | 2 (head, flight_controller) |
| **Total Components** | 11 (6 head, 5 flight) |
| **Motors** | 4 (580 KV, 950 gf each) |
| **Servos** | 6 (11 kgcm, 55g each) |
| **ESCs** | 4 (40A continuous) |
| **Sensors** | 2 (1 IMU, 1 barometer) |
| **Total Mass** | 2560g |
| **Flight Time** | 5.8 min hover, 15 min max |
| **Max Speed** | 15 m/s |
| **Ceiling** | 50 m |

---

## Status Summary

 **COMPLETE**: Flattened structure  
 **COMPLETE**: Part-based organization  
 **COMPLETE**: Master definition & documentation at bothome root  
 **COMPLETE**: Hardware templates accessible  
 **COMPLETE**: Subsystem integration validated  
 **PENDING**: Component definition organization (next phase)  
 **PENDING**: Variant bot creation (ancient_fire, frost_guardian, etc.)  

---

Generated: November 5, 2025 | Effective immediately
