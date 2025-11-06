# MULTI-HARDWARE COMPONENT SPECIFICATION
**Date**: November 4, 2025  
**Author**: User Direction  
**Context**: P32 Animatronic Bot - ESP32 Embedded System

---

## PROBLEM STATEMENT

Many components inherently require **multiple different hardware devices**:
- **Eye components**: Display (TFT/OLED) + Camera (optional)
- **Mouth components**: Display (TFT) + Speaker (audio)
- **Sensor clusters**: Multiple sensors (IMU + temperature + humidity)
- **Controller nodes**: Processor + Communication module + Power monitoring

**Current limitation**: Component definitions only specify a single `hardware_template`, but real hardware deployments require **2-N different hardware devices** working together within a single component's `init()` and `act()` functions.

**Goal**: Allow component specifications to declare multiple hardware templates and pass full template specifications (with all arguments) down into compiled `init()` and `act()` functions.

---

## DESIGN PRINCIPLE: HARDWARE SPECIFICATION VIA COMPOSITION

**Key insight**: Component behavior and logic should be **hardware-agnostic**. Hardware choices are made at the **bot composition level** (in `components:[]` arrays), not baked into component source files.

**Single Source of Hardware Truth**: The `components:[]` array in bot definitions is the ONLY place where hardware is specified.

**Benefits:**
- **Reusability**: Same `goblin_left_eye.src` component works with GC9A01, ST7789, or any display
- **Flexibility**: Change display type by editing one line in bot JSON, no code changes needed
- **Testability**: Deploy same component with different hardware for testing
- **Maintainability**: Component logic never needs to mention specific hardware names

**Critical use case - Stepper Motors**: There are dozens of stepper motor variants (NEMA17, NEMA23, bipolar, unipolar, microstepping drivers, etc.). With this architecture:
- **Same jaw/limb component works with ANY stepper motor**
- Change stepper by editing one line in bot JSON
- Easy to swap motors for different gearing, torque, or speed requirements
- No recompilation needed to try different stepper configurations
- **Massive win** for prototyping and hardware iteration

**Example: Changing stepper hardware**

Current bot uses NEMA17 stepper:
```json
"components": ["goblin_jaw<NEMA17_STEPPER,DRVACC8834>"]
```

To switch to NEMA23 (higher torque):
```json
"components": ["goblin_jaw<NEMA23_STEPPER,DRVACC8834>"]
```

Or try bipolar vs unipolar:
```json
"components": ["goblin_jaw<NEMA17_BIPOLAR,DRVACC8834>"]
"components": ["goblin_jaw<NEMA17_UNIPOLAR,DRVULN2003>"]
```

All work with the SAME `goblin_jaw.src` component. Hardware choice is purely a bot composition decision.

**Example: Changing display hardware**

Current bot definition uses GC9A01:
```json
"components": ["goblin_left_eye<GC9A01,OV2640>"]
```

To switch to ST7789 display:
```json
"components": ["goblin_left_eye<ST7789,OV2640>"]
```

That's it. No changes to `goblin_left_eye.src`, `goblin_left_eye.hdr`, or `goblin_left_eye.json`. Only the bot composition changes.

---

## SOLUTION ARCHITECTURE

### 1. COMPONENT TEMPLATE SYNTAX IN ARRAYS

Components are declared in `components:[]` arrays using **template syntax**. Each component can have **0, 1, or more hardware templates** specified inline.

**Syntax**: `ComponentName<Template1,Template2,...>` or `ComponentName` (no templates)

**Examples in components array:**
```json
{
  "name": "goblin_head",
  "components": [
    "goblin_left_eye<GC9A01,OV2640>",
    "goblin_right_eye<GC9A01,OV2640>",
    "goblin_mouth<GC9A01,MINISPKR>",
    "goblin_jaw<SERVO>",
    "sdi_display_bus",
    "goblin_head_controller<ESP32S3>"
  ]
}
```

**How it works:**
- `goblin_left_eye<GC9A01,OV2640>`  Component named `goblin_left_eye` using display (GC9A01) + camera (OV2640)
- `goblin_mouth<GC9A01,MINISPKR>`  Component named `goblin_mouth` using display (GC9A01) + speaker (MINISPKR)
- `goblin_jaw<SERVO>`  Component named `goblin_jaw` using single servo template
- `sdi_display_bus`  Component named `sdi_display_bus` with no hardware templates (pure driver/bus)
- `goblin_head_controller<ESP32S3>`  Component named `goblin_head_controller` using processor

**Multi-hardware component JSON** (expanded at build time):

When `tools/generate_tables.py` encounters `goblin_left_eye<GC9A01,OV2640>`, it:
1. Looks up `config/components/creature_specific/goblin_left_eye.json`
2. Finds template mapping for `GC9A01` and `OV2640`
3. Generates multi-hardware config struct
4. Passes all template arguments to `init()` and `act()`

**Component definition file** (`config/components/creature_specific/goblin_left_eye.json`):
```json
{
  "name": "goblin_left_eye",
  "positioned_component_type": "BIMODAL_EYE",
  "templates": {
    "display": {
      "GC9A01": {
        "role": "display",
        "device": "GC9A01",
        "args": {
          "resolution": "240x240",
          "color_depth": "16bit",
          "interface": "SPI"
        }
      },
      "ST7789": {
        "role": "display",
        "device": "ST7789",
        "args": {
          "resolution": "240x320",
          "color_depth": "16bit",
          "interface": "SPI"
        }
      }
    },
    "camera": {
      "OV2640": {
        "role": "camera",
        "device": "OV2640",
        "args": {
          "resolution": "160x120",
          "format": "RGB565",
          "interface": "I2C"
        }
      },
      "OV7670": {
        "role": "camera",
        "device": "OV7670",
        "args": {
          "resolution": "320x240",
          "format": "RGB565",
          "interface": "I2C"
        }
      }
    }
  },
  "software": {
    "init_func": "goblin_left_eye_init",
    "act_func": "goblin_left_eye_act"
  }
}
```

This means:
- `goblin_left_eye<GC9A01,OV2640>`  Uses GC9A01 display + OV2640 camera
- `goblin_left_eye<ST7789,OV7670>`  Uses ST7789 display + OV7670 camera  
- `goblin_left_eye<GC9A01>`  Uses only GC9A01 (no camera - single hardware)
- Both `2-tuple` and `1-tuple` declarations are valid

---

### 2. TEMPLATE ARGUMENT FLOW: BUILD SYSTEM TO RUNTIME

When `tools/generate_tables.py` encounters `goblin_left_eye<GC9A01,OV2640>`:

1. **Parse template list**: Extract `["GC9A01", "OV2640"]`
2. **Lookup template specifications** from `goblin_left_eye.json`
3. **Generate config struct** with all template args baked in
4. **Pass to init() and act()** as parameters

**Generated config struct** in `include/components/subsystems/goblin_head/goblin_left_eye_config.hpp`:

```cpp
/* Config for: goblin_left_eye<GC9A01,OV2640> */

typedef struct {
    /* GC9A01 display args */
    uint16_t display_resolution;
    uint8_t display_color_depth;
    spi_device_handle_t display_spi_handle;
    uint8_t display_cs_pin;
    uint8_t display_dc_pin;
    uint8_t display_rst_pin;
} GoblinLeftEyeDisplay_Args_t;

typedef struct {
    /* OV2640 camera args */
    uint16_t camera_resolution;
    uint8_t camera_format;
    i2c_port_t camera_i2c_port;
    uint8_t camera_i2c_address;
    uint8_t camera_power_pin;
} GoblinLeftEyeCamera_Args_t;

typedef struct {
    GoblinLeftEyeDisplay_Args_t display;
    GoblinLeftEyeCamera_Args_t camera;
} GoblinLeftEyeConfig_t;
```

**Dispatch function** in `src/components/subsystems/goblin_head/component_dispatch.cpp`:

```cpp
esp_err_t goblin_left_eye_init(void) {
    /* Template args from goblin_left_eye<GC9A01,OV2640> baked in at build time */
    
    static GoblinLeftEyeConfig_t config = {
        .display = {
            .display_resolution = 240,
            .display_color_depth = 16,
            .display_spi_handle = spi_display_handle,
            .display_cs_pin = GPIO_GOBLIN_EYE_CS,
            .display_dc_pin = GPIO_GOBLIN_EYE_DC,
            .display_rst_pin = GPIO_GOBLIN_EYE_RST
        },
        .camera = {
            .camera_resolution = 160,
            .camera_format = PIXFORMAT_RGB565,
            .camera_i2c_port = I2C_NUM_0,
            .camera_i2c_address = 0x30,
            .camera_power_pin = GPIO_GOBLIN_EYE_CAM_PWR
        }
    };
    
    return goblin_left_eye_init(&config);
}

void goblin_left_eye_act(void) {
    static GoblinLeftEyeConfig_t* config = NULL;
    if (!config) return;
    goblin_left_eye_act(config);
}
```

**Component receives config with all template args:**

```cpp
/* In goblin_left_eye.src */

esp_err_t goblin_left_eye_init(GoblinLeftEyeConfig_t* config) {
    /* config->display.display_resolution already populated (240) */
    /* config->camera.camera_resolution already populated (160) */
    return initialize_hardware(config);
}

void goblin_left_eye_act(GoblinLeftEyeConfig_t* config) {
    /* All template args available; no runtime discovery needed */
    display_frame(config->display, config->camera);
}
```

---

### 3. COMPONENT SOURCE FILE (config/components/creature_specific/goblin_left_eye.src)

**Component receives full template args via config struct - NEVER specifies hardware directly:**

The `.src` file is **hardware-agnostic**. It receives all configuration at runtime via the config struct. No hardware-specific code or conditionals.

```cpp
/* goblin_left_eye.src - Multi-hardware eye display+camera component */
/* NOTE: This file never mentions GC9A01, ST7789, OV2640, OV7670, etc. */
/* Hardware choice is made in bot composition (components:[] array) */

#include "component_configs.hpp"

static GoblinLeftEyeConfig_t* eye_config = NULL;
static uint32_t frame_count = 0;

esp_err_t goblin_left_eye_init(GoblinLeftEyeConfig_t* config) {
    ESP_LOGI(TAG, "Initializing goblin_left_eye with dual hardware");
    
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }
    
    eye_config = config;
    
    /* All template args already populated by dispatcher */
    /* We just use them - we don't know or care what hardware they are */
    
    /* Initialize first hardware role using generic interface */
    esp_err_t err = display_device_init(
        config->display.display_spi_handle,
        config->display.display_cs_pin,
        config->display.display_dc_pin,
        config->display.display_rst_pin
    );
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to init display role");
    
    /* Initialize second hardware role using generic interface */
    err = camera_device_init(
        config->camera.camera_i2c_port,
        config->camera.camera_i2c_address,
        config->camera.camera_power_pin
    );
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to init camera role");
    
    return ESP_OK;
}

void goblin_left_eye_act(GoblinLeftEyeConfig_t* config) {
    if (!config) return;
    
    /* Get emotion/mood from shared memory */
    Mood_t mood;
    GSM.read(MOOD_REGISTER, &mood, sizeof(mood));
    
    /* Capture frame from camera using its template args */
    uint8_t* frame_buffer = NULL;
    size_t frame_len = camera_device_capture(
        config->camera.camera_i2c_port,
        config->camera.camera_resolution,
        &frame_buffer
    );
    
    /* Process frame and detect emotion */
    uint16_t eye_color = detect_emotion_color(frame_buffer, frame_len, mood);
    
    /* Render to display using generic interface */
    display_device_clear(config->display.display_spi_handle, eye_color);
    display_device_draw_circle(
        config->display.display_spi_handle,
        120, 120, 100,
        BLEND_COLOR(eye_color, mood.intensity)
    );
    
    frame_count++;
}
```

**Key principle**: The component uses **generic hardware abstraction functions** (`display_device_init()`, `camera_device_init()`, etc.) that work with ANY hardware in that category. The actual hardware type is specified ONLY in the `components:[]` array.



---

### 4. TEMPLATE SYNTAX IN COMPONENTS ARRAYS

Components are referenced in `components:[]` arrays using **template syntax**. The build system parses this syntax to generate proper hardware configs.

**Syntax**: `ComponentName<TemplateArg1,TemplateArg2,...>` or `ComponentName` (no templates)

**Real-world examples from a bot definition:**

```json
{
  "name": "goblin_full",
  "subsystems": {
    "head": {
      "components": [
        "goblin_left_eye<GC9A01,OV2640>",
        "goblin_right_eye<GC9A01,OV2640>",
        "goblin_mouth<GC9A01,MINISPKR>",
        "goblin_jaw<SERVO>",
        "sdi_display_bus",
        "goblin_head_controller<ESP32S3>"
      ]
    },
    "torso": {
      "components": [
        "goblin_chest_led_matrix<WS2812B>",
        "goblin_arm_left<SERVO,SERVO>",
        "goblin_arm_right<SERVO,SERVO>",
        "goblin_limb_controller<ESP32C3>"
      ]
    }
  }
}
```

**Component mapping:**
- `goblin_left_eye<GC9A01,OV2640>`  Component `goblin_left_eye` + templates `GC9A01` (display) + `OV2640` (camera)
- `goblin_mouth<GC9A01,MINISPKR>`  Component `goblin_mouth` + templates `GC9A01` (display) + `MINISPKR` (speaker)
- `goblin_jaw<SERVO>`  Component `goblin_jaw` + template `SERVO` (single actuator)
- `sdi_display_bus`  Component `sdi_display_bus` (pure driver, no hardware templates)
- `goblin_head_controller<ESP32S3>`  Component `goblin_head_controller` + template `ESP32S3` (processor)

**Build system processing:**

1. Parse `goblin_left_eye<GC9A01,OV2640>`
2. Lookup `config/components/creature_specific/goblin_left_eye.json`
3. Extract template mappings for `GC9A01` and `OV2640` 
4. Generate `GoblinLeftEyeConfig_t` struct with all args populated
5. Generate dispatch function that calls `goblin_left_eye_init(&config)` and `goblin_left_eye_act(&config)`

---

### 5. COMPONENT JSON DEFINITION SCHEMA

Each component's JSON file defines the available templates and their parameters:

**Example: goblin_left_eye.json**

```json
{
  "name": "goblin_left_eye",
  "positioned_component_type": "BIMODAL_EYE",
  "templates": {
    "display": {
      "GC9A01": {
        "role": "display",
        "device": "GC9A01",
        "args": {
          "resolution": "240x240",
          "color_depth": "16bit",
          "interface": "SPI",
          "spi_speed": "40MHz"
        }
      },
      "ST7789": {
        "role": "display",
        "device": "ST7789",
        "args": {
          "resolution": "240x320",
          "color_depth": "16bit",
          "interface": "SPI",
          "spi_speed": "40MHz"
        }
      }
    },
    "camera": {
      "OV2640": {
        "role": "camera",
        "device": "OV2640",
        "args": {
          "resolution": "160x120",
          "format": "RGB565",
          "interface": "I2C",
          "i2c_speed": "400kHz"
        }
      },
      "OV7670": {
        "role": "camera",
        "device": "OV7670",
        "args": {
          "resolution": "320x240",
          "format": "RGB565",
          "interface": "I2C",
          "i2c_speed": "400kHz"
        }
      }
    }
  },
  "software": {
    "init_func": "goblin_left_eye_init",
    "act_func": "goblin_left_eye_act"
  }
}
```

**Schema structure in master_rules.json:**

```json
{
  "component_template_schema": {
    "description": "Component template definitions for multi-hardware support",
    "properties": {
      "name": {
        "type": "string",
        "description": "Component name (matches directory/filename)"
      },
      "templates": {
        "type": "object",
        "description": "Template categories (display, camera, speaker, etc.)",
        "additionalProperties": {
          "type": "object",
          "description": "Template implementations grouped by role",
          "additionalProperties": {
            "type": "object",
            "required": ["role", "device", "args"],
            "properties": {
              "role": {
                "type": "string",
                "description": "Semantic role (display, camera, speaker, sensor, etc.)"
              },
              "device": {
                "type": "string",
                "description": "Hardware device identifier"
              },
              "args": {
                "type": "object",
                "description": "Template arguments passed to init()/act()"
              }
            }
          }
        }
      }
    }
  }
}
```

---

## IMPLEMENTATION ROADMAP

### Phase 1: Build System Support (CURRENT FOCUS)

Get multi-hardware component system working with `goblin_left_eye` as proof-of-concept.

- [ ] Update `tools/generate_tables.py` to:
  - Parse template syntax from `components:[]` arrays (e.g., `goblin_left_eye<GC9A01,OV2640>`)
  - Extract template list from angle brackets
  - Lookup component JSON and resolve template definitions
  - Generate multi-hardware config structs
  - Generate dispatch functions with config struct initialization

- [ ] Create generator for `include/components/subsystems/{subsystem}/{component}_config.hpp`:
  - Define role-specific args structs (e.g., `GoblinLeftEyeDisplay_Args_t`)
  - Define combined config struct (e.g., `GoblinLeftEyeConfig_t`)
  - Generate struct field names matching template args

- [ ] Update `src/components/subsystems/{subsystem}/component_dispatch.cpp` generator:
  - Generate `{component}_init(void)` wrapper that populates config struct
  - Generate `{component}_act(void)` wrapper that calls component functions
  - Embed template args as static initializers

- [ ] Implement `config/components/creature_specific/goblin_left_eye.json` with GC9A01 + OV2640 templates
- [ ] Implement `config/components/creature_specific/goblin_left_eye.src` using generic abstraction functions
- [ ] Test build with new template syntax
- [ ] **GOAL**: Prove that multi-hardware templates work and establish best practices

### Phase 2: Personality & Mood System (FUTURE)

Once Phase 1 proves the multi-hardware template system:

- [ ] Create family-level personality modules (`goblin_personality.json`, `bear_personality.json`, etc.)
- [ ] Design personality trait system (aggression, playfulness, intelligence, fear_response, etc.)
- [ ] Create `mood_adjuster` component that reads personality and modifies base mood
- [ ] Extract truly universal logic from `goblin_left_eye`  `creature_eye`
- [ ] Make `creature_eye` work with any creature family via personality injection

### Phase 3: Creature Abstraction (FUTURE)

- [ ] Replace all creature-specific eye/mouth/etc. components with universal versions
- [ ] Test across all creature families
- [ ] Measure code size savings (should be significant)

---

## BACKWARD COMPATIBILITY

**Single-hardware components remain unchanged:**

```json
{
  "name": "simple_speaker",
  "hardware_template": "config/components/hardware/speaker.json",
  "software": {
    "init_func": "simple_speaker_init",
    "act_func": "simple_speaker_act"
  }
}
```

**Build system auto-detects:**
- Old format (single `hardware_template`)  single-hardware dispatch
- New format (array `hardware_templates`)  multi-hardware config struct

---

## FUTURE EXTENSIBILITY

This design supports:
- **2 hardware devices**: Current scope
- **3+ hardware devices**: Arrays extend naturally
- **Hardware swapping at runtime**: Config struct can be updated
- **Conditional hardware**: Role marked `required: false` for optional sensors
- **Hardware cascading**: Fallback specs if primary hardware fails
- **Dynamic hardware discovery**: Config can be populated at init-time

---

## FUTURE: PERSONALITY & CREATURE-AGNOSTIC COMPONENTS

### Vision: Universal Components with Family-Level Personality

**Current problem**: We have creature-specific components (`goblin_eye`, `bear_eye`, `dragon_eye`) that are essentially identical except for the name and mood calculation.

**Proposed solution**: 
1. Create **family-level personality modules** (e.g., `goblin_personality.json`) that define how creature traits affect behavior
2. Create **universal, reusable components** (e.g., `creature_eye`, `creature_mouth`) that accept personality as input
3. Use **Mood adjustment components** that calculate final mood from base mood + personality traits

**Example architecture**:

```json
{
  "name": "goblin_family",
  "personality": {
    "aggression": 0.8,
    "playfulness": 0.6,
    "intelligence": 0.7,
    "fear_response": 0.5
  },
  "subsystems": {
    "head": {
      "components": [
        "goblin_personality<FAMILY>",
        "creature_eye<GC9A01,OV2640>",
        "creature_mouth<GC9A01,MINISPKR>",
        "mood_adjuster<goblin_personality>"
      ]
    }
  }
}
```

**Benefits:**
- **Single eye component** works for all creatures (goblin, bear, dragon, etc.)
- **Personality module** defines creature-specific behavior modification
- **Mood adjuster** applies personality traits to calculate final mood
- **Reuse across families**: Same components for different creatures

**Implementation notes:**
- This is a FUTURE enhancement after multi-hardware components are working
- Start with current `goblin_left_eye` to validate multi-hardware template system
- Once working, extract universal `creature_eye` component
- Add personality layer as second phase

### Current Focus

**PHASE 1 (NOW)**: Get `goblin_left_eye<GC9A01,OV2640>` working with multi-hardware templates.

This will establish:
- How config structs flow from build system to components
- How generic hardware abstraction functions work
- Best practices for hardware-agnostic component logic

**PHASE 2 (LATER)**: Extract universal `creature_eye` component and personality system.

Once PHASE 1 is proven, we'll have the foundation to:
- Identify truly universal logic in `goblin_left_eye`
- Design personality trait system
- Implement mood adjustment layer
- Roll out to all creatures

### Personality Module Structure (Phase 2 Specification)

**Family-level personality JSON** (`config/components/system/goblin_personality.json`):

```json
{
  "name": "goblin_personality",
  "type": "PERSONALITY_MODULE",
  "traits": {
    "aggression": {
      "value": 0.8,
      "range": [0.0, 1.0],
      "affects": ["eye_color_intensity", "mouth_movement_speed"]
    },
    "playfulness": {
      "value": 0.6,
      "range": [0.0, 1.0],
      "affects": ["blink_frequency", "eye_animation_style"]
    },
    "intelligence": {
      "value": 0.7,
      "range": [0.0, 1.0],
      "affects": ["decision_complexity"]
    },
    "fear_response": {
      "value": 0.5,
      "range": [0.0, 1.0],
      "affects": ["eye_dilation", "freeze_duration"]
    }
  },
  "software": {
    "init_func": "goblin_personality_init",
    "act_func": "goblin_personality_act"
  }
}
```

**Mood Adjustment Component** (`config/components/system/mood_adjuster.json`):

```json
{
  "name": "mood_adjuster",
  "type": "MOOD_MODIFIER",
  "description": "Applies family personality traits to base mood calculation",
  "hardware_templates": [],
  "software": {
    "init_func": "mood_adjuster_init",
    "act_func": "mood_adjuster_act"
  }
}
```

**Usage in bot definition**:

```json
{
  "name": "goblin_full",
  "subsystems": {
    "head": {
      "components": [
        "goblin_personality",
        "mood_adjuster<goblin_personality>",
        "creature_eye<GC9A01,OV2640>",
        "creature_mouth<GC9A01,MINISPKR>"
      ]
    }
  }
}
```

**Component implementation pattern**:

- `goblin_personality` reads personality traits into shared memory
- `mood_adjuster` takes base mood + personality  calculates adjusted mood
- `creature_eye` reads adjusted mood (no personality knowledge needed)
- All creature-specific logic lives in personality module, not in component
- Same `creature_eye` component works for goblin, bear, dragon, etc.

---

## SUMMARY

 **Hardware specified at composition level ONLY**: No hardware names in `.src` files  
 **Single source of hardware truth**: The `components:[]` array in bot JSON  
 **Hardware-agnostic component logic**: Same component works with any compatible hardware  
 **Type safety**: Generated C structs for each multi-hardware combination  
 **Build-time optimization**: Template args embedded directly in dispatch  
 **Extensible**: Scales to 2, 3, or N hardware devices  
 **Backward compatible**: Existing single-hardware components unaffected  
 **Real-time safe**: All config determined at init(), no allocations in act()  
 **Easy hardware swapping**: Change hardware by editing one line in bot JSON  

**Future vision (Phase 2+):**
 **Universal creature components**: Single `creature_eye` for all creatures  
 **Family-level personality**: Each creature family has personality module with traits  
 **Personality-driven behavior**: Components read personality from GSM, adapt behavior  
 **Massive code reuse**: Eliminate goblin_eye, bear_eye, dragon_eye  one creature_eye

---

## STEPPER MOTOR HARDWARE ABSTRACTION: KEY WIN

**The Problem**: There are DOZENS of stepper motor variants in the wild:
- **Form factors**: NEMA17, NEMA23, NEMA34, planetary gearboxes, custom winding
- **Drive modes**: Bipolar, Unipolar, with different current ratings
- **Microstepping drivers**: DRV8834, ULN2003, A4988, DRV8825, TMC2209, etc.
- **Mechanical variants**: Different gear ratios, holding torque, speed curves

**Traditional problem**: Component code gets polluted with conditional logic:

```cpp
// BAD - Hardware-specific in component
#ifdef NEMA17_STEPPER
  nema17_step(pin_step, pin_dir);
#elif NEMA23_STEPPER
  nema23_step(pin_step, pin_dir);
#else
  generic_step(...);
#endif

// Multiply this across ALL components that use any motor...
// Recompile everything to try a different motor
```

**This architecture's solution**: Change stepper motor with ONE LINE in bot JSON:

```json
// Try NEMA17 with DRV8834
"components": ["goblin_jaw<NEMA17_STEPPER,DRV8834>"]

// Switch to NEMA23 for more torque - NO CODE CHANGES
"components": ["goblin_jaw<NEMA23_STEPPER,DRV8834>"]

// Try different driver for speed
"components": ["goblin_jaw<NEMA17_STEPPER,TMC2209>"]

// Planetary gearbox variant for speed reduction
"components": ["goblin_jaw<NEMA17_PLANETARY,DRV8834>"]
```

**Component code stays CLEAN:**

```cpp
// GOOD - Hardware-agnostic in component
void goblin_jaw_act(GoblinJawConfig_t* config) {
    // We DON'T KNOW what stepper we're using
    // We just use the generic interface
    
    if (should_move_jaw) {
        stepper_motor_step(
            config->motor.step_pin,
            config->motor.direction_pin,
            config->motor.steps_per_rotation
        );
    }
}
```

**The WIN**: 
- **Instant experimentation**: Try 10 different stepper variants without recompilation
- **Prototyping speed**: Iterate on mechanical design without code changes
- **Production optimization**: Swap components when you find the right balance of cost/torque/speed
- **Testing**: Same component tested against ALL stepper variants
- **Maintenance**: Bug fix in one place applies to ALL stepper configurations
- **Code quality**: No #ifdefs, no conditional spaghetti - just clean logic

This is exactly why this architecture matters: **Hardware diversity without code bloat**.

---

  

