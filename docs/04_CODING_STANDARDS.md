# Topic 4: Coding Standards

**Purpose**: Enforce consistency, prevent build failures, maintain ESP-IDF compatibility.

---

## 1. File Encoding

**Rule**: ASCII-only, no BOM, no UTF-8 characters.

**Why**: ESP-IDF toolchain chokes on non-ASCII. Build failures are cryptic.

```cpp
// ✓ DO
const char* msg = "Error";

// ✗ DON'T (uses em-dash U+2014)
const char* msg = "Error—failed";  // Build fails
```

**Validation**: `.\ascii_validator.ps1` scans workspace, reports violations.

---

## 2. Brace Style

**Rule**: Allman style (braces on new line). Mandatory `{}` for all control flow.

```cpp
// ✓ DO
if (condition)
{
    action();
}

void my_function(void)
{
    // body
}

// ✗ DON'T
if (condition) {  // K&R style - forbidden
    action();
}

if (condition)
    action();  // No braces - forbidden, even single line
```

**Why**: Consistency, prevents insertion bugs when adding statements.

---

## 3. Naming Conventions

### Functions
- `snake_case` with subsystem prefix
- `init()` returns `esp_err_t`, `act()` returns `void`

```cpp
// ✓ DO
esp_err_t goblin_eye_init(void);
void goblin_eye_act(void);

// ✗ DON'T
esp_err_t GoblinEyeInit();  // CamelCase forbidden
void eye_act();  // Missing subsystem prefix
```

### Variables
- File-scope static: `snake_case`
- Local variables: `snake_case`
- Constants: `SCREAMING_SNAKE_CASE`

```cpp
// ✓ DO (file scope)
static uint16_t* display_buffer = NULL;
static const uint32_t MAX_RETRIES = 3;

// ✓ DO (local)
void process_frame(void)
{
    uint32_t pixel_count = 0;
    for (uint32_t i = 0; i < pixel_count; i++)
    {
        // ...
    }
}

// ✗ DON'T
uint16_t DisplayBuffer;  // CamelCase forbidden
int maxRetries = 3;  // Should be const and UPPERCASE
```

### Types
- Structs/classes: `PascalCase`
- Enums: `PascalCase` for type, `SCREAMING_SNAKE_CASE` for values

```cpp
// ✓ DO
struct PixelData
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
};

enum DisplayMode
{
    DISPLAY_MODE_DIRECT,
    DISPLAY_MODE_BUFFERED
};

// ✗ DON'T
struct pixel_data { };  // Should be PascalCase
enum displayMode { };  // Should be PascalCase
```

---

## 4. Include Directives

### Order
1. Own header (for `.cpp` files)
2. Project headers (subsystem, core, shared)
3. Third-party libraries
4. System headers

```cpp
// ✓ DO (in goblin_eye.src)
// (Auto-includes injected by generator)
#include "drivers/gc9a01_driver.hpp"  // Project headers
#include "ArduinoJson.h"  // Third-party
#include "esp_timer.h"  // System

// ✗ DON'T
#include "esp_timer.h"  // System first - wrong order
#include "goblin_eye.hdr"  // Never include .hdr from .src
```

### Forbidden Includes in `.src` Files
- **Never** `#include "*.hdr"` — aggregated into `.hpp` by generator
- **Never** `#include` shared state classes — auto-injected by generator
- **Never** cross-component includes — use GSM for communication

```cpp
// ✗ DON'T (in any .src file)
#include "goblin_eye.hdr"  // Causes duplicate definitions
#include "Mood.hpp"  // Auto-included, causes duplicate
#include "goblin_servo.hdr"  // Cross-component - forbidden
```

---

## 5. Variable Scope

### File-Scoped Static (Intra-Subsystem Communication)
**When**: Passing data between components in same subsystem.

```cpp
// ✓ DO (at top of .src file, outside functions)
static uint16_t* display_buffer = NULL;
static uint32_t buffer_size = 0;

// goblin_eye_init() allocates buffer
// goblin_display_act() reads buffer
```

### Local Variables
**When**: Temporary computation, no sharing needed.

```cpp
// ✓ DO
void goblin_eye_act(void)
{
    uint32_t elapsed_ms = get_time();  // Local only
    // ...
}
```

### Global Shared Memory (Inter-Subsystem Communication)
**When**: Passing data between different ESP32 chips.

```cpp
// ✓ DO
void goblin_head_act(void)
{
    Mood mood = GSM.read<Mood>();
    mood.happiness = 75;
    GSM.write(mood);  // Syncs to torso ESP32
}

// ✗ DON'T
extern Mood global_mood;  // Never use extern for inter-chip
```

---

## 6. Comments

### Required Comments
- **Component purpose** (top of `.src`)
- **Complex algorithms** (inline)
- **Hardware-specific workarounds** (inline with reference)

```cpp
// ✓ DO
// Component: goblin_eye_display
// Purpose: Renders eye animations to GC9A01 240x240 circular display
// Hardware: SPI bus shared with torso, requires mutex

// Workaround: GC9A01 requires 10ms delay after init (datasheet p.42)
vTaskDelay(pdMS_TO_TICKS(10));
```

### Avoid
- Obvious comments
- Commented-out code (use git)
- TODO without issue reference

```cpp
// ✗ DON'T
i++;  // Increment i (obvious)
// old_function();  // Delete, don't comment
// TODO: Fix this later (vague, no tracking)
```

---

## 7. Function Signatures

### Component Lifecycle Functions
```cpp
// ✓ DO (exact signature required)
esp_err_t goblin_eye_init(void)
{
    // Return ESP_OK on success, ESP_FAIL on error
    return ESP_OK;
}

void goblin_eye_act(void)
{
    // No return value, called every loop iteration
}

// ✗ DON'T
int goblin_eye_init()  // Wrong return type
bool goblin_eye_act()  // act() must return void
```

### Helper Functions
```cpp
// ✓ DO (static for file scope)
static uint16_t calculate_brightness(uint8_t level)
{
    return level * 256;
}

// ✗ DON'T
uint16_t calculate_brightness(uint8_t level)  // Should be static
```

---

## 8. Error Handling

### init() Functions
**Must** return `esp_err_t`. Check and propagate errors.

```cpp
// ✓ DO
esp_err_t goblin_eye_init(void)
{
    esp_err_t ret = spi_bus_initialize();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPI init failed: %d", ret);
        return ret;
    }
    return ESP_OK;
}

// ✗ DON'T
esp_err_t goblin_eye_init(void)
{
    spi_bus_initialize();  // Ignore return value - forbidden
    return ESP_OK;
}
```

### act() Functions
**Cannot** return errors (void). Handle gracefully or log.

```cpp
// ✓ DO
void goblin_eye_act(void)
{
    if (display_buffer == NULL)
    {
        ESP_LOGW(TAG, "Buffer not initialized, skipping");
        return;  // Safe early exit
    }
    // Process...
}

// ✗ DON'T
void goblin_eye_act(void)
{
    assert(display_buffer != NULL);  // Crashes device - forbidden
}
```

---

## 9. Memory Management

### Static Allocation (Preferred)
```cpp
// ✓ DO
static uint8_t buffer[4096];  // Known at compile time
```

### Dynamic Allocation (Use Sparingly)
```cpp
// ✓ DO
static uint8_t* buffer = NULL;

esp_err_t init(void)
{
    buffer = (uint8_t*)malloc(size);
    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "malloc failed");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

// ✗ DON'T
void act(void)
{
    uint8_t* temp = (uint8_t*)malloc(1024);  // Memory leak risk
    // Never freed - forbidden in act()
}
```

### Freeing Memory
- `free()` in cleanup or error paths, never in `act()`
- Check for `NULL` before freeing

```cpp
// ✓ DO
if (buffer != NULL)
{
    free(buffer);
    buffer = NULL;  // Prevent double-free
}
```

---

## 10. Platform-Specific Code

### Use ESP-IDF APIs
```cpp
// ✓ DO
#include "esp_timer.h"
uint64_t now_us = esp_timer_get_time();

// ✗ DON'T
#include <chrono>  // C++ std library not fully supported
auto now = std::chrono::high_resolution_clock::now();
```

### Logging
```cpp
// ✓ DO
static const char* TAG = "goblin_eye";
ESP_LOGI(TAG, "Initialized successfully");
ESP_LOGW(TAG, "Buffer nearly full: %d%%", percent);
ESP_LOGE(TAG, "Fatal error: %d", code);

// ✗ DON'T
printf("Debug message\n");  // Use ESP_LOG macros
```

---

## Common Mistakes

| Mistake | Fix |
|---------|-----|
| `#include "component.hdr"` in `.src` | Remove — auto-aggregated |
| K&R braces `if (x) {` | Use Allman: `if (x)\n{` |
| `if (x) action();` | Add braces: `if (x)\n{\n    action();\n}` |
| UTF-8 quotes `"text"` | Use ASCII: `"text"` |
| `CamelCase` functions | Use `snake_case` |
| `malloc()` in `act()` | Move to `init()` or use static |
| Ignoring `esp_err_t` returns | Check and propagate errors |

---

**Token count**: ~1,400
