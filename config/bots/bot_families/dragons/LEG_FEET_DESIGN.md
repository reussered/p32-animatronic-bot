# P32 Flying Dragon - Leg & Feet Assembly Design Document

**Date**: November 5, 2025  
**Status**:  Design Complete, Implementation Ready  
**Scope**: Retractable bipedal walking subsystem with Poppy-inspired biomechanics

---

## 1. Overview

The flying dragon now features **retractable legs** that enable ground locomotion (walking, running, jumping) while remaining completely stowed during flight operations. This hybrid aerial-terrestrial design maintains the original 2560g mass budget while adding ~250-400g for legs and feet (offset by quadcopter lift assist if needed).

### Key Capabilities
- **Walking**: Smooth sinusoidal gait, ~1.2 sec cycle time, ~150mm step length
- **Running**: High-frequency aggressive gait, ~600ms cycle, ~250mm steps
- **Jumping**: Explosive bipedal jumping, ~100-150mm height, ~300mm distance
- **Energy Seeking**: Autonomous ground movement at 75% battery depletion
- **Flight Integration**: Legs retract into body cavity, zero flight interference

---

## 2. Leg Subsystem Design

### 2.1 Architecture

**Bipedal Structure** (Poppy-inspired):
- 4 degrees of freedom per leg (hip_x, hip_y, knee_y, ankle_y)
- Symmetric left/right leg control for stability
- Servo actuation: MG996R servos (same as wings, reuses existing power budget)
- Structural material: Aluminum/composite frame, lightweight linkages

**Degrees of Freedom Per Leg**:
1. **Hip X** (abduction/adduction): 45 - lateral leg positioning
2. **Hip Y** (flexion/extension): 60 - forward/backward leg swing
3. **Knee Y** (extension/flexion): 0-120 - knee bend for gait
4. **Ankle Y** (dorsiflexion/plantarflexion): 45 - foot pitch for ground contact

### 2.2 Biomechanics (Poppy Project Reference)

The leg control algorithms implement **three proven bipedal gaits** from the Poppy humanoid project:

#### 2.2.1 Walking Gait
```
Cycle: 1.2 seconds @ nominal speed
Hip Motion: 40 sinusoidal oscillation (frequency: 0.833 Hz)
Knee Coupling: Counter-phase to hip (pendulum effect)
Ankle Behavior: Subtle dorsiflexion during stance for traction
Step Characteristics: 150mm length, 30mm height, smooth trajectory
Principle: Energy-efficient locomotion, low peak forces
```

**Mathematical Model**:
```
Left Hip Y:  L_hip_y = 40 * sin(2 * (phase - 0.0))
Right Hip Y: R_hip_y = 40 * sin(2 * (phase - 0.5))  // Opposite phase
Knee Y:      Knee = max(0, -hip_y * 0.6 + 20)         // Dampened inverse
```

#### 2.2.2 Running Gait
```
Cycle: 600 ms @ nominal speed (2 faster than walking)
Hip Motion: 55 amplitude (wider swing)
Knee Extension: More aggressive (0-90 range vs. 20-50 for walking)
Ankle Motion: Active dorsiflexion during swing phase
Step Characteristics: 250mm length, 50mm height, bouncy
Principle: High-speed locomotion with flight assist from quadcopter
```

#### 2.2.3 Jumping Gait
```
Single Jump Cycle: 400 ms
Crouch Phase (0-15%): Hip/knee flex to ~90 for energy storage
Extension Phase (15-30%): Explosive straightening (high power)
Flight Phase (30-60%): Legs relaxed, airborne
Landing Prep (60-100%): Re-engagement of crouch for next jump
Characteristics: Symmetric bipedal motion, coordinated landing
Jump Height: ~100-150mm (with quadcopter lift assist)
Jump Distance: ~300mm horizontal
```

### 2.3 Servo Control

**PWM Servo Commands** (MG996R standard):
- Neutral position: 1500 s
- Full range: 1000-2000 s
- Conversion: angle_deg = (pulse_width_us - 1500) / 5.55
- Update rate: 50 Hz (20 ms period)

**Smoothing Filter** (Poppy principle):
```
output = current +  * (target - current)
where  = dt / ( + dt)
 = 50 ms time constant
Effect: Smooth servo trajectories over ~150 ms, eliminating jerkiness
```

**Gait Mode Control**:
- Mode 0: Idle (stowed)
- Mode 1: Walking
- Mode 2: Running
- Mode 3: Jumping
- External API: `flying_dragon_leg_set_gait_mode(mode)`

### 2.4 Retraction Mechanism

**Deployment/Stowage** (2-second transitions):

**Fully Stowed Position** (for flight):
- Hip Y: -80
- Knee Y: 110 (maximum flexion)
- Ankle Y: -40
- Legs folded against body cavity

**Fully Deployed Position** (for ground locomotion):
- Hip Y: -60 to +60 (gait-dependent)
- Knee Y: 0-120 (gait-dependent)
- Ankle Y: -45 to +45 (gait-dependent)
- Legs extended for ground contact

**Retraction Logic**:
```
While deploying: retraction_progress += 0.5 * dt_ms / 2000  // 2 sec deploy
While retracting: retraction_progress -= 0.5 * dt_ms / 2000  // 2 sec retract

Target angles = deployed_angles * retraction_progress 
              + stowed_angles * (1 - retraction_progress)
```

**Flight Safety**:
- Legs automatically stow when entering flight controller
- Zero interference with quadcopter dynamics once stowed
- Prevents blade strikes, maintains CG during transitions

### 2.5 Mass Budget

| Component | Mass (g) |
|-----------|----------|
| Left leg servos (4) | 220 |
| Right leg servos (4) | 220 |
| Left leg structure | 80 |
| Right leg structure | 80 |
| Left foot | 50 |
| Right foot | 50 |
| **Total** | **700** |

**Allocation**: Original 2560g airframe + 700g legs = 3260g total  
**Offset**: Quadcopter can use increased T-Motor thrust or reduce battery mass slightly.

---

## 3. Feet Assembly Design

### 3.1 Passive Mechanical Structure

**Material**: Thermoplastic Elastomer (TPE) with foam insert
- Grip coefficient: ~0.6-0.8 (typical rubber-to-concrete)
- Cushion stiffness: ~60 N/mm
- Max compression: ~4 mm under landing load

**Geometry**:
- Dual-toe design (fore + aft toe) for rotation and stability
- Contact area: ~5 cm per foot (low footprint for agility)
- Mass: ~50g per foot (100g total)
- Surface texture: Micro-ridged for traction in multiple directions

### 3.2 Shock Absorption

**Passive Compliance Model**:
```
Spring-damper system:
  F_spring = k * compression_depth  (k = 60 N/mm)
  F_damping = c * velocity          (c  5 Ns/m for TPE)
  
Landing impact spreads over ~50-80 ms:
  Peak deceleration: ~240 N / 0.2 kg  1200 m/s
  This reduces peak stresses on servo gearboxes vs. rigid feet
```

**No Active Control Required**: Shock absorption is purely mechanical.

### 3.3 Integration with Leg Assembly

**Ankle Joint Connection**:
- Ankle Y servo connects to foot via tendon/linkage
- Allows foot pitch control (dorsiflexion during swing, plantarflexion during stance)
- Optional future enhancement: Tendon-driven proprioception for force feedback

**Contact State Estimation** (heuristic):
```
ground_contact = TRUE  if:
  - leg_velocity_z < 0.05 m/s  (near-stationary or slow)
  - AND leg_acceleration_z > -2.0 m/s  (not rapidly descending)
  
ground_contact = FALSE if:
  - leg_velocity_z > 0.2 m/s  (clear upward motion)
```

---

## 4. Control Integration

### 4.1 Main Control Loop (50 Hz)

```cpp
// Pseudo-code from flying_dragon_leg_assembly_act()

if (!legs_deployed && current_gait_mode > 0) {
  deploy_legs();  // Smooth 2-sec transition
}
else if (legs_deployed && current_gait_mode == 0) {
  retract_legs();  // Smooth 2-sec transition
}

// Compute target angles based on gait mode
switch (current_gait_mode) {
  case 1: compute_walking_gait(timer_ms, &l_target, &r_target); break;
  case 2: compute_running_gait(timer_ms, &l_target, &r_target); break;
  case 3: compute_jumping_gait(timer_ms, &l_target, &r_target); break;
  default: l_target = r_target = {0, 0, 0, 0};  // Idle
}

// Apply smoothing and send to servos
l_current = smoothing_filter(l_current, l_target, dt_ms);
r_current = smoothing_filter(r_current, r_target, dt_ms);

send_servo_commands(l_current, r_current);  // 8 PWM outputs
```

### 4.2 Behavior System Integration

**External API Calls** (from `flying_dragon_behavior_sequencer`):

```cpp
// Behavior state changes gait mode
flying_dragon_leg_set_gait_mode(1);  // Start walking

// At 75% battery depletion, energy-seeking behavior triggers
flying_dragon_leg_set_energy_seek(true);
//  Behavior system increases gait_mode to walking/running

// Adjust speed based on terrain/energy level
flying_dragon_leg_set_speed_factor(1.2f);  // 20% faster
```

### 4.3 Safety Interlocks

**Conflict Resolution** (flight vs. ground locomotion):

```
Priority 1: Flight mode active  Retract legs, ignore gait commands
Priority 2: Battery < 20%  Reduce gait speed, prepare for energy-seeking
Priority 3: Normal ground ops  Execute gait commands, manage servo temp
```

**Watchdog** (500 ms timeout):
- If no gait command received: Return to idle (mode 0)
- If leg assembly unresponsive: Flight controller takes over, retracts legs

---

## 5. Implementation Roadmap

### Phase 1: Core Development (Weeks 1-2)
-  Design leg servo architecture
-  Implement gait algorithms (walking, running, jumping)
-  Implement retraction kinematics
-  Create hardware specification JSON
-  Integrate servo controller with main ESP32 firmware

### Phase 2: Sensor Integration (Weeks 3-4)
-  Add force sensors in feet (optional)
-  Implement contact state estimation
-  Validate shock absorption via accelerometer
-  Tune smoothing filter time constants

### Phase 3: Behavior Integration (Weeks 5-6)
-  Connect leg_assembly to behavior_sequencer
-  Implement energy-seeking state machine
-  Test gait transitions at 75% battery
-  Validate flight/ground mode switching

### Phase 4: Field Testing (Weeks 7+)
-  Open-loop gait validation on test track
-  Terrain classification and gait selection
-  Endurance testing (2-4 hour operation)
-  Autonomous energy-seeking demo

---

## 6. Performance Targets

| Metric | Target |
|--------|--------|
| Walking speed | 0.15 m/s (150mm step length @ 1.2s cycle) |
| Running speed | 0.4 m/s (250mm step length @ 0.6s cycle) |
| Jump height | 100-150 mm (with quadcopter assist) |
| Jump distance | ~300 mm |
| Cycle accuracy | 5% phase timing |
| Servo response time | <50 ms (2-3 updates) |
| Stow/deploy time | ~2 seconds full transition |
| Max servo temperature | <70C during continuous walking |

---

## 7. Future Enhancements (Phase 5+)

### 7.1 Terrain Adaptation
- IMU-based slope detection
- Gait adjustment for inclines
- Terrain classification (carpet, tile, grass)

### 7.2 Learning-Based Optimization
- Machine learning for custom gaits
- Energy efficiency tuning
- Individual servo calibration

### 7.3 Advanced Sensing
- Force sensors in feet for ground reaction forces
- Accelerometer on leg segments for kinematic validation
- IMU fusion for center-of-mass estimation

### 7.4 Tendon-Driven Proprioception
- Ankle Y tendon with tension sensor
- Passive feedback loop for compliant landing
- Reduced servo load during shock absorption

---

## 8. References

### Design Inspiration
- **Poppy Project**: Open-source humanoid robot with proven bipedal gait algorithms
- **Leg geometry**: Derived from Poppy Humanoid leg kinematics (adapted for quadcopter hybrid)
- **Gait algorithms**: Sinusoidal oscillation patterns from Poppy walking controller

### Key Documents
- `flying_dragon_leg_assembly.src` - Core leg control logic
- `flying_dragon_foot_assembly.src` - Foot mechanics and contact estimation
- `flying_dragon_leg_controller.json` - Hardware specification (8 MG996R servos)
- `flying_dragon_behavior_sequencer.json` - Behavior state machine (includes leg modes)

### Hardware Files
- Servo specs: MG996R datasheet (20 kgcm torque, 56 rpm no-load speed)
- Power calculation: 8 servos  0.12A nominal = 0.96A @ 5V (4.8W idle)

---

## 9. Key Design Decisions

### Why Poppy Biomechanics?
- Proven effective for bipedal humanoid robots
- Smooth sinusoidal gaits reduce servo jerk and wear
- Symmetric control enables easy implementation
- Community documentation readily available

### Why Retractable Design?
- Eliminates flight interference (critical for quadcopter stability)
- Keeps legs out of rotating props
- Maintains CG within quadcopter envelope
- Allows rapid mode switching (2-second deploy/retract)

### Why MG996R Servos?
- Reuses existing servo bus (wings also use MG996R)
- Power distribution already in place
- Sufficient torque (20 kgcm) for leg loading
- Cost-effective for rapid prototyping

### Why Passive Feet?
- Eliminates additional servo weight
- Mechanical compliance naturally absorbs shock
- No software overhead for foot control
- Future force sensors can enhance without major redesign

---

## 10. Testing Checklist

- [ ] Servo pulse-width calibration (1000-2000 s mapping)
- [ ] Gait cycle timing (verify period within 5%)
- [ ] Walking stability (step height/length within spec)
- [ ] Running cycle speed (600ms 30ms)
- [ ] Jumping trajectory validation
- [ ] Deployment/retraction smoothness
- [ ] Flight mode leg stowage (verify no flight interference)
- [ ] Temperature monitoring (servo limits <70C)
- [ ] Battery integration (energy-seeking trigger at 75%)
- [ ] Energy efficiency measurement (Wh per meter walked)

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-05  
**Status**:  Ready for Implementation
