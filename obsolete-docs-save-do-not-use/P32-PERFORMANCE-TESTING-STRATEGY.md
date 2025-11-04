# P32 Performance Testing Strategy

## Two-Phase Testing Approach

### Phase 1: Component Connection Verification (First Few Minutes)
**Goal**: Verify all JSON-defined components are correctly invoked by the universal main loop

**Method**: SIMPLE_TEST printf statements show:
- Which components are loaded from goblin_simple.json
- Each component's hitCount timing from JSON
- Confirmation that universal main loop calls each function correctly

**Expected Output**:
```
INIT: heartbeat - System heartbeat
INIT: network_monitor - Network monitoring  
INIT: left_eye - Left eye display animation
INIT: right_eye - Right eye display animation
INIT: nose_sensor - Proximity sensor monitoring

ACT[1]: heartbeat - hitCount:1
ACT[5]: left_eye - hitCount:5
ACT[5]: right_eye - hitCount:5
ACT[15]: nose_sensor - hitCount:15
ACT[50]: network_monitor - hitCount:50
```

### Phase 2: JSON Definition Testing & Performance Measurement
**Goal**: Measure actual loop performance and test JSON component complexity

**Loop Timing Strategy**: 
Use network_monitor to send timing packets every 100,000 loops:
- `loop_time = measured_time / 100,000`
- Provides accurate ESP32-S3 performance metrics
- Shows impact of component complexity on loop timing

**Implementation Plan**:
```c
// In network_monitor component
static uint32_t timing_start = 0;
static uint32_t packet_counter = 0;

void p32_comp_network_monitor_act(uint32_t loopCount) {
    if (loopCount % 100000 == 0) {
        uint32_t timing_end = esp_timer_get_time();
        float loop_time_us = (timing_end - timing_start) / 100000.0f;
        
        // Send timing packet to attached server
        send_timing_packet(packet_counter++, loop_time_us, loopCount);
        
        timing_start = timing_end;
    }
}
```

## Progressive JSON Complexity Testing

### Simple JSON Phase (Current)
- Basic component definitions with minimal functionality
- Focus on timing verification and loop performance baseline

### Intermediate JSON Phase
- Add sensor reading logic to nose_sensor
- Add simple animation patterns to eye components
- Measure performance impact of increased complexity

### Advanced JSON Phase  
- Full hardware interaction (SPI displays, I2S audio, GPIO sensors)
- Complex behavioral algorithms
- Multi-component coordination
- Performance optimization based on timing measurements

## Performance Metrics to Track

### Loop Performance Indicators
- **Base Loop Time**: Empty loop with no components
- **System Component Impact**: Time added by heartbeat + network_monitor
- **Bot Component Impact**: Time added per eye/sensor component
- **Scaling Factors**: How performance changes with component count

### Network Timing Packets
```json
{
  "packet_id": 123,
  "loop_time_microseconds": 45.7,
  "total_loops": 10000000,
  "component_count": 5,
  "esp32_frequency_mhz": 240,
  "timestamp": "2025-10-12T15:30:45Z"
}
```

## JSON Evolution Strategy

### Phase 2a: Basic Hardware Integration
- Replace printf with actual GPIO/SPI/I2S operations
- Maintain same timing but add real functionality
- Measure performance impact of hardware operations

### Phase 2b: Behavioral Complexity
- Add state machines to components
- Implement component-to-component communication
- Test JSON-driven behavior configuration

### Phase 2c: Full Animation System
- Complex eye movement patterns
- Audio synchronization
- Sensor-driven behavior changes
- Real-time performance optimization

## Success Metrics
- **Loop Consistency**: Timing variance < 5% over extended runs
- **Component Scaling**: Linear performance degradation with component count
- **JSON Flexibility**: Easy addition/removal of components via JSON changes
- **Network Monitoring**: Reliable timing data transmission to development server

This approach ensures the core P32 architecture remains stable while enabling comprehensive testing of JSON-driven component complexity and performance characteristics.