# Topic 9: Common Build Errors

**Purpose**: Diagnose and fix frequent build failures with PowerShell diagnostics.

---

## Error Categories

1. **Multiple Definition Errors** - Symbol declared in multiple .src files
2. **Missing Symbol Errors** - Component not found or init/act missing
3. **Linker Failures** - RAM/Flash overflow or undefined references
4. **JSON Configuration Errors** - Malformed config or missing components

---

## 1. Multiple Definition Errors

### Symptom

```
ld: error: multiple definition of 'display_buffer'
```

### Causes

| Cause | Why It Happens |
|-------|----------------|
| Variable declared in .hdr without `static` | .hdr content goes into .hpp, included by all .src files |
| Variable declared in multiple .src files | All .src files compile into one .cpp (same scope) |
| Forgot to use `static` for file-scope variables | Non-static = external linkage = multiple definition |

### Diagnostic Command

```powershell
pio run -e goblin_head 2>&1 | Select-String "multiple definition" -Context 0,2 | Select-Object -First 5
```

### Fix

**DO**: Declare shared variables in ONE .src file with `static`

```cpp
// In first_component.src (top of file)
static uint16_t* display_buffer = NULL;
static size_t buffer_size = 0;
```

**DON'T**: Declare in .hdr or in multiple .src files

```cpp
// WRONG: In component.hdr
uint16_t* display_buffer;  // Goes into .hpp, multiple inclusion

// WRONG: In multiple .src files
// first_component.src
static uint16_t* display_buffer = NULL;

// second_component.src
static uint16_t* display_buffer = NULL;  // Different variable!
```

### Prevention

- Declare file-scope variables ONCE at top of the first .src file that needs them
- Use `static` for all intra-subsystem shared data
- Never declare variables in .hdr files (declarations only)

---

## 2. Missing Symbol Errors

### Symptom A: Undefined Reference

```
undefined reference to `goblin_eye_init'
undefined reference to `goblin_eye_act'
```

### Causes

| Cause | Fix |
|-------|-----|
| Component name in JSON doesn't match function names | JSON: `"name": "goblin_eye"` must have `goblin_eye_init()` and `goblin_eye_act()` |
| Functions declared but not defined | Implement both functions in .src file |
| Typo in function name | Match exactly: `{component_name}_init` / `{component_name}_act` |

### Diagnostic Commands

```powershell
# Find missing init/act functions
python tools/generate_tables.py goblin_head src 2>&1 | Select-String "ERROR|Warning"

# Check generated dispatch table
Select-String -Path "src/subsystems/goblin_head.cpp" -Pattern "init|act" | Select-Object -First 20
```

### Fix

**DO**: Match JSON name exactly to function names

```json
// In component JSON
{
  "name": "goblin_eye",
  "timing": {"hitCount": 1}
}
```

```cpp
// In goblin_eye.src
esp_err_t goblin_eye_init(void)
{
    ESP_LOGI(TAG, "Eye init");
    return ESP_OK;
}

void goblin_eye_act(void)
{
    // Implementation
}
```

**DON'T**: Mismatch names or omit functions

```cpp
// WRONG: Name mismatch
esp_err_t goblin_display_init(void)  // JSON says "goblin_eye"
{
    return ESP_OK;
}

// WRONG: Missing act function
esp_err_t goblin_eye_init(void)
{
    return ESP_OK;
}
// goblin_eye_act() missing!
```

---

### Symptom B: Component Not Found

```
ERROR: Component 'goblin_eye' not found in any search path
```

### Causes

| Cause | Fix |
|-------|-----|
| .src/.hdr files don't exist | Create `goblin_eye.src` and `goblin_eye.hdr` |
| Files in wrong directory | Check discovery order: explicit path → creature-specific → shared |
| Typo in filename | `goblin_eye.src` must match `"name": "goblin_eye"` in JSON |

### Diagnostic Commands

```powershell
# Find component files
Get-ChildItem -Recurse -Filter "goblin_eye.*" -Path "config/"

# Check JSON for component name
Select-String -Path "config/bots/bot_families/goblins/goblin_full.json" -Pattern "goblin_eye"
```

### Fix

**DO**: Create files with correct naming

```powershell
# Create in creature-specific location
New-Item -ItemType File -Path "config/bots/bot_families/goblins/head/goblin_eye.src"
New-Item -ItemType File -Path "config/bots/bot_families/goblins/head/goblin_eye.hdr"
```

**DON'T**: Use wrong names or locations

```powershell
# WRONG: Name mismatch
New-Item "config/bots/bot_families/goblins/head/eye_display.src"  # JSON says "goblin_eye"

# WRONG: Wrong directory level
New-Item "config/components/hardware/goblin_eye.src"  # Not shared, creature-specific!
```

---

## 3. Linker Failures

### Symptom A: RAM Overflow

```
region `dram0_0_seg' overflowed by 45678 bytes
```

### Causes

| Cause | Fix |
|-------|-----|
| Display buffer too large | Reduce buffer size or use PSRAM |
| Too many static arrays | Move large buffers to heap with `malloc()` |
| Stack overflow | Reduce local array sizes in functions |

### Diagnostic Commands

```powershell
# Check RAM usage
pio run -e goblin_head 2>&1 | Select-String "RAM:|Flash:" | Select-Object -Last 2

# Find large static allocations
Select-String -Path "config/**/*.src" -Pattern "static.*\[.*\]" -CaseSensitive
```

### Fix

**DO**: Use heap for large buffers

```cpp
// In init()
static uint16_t* display_buffer = NULL;

esp_err_t display_init(void)
{
    display_buffer = (uint16_t*)heap_caps_malloc(
        240 * 240 * sizeof(uint16_t),
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (display_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate display buffer");
        return ESP_ERR_NO_MEM;
    }
    
    return ESP_OK;
}
```

**DON'T**: Declare huge static arrays

```cpp
// WRONG: 115KB static array
static uint16_t display_buffer[240 * 240];  // Eats all DRAM!
```

---

### Symptom B: Flash Overflow

```
region `flash' overflowed by 123456 bytes
```

### Causes

| Cause | Fix |
|-------|-----|
| Too many components enabled | Remove unused components from JSON |
| Large lookup tables in code | Move to PROGMEM or external storage |
| Debug logging bloat | Reduce log level in platformio.ini |

### Diagnostic Commands

```powershell
# Check Flash usage
pio run -e goblin_head 2>&1 | Select-String "Flash:" | Select-Object -Last 1

# Count enabled components
(Get-Content "config/bots/bot_families/goblins/goblin_full.json" | Select-String '"name":').Count
```

### Fix

**DO**: Disable unused components

```json
// Remove or comment out unused components
{
  "subsystems": {
    "head": {
      "components": [
        {"name": "goblin_eye"},
        {"name": "mood_processor"}
        // {"name": "debug_overlay"}  // Disabled for production
      ]
    }
  }
}
```

**DON'T**: Enable everything at once

```json
// WRONG: 50 components enabled
"components": [
  {"name": "component_1"},
  {"name": "component_2"},
  // ... 48 more ...
]
```

---

## 4. JSON Configuration Errors

### Symptom

```
ERROR: Invalid JSON in goblin_full.json
KeyError: 'subsystems'
```

### Causes

| Cause | Fix |
|-------|-----|
| Syntax error (missing comma, bracket) | Validate JSON with linter |
| Missing required keys | Check schema: `name`, `timing`, `subsystems` |
| Circular dependencies | Remove component reference loops |

### Diagnostic Commands

```powershell
# Validate JSON syntax
python -m json.tool config/bots/bot_families/goblins/goblin_full.json

# Check for required keys
Select-String -Path "config/bots/bot_families/goblins/goblin_full.json" -Pattern '"name":|"subsystems":|"timing":'
```

### Fix

**DO**: Validate JSON before building

```json
{
  "name": "goblin_full",
  "subsystems": {
    "head": {
      "controller": "ESP32-S3-R8N16",
      "components": [
        {
          "name": "goblin_eye",
          "timing": {"hitCount": 1}
        }
      ]
    }
  }
}
```

**DON'T**: Leave syntax errors

```json
// WRONG: Missing comma
{
  "name": "goblin_full"
  "subsystems": {  // Missing comma after "goblin_full"
    "head": {}
  }
}

// WRONG: Trailing comma
{
  "components": [
    {"name": "goblin_eye"},
  ]  // Trailing comma invalid in JSON
}
```

---

## 5. Build Workflow Errors

### Symptom: Stale Generated Code

```
error: 'new_component_act' was not declared in this scope
```

**Cause**: Added component to JSON but didn't regenerate tables.

### Fix

**ALWAYS regenerate before building**:

```powershell
# Sacred 3-step workflow
python tools/generate_tables.py goblin_head src
pio run -e goblin_head
pio run -e goblin_head -t upload
```

---

## Quick Diagnostic Workflow

```powershell
# 1. Check generation succeeded
python tools/generate_tables.py goblin_head src 2>&1 | Select-String "complete|ERROR"

# 2. Build and capture errors
pio run -e goblin_head 2>&1 | Tee-Object build.log

# 3. Find error type
Select-String -Path build.log -Pattern "error:" -Context 2,1 | Select-Object -First 5

# 4. Check specific error patterns
Select-String -Path build.log -Pattern "multiple definition|undefined reference|overflowed"

# 5. Clean build if needed
pio run -e goblin_head -t clean; python tools/generate_tables.py goblin_head src; pio run -e goblin_head
```

---

## Common Mistakes Summary

| Mistake | Symptom | Fix |
|---------|---------|-----|
| Forgot to regenerate tables | Missing symbol errors | Run `generate_tables.py` first |
| Variable in .hdr file | Multiple definition | Move to .src with `static` |
| JSON name ≠ function name | Undefined reference | Match exactly: `{name}_init` / `{name}_act` |
| Huge static array | RAM overflow | Use `heap_caps_malloc()` |
| Missing component files | Component not found | Create `.src` and `.hdr` in correct directory |
| JSON syntax error | Parse failure | Validate with `python -m json.tool` |

---

**Token count**: ~1,600
