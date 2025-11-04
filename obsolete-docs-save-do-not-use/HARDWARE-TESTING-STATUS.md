# Hardware Testing Status - October 15, 2025

## Component Availability for Testing

### ✅ Available Now (Ready for Testing)

| Component | Type | Quantity | Status |
|-----------|------|----------|--------|
| **ESP32-S3 DevKitC-1** | Microcontroller | 1 | ✅ On hand |
| **GC9A01 Displays** | Circular LCD (1.28") | 2 | ✅ On hand (eyes) |
| **HC-SR04 Sensor** | Ultrasonic distance | 1 | ✅ On hand (nose) |
| **HW-496 Microphones** | MEMS microphone | 2 | ✅ On hand (ears) |
| **Power Supply** | 3.3V/5V | 1 | ✅ On hand |

### ⏳ On Order (Testing Delayed)

| Component | Type | Expected | Testing Impact |
|-----------|------|----------|----------------|
| **Audio Amplifier** | I2S/Analog | Few days | ⚠️ Skip audio output tests |
| **Speakers** | 40mm drivers | Few days | ⚠️ Skip audio output tests |
| **Mouth Display** | GC9A01 + ESP32 | Few days | 🔍 **CRITICAL DISCOVERY** - Has onboard ESP32 chip! |

### 🔍 Critical Discovery: Mouth Display Architecture

**Finding**: Mouth display module has **onboard ESP32 chip** connected via UART  
**Implication**: Should be programmed as **autonomous mesh subsystem node**  
**Benefit**: Offloads head ESP32 by ~30% CPU, enables 60 FPS mouth rendering

**See**: `docs/MOUTH-SUBSYSTEM-ARCHITECTURE.md` for complete analysis

**Action Items When Hardware Arrives**:
1. Identify ESP32 chip variant (ESP32, ESP32-S2, ESP32-S3?)
2. Map GPIO pins (display connection, UART, available pins)
3. Test factory firmware capabilities
4. Flash custom firmware to join ESP-NOW mesh
5. Integrate as autonomous subsystem node

### Testing Strategy (Current Hardware)

**Phase 1: Visual & Sensor Testing** (Now)
```
✅ Can test:
- ESP32-S3 basic functionality
- SPI display initialization (2x GC9A01)
- Display rendering (eyes)
- HC-SR04 distance sensing (nose)
- HW-496 microphone input (ears)
- WiFi connectivity
- ESP-NOW mesh (if 2nd ESP32 available)
- Mood system (visual feedback via eyes)
- Component timing (hitCount execution)

⚠️ Cannot test:
- Audio output/playback
- Speaker mounting
- I2S audio amplifier
- Audio-driven animations
```

**Phase 2: Audio Testing** (When parts arrive)
```
Will add:
- I2S audio amplifier configuration
- Speaker output testing
- Audio-synchronized eye animations
- Voice/sound effect playback
- Audio feedback loops
```

---

## Manufacturer-Specific Shape Variations

### Problem Statement

Different component manufacturers have different physical dimensions and mounting requirements:

**Example: 40mm Speakers**
- **Generic Chinese**: 40mm diameter, 4x M3 mounting holes at 35mm PCD
- **Adafruit**: 40mm diameter, 2x mounting tabs
- **SparkFun**: 40mm diameter, snap-fit enclosure
- **Waveshare**: 40mm diameter, 6x M2.5 mounting holes

**Current Issue**: Single SCAD file per component type doesn't accommodate manufacturer variations

### Solution: Manufacturer Variants in Component JSON

Enhanced component JSON structure with manufacturer-specific shape generation:

```json
{
  "hardware_type": "SPEAKER_40MM",
  "component_name": "40mm Audio Speaker",
  "mounting_architecture": {
    "approach": "two_tier",
    "shape_generation": {
      "scad_template": "templates/speaker_enclosure.scad",
      "manufacturer_variants": {
        "generic": {
          "stl_file": "basic_mounts/speaker_40mm_generic.stl",
          "parameters": {
            "speaker_diameter": 40.0,
            "mounting_holes": 4,
            "hole_diameter": 3.0,
            "hole_pcd": 35.0,
            "mounting_type": "through_hole"
          },
          "description": "Generic 40mm speaker with 4x M3 holes at 35mm PCD"
        },
        "adafruit": {
          "stl_file": "basic_mounts/speaker_40mm_adafruit.stl",
          "parameters": {
            "speaker_diameter": 40.0,
            "mounting_tabs": 2,
            "tab_width": 8.0,
            "tab_thickness": 2.0,
            "mounting_type": "tab_slots"
          },
          "description": "Adafruit 40mm speaker with 2 mounting tabs"
        },
        "sparkfun": {
          "stl_file": "basic_mounts/speaker_40mm_sparkfun.stl",
          "parameters": {
            "speaker_diameter": 40.0,
            "snap_fit_clips": 4,
            "clip_depth": 1.5,
            "mounting_type": "snap_fit"
          },
          "description": "SparkFun 40mm speaker with snap-fit enclosure"
        }
      },
      "default_variant": "generic"
    }
  }
}
```

### Updated Component Selection Workflow

**1. User Specifies Manufacturer**

In positioned component JSON:
```json
{
  "component_name": "goblin_speaker",
  "hardware": "config/components/hardware/speaker_40mm.json",
  "manufacturer_variant": "adafruit",  // ← User specifies which variant
  "position": {
    "x": "0.0 INCH",
    "y": "-2.0 INCH", 
    "z": "-1.0 INCH"
  }
}
```

**2. Code Generator Selects Correct STL**

```python
def get_component_stl(positioned_component):
    hardware_def = load_json(positioned_component["hardware"])
    variant = positioned_component.get("manufacturer_variant", 
                                      hardware_def["mounting_architecture"]
                                                  ["shape_generation"]
                                                  ["default_variant"])
    
    stl_path = hardware_def["mounting_architecture"]["shape_generation"] \
                          ["manufacturer_variants"][variant]["stl_file"]
    
    return stl_path
```

**3. Shape Generator Creates All Variants**

```powershell
.\tools\generate-mounting-system.ps1 -BotType goblin -GenerateAllVariants

# Generates:
# assets/shapes/stl/basic_mounts/speaker_40mm_generic.stl
# assets/shapes/stl/basic_mounts/speaker_40mm_adafruit.stl
# assets/shapes/stl/basic_mounts/speaker_40mm_sparkfun.stl
```

### Benefits

1. **Hardware Flexibility**: Support any manufacturer's physical design
2. **Easy Switching**: Change one JSON field to try different manufacturers
3. **Pre-Generated STLs**: All variants ready for 3D printing
4. **Self-Documenting**: Variant descriptions explain physical differences
5. **Testing Isolation**: Test with available hardware, swap later

### Implementation Plan

**Phase 1**: Add variant support to component JSON schema
```json
{
  "manufacturer_variants": {
    "type": "object",
    "patternProperties": {
      ".*": {
        "type": "object",
        "properties": {
          "stl_file": {"type": "string"},
          "parameters": {"type": "object"},
          "description": {"type": "string"}
        }
      }
    }
  }
}
```

**Phase 2**: Enhance OpenSCAD templates with variant parameters
```scad
// templates/speaker_enclosure.scad

module speaker_mount(
    diameter = 40.0,
    mounting_type = "through_hole",  // or "tab_slots", "snap_fit"
    // ... variant-specific parameters
) {
    if (mounting_type == "through_hole") {
        through_hole_mount(diameter, hole_count, hole_pcd);
    } else if (mounting_type == "tab_slots") {
        tab_slot_mount(diameter, tab_width, tab_count);
    } else if (mounting_type == "snap_fit") {
        snap_fit_mount(diameter, clip_count, clip_depth);
    }
}
```

**Phase 3**: Update shape generation script
```powershell
# Generate all manufacturer variants for a component type
foreach ($variant in $hardware_def.manufacturer_variants.Keys) {
    $params = $hardware_def.manufacturer_variants[$variant].parameters
    openscad.exe -D "variant='$variant'" `
                 -D "params='$params'" `
                 -o "$output_dir/$variant.stl" `
                 "$template.scad"
}
```

**Phase 4**: Update positioned component JSON
```json
{
  "component_name": "goblin_speaker",
  "manufacturer_variant": "adafruit",  // ← Specify when known
  "notes": "Using Adafruit #3885 - 40mm 4Ω 3W speaker"
}
```

---

## Testing Workarounds (Audio Missing)

### Mock Audio Component

Create temporary stub implementation for testing without speakers:

```c
// src/components/goblin_speaker_STUB.c

esp_err_t goblin_speaker_init(void) {
    ESP_LOGW(TAG, "Speaker STUB: Hardware not available");
    ESP_LOGW(TAG, "Audio output will be logged only");
    return ESP_OK;
}

void goblin_speaker_act(void) {
    // Read what WOULD be played
    if (g_shared_state.is_speaking) {
        ESP_LOGI(TAG, "AUDIO: Would play speech sound");
    }
    
    // Provide visual feedback via eyes instead
    if (g_shared_state.audio_level > 0) {
        // Flash eyes to indicate audio activity
        g_mood.addExcitement(10);
    }
}
```

### Visual Audio Feedback

Use displays as temporary audio indicators:

```c
void render_audio_indicator(uint8_t audio_level) {
    // Eye brightness follows audio amplitude
    uint8_t brightness = map(audio_level, 0, 255, 50, 255);
    set_eye_brightness(brightness);
    
    // Eye color indicates audio frequency
    if (audio_level > 200) {
        set_eye_color(RED);   // High volume
    } else if (audio_level > 100) {
        set_eye_color(YELLOW); // Medium volume
    } else {
        set_eye_color(GREEN);  // Low volume
    }
}
```

### Serial Port Audio Log

Log audio events for debugging:

```c
void goblin_speaker_act(void) {
    if (should_play_audio()) {
        // Log what WOULD play
        ESP_LOGI(TAG, "AUDIO: [%.3f sec] Playing: %s (volume: %d%%)",
            esp_timer_get_time() / 1000000.0,
            current_sound_name,
            audio_volume);
    }
}
```

### Testing Checklist (Audio Absent)

```
□ ESP32-S3 boots and initializes
□ Both GC9A01 displays initialize via SPI
□ Eyes render basic graphics (circles, colors)
□ HC-SR04 sensor detects distance
□ Mood system responds to sensor input
□ Eyes reflect mood changes (color/expression)
□ WiFi connects to network
□ ESP-NOW mesh establishes (if 2nd ESP32)
□ Component timing verified (hitCount execution)
□ Serial logging shows all component activity

⚠️ SKIP (until speakers arrive):
□ Audio amplifier I2S initialization
□ Speaker output testing
□ Audio-synchronized animations
□ Sound effect playback
```

---

## Manufacturer Documentation Needs

When speakers arrive, document:

1. **Physical Measurements**
   - Exact diameter
   - Mounting hole positions
   - Thread size (M2.5, M3, etc.)
   - Depth/thickness
   - Wire connector type

2. **Electrical Specs**
   - Impedance (4Ω, 8Ω)
   - Power rating (3W, 5W)
   - Frequency response
   - Sensitivity (dB/mW)

3. **Photos**
   - Front view
   - Back view with mounting holes visible
   - Side profile
   - Wire connections

4. **Create Variant**
   - Measure actual hardware
   - Generate SCAD parameters
   - Create STL variant
   - Test 3D print mounting bracket
   - Update component JSON with exact variant

---

## Related Documentation

📘 **[Component Pin Aliasing Spec](COMPONENT-PIN-ALIASING-SPEC.md)** - Pin name variations  
📘 **[Hardware Config Updates](HARDWARE-CONFIG-UPDATES.md)** - Recent hardware changes  
📘 **[Two-Tier Mounting System](two-tier-mounting-system.md)** - Basic mounts + shells

---

## Summary

**Current Testing Capability**: ✅ Visual + Sensors (Eyes, Nose, Ears)  
**Blocked Testing**: ⚠️ Audio output (speakers on order)  
**Solution**: Stub audio components, use visual feedback  
**Future Enhancement**: Manufacturer-specific shape variants in component JSON  
**When Speakers Arrive**: Measure, document, create variant, update JSON

**Testing can proceed with 80% of functionality available!** 🎉

---

*Updated: October 15, 2025, 10:00 AM*  
*Next Update: When audio hardware arrives*
