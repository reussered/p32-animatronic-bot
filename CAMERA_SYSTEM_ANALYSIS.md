# Goblin Camera System Analysis

## Autofocus + Gimbaled Positioning

**Date:** November 6, 2025  
**Context:** Two independent cameras (left/right eye sockets), each requiring:
- Autofocus capability (variable focus distance)
- Pan/tilt/roll orientation control (3-axis positioning)
- Compact form factor (fits in eye socket)
- Reasonable power budget (~2W per camera system)
- ESP-NOW communication to ESP32-S3 head controller

---

## 1. Camera Options (Autofocus Models)

### 1.1 Raspberry Pi Camera Module v3 (IMX708 sensor)
**Specifications:**
- Resolution: 12MP (4056 x 3040)
- Sensor Size: 1/1.56" (larger = better light gathering)
- Autofocus: Yes (continuous, fixed focus ~50cm default, adjustable via software)
- Field of View: 66degrees (default lens)
- Interface: MIPI CSI-2 (ribbon cable only)
- Power: ~400-500mA @ 3.3V during active capture (~1.5-2W)
- Cost: ~$25-30 USD
- **Issue:** Requires Raspberry Pi or compute module; doesn't directly integrate with ESP32

### 1.2 ArduCAM OV5647 / IMX219 / IMX477 (various)
**OV5647 (5MP, older):**
- Autofocus: No (fixed focus)
- Power: ~200mA @ 3.3V (~0.66W)
- Cost: ~$15-20
- **Limitation:** No autofocus

**IMX219 (8MP, medium):**
- Autofocus: No (fixed focus)
- Power: ~250mA @ 3.3V (~0.8W)
- Cost: ~$20-25
- **Limitation:** No autofocus

**IMX477 HQ (12MP, professional):**
- Autofocus: No (manual focus barrel, mechanical adjustment only)
- Power: ~300mA @ 3.3V (~1W)
- Cost: ~$40-50
- **Limitation:** No electronic autofocus

### 1.3 Luxonis OAK-D Lite (Stereo Depth + RGB)
**Specifications:**
- RGB Resolution: 12MP (4032 x 3040)
- Stereo Depth: 400p stereo pair for depth
- Autofocus: Yes (continuous autofocus on RGB + stereo active IR)
- Field of View: RGB 73degrees, Stereo ~120degrees
- Interface: USB 3.1 (or PoE variant available)
- Power: ~1.2W typical (USB 3.1 powered)
- Cost: ~$99-120 USD
- **Pros:**
  - Autofocus on RGB stream
  - Depth sensing (could enable autonomous focus distance)
  - Open-source DepthAI SDK
  - Small form factor (PCB board ~7.5cm x 7cm)
  - Can disable stereo for reduced power/bandwidth
- **Cons:**
  - USB 3.1 interface (requires USB hub or special adapter for ESP32)
  - Higher power than simple camera modules
  - Overkill if you only need video, not depth

### 1.4 Arducam IMX585 (32MP Autofocus)
**Specifications:**
- Resolution: 32MP (6528 x 4896)
- Autofocus: Yes (continuous autofocus, electronic control via I2C)
- Sensor Size: 1/1.3" (large, excellent low-light)
- Interface: MIPI CSI-2 (8-lane or 4-lane)
- Power: ~300-400mA @ 3.3V (~1-1.3W)
- Cost: ~$45-60 USD
- Focus Range: ~5cm to infinity (software controlled)
- **Pros:**
  - Native autofocus (no mechanical focus motor needed)
  - Electronic focus control via I2C register writes
  - High resolution (useful for recognition tasks)
  - Compact
- **Cons:**
  - MIPI CSI-2 requires compute module (Raspberry Pi CM, NVIDIA Jetson, or Arducam ISP bridge)
  - Not directly ESP32-native

### 1.5 OV2640 + External Autofocus Module (DIY)
**OV2640 (2MP, simple, ESP32-friendly):**
- Native ESP32 support (parallel DVP interface via ESPIF driver)
- No autofocus (fixed focus ~1m)
- Power: ~100mA @ 3.3V (~0.3W)
- Cost: ~$8-12
- **Workaround:** Add external stepper-driven mechanical focus barrel (~$5-10, +0.5W)
- **Pros:** Minimal power, direct ESP32 integration
- **Cons:** Low resolution, mechanical focus unreliable for servo-based positioning

### 1.6 USB UVC Generic Webcams (100+ options)
**Examples: Logitech C920, etc.**
- Autofocus: Most models have it
- Resolution: 1080p-4K
- Interface: USB 2.0 / USB 3.0
- Power: Highly variable (0.5W-3W)
- Cost: $20-150+
- **Issue:** Generic USB requires USB host adapter for ESP32 (adds complexity, power draw)

---

## 2. Mechanical Positioning (Gimbal / Pan-Tilt-Roll)

The camera must orient in 3D space. Options:

### 2.1 Commercial Pan-Tilt-Roll (PTR) Gimbal Mounts
**Example: Feetech SGS16/FT-TBS15 Servo Gimbal**
- Axes: Pan (+/-150degrees), Tilt (+/-90degrees), Roll (+/-45degrees)
- Servo Count: 3x MG996R equivalent servos
- Power: ~2-3W active (6-9W peak stall torque)
- Cost: ~$25-50 per gimbal (x2 for stereo = $50-100)
- Size: ~10cm x 8cm x 8cm (too large for eye socket)
- **Issue:** Designed for action cams (GoPro size), oversized for goblin head

### 2.2 Compact DIY 2-Axis Pan-Tilt (Budget Approach)
**Construction:**
- Base: 2x high-speed servo (e.g., Feetech FS5115, MG90S)
  - FS5115: Speed 0.16s/60degrees, Torque 15 kg.cm, Power ~1.2W per servo
  - Cost: ~$8-15 each = $16-30
- Mount: 3D-printed plastic bracket (supports ~200g camera)
- Axes: Pan (servo 1) + Tilt (servo 2)
- **Roll:** Handled via camera body rotation (requires motor or manual orientation)
- Power: ~2.4W active (both servos engaged)
- Size: ~8cm x 6cm x 5cm (fits eye socket with tight tolerances)
- **Pros:**
  - Minimal cost ($25-50 total per camera system)
  - Compact, 3D-printable
  - Direct servo control via PWM from ESP32
- **Cons:**
  - Only 2 axes native (roll via body rotation)
  - Servo jitter visible if camera moves slowly

### 2.3 Compact 3-Axis Gimbal (Advanced)
**Construction:**
- Base: 3x micro servos (e.g., Tower Pro SG90, 9g weight each)
  - Power: ~1W per servo
  - Torque: 1.8 kg.cm (marginal for camera weight)
  - Speed: 0.1s/60degrees (good responsiveness)
- Mechanism: Gimbal ring or spherical joint design
- **Issue:** 3x 9g servos = 27g servo mass alone; camera + electronics add 50-100g; total torque insufficient for stable hold under gravity

### 2.4 Hybrid: Stepper Motor + Mechanical Ball Joint
**Alternative for precision:**
- Pan: Stepper motor (NEMA 17) + gear reduction
- Tilt: Stepper motor (NEMA 17) + gear reduction
- Power: ~2W per stepper (active holding)
- Advantages: High precision, repeatable positioning
- **Issue:** Stepper motors are power-hungry, unsuitable for battery operation; overkill for a goblin head

---

## 3. Integration Architecture (Recommended)

### **Option A: Two-Axis Servo Gimbal + Autofocus Camera (RECOMMENDED)**

**Left Eye Camera Assembly:**
1. **Camera Module:** ArduCAM IMX585 (32MP autofocus) or Luxonis OAK-D Lite
   - Autofocus handled electronically
   - Mass: ~40g
2. **Gimbal Mount:** 2-axis servo bracket
   - Servo 1 (Pan): FS5115 or MG996R mini
   - Servo 2 (Tilt): FS5115 or MG996R mini
   - Total mass: ~80g (servos + plastic mount)
3. **Integration:**
   - Servos receive PWM signals from goblin_left_eye GPIO pins
   - Camera streams to ESP32 via USB adapter OR MIPI bridge board
   - Autofocus controlled via I2C register writes (if IMX585)
   - OR USB autofocus commands (if OAK-D)

**Right Eye Camera Assembly:** Mirror of left

**Power Budget per Camera System:**
- Camera: 1-1.5W
- Servos (pan/tilt, idle): ~0.1W
- Servos (pan/tilt, active): ~2.4W
- **Total:** ~1.2W baseline, up to 3.9W under active movement

**Cost per Camera System:**
- Camera: $45-120
- Gimbal mount (3D-printed): $5
- Servos (2x): $15-30
- Connectors/wiring: $3-5
- **Total:** $70-160 per camera system (x2 = $140-320 for stereo pair)

---

## 4. Focus Control Strategy

### **For ArduCAM IMX585 (I2C Autofocus):**
```cpp
// Focus mode control (0 = manual, 1 = continuous autofocus, 2 = single autofocus)
i2c_write_register(CAMERA_I2C_ADDR, 0x3C80, 0x01);  // Enable continuous autofocus

// Manual focus distance (register 0x3C82-0x3C83, focus value 0-255, 0=infinity, 255=minimum distance)
uint16_t focus_value = 128;  // Mid-range (approx 30cm)
i2c_write_register(CAMERA_I2C_ADDR, 0x3C82, (focus_value >> 8) & 0xFF);
i2c_write_register(CAMERA_I2C_ADDR, 0x3C83, focus_value & 0xFF);
```

### **For Luxonis OAK-D (USB Autofocus):**
```python
# Via DepthAI Python SDK
cap = dai.Device()
cam_rgb = cap.createColorCamera()
cam_rgb.setAutoFocus(True)
cam_rgb.setFocusDistance(300)  # millimeters
```

### **For OV2640 (No Native Autofocus):**
- Mechanical focus barrel: stepper motor driving M25.4x0.5mm thread (focus distance ~0.3-3m)
- OR: Fixed focus at dominant distance (e.g., 50cm for animatronic interaction)

---

## 5. Power & Communication

### **Power Delivery:**
- Camera: 3.3V, 1-1.5A typical
- Servos: 5V, 0.5-2A peak (per servo)
- **Total per camera system:** 3.3V @ 2A + 5V @ 3A (simultaneous movement)
- **Recommendation:** Dedicated 5V/3A supply per eye system; share via GPIO-controlled buck converters

### **Communication:**
- **Camera to ESP32:**
  - USB: USB OTG adapter (Luxonis OAK-D, generic webcams) -> USB 2.0/3.0 adapter IC (e.g., CH340)
  - OR MIPI CSI-2 via Arducam ISP bridge board (more complex, lower latency)
  - OR SPI/I2C for simple sensors (OV2640 via DVP parallel interface)
- **Servo Control to ESP32:**
  - PWM on GPIO pins (standard servo control, 50Hz, 1-2ms pulse width)
- **Inter-ESP32 Communication:**
  - Gimbal state + camera focus status -> ESP-NOW to torso controller (optional logging)

---

## 6. Recommendation Summary

| Component | Model | Cost | Power | Focus | Notes |
|-----------|-------|------|-------|-------|-------|
| **Camera** | ArduCAM IMX585 | $45-60 | 1-1.3W | [YES] I2C autofocus | Best compromise: autofocus + low power + compact |
| **Camera (Alt)** | Luxonis OAK-D Lite | $99-120 | 1.2W | [YES] Stereo AF | Overkill but powerful; depth sensing bonus |
| **Camera (Alt)** | Pi Camera v3 | $25-30 | 1.5-2W | [YES] Software AF | Requires Raspberry Pi compute module |
| **Gimbal (Pan)** | FS5115 servo | $8-12 | <1W active | N/A | Compact, fast, adequate torque |
| **Gimbal (Tilt)** | FS5115 servo | $8-12 | <1W active | N/A | Matches pan servo |
| **Mount (3D)** | Custom bracket | $5 | N/A | N/A | PLA/PETG printed, ~5 parts |
| **TOTAL per system** | - | $70-160 | 1.2-3.9W | [YES] | Sufficient for stereo goblin eyes |

---

## 7. Next Steps

1. **Decide on camera model** (IMX585 vs OAK-D vs Pi v3)
2. **Source gimbal servos** (FS5115 recommended, or equivalent MG996R mini)
3. **Design 3D gimbal bracket** (pan-tilt joint, camera plate, servo horn mounts)
4. **Prototype focus controller** (I2C register writes for IMX585, or fixed focus + mechanical adjustment)
5. **Define eye tracking algorithm** (pan/tilt servo angles as function of detected face/attention target)
6. **Integrate with existing head subsystem** (GPIO allocation, power rails, ESP-NOW communication)

---

## Appendix: Component Links & Suppliers

**Cameras:**
- ArduCAM IMX585: arducam.com, Amazon, AliExpress (~$50-60)
- Luxonis OAK-D Lite: shop.luxonis.com, Amazon (~$99-120)
- Pi Camera v3: raspberrypi.com, adafruit.com (~$25-30)

**Servos:**
- FS5115 (Feetech): aliexpress.com, amazon.com (~$8-15 each)
- MG996R mini: amazon.com, aliexpress.com (~$8-12 each)

**3D Printing:**
- Local makerspaces, Prusa printers, or commercial services (Shapeways, Sculpteo)

**Adapters:**
- USB OTG for ESP32: amazon.com, aliexpress.com (~$3-8)
- MIPI CSI-2 breakout boards: arducam.com, amazon.com (~$20-40)
