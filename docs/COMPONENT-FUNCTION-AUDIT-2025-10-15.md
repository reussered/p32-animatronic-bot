# Component Function Signature Audit - 2025-10-15

## CRITICAL ISSUE: Function Signature Inconsistency

**Problem**: Some component implementations use old `(uint32_t loopCount)` parameter, violating the NO ARGUMENTS architecture.

## Architecture Requirement

**ALL component functions MUST use NO ARGUMENTS**:
```c
esp_err_t component_init(void);  // ✓ CORRECT
void component_act(void);        // ✓ CORRECT
```

**Components access `g_loopCount` via extern declaration**:
```c
extern uint64_t g_loopCount;  // Declared in p32_component_tables.c
```

## Audit Results

### ✅ CORRECT Implementations (void signature)

| File | Function | Status |
|------|----------|--------|
| `goblin_eye_left.c` | `goblin_eye_left_init()` | ✅ Correct |
| `goblin_eye_left.c` | `goblin_eye_left_act()` | ✅ Correct |
| `goblin_eye_right.c` | `goblin_eye_right_init()` | ✅ Correct |
| `goblin_eye_right.c` | `goblin_eye_right_act()` | ✅ Correct |
| `goblin_nose.c` | `goblin_nose_init()` | ✅ Correct |
| `goblin_nose.c` | `goblin_nose_act()` | ✅ Correct |
| `goblin_mouth_STUB.c` | `goblin_mouth_init()` | ✅ Correct |
| `goblin_mouth_STUB.c` | `goblin_mouth_act()` | ✅ Correct |
| `goblin_speaker_STUB.c` | `goblin_speaker_init()` | ✅ Correct |
| `goblin_speaker_STUB.c` | `goblin_speaker_act()` | ✅ Correct |
| `system/watchdog.c` | `watchdog_init()` | ✅ Correct |
| `system/watchdog.c` | `watchdog_act()` | ✅ Correct |
| `system/system_core.c` | `system_core_init()` | ✅ Correct |
| `system/system_core.c` | `system_core_act()` | ✅ Correct |
| `system/serial_console.c` | `serial_console_init()` | ✅ Correct |
| `system/serial_console.c` | `serial_console_act()` | ✅ Correct |
| `system/power_monitor.c` | `power_monitor_init()` | ✅ Correct |
| `system/power_monitor.c` | `power_monitor_act()` | ✅ Correct |
| `system/network_monitor.c` | `network_monitor_init()` | ✅ Correct |
| `system/network_monitor.c` | `network_monitor_act()` | ✅ Correct |
| `system/bluetooth_central.c` | `bluetooth_central_init()` | ✅ Correct |
| `system/bluetooth_central.c` | `bluetooth_central_act()` | ✅ Correct |

### ❌ INCORRECT Implementations (uint32_t loopCount parameter)

| File | Function | Issue | Action |
|------|----------|-------|--------|
| `system_init.c` | `system_init_act(uint32_t loopCount)` | ❌ Has parameter | DELETE (obsolete) |
| `system_core.c` | `system_core_act(uint32_t loopCount)` | ❌ Has parameter | DELETE (obsolete) |
| `network_monitor.c` | `network_monitor_act(uint32_t loopCount)` | ❌ Has parameter | DELETE (obsolete) |

### 🔍 Analysis

**Obsolete Files** (OLD component generation approach):
- `src/components/system_init.c` - OLD, replaced by `system_components.c`
- `src/components/system_core.c` - OLD, replaced by `system_components.c`
- `src/components/network_monitor.c` - OLD, replaced by `system_components.c`

**Current Architecture** (NEW approach):
- `src/system_components.c` - Generated system component stubs (✓ void signature)
- `src/bot_components.c` - Generated bot component stubs (✓ void signature)
- Real implementations in `src/components/goblin_*.c` (✓ void signature)

## Component Mapping (JSON → C Implementation)

### Bot Components (from goblin_full.json)

| JSON component_name | JSON file | C Implementation | Status |
|---------------------|-----------|------------------|--------|
| `left_eye` | `goblin_eye_left.json` | `goblin_eye_left.c` | ✅ Linked |
| `right_eye` | `goblin_eye_right.json` | `goblin_eye_right.c` | ✅ Linked |
| `mouth` | `goblin_mouth.json` | `goblin_mouth_STUB.c` | ⚠️ STUB only |
| `speaker` | `goblin_speaker.json` | `goblin_speaker_STUB.c` | ⚠️ STUB only |
| `nose_sensor` | `goblin_nose.json` | `goblin_nose.c` | ✅ Linked |
| `left_ear_microphone` | `goblin_ear_left.json` | ❌ MISSING | 🔴 Need implementation |
| `right_ear_microphone` | `goblin_ear_right.json` | ❌ MISSING | 🔴 Need implementation |

### System Components (generated stubs)

| Component | C Implementation | Status |
|-----------|------------------|--------|
| `heartbeat` | `system_components.c` | ✅ Stub (void signature) |
| `network_monitor` | `system_components.c` | ✅ Stub (void signature) |

## Required Actions

### 1. DELETE Obsolete Files ❌

These files use OLD architecture with `(uint32_t loopCount)` parameter:
```bash
src/components/system_init.c
src/components/system_core.c
src/components/network_monitor.c
```

**Reason**: Replaced by generated `system_components.c` which uses correct `(void)` signature.

### 2. Missing Implementations 🔴

Need to create (if hardware available):
- `src/components/goblin_ear_left.c` (HW-496 microphone - ADC_PIN_35)
- `src/components/goblin_ear_right.c` (HW-496 microphone - ADC_PIN_36)

**OR**: Update JSON to mark as not implemented yet.

### 3. Link Remaining Stubs ⚠️

If real implementations exist:
- `goblin_mouth.c` (replace `goblin_mouth_STUB.c`)
- `goblin_speaker.c` (replace `goblin_speaker_STUB.c`)

Update `bot_components.c` to link to real implementations.

## Verification Checklist

- [x] All `*_init()` functions use `(void)` signature
- [x] All `*_act()` functions use `(void)` signature  
- [ ] Delete 3 obsolete component files
- [ ] Create or stub ear microphone implementations
- [ ] Link mouth/speaker if real implementations exist
- [x] Verify generated tables use correct signatures
- [x] Verify main.c calls functions without arguments

## Summary

**Root Cause**: Obsolete component files from OLD generation approach still exist in codebase.

**Solution**: Delete 3 obsolete files (`system_init.c`, `system_core.c`, `network_monitor.c`).

**Current State**: 
- ✅ All ACTIVE components use correct `(void)` signature
- ❌ 3 OBSOLETE files use old `(uint32_t loopCount)` signature
- ⚠️ 2 components missing implementations (ear microphones)

**Next Steps**: Clean up obsolete files, then handle missing implementations.
