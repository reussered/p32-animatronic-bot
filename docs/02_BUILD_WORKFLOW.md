# 02. Build Workflow

**Read Time**: 3 minutes | **Criticality**: ⚠️ WRONG ORDER = CORRUPTED BUILD

---

## The Sacred Three-Step Workflow

⚠️ **ALWAYS in this order after ANY config change:**

```powershell
# Step 1: Regenerate dispatch tables
python tools\generate_tables.py <subsystem_name> src

# Step 2: Compile firmware
pio run -e <subsystem_name>

# Step 3: Flash to ESP32
pio run -e <subsystem_name> -t upload
```

**Why order matters**: Step 1 generates `.cpp`/`.hpp` files that Step 2 compiles. Skip Step 1 = stale code compiled.

---

## When to Run Table Generation

**MUST run `generate_tables.py` after:**
- ✅ Editing any `.src` file
- ✅ Editing any `.hdr` file
- ✅ Editing any `.json` file in `config/`
- ✅ Adding new components
- ✅ Changing component timing (`hitCount`)
- ✅ Modifying bot assembly structure

**NO need to run after:**
- ❌ Editing files in `shared/` (compiled directly)
- ❌ Editing `platformio.ini` manually
- ❌ Changing hardware pins (unless in component JSON)

---

## Command Reference

| Command | When | What It Does | Output Location |
|---------|------|--------------|-----------------|
| `python tools\generate_tables.py goblin_head src` | After config changes | Aggregates .src/.hdr, generates dispatch tables | `src/subsystems/goblin_head/` |
| `pio run -e goblin_head` | After table regen | Compiles firmware | `.pio/build/goblin_head/` |
| `pio run -e goblin_head -t clean` | Build cache issues | Deletes build artifacts | (removes `.pio/build/`) |
| `pio run -e goblin_head -t upload` | After successful build | Flashes to ESP32 via serial | (writes to flash) |
| `pio device list` | Find ESP32 port | Lists connected serial devices | (console output) |
| `pio device monitor` | View serial output | Opens serial console | (streams to terminal) |

---

## Subsystem Names

Valid subsystem targets (must match JSON config):
- `goblin_head` - Head subsystem (eyes, mouth, ears)
- `goblin_torso` - Torso subsystem (system core, personality)
- `left_arm` - Left arm subsystem
- `right_arm` - Right arm subsystem
- (Add more as defined in `config/bots/bot_families/`)

---

## PowerShell Log Capture Patterns

⚠️ **NEVER stream build output directly** - it can hang or hide errors.

### Capture to File
```powershell
# Build with full log capture
pio run -e goblin_head > build.log 2>&1

# Review for errors
Select-String -Path build.log -Pattern "error|failed|success" -CaseSensitive
```

### Inline Filtering (Recommended)
```powershell
# Build + filter for key status
python tools\generate_tables.py goblin_head src; pio run -e goblin_head 2>&1 | Select-String "SUCCESS|error:|RAM:|Flash:" | Select-Object -Last 10

# Upload + verify flash writes
pio run -e goblin_head -t upload 2>&1 | Select-String "SUCCESS|error:|Writing at" | Select-Object -Last 5
```

### Error Hunting
```powershell
# Find specific error types
pio run -e goblin_head 2>&1 | Select-String "multiple definition" -Context 0,2

# First 10 errors only
pio run -e goblin_head 2>&1 | Select-String "error:" -Context 2,1 | Select-Object -First 10
```

**Why**: `2>&1` redirects stderr to stdout, `Select-String` filters, `Select-Object` limits output.

---

## Build Success Indicators

**Table Generation Success:**
```
Generating subsystem: goblin_head
  Writing: src/subsystems/goblin_head/goblin_head_component_functions.cpp
  Writing: src/subsystems/goblin_head/goblin_head_dispatch_tables.cpp
  Writing: include/subsystems/goblin_head/goblin_head_component_functions.hpp
```

**Compilation Success:**
```
Linking .pio\build\goblin_head\firmware.elf
RAM:   [==        ]  18.2% (used 59584 bytes from 327680 bytes)
Flash: [====      ]  35.8% (used 467856 bytes from 1310720 bytes)
Building .pio\build\goblin_head\firmware.bin
```

**Upload Success:**
```
Writing at 0x00010000... (25%)
Writing at 0x00020000... (50%)
Writing at 0x00030000... (75%)
Writing at 0x00040000... (100%)
Wrote 467856 bytes at 0x00010000 in 41.2 seconds
Hard resetting via RTS pin...
```

---

## Validation Checklist

After running workflow, verify:

- [ ] `src/subsystems/<subsystem>/` has 3 new `.cpp` files
- [ ] `include/subsystems/<subsystem>/` has 3 new `.hpp` files
- [ ] Build completes with "SUCCESS" message
- [ ] RAM usage < 80% (watch for memory issues)
- [ ] Flash usage < 90% (firmware fits)
- [ ] Upload shows "Writing at 0x000..." progress
- [ ] Hard reset occurs after upload

---

## Common Workflow Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `No such file or directory: goblin_head_component_functions.cpp` | Forgot Step 1 | Run `generate_tables.py` first |
| `multiple definition of 'component_x_init'` | `.hdr` included in `.src` | Remove `#include` from `.src` (auto-stripped next regen) |
| `error: 'static_variable' was not declared` | Variable in wrong `.src` | Move to first `.src` that needs it |
| Build hangs at "Linking..." | Streaming output | Kill + use log capture pattern |
| `No serial port found` | ESP32 not connected | Check USB cable, run `pio device list` |

---

## Quick Start Commands

**First time setup:**
```powershell
# Verify environment
python --version  # Should be 3.8+
pio --version     # Should be installed

# Full clean build
python tools\generate_tables.py goblin_head src
pio run -e goblin_head -t clean
pio run -e goblin_head
```

**Daily workflow:**
```powershell
# Edit config files, then:
python tools\generate_tables.py goblin_head src; pio run -e goblin_head -t upload 2>&1 | Select-String "SUCCESS|error:|Writing" | Select-Object -Last 8
```

**Troubleshooting workflow:**
```powershell
# Clean everything and rebuild
pio run -e goblin_head -t clean
python tools\generate_tables.py goblin_head src
pio run -e goblin_head 2>&1 | Tee-Object build_full.log
Select-String -Path build_full.log -Pattern "error|warning" -CaseSensitive
```

---

**Previous**: [01. Architecture Overview](01_ARCHITECTURE_OVERVIEW.md)  
**Next**: [03. Component System Rules](03_COMPONENT_SYSTEM_RULES.md)
