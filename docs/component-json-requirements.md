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
  "component_name": "unique_component_name",
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
1. **Component Name**: `"component_name": "left_eye"` - Used to generate function names
2. **Function Names**: `init_function` and `act_function` (or "STUB" if not needed)
3. **Loop Count Modulus**: `"timing": {"hitCount": 5}` - How often component executes (every N loops)  
4. **Description**: Human-readable description for generated comments
5. **Component Type**: system/positioned/family level categorization

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
    cJSON *name_item = cJSON_GetObjectItem(config, "component_name");
    const char *component_name = name_item->valuestring;
    
    // Get numeric values
    cJSON *timing = cJSON_GetObjectItem(config, "timing");
    cJSON *hitcount_item = cJSON_GetObjectItem(timing, "hitCount");
    int hitCount = hitcount_item->valueint;
    
    // Cleanup
    cJSON_Delete(config);
    return ESP_OK;
}
```

### **Array Iteration Pattern**
```cpp
// Process array of objects
cJSON *array = cJSON_GetObjectItem(config, "components");
cJSON *element = NULL;
cJSON_ArrayForEach(element, array) {
    const char *name = cJSON_GetObjectItem(element, "name")->valuestring;
    int value = cJSON_GetObjectItem(element, "value")->valueint;
    // Process each element
}
```

### **Error Handling Requirements**
```cpp
// Always check for NULL before accessing cJSON values
cJSON *item = cJSON_GetObjectItem(config, "field");
if (!item) {
    ESP_LOGE(TAG, "Missing required field: %s", "field");
    return ESP_ERR_NOT_FOUND;
}

// Validate data types
if (!cJSON_IsString(item)) {
    ESP_LOGE(TAG, "Field 'field' must be string");
    return ESP_ERR_INVALID_ARG;
}
```

## ✅ VALIDATION REQUIREMENTS

### **Automated Validation Tools**
- **Syntax Check**: Use `config/validate.py` for JSON syntax validation
- **Structure Check**: Ensure consistent keys within folder groups
- **BOM Detection**: Automated checking for UTF-8 BOM corruption
- **Reference Validation**: Verify `"author": "config/author.json"` paths exist

### **Validation Script Usage**
```powershell
# Run validation on all config files
python config/validate.py

# Check specific folder for consistency
python config/validate_families.py
```

### **Manual Validation Checklist**
- [ ] File saved as ASCII without BOM
- [ ] JSON syntax is valid (no trailing commas, proper quotes)
- [ ] All mandatory fields present (`component_name`, `hitCount`, etc.)
- [ ] `relative_filename` matches actual file path
- [ ] Referenced files exist (`author.json`, etc.)
- [ ] Function names follow naming convention

## JSON Structure Template
```json
{
  "relative_filename": "config/components/positioned/component_name.json",
  "component_name": "unique_component_name",
  "description": "Human readable description",
  "timing": {
    "hitCount": 10
  },
  "custom_functions": {
    "init_func": "p32_custom_init", 
    "act_func": "p32_custom_act"
  }
}
```

## Component Composition Rules

### Wildcard Component Parsing
The P32 component generation system automatically detects component composition using wildcard patterns:

**Pattern**: Any JSON field ending with `*_components` triggers recursive component inclusion
- `"left_eye_components": "config/components/positioned/goblin_left_eye.json"`
- `"right_eye_components": "config/components/positioned/goblin_right_eye.json"`
- `"subsystem_components": "config/components/assemblies/goblin_head.json"`

**Processing**: Component generator parses these fields to:
1. Load referenced JSON configuration
2. Extract component hierarchy and dependencies  
3. Generate dispatch table entries (init/act functions)
4. Handle duplicate component detection across composition tree
5. Create proper include statements for generated source code

## Bus Interface Pin Requirements

### Bus Types and Pin Assignment

The P32 system supports multiple bus interface types for hardware communication. Each bus type has specific pin requirements that are validated during code generation to ensure compatibility with the target ESP32 chip.

#### Supported Bus Types

1. **SPI Bus** (`bus_type: "SPI"`)
   - **Shared Pins**: SCLK (bit clock), MISO (master in, slave out)
   - **Unique Pins**: CS (chip select), MOSI (master out, slave in) per device
   - **Example**: `spi_bus.json` - Generic SPI bus interface

2. **I2S Bus** (`bus_type: "I2S"`)
   - **Shared Pins**: BCLK (bit clock), WS (word select)
   - **Unique Pins**: DATA (data line) per device/direction
   - **Example**: `i2s_bus.json` - Generic I2S bus interface for audio

3. **I2C Bus** (`bus_type: "I2C"`)
   - **Shared Pins**: SCL (serial clock), SDA (serial data)
   - **Unique Pins**: None (address-based multiplexing)
   - **Note**: Devices share SDA/SCL lines, addressed via software

4. **ADC Bus** (`bus_type: "ADC"`)
   - **Shared Pins**: None
   - **Unique Pins**: ADC input pins per channel
   - **Note**: Analog-to-digital conversion channels

5. **PWM Bus** (`bus_type: "PWM"`)
   - **Shared Pins**: None
   - **Unique Pins**: PWM output pins per channel
   - **Note**: Pulse-width modulation for servo/LED control

6. **GPIO Pairs** (`bus_type: "GPIO_PAIR"`)
   - **Shared Pins**: None
   - **Unique Pins**: 2 GPIO pins per pair (input/output)
   - **Note**: Simple digital I/O pairs with no shared resources

#### Pin Requirements JSON Structure

Bus interface components must include a `pin_requirements` object:

```json
{
  "pin_requirements": {
    "shared_pins_needed": [
      {
        "function": "SPI_SCLK",
        "count": 1,
        "description": "SPI serial clock"
      }
    ],
    "unique_pins_needed": [
      {
        "function": "SPI_CS",
        "count": 1,
        "description": "Chip select (unique per device)"
      }
    ]
  }
}
```

#### Pin Validation Algorithm

During code generation, the system validates total pin requirements against the ESP32 chip's available pins:

1. **Count Bus Encounters**: For each component loop iteration, count how many times each bus type is encountered
2. **Calculate Shared Pins**: Shared pins are assigned once per bus type (e.g., SPI SCLK assigned once for all SPI devices)
3. **Calculate Unique Pins**: Unique pins are assigned per device (e.g., SPI CS pin per SPI device)
4. **Total Pins Needed**: `shared_pins + (num_devices × unique_pins_per_device)`
5. **Validate Against Chip**: Ensure total pins ≤ chip's `exposed_pins` array length
6. **Runtime Simulation**: Pin assignment simulates `active_pin_index` resetting per component loop

**Example Calculation**:
- Goblin head: 5 SPI devices
- SPI bus: 2 shared pins (SCLK, MISO) + 2 unique pins per device (CS, MOSI)
- Total pins: 2 + (5 × 2) = 12 pins
- ESP32-S3 has 45 exposed pins → Validation passes

### Hardware Validation Rules

- All pin assignments must be within the chip's `exposed_pins` array
- Shared pins are allocated once per bus type across all components
- Unique pins are allocated per device instance
- Pin conflicts are detected during generation, not runtime
- GPIO pairs require exactly 2 unique pins with no shared resources

## Files Updated So Far
- `goblineye_left.json` - left_eye, hitCount: 5
- `goblineye_right.json` - right_eye, hitCount: 5  
- `goblin_mouth.json` - mouth, hitCount: 3
- `goblin_speaker.json` - audio, hitCount: 7
- `goblin_nose.json` - nose_sensor, hitCount: 15

## Next Steps
1. Create `goblin_simple.json` bot configuration
2. Test generator with simple bot
3. Build and verify P32 universal main loop
4. Systematically update all component JSON files