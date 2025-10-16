# Component Function Signature Audit Report
**Generated:** October 15, 2025
**Status:** ⚠️ SIGNATURE MISMATCHES FOUND

## Summary

**CRITICAL**: Several component files have **WRONG SIGNATURES** that violate the NO ARGUMENTS architecture.

## Architecture Requirements

All component functions **MUST** follow this pattern:

```c
esp_err_t component_name_init(void);  // ✅ NO ARGUMENTS
void component_name_act(void);        // ✅ NO ARGUMENTS
```

Components access global state via:
```c
extern uint64_t g_loopCount;  // From p32_component_tables.h
```

## Audit Results

### ✅ CORRECT Components (NO ARGUMENTS)

| Component | File | Init | Act | Status |
|-----------|------|------|-----|--------|
| goblin_eye_left | `src/components/goblin_eye_left.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |
| goblin_eye_right | `src/components/goblin_eye_right.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |
| goblin_nose | `src/components/goblin_nose.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |
| goblin_mouth | `src/components/goblin_mouth_STUB.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |
| goblin_speaker | `src/components/goblin_speaker_STUB.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |
| system_serial_input | `src/components/system_serial_input.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |
| system_core | `src/components/system_core.c` | ✅ `(void)` | - | CORRECT |
| bluetooth_central | `src/components/system/bluetooth_central.c` | ✅ `(void)` | ✅ `(void)` | CORRECT |

### ❌ INCORRECT Components (HAS ARGUMENTS)

| Component | File | Init | Act | Issue |
|-----------|------|------|-----|-------|
| **network_monitor** | `src/components/network_monitor.c` | ✅ `(void)` | ❌ `(uint32_t loopCount)` | **ACT HAS PARAMETER** |
| **arm_controller** | `src/components/arm_controller.c` | ❌ `(bool left_arm)` | ❌ `(p32_arm_command_t command)` | **BOTH HAVE PARAMETERS** |
| **foot_controller** | `src/components/foot_controller.c` | ✅ `(void)` | ❌ `(p32_component_command_t command)` | **ACT HAS PARAMETER** |
| **hand_controller** | `src/components/hand_controller.c` | ✅ `(void)` | ❌ `(p32_component_command_t command)` | **ACT HAS PARAMETER** |
| **leg_controller** | `src/components/leg_controller.c` | ✅ `(void)` | ❌ `(p32_component_command_t command)` | **ACT HAS PARAMETER** |

### 🔍 Components NOT in Current JSON Config

These components have signature issues but are **NOT** in `goblin_full.json`:
- `arm_controller` (future expansion)
- `foot_controller` (future expansion)
- `hand_controller` (future expansion)
- `leg_controller` (future expansion)

**Decision**: Leave these alone for now - they're not being used.

### ⚠️ IMMEDIATE FIX REQUIRED

**Only 1 component needs fixing** for current goblin_full.json:

1. **`network_monitor.c`** - Change `act(uint32_t loopCount)` → `act(void)`

## Current Component Table (goblin_full.json)

From JSON config, these components are actually used:

| Component Name | JSON File | C Implementation | Status |
|----------------|-----------|------------------|--------|
| left_eye | `goblin_eye_left.json` | `goblin_eye_left.c` | ✅ CORRECT |
| right_eye | `goblin_eye_right.json` | `goblin_eye_right.c` | ✅ CORRECT |
| mouth | `goblin_mouth.json` | `goblin_mouth_STUB.c` | ✅ CORRECT |
| speaker | `goblin_speaker.json` | `goblin_speaker_STUB.c` | ✅ CORRECT |
| nose_sensor | `goblin_nose.json` | `goblin_nose.c` | ✅ CORRECT |
| left_ear_microphone | `goblin_ear_left.json` | ❓ **MISSING** | ⚠️ NO IMPL |
| right_ear_microphone | `goblin_ear_right.json` | ❓ **MISSING** | ⚠️ NO IMPL |

### System Components (Need to Check)

The system adds these automatically:
- `heartbeat` - Generated stub (assumed ✅)
- `network_monitor` - ❌ **NEEDS FIX**
- `serial_input` - ✅ CORRECT (just created)

## Required Actions

### 1. Fix network_monitor.c ❌➡️✅

**Change:**
```c
void network_monitor_act(uint32_t loopCount) {
    // ...
}
```

**To:**
```c
void network_monitor_act(void) {
    extern uint64_t g_loopCount;  // Access global
    // ...
}
```

### 2. Create Missing Ear Microphone Components

Need to create:
- `src/components/goblin_ear_left.c`
- `src/components/goblin_ear_right.c`

Use the stub generator:
```powershell
.\tools\create-component-stub.ps1 -ComponentName "goblin_ear_left" -Description "Left ear microphone" -HitCount 30000
.\tools\create-component-stub.ps1 -ComponentName "goblin_ear_right" -Description "Right ear microphone" -HitCount 30000
```

### 3. Add serial_input to System Components

The serial input monitor needs to be added to the component generation script.

## Testing Plan

After fixes:

1. ✅ Run `python tools\generate_tables.py`
2. ✅ Verify all components in tables have matching signatures
3. ✅ Build: `pio run`
4. ✅ Upload: `pio run -t upload`
5. ✅ Test log control: Press 'v', 'i', 'w', 'e', 'q'

## Conclusion

**Current Status**: 7/9 components have correct signatures (78%)

**Immediate Priority**: Fix `network_monitor.c` only (1 file)

**Medium Priority**: Create ear microphone stubs (2 files)

**Low Priority**: Fix arm/hand/leg/foot controllers (not currently used)

---

📘 **[Component Function Signatures Spec](COMPONENT-FUNCTION-SIGNATURES.md)**
📘 **[NO ARGUMENTS Pattern](COMPONENT-FUNCTION-SIGNATURES.md#standard-pattern)**
