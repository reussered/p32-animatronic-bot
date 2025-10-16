# Serial Monitor Control - Pause/Resume Logging

## Problem Statement

During development, components log execution at high frequency (2-20 Hz), making it difficult to read specific messages. We need a way to **pause logging** without stopping component execution.

## Solution: Input-Triggered Pause

### Architecture

```
┌────────────────────────────────────────────────────────────┐
│                    ESP32 Main Loop                         │
│                                                            │
│  while (true) {                                            │
│      check_serial_input();  // Check for 'p' key          │
│                                                            │
│      for (component in actTable) {                         │
│          if (g_loopCount % hitCount == 0) {                │
│              if (!g_log_paused) {  // ◄── PAUSE FLAG       │
│                  component.act();  // Only log if enabled  │
│              }                                             │
│          }                                                 │
│      }                                                     │
│      g_loopCount++;                                        │
│  }                                                         │
└────────────────────────────────────────────────────────────┘
```

### Implementation

#### 1. Global Pause Flag

Add to `include/p32_shared_state.h`:

```c
// Serial logging control
extern bool g_log_paused;
```

Add to `src/p32_component_tables.c`:

```c
bool g_log_paused = false;  // Start with logging enabled
```

#### 2. Serial Input Monitor Component

Create `src/components/system_serial_input.c`:

```c
#include <esp_log.h>
#include <driver/uart.h>

static const char* TAG = "serial_input";

extern bool g_log_paused;

esp_err_t system_serial_input_init(void)
{
    ESP_LOGI(TAG, "Serial input monitor initialized");
    ESP_LOGI(TAG, "Press 'p' to pause/resume logging");
    return ESP_OK;
}

void system_serial_input_act(void)
{
    uint8_t data[1];
    int len = uart_read_bytes(UART_NUM_0, data, 1, 0);
    
    if (len > 0) {
        if (data[0] == 'p' || data[0] == 'P') {
            g_log_paused = !g_log_paused;
            ESP_LOGI(TAG, "Logging %s", g_log_paused ? "PAUSED" : "RESUMED");
        }
    }
}
```

#### 3. Component Logging Wrapper

Modify component act functions to check flag:

```c
void component_act(void)
{
    extern bool g_log_paused;
    
    // Component logic always executes
    do_hardware_work();
    
    // Only log if not paused
    if (!g_log_paused) {
        ESP_LOGI(TAG, "[component] Status: %d", value);
    }
}
```

#### 4. Add to Component Tables

In JSON config, add serial input component:

```json
{
  "component_name": "serial_input",
  "description": "Serial input monitor for pause control",
  "timing": {
    "hitCount": 1000
  }
}
```

This gives ~120 Hz polling (every 8ms), responsive enough for key presses.

## Alternative: Log Level Control

Instead of pause flag, use ESP-IDF log levels:

```c
// In serial input handler
if (data[0] == 'v') {
    esp_log_level_set("*", ESP_LOG_VERBOSE);  // Show all
} else if (data[0] == 'i') {
    esp_log_level_set("*", ESP_LOG_INFO);     // Normal
} else if (data[0] == 'w') {
    esp_log_level_set("*", ESP_LOG_WARN);     // Warnings only
} else if (data[0] == 'e') {
    esp_log_level_set("*", ESP_LOG_ERROR);    // Errors only
}
```

## Recommended Approach

**Use log levels** instead of pause flag:

1. **Benefits**:
   - No code changes to existing components
   - Standard ESP-IDF mechanism
   - Granular control per component tag
   - No performance impact when disabled

2. **Usage**:
   ```
   Press 'v' = Verbose (all logs)
   Press 'i' = Info (normal operation)
   Press 'w' = Warnings only
   Press 'e' = Errors only
   ```

3. **Default hitCount for components**:
   - Use 60000 (2 Hz) for readable INFO logs
   - Use ESP_LOGV() for high-frequency debug data
   - Users can enable verbose mode when needed

## Component JSON Template

```json
{
  "relative_filename": "config/components/positioned/example.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "component_id": "example_component",
  "component_name": "example",
  "description": "Example component with safe logging",
  "timing": {
    "hitCount": 60000
  },
  "notes": "hitCount 60000 = 2 Hz = readable scrolling in serial monitor"
}
```

## Summary

**For new component development**:

1. ✅ Use `create-component-stub.ps1` with default hitCount 60000
2. ✅ Component logs at readable 2 Hz by default
3. ✅ Add serial input monitor to change log levels dynamically
4. ✅ Use ESP_LOGV() for verbose debug data
5. ✅ No need to modify component code for pause/resume

**Benefits**:

- Readable serial output by default
- Dynamic log level control
- Standard ESP-IDF patterns
- No performance impact
- Easy to debug individual components

📘 **[Component Function Signatures](COMPONENT-FUNCTION-SIGNATURES.md)**
