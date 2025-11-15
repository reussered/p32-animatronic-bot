# Topic 10: Memory & Performance

**Purpose**: Optimize for ESP32 constraints (~512KB RAM) and real-time execution.

---

## Memory Budget (ESP32-S3-R8N16)

| Region | Size | Typical Usage | Available for App |
|--------|------|---------------|-------------------|
| Internal DRAM | 512 KB | Stack, heap, static | ~300-350 KB |
| PSRAM (external) | 8 MB | Large buffers, display | ~7.5 MB |
| Flash (code) | 16 MB | Program, constants | ~15 MB |

**Critical Rule**: Large buffers (>10KB) MUST use PSRAM, not internal DRAM.

---

## 1. Display Buffer Architecture

### P32 Display Chain Pattern

Display chains use **positioned components** at the top to allocate buffers, with **generic_spi_display** at the bottom for output routing.

### Display Chain Top: use_fields Parameters

Each display chain (goblin_left_eye, goblin_right_eye, goblin_mouth) defines buffer allocation in JSON:

```json
{
  "name": "goblin_left_eye",
  "type": "POSITIONED_COMPONENT",
  "use_fields": {
    "display_width": 240,
    "display_height": 240,
    "bytes_per_pixel": 2,
    "color_schema": "RGB565",
    "debug": true
  },
  "components": ["goblin_eye"]
}
```

**Generated code** (in `{name}_init` via `use_fields`):

```cpp
static uint16_t* display_buffer = NULL;
static uint16_t display_width = 240;
static uint16_t display_height = 240;
static uint8_t bytes_per_pixel = 2;

esp_err_t goblin_left_eye_init(void)
{
    // use_fields generates these assignments
    display_width = 240;
    display_height = 240;
    bytes_per_pixel = 2;
    
    // Allocate buffer in PSRAM
    size_t buffer_size = display_width * display_height * bytes_per_pixel;
    display_buffer = (uint16_t*)heap_caps_malloc(
        buffer_size,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    
    if (display_buffer == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate %zu bytes in PSRAM", buffer_size);
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Allocated %zu KB display buffer", buffer_size / 1024);
    return ESP_OK;
}
```

### Display Chain Bottom: generic_spi_display Output Router

`generic_spi_display` component routes frames to **either**:
1. **Physical SPI display** (hardware output)
2. **Serial bridge → PC display server** (development/testing)

```cpp
void generic_spi_display_act(void)
{
    if (display_buffer == NULL)
    {
        ESP_LOGW(TAG, "No display_buffer set");
        return;
    }
    
    #ifdef SERIAL_DISPLAY_MODE
    // Development: Send JSON to PC via serial
    printf("DISPLAY_DATA:{\"device_id\":\"%s\",\"data\":[", component_name);
    for (size_t i = 0; i < buffer_size; i++)
    {
        printf("%u%s", display_buffer[i], (i < buffer_size - 1) ? "," : "");
    }
    printf("]}\n");
    #else
    // Production: Send to SPI display hardware
    send_spi_frame(display_buffer, display_width, display_height);
    #endif
}
```

### Serial Bridge for PC Development

**Purpose**: View ESP32 display output on PC during development without physical displays.

```powershell
# Run bridge (monitors COM port, forwards to local server)
python tools/serial_to_display_bridge.py
```

**Bridge workflow**:
1. ESP32 prints `DISPLAY_DATA:{...}` to serial
2. Bridge parses JSON and forwards to `http://localhost:3000/api/animation-data`
3. Web server renders frames in browser

### Memory Budget by Display

| Display Chain | Resolution | Format | Buffer Size |
|---------------|-----------|--------|-------------|
| goblin_left_eye | 240×240 | RGB565 (2 bytes) | 115 KB |
| goblin_right_eye | 240×240 | RGB565 (2 bytes) | 115 KB |
| goblin_mouth | 480×320 | RGB666 (3 bytes) | 450 KB |
| **Total** | - | - | **680 KB (PSRAM)** |

**Critical**: All buffers MUST use PSRAM (`MALLOC_CAP_SPIRAM`), not internal DRAM.

### Display Chain Patterns

**DO**: Define buffer parameters in positioned component JSON

```json
{
  "name": "goblin_left_eye",
  "type": "POSITIONED_COMPONENT",
  "use_fields": {
    "display_width": 240,
    "display_height": 240,
    "bytes_per_pixel": 2,
    "color_schema": "RGB565"
  }
}
```

**DO**: Allocate in PSRAM at chain top

```cpp
// In positioned component init
display_buffer = (uint16_t*)heap_caps_malloc(
    display_width * display_height * bytes_per_pixel,
    MALLOC_CAP_SPIRAM  // MUST use PSRAM
);
```

**DO**: Use static variables to share buffer down chain

```cpp
// Declared at file scope (single compilation unit)
static uint16_t* display_buffer = NULL;
static uint16_t display_width = 240;

// Rendering component uses shared buffer
void goblin_eye_act(void)
{
    if (display_buffer != NULL)
    {
        draw_eye(display_buffer, display_width);
    }
}
```

**DON'T**: Allocate display buffers in internal DRAM

```cpp
// WRONG: 115KB in internal DRAM = instant crash
static uint16_t display_buffer[240 * 240];
```

**DON'T**: Allocate separate buffers in each component

```cpp
// WRONG: Multiple allocations waste memory
void goblin_eye_init(void)
{
    uint16_t* my_buffer = malloc(240 * 240 * 2);  // Redundant!
}
```

---

## 2. Memory Allocation Best Practices

### Check Available Memory

```cpp
void log_memory_stats(void)
{
    ESP_LOGI(TAG, "Free DRAM: %lu bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    ESP_LOGI(TAG, "Free PSRAM: %lu bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI(TAG, "Largest free block: %lu bytes", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
}
```

### Allocation Patterns

| Pattern | Use Case | Memory Type |
|---------|----------|-------------|
| `malloc()` | Default allocation | Internal DRAM (prefers internal) |
| `heap_caps_malloc(..., MALLOC_CAP_SPIRAM)` | Large buffers (>10KB) | External PSRAM |
| `heap_caps_malloc(..., MALLOC_CAP_DMA)` | SPI/I2S DMA buffers | Internal DRAM (DMA-capable) |
| Static arrays | Small fixed buffers (<1KB) | Internal DRAM |

### DO: Check Allocation Success

```cpp
esp_err_t init_buffers(void)
{
    uint16_t* buffer = (uint16_t*)heap_caps_malloc(
        BUFFER_SIZE,
        MALLOC_CAP_SPIRAM
    );
    
    if (buffer == NULL)
    {
        ESP_LOGE(TAG, "Allocation failed");
        log_memory_stats();  // Diagnostic output
        return ESP_ERR_NO_MEM;
    }
    
    memset(buffer, 0, BUFFER_SIZE);  // Initialize
    return ESP_OK;
}
```

### DON'T: Assume Allocation Succeeds

```cpp
// WRONG: No NULL check
uint16_t* buffer = (uint16_t*)malloc(BUFFER_SIZE);
memset(buffer, 0, BUFFER_SIZE);  // CRASH if buffer is NULL!
```

---

## 3. Performance Optimization

### Execution Time Constraints

| Component Type | Target Time | Max Time | Action if Exceeded |
|----------------|-------------|----------|-------------------|
| Sensor read | <1 ms | 5 ms | Cache results, reduce frequency |
| Behavior logic | <2 ms | 10 ms | Simplify decision tree |
| Display render | <16 ms | 33 ms | Reduce resolution, optimize drawing |
| Motor control | <0.5 ms | 2 ms | Precompute trajectories |

**Loop target**: 60 Hz (16.67 ms per frame) for smooth animation.

### Measure Execution Time

```cpp
void timed_component_act(void)
{
    uint64_t start_us = esp_timer_get_time();
    
    // Component logic here
    do_work();
    
    uint64_t elapsed_us = esp_timer_get_time() - start_us;
    
    if (elapsed_us > 5000)  // Warn if >5ms
    {
        ESP_LOGW(TAG, "Slow execution: %llu us", elapsed_us);
    }
}
```

### Optimize Hot Paths

**DO**: Cache expensive calculations

```cpp
static float sin_table[360];

esp_err_t math_init(void)
{
    // Precompute once at init
    for (int i = 0; i < 360; i++)
    {
        sin_table[i] = sin(i * M_PI / 180.0);
    }
    return ESP_OK;
}

void math_act(void)
{
    // Fast lookup instead of sin() call
    float value = sin_table[angle_deg % 360];
}
```

**DON'T**: Recompute every loop

```cpp
// WRONG: Expensive calculation every frame
void math_act(void)
{
    float value = sin(angle_deg * M_PI / 180.0);  // 100+ cycles!
}
```

---

## 4. hitCount Timing Optimization

### Execution Frequency Management

Lower hitCount = executes MORE frequently (1 = every loop, 10 = every 10th loop).

```json
{
  "components": [
    {"name": "eye_render", "timing": {"hitCount": 1}},      // 60 Hz (every frame)
    {"name": "proximity_sensor", "timing": {"hitCount": 3}}, // 20 Hz (every 3rd frame)
    {"name": "mood_update", "timing": {"hitCount": 10}},     // 6 Hz (every 10th frame)
    {"name": "battery_check", "timing": {"hitCount": 60}}    // 1 Hz (every 60th frame)
  ]
}
```

### Balancing Load

**DO**: Distribute heavy components across different hitCounts

```json
{
  "components": [
    {"name": "display_render", "timing": {"hitCount": 1}},     // Critical path
    {"name": "sensor_fusion", "timing": {"hitCount": 2}},      // Half rate
    {"name": "path_planning", "timing": {"hitCount": 5}},      // Lower priority
    {"name": "diagnostics", "timing": {"hitCount": 30}}        // Background task
  ]
}
```

**DON'T**: Run all components every loop

```json
// WRONG: Everything at hitCount 1
{
  "components": [
    {"name": "display_render", "timing": {"hitCount": 1}},
    {"name": "sensor_fusion", "timing": {"hitCount": 1}},   // TOO FREQUENT
    {"name": "path_planning", "timing": {"hitCount": 1}},   // TOO FREQUENT
    {"name": "diagnostics", "timing": {"hitCount": 1}}      // TOO FREQUENT
  ]
}
```

---

### B. Blocking I/O in act()

**DON'T**: Wait for I2C/SPI in act()

```cpp
// WRONG: Blocks for 10ms waiting for sensor
void sensor_act(void)
{
    float temp = read_i2c_sensor_blocking();  // 10ms delay!
    process_temperature(temp);
}
```

**DO**: Use non-blocking I/O or reduce frequency

```cpp
// Option 1: Higher hitCount
// In JSON: "timing": {"hitCount": 10}

// Option 2: Non-blocking I/O
void sensor_act(void)
{
    static bool read_pending = false;
    
    if (!read_pending)
    {
        start_i2c_read_async();  // Non-blocking
        read_pending = true;
    }
    else if (i2c_read_complete())
    {
        float temp = get_i2c_result();
        process_temperature(temp);
        read_pending = false;
    }
}
```

---

### C. Large Stack Frames

**DON'T**: Allocate large arrays on stack

```cpp
// WRONG: 10KB on stack
void process_frame(void)
{
    uint16_t temp_buffer[5000];  // 10,000 bytes on stack!
    // ... work ...
}
```

**DO**: Use static or heap allocation

```cpp
// Option 1: Static (shared across calls)
void process_frame(void)
{
    static uint16_t temp_buffer[5000];  // In .bss, not stack
    // ... work ...
}

// Option 2: Heap (if needed per-call)
void process_frame(void)
{
    uint16_t* temp_buffer = (uint16_t*)malloc(10000);
    if (temp_buffer == NULL) return;
    
    // ... work ...
    
    free(temp_buffer);
}
```

---

## 6. Diagnostic Commands

### Check Memory Usage After Build

```powershell
# View RAM/Flash usage
pio run -e goblin_head 2>&1 | Select-String "RAM:|Flash:"

# Find large static allocations
Select-String -Path "config/**/*.src" -Pattern "static.*\[.*\]" | Select-String "\[[0-9]{4,}\]"
```

### Runtime Memory Monitoring

```cpp
// Add to periodic diagnostic component (hitCount 60+)
void diagnostics_act(void)
{
    ESP_LOGI("MEM", "Free DRAM: %lu / Free PSRAM: %lu",
        heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
        heap_caps_get_free_size(MALLOC_CAP_SPIRAM)
    );
}
```

---

## Memory Budget Example (Goblin Head Subsystem)

**Displays:**
- Left eye: 240×240 RGB565 = 115 KB (PSRAM)
- Right eye: 240×240 RGB565 = 115 KB (PSRAM)
- Mouth: 480×320 RGB666 = 450 KB (PSRAM)

| Component | Size | Location | Notes |
|-----------|------|----------|-------|
| Left eye buffer | 115 KB | PSRAM | Via goblin_left_eye use_fields |
| Right eye buffer | 115 KB | PSRAM | Via goblin_right_eye use_fields |
| Mouth buffer | 450 KB | PSRAM | Via goblin_mouth use_fields |
| SPI DMA buffers | 12 KB | Internal DRAM | 3× 4KB DMA-capable |
| Static variables | 20 KB | Internal DRAM | Component state |
| Stack (main task) | 8 KB | Internal DRAM | ESP-IDF default |
| Stack (other tasks) | 16 KB | Internal DRAM | 2× 8KB tasks |
| Heap overhead | 30 KB | Internal DRAM | Allocator metadata |
| **Total Internal** | **~86 KB** | **DRAM** | **Leaves ~260 KB free** |
| **Total External** | **680 KB** | **PSRAM** | **Leaves ~7.3 MB free** |

**Key Pattern**: Display buffers (largest allocations) use `use_fields` parameters → PSRAM allocation in positioned component `_init()` → shared via static variables → output via `generic_spi_display` to hardware or serial bridge.

---

## Performance Checklist

| Check | Target | Command |
|-------|--------|---------|
| RAM usage | <60% internal DRAM | `pio run -e {env} 2>&1 \| Select-String "RAM:"` |
| Flash usage | <80% | `pio run -e {env} 2>&1 \| Select-String "Flash:"` |
| Loop time | <16.67 ms (60 Hz) | Measure with `esp_timer_get_time()` |
| Critical path | <10 ms | Profile hitCount=1 components |
| Free heap at runtime | >100 KB internal | `heap_caps_get_free_size()` |

---

**Token count**: ~1,700
