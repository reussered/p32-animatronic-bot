# Motor Characterization Protocol - Multimeter Only
**Date Created**: November 7, 2025  
**Purpose**: Identify unknown stepper motors and clock motors to determine suitability for facial animation  
**Equipment Required**: 
- Digital Multimeter (with DC voltage, DC current, resistance functions)
- Adjustable DC Power Supply (0-24V, 5A minimum recommended)
- String + small weights (gram scale helpful, but can use coins: US penny = 2.5g)
- Protractor or angle ruler
- Small stand or clamp to hold motor shaft vertical
- Marker or tape for marking shaft position

---

## **QUICK REFERENCE: What Numbers Matter for Facial Animation**

| Parameter | Needed For Face | Red Flag |
|---|---|---|
| **Torque** | 0.5 - 2 Ncm minimum | < 0.3 Ncm = too weak |
| **Step Angle** | 1.8 - 5.6 (standard) | > 15 = too coarse |
| **Voltage** | 5V or 12V preferred | > 24V = power supply headache |
| **Current Draw** | < 500 mA per motor | > 1A = needs big power supply |
| **Holding Power** | Should lock when powered | Spins freely = wrong type |

---

## **PROTOCOL: Motor Testing Checklist**

### **MOTOR ID**: ________________  
**Location Found**: ________________  
**Physical Description**: Size ______mm, Wire count: _____, Shaft diameter: ____mm

---

## **TEST 1: IDENTIFY MOTOR TYPE (5 minutes)**
**Goal**: Determine if it's a stepper motor, clock motor, or DC brushed motor

### Step 1.1: Visual Inspection
- [ ] Count the wires coming out of the motor
  - **2 wires**  Likely DC brushed motor (will spin continuously)
  - **4 wires**  Likely stepper motor or bipolar DC
  - **5 wires**  Likely unipolar stepper motor
  - **6+ wires**  Clock motor or multi-phase stepper

- [ ] Look for coils (looped wires) visible inside
  - **Multiple distinct coils**  Stepper motor (GOOD for animation)
  - **Smooth copper contacts**  DC brushed motor (spins continuously)
  - **Tiny gear visible**  Clock motor (precision, but may be too slow)

### Step 1.2: Resistance Check (Multimeter - Resistance Mode)
**Procedure**: 
- Disconnect motor from any power
- Set multimeter to Resistance () mode
- Probe between each pair of wires
- Record all readings

| Wire Pair | Resistance | Motor Type Indicator |
|---|---|---|
| All pairs high (> 1 M) | Open circuit | Broken motor, SKIP THIS ONE |
| All pairs 0  | Short circuit | Broken motor, SKIP THIS ONE |
| Two pairs ~10-100 , rest high | Unipolar stepper |  Good for animation |
| Two pairs ~5-20 , diagonal match | Bipolar stepper |  Good for animation |
| One pair 0-10 , others high | DC brushed motor |  Wrong type for animation |
| All pairs 50-200  similar | Clock motor or DC | ? Unknown, test more |

**Record**: ________________

### Step 1.3: Listen Test (Optional but informative)
- [ ] Connect to 5V DC power supply briefly (< 2 seconds)
- **Clock motor**: Hums at high pitch (100-200 Hz), sounds like buzzing bee
- **Stepper motor**: Clicks/ticks at slow rate (1-10 clicks per second), clicky sound
- **DC motor**: Smooth whirring sound, no clicking

**What I heard**: ________________

### **TEST 1 RESULT**: 
- [ ] **STEPPER MOTOR** (Proceed to Test 2)
- [ ] **DC BRUSHED MOTOR** (Not suitable for facial animation, note this and move to next motor)
- [ ] **CLOCK MOTOR** (Proceed to Test 2, but may be too specialized)
- [ ] **UNKNOWN** (Proceed to Test 2 anyway)

---

## **TEST 2: MEASURE TORQUE (10 minutes)**
**Goal**: Determine if motor is strong enough for facial animation (need  0.5 Ncm)

### Step 2.1: Build Test Rig
- [ ] Mount motor vertically on a stand/clamp so shaft points straight up
- [ ] Tie a string around the shaft (wrap 3-4 times so it doesn't slip)
- [ ] Let string hang off edge so you can attach weights
- [ ] Mark shaft position with tape or marker

### Step 2.2: Energize Motor
- [ ] Connect multimeter in series (current mode) to measure draw
- [ ] Apply **starting voltage**: 5V (if motor is rated for 5V) or 12V (if rated for 12V)
- [ ] Record current draw: _______ mA
  - **Stepper at 5V**: Expect 100-300 mA
  - **Clock motor at 5V**: Expect 20-50 mA
  - **DC motor at 5V**: Expect 100-500 mA

### Step 2.3: Measure Torque with Weights
**For US Coins** (if no gram scale):
- Penny = 2.5g
- Nickel = 5g
- Dime = 2.268g
- Quarter = 5.67g

**Procedure**:
1. With motor powered at rated voltage, hang coins on string one by one
2. Keep adding until motor just barely starts to struggle or lift weight
3. Record **maximum weight lifted before motor stalls**: _______ grams

**Distance from shaft center to string**: _______ cm

### Step 2.4: Calculate Torque
```
Torque (Ncm) = (Weight in grams  0.00981)  Distance in cm

Example: 100 grams at 2 cm = (100  0.00981)  2 = 1.96 Ncm
```

**MY CALCULATION**:  
Weight: _______ g  0.00981 = _______ N  
Distance: _______ cm  
**TORQUE = _______ Ncm**

### **TEST 2 RESULT**:
- [ ] **> 2 Ncm**: Strong! Good for cheek movement or head rotation
- [ ] **0.5 - 2 Ncm**: Perfect! Suitable for facial animation (cheeks, jaw, eyebrows)
- [ ] **0.1 - 0.5 Ncm**: Weak. Only suitable for nostril/eyebrow twitches, not cheek raise
- [ ] **< 0.1 Ncm**: Too weak. Not suitable for facial animation.

---

## **TEST 3: MEASURE STEP ANGLE & RESOLUTION (10 minutes)**
**Goal**: Determine if motor can do fine, controlled movements (need  5.6 per step)

### Step 3.1: Setup for Step Testing
- [ ] Mark shaft position with marker/tape (reference point at "0")
- [ ] Place protractor under shaft so you can measure rotation
- [ ] Have power supply and multimeter ready

### Step 3.2: Test Stepper Motor (if applicable)
**If you have a function generator**: Send single pulses at 1 Hz
**If you DON'T have a function generator**: 
- Briefly touch motor wires to 5V/12V for 0.5 seconds, then disconnect
- Each pulse = one step

**Procedure**:
1. Apply single pulse (0.5 sec on, 1 sec off)
2. Measure shaft rotation with protractor
3. Repeat 10 times
4. Record: _______ degrees per step (average)

### Step 3.3: Calculate Useful Resolution
```
If motor steps at 1.8:     Can position at 1.8, 3.6, 5.4, etc.   EXCELLENT
If motor steps at 5.6:     Can position at 5.6, 11.2, 16.8, etc.  GOOD
If motor steps at 11.2:    Too coarse for smooth face animation  POOR
If motor spins continuously: Not a stepper  Can't control position
```

**MY MEASUREMENT**: _______ degrees per step

### **TEST 3 RESULT**:
- [ ] **1.8 - 5.6**: Excellent resolution. Perfect for facial animation.
- [ ] **5.6 - 11.2**: Acceptable but coarse. May see jerky animation.
- [ ] **> 11.2**: Too coarse. Poor for facial animation.
- [ ] **Continuous rotation**: Not a stepper. Wrong motor type.

---

## **TEST 4: CHECK HOLDING POWER (2 minutes)**
**Goal**: Can motor maintain position when power is removed? (important for fail-safe expressions)

### Step 4.1: De-Energize Test
- [ ] Motor currently powered and holding a position (from Test 2)
- [ ] Disconnect power supply completely
- [ ] Try to turn motor shaft by hand with gentle force

### Step 4.2: Record Result
- [ ] **Shaft is locked solid**  Has permanent magnet. Good! Will hold expression if power fails.
- [ ] **Shaft turns with light force (< 50g resistance)**  Weak holding. Risky.
- [ ] **Shaft spins freely**  No holding power. Clock motor or brushed DC motor.

**MY RESULT**: ________________

### **TEST 4 RESULT**:
- [ ] **LOCKED**: Ideal for facial animation - face won't droop if power lost
- [ ] **WEAK HOLD**: Acceptable but needs external latch/friction to hold position
- [ ] **FREE SPIN**: Not suitable - face will collapse when powered off

---

## **TEST 5: POWER CONSUMPTION (2 minutes)**
**Goal**: Ensure you won't overload your power supply

### Step 5.1: Measure Current Under Load
- [ ] Set multimeter to DC Current (Amps or mA mode)
- [ ] Connect in series with motor power supply
- [ ] Apply rated voltage with motor under load (holding the weight from Test 2)
- [ ] Record current: _______ mA (or _______ A)

### Step 5.2: Calculate Power Supply Needs for 9 Motors
```
Total Current for 9 motors = 9  (current per motor in amps)
Total Power needed = Voltage  Total Current

Example: 9 motors  300 mA = 2700 mA = 2.7 A at 12V
Recommended PSU: 5A at 12V (gives headroom)
```

**MY CALCULATION**:  
Per motor: _______ mA = _______ A  
For 9 motors: _______ A  
Voltage needed: _______ V  
**Recommended PSU rating: _______ A at _______ V**

---

## **FINAL ASSESSMENT SUMMARY**

| Test | Result | Pass/Fail |
|---|---|---|
| **Test 1: Motor Type** | Stepper / DC / Clock |  PASS  FAIL |
| **Test 2: Torque** | _______ Ncm |  PASS  FAIL |
| **Test 3: Step Angle** | _______  per step |  PASS  FAIL |
| **Test 4: Holding Power** | Locked / Weak / Free |  PASS  FAIL |
| **Test 5: Power Draw** | _______ mA per motor |  PASS  FAIL |

---

## **RECOMMENDATION**

### For Facial Animation (Cheeks, Jaw, Eyebrows):
- [ ] ** YES - Use these motors**  
  Reason: ________________________________________________

- [ ] ** NO - Do not use for face**  
  Reason: ________________________________________________

- [ ] **? MAYBE - Use for specific features only**  
  Best for: ________________________________________________

### Alternative Allocation:
Could use these motors for: (neck, arms, head rotation, etc.)  
________________________________________________

---

## **MOTOR IDENTIFICATION NOTES**
(Fill in for your records)

**Best guess on model**: ________________

**Likely manufacturer**: ________________

**Estimated specs**:
- Voltage: _______ V
- Torque: _______ Ncm
- Step angle: _______ 
- Current draw: _______ mA
- Type:  Unipolar Stepper  Bipolar Stepper  Clock Motor  DC Brushed

**Internet search to try**:
"Stepper motor _______ degrees _______ Ncm _______ V"

---

## **NOTES & OBSERVATIONS**
(Write anything unusual you observed)

________________________________________________

________________________________________________

________________________________________________

---

## **Next Steps**

### If Motor PASSES All Tests:
1. [ ] Add to `inventory.json` under identified motor type with measured specs
2. [ ] Order 27 total (9 for 3 bots + 3-4 spares) if quantity on hand insufficient
3. [ ] Test one sample with actual ESP32 driver module to verify compatibility

### If Motor FAILS Tests:
1. [ ] Consider for non-facial subsystems (neck, arms, if torque/speed suitable)
2. [ ] Or mark as "not suitable" and store for future projects
3. [ ] Continue testing other unknown motors

---

**TEST COMPLETED BY**: _________________  
**DATE**: _________________  
**TIME SPENT**: _______ minutes  

