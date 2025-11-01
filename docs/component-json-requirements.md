# P32 Component JSON Structure Requirements & Parsing Rules

## 🚨 CRITICAL ENCODING REQUIREMENTS

### **ASCII ENCODING MANDATORY (NO UTF-8 BOM)**
**ALL JSON configuration files MUST be saved as pure ASCII without UTF-8 BOM.**

**PROBLEM:**
- UTF-8 BOM (bytes `EF BB BF`) at start of file breaks Python JSON parser
- Error: "Expecting value: line 1 column 1 (char 0)"
- We've debugged this multiple times

**CORRECT FILE FORMAT:**
```powershell
# Save JSON as ASCII without BOM
[System.IO.File]::WriteAllText($path, $content, [System.Text.Encoding]::ASCII)
```

**DETECTION:**
```powershell
# Check for BOM corruption
$bytes = [System.IO.File]::ReadAllBytes($jsonFile)
if ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
    Write-Host "UTF-8 BOM DETECTED - File is corrupted!" -ForegroundColor Red
}
```

**Validation Rules:**
- Always use ASCII encoding
- Never use UTF-8 with BOM
- Verify first 3 bytes are NOT `239, 187, 191`
- First byte should be `123` (the `{` character)

## 📋 JSON STRUCTURE REQUIREMENTS

### **Mandatory Fields (All JSON Files)**
```json
{
  "relative_filename": "config/components/positioned/component_name.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "name": "unique_component_name",
  "type": "POSITIONED_COMPONENT",
  "description": "Human readable description",
  "timing": {
    "hitCount": 10
  },
  "software": {
    "init_function": "component_name_init",
    "act_function": "component_name_act"
  }
}
```

### **Component-Specific Fields**

1. **Component Name**: `"name": "left_eye"` - Used to generate function names
2. **Function Names**: `init_function` and `act_function` (or "STUB" if not needed)
3. **Loop Count Modulus**: `"timing": {"hitCount": 5}` - How often component executes (every N loops)  
4. **Description**: Human-readable description for generated comments
5. **Component Type**: `"type": "POSITIONED_COMPONENT"` - Categorizes component (DISPLAY_DRIVER, MICRO_CONTROLLER_MODULE, etc.)

### **Reference Pattern**
- Use `"author": "config/author.json"` for consistent metadata across all configs
- Use relative paths for all file references

## Standard Function Name Generation Pattern
- Init functions: `p32_{component_name}_init()`
- Action functions: `p32_{component_name}_act(uint32_t loopCount)`

## Component Categories

📘 **[Complete Architecture Specification](THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md)**

- **System Level**: Serial, WiFi, Bluetooth - always present, low hitCount values (50-500)
- **Family Level**: Personality, mood, behavior - mid-range hitCount values (10-50)
- **Bot Specific**: Eyes, mouth, sensors - higher frequency, low hitCount values (3-30)

**CRITICAL**: NOTHING executes unless it's a component with init() and act() functions.

## Timing Guidelines

- System heartbeat: hitCount = 1 (every loop)
- Network/WiFi: hitCount = 50+ (slow)
- Personality: hitCount = 25 (moderate)
- Mood: hitCount = 10-15 (frequent)
- Animations: hitCount = 3-7 (fast)
- Sensors: hitCount = 10-20 (responsive)

## 💻 C++ JSON PARSING PATTERNS

### **Standard cJSON Implementation**
```cpp
#include <cJSON.h>

// Standard parsing pattern
esp_err_t parse_component_config(const char* json_string) {
    cJSON *config = cJSON_Parse(json_string);
    if (!config) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Get string values
    cJSON *name_item = cJSON_GetObjectItem(config, "name");
    const char *component_name = name_item->valuestring;
    
    // Get numeric values
    cJSON *timing = cJSON_GetObjectItem(config, "timing");
    cJSON *hitcount_item = cJSON_GetObjectItem(timing, "hitCount");
    int hitCount = hitcount_item->valueint;
    
    # P32 Component JSON & Generation Rules

    This document is the single source of truth for how JSON configuration drives code generation, subsystem construction, and dynamic pin assignment in the P32 Animatronic Bot project.

    ---

    ## 1. Encoding & File Hygiene

    - **ASCII only**: Save every JSON file as ASCII without a UTF-8 BOM. The first byte must be `{` (0x7B).
    - **Detection snippet**:

      ```powershell
      $bytes = [System.IO.File]::ReadAllBytes($jsonFile)
      if ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
          Write-Host "UTF-8 BOM detected" -ForegroundColor Red
      }
      ```

    - Always validate after edits using `python config/validate.py`.

    ---

    ## 2. JSON Structure Requirements

    ### 2.1 Mandatory Fields

    ```json
    {
      "relative_filename": "config/components/positioned/component.json",
      "version": "1.0.0",
      "author": "config/author.json",
      "name": "unique_component_name",
      "type": "POSITIONED_COMPONENT",
      "description": "Human readable description",
      "timing": {
        "hitCount": 10
      }
    }
    ```

    - `name` must be globally unique and forms `{name}_init` / `{name}_act`.
    - `type` categorises the component (DISPLAY_DRIVER, HARDWARE_DEVICE, etc.).
    - `timing.hitCount` controls how often the `act()` function executes.

    ### 2.2 Referencing Other Components

    - Use a single containment keyword: `"components": [ ... ]`.
    - All legacy patterns (`*_components`, `contained_components`, etc.) are fatal errors.
    - Paths are always relative to project root.

    ### 2.3 Additional Metadata

    - Optional sections (`software`, `shape_assembly`, `position`, `hardware_reference`, etc.) appear as needed.
    - Every JSON keyword **except** `components` becomes part of a generated C-style struct during header generation (see §4).

    ---

    ## 3. Component Traversal & Dispatch Semantics

    - The generator performs a depth-first walk of the `components` arrays, preserving JSON order.
    - Every visit adds an entry to the subsystem dispatch tables:
      - `initTable[]` stores `{component}_init` pointers.
      - `actTable[]` stores `{component}_act` pointers.
      - `hitCount[]` stores `timing.hitCount` values.
    - **Duplicates are intentional**. If `goblin_eye` appears twice, it is queued twice in all three tables, ensuring both instances execute.

    ---

    ## 4. Component Aggregation Outputs

    Each unique component contributes to generated sources exactly once. Subsequent references skip regeneration to avoid duplicate symbols.

    ### 4.1 Aggregated Implementation (`{subsystem}_component_functions.cpp`)

    - Concatenates the permanent `config/components/{component}.src` file on first encounter.
    - Holds the actual `init()`/`act()` definitions used by the subsystem.

    ### 4.2 Generated Header (`{subsystem}_component_functions.hpp`)

    - For the first encounter of a component:
      - Declares `esp_err_t {component}_init(void);`
      - Declares `void {component}_act(void);`
      - Emits nested structs mirroring the component’s JSON keywords (excluding `components`).
    - Literal JSON values appear as default initialisers inside those structs, giving compile-time access to configuration.

    ---

    ## 5. Subsystem Boundaries & Controller Keyword

    Encountering a `controller` keyword marks a subsystem boundary and triggers the creation of six files named after the component’s `name` field:

    1. `src/subsystems/{name}/{name}_component_functions.cpp`
    2. `include/subsystems/{name}/{name}_component_functions.hpp`
    3. `src/subsystems/{name}/{name}_dispatch_tables.cpp`
    4. `include/subsystems/{name}/{name}_dispatch_tables.hpp`
    5. `src/subsystems/{name}/{name}_main.cpp`
    6. `include/subsystems/{name}/{name}_main.hpp`

    `{name}_main.cpp` includes the dispatch tables and component-functions headers, providing the subsystem’s local main loop. All generated files are appended to `CMakeLists.txt` so PlatformIO/ESP-IDF builds pick them up automatically.

    After the full JSON tree finishes, the generator also produces or updates a dedicated `platformio.ini` (or environment entry) for each subsystem using the board defined by `controller`.

    ---

    ## 6. Dynamic Pin Assignment (Consolidated Rules)

    ### 6.1 Runtime Philosophy

    - Pins are claimed during `init()` only and remain assigned for the life of the program.
    - Assignments reflect physical wiring; releasing pins is forbidden.
    - `assigned_pins[]` (shared across bus types) prevents two components from claiming the same GPIO.
    - Components claim pins in the order their `init()` functions run (same order as dispatch traversal).

    ### 6.2 Controller-Specific Pin Pools

    When a subsystem declares a `controller`, the generator loads that board’s pin capabilities and constructs assignable arrays:

    - `spi_assignable[]`
    - `i2c_assignable[]`
    - `i2s_assignable[]`
    - `adc_assignable[]`
    - `pwm_assignable[]`
    - `gpio_assignable[]`

    Arrays are ordered according to the historical `PIN_ASSIGNMENT_RULES.md`:

    1. Pins with the fewest alternate capabilities first.
    2. Multiply-capable pins (SPI/I2C/PWM/ADC capable) pushed to the end so they are consumed last.

    ### 6.3 Assignment Helpers

    Example helper (simplified):

    ```cpp
    int assign_pin(int *pool, size_t pool_size) {
        for (size_t i = 0; i < pool_size; ++i) {
            if (!is_assigned(pool[i])) {
                mark_assigned(pool[i]);
                return pool[i];
            }
        }
        return -1; // No free pins remain
    }
    ```

    `spi_bus_init()` (and analogous helpers) pull from these arrays and record the allocation. They are invoked exactly once per hardware interface instance—e.g., two `goblin_eye` displays mean two SPI allocations.

    ### 6.4 Alias Resolution

    Hardware JSON files may define `pin_mapping` blocks listing alternate manufacturer names (e.g., `SDA`, `DIN`, `MOSI`). During generation, the struct for that component exposes canonical names so init code can map aliases to the pins claimed at runtime.

    ### 6.5 Verification Checklist (Test Bots)

    For quick hardware validation (e.g., `tests/test_bot`):

    1. Run `python tools/generate_tables.py tests/test_bot src`.
    2. Inspect the generated `{subsystem}_component_functions.cpp` to confirm the SPI allocations align with the expected `spi_assignable` ordering.
    3. Ensure both `goblin_left_eye` and `goblin_right_eye` log/publish their assigned pins before applying full power.

    ---

    ## 7. cJSON Parsing Pattern (For Manual Code)

    ```cpp
    #include <cJSON.h>

    esp_err_t parse_component(const char *json) {
        cJSON *root = cJSON_Parse(json);
        if (!root) {
            ESP_LOGE(TAG, "Parse error");
            return ESP_ERR_INVALID_ARG;
        }

        const cJSON *name = cJSON_GetObjectItem(root, "name");
        const cJSON *timing = cJSON_GetObjectItem(root, "timing");
        const cJSON *hit = cJSON_GetObjectItem(timing, "hitCount");

        if (!cJSON_IsString(name) || !cJSON_IsNumber(hit)) {
            cJSON_Delete(root);
            return ESP_ERR_INVALID_ARG;
        }

        // Use values...
        cJSON_Delete(root);
        return ESP_OK;
    }
    ```

    Always guard against missing keys or wrong types when writing manual parsers.

    ---

    ## 8. Validation Checklist

    - [ ] File saved as ASCII without BOM.
    - [ ] `components` is the only containment keyword.
    - [ ] `timing.hitCount` present on every executable component.
    - [ ] `controller` declared for every subsystem boundary.
    - [ ] Generated dispatch tables contain expected duplicates.
    - [ ] Generated PlatformIO environments match controller hardware.
    - [ ] Pin assignments verified against hardware wiring before applying power.

    ---

    ## 9. Reference Documents (Superseded but Historical)

    - `DYNAMIC_PIN_ASSIGNMENT_SYSTEM.md`
    - `PIN_ASSIGNMENT_RULES.md`
    - `COMPONENT-PIN-ALIASING-SPEC.md`

    These documents are maintained for historical context, but all active rules now live here.
