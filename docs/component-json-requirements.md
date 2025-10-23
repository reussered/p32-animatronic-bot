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

**Example**: `goblin_head.json` contains multiple `*_components` fields that automatically compose the complete head subsystem from individual positioned components.

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