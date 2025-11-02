# P32 Animatronic Bot - AI Coding Agent Instructions

## 🚨 CRITICAL CONTEXT - READ THIS FIRST 🚨
**THIS IS EMBEDDED ESP32-S3 DEVELOPMENT** - NOT web/mobile dev. Think microcontroller constraints: ~512KB RAM, 8MB flash, real-time, ASCII-only encoding, interrupt-driven, hardware registers.

**NO web frameworks, NO npm, NO UTF-8 encoding, NO browser dev tools**  
**YES hardware registers, GPIO pins, SPI buses, interrupt handlers, component datasheets**

**Project Root**: `f:\GitHub\p32-animatronic-bot` (absolute Windows paths required)  
**MANDATORY PRE-FLIGHT**: Read `.github/AI-AGENT-RULES.md`, `NAMING_RULES.md`, `ARCHITECTURE_DECISION_LOG.md` before ANY changes.

**RULE 1 - REPORT BLOCKERS IMMEDIATELY**: If any rule or constraint prevents proceeding, report it immediately with exact reason. Never pause or fail silently.

## Core Architecture (The "Why")
**Pure Component System**: Multi-ESP32 distributed animatronic bot. Everything is a component with `{name}_init()` → `{name}_act()` functions. Main loop iterates components ONLY - zero application logic in `app_main()`.

**Component Pattern**:
```cpp
esp_err_t component_name_init(void);  // Returns ESP_OK, runs once
void component_name_act(void);        // No args, called every N loops (hitCount)
```

**Communication Patterns** (CRITICAL - mixing these is an architecture violation):
- **INTER-subsystem** (between ESP32 chips): `SharedMemory` ONLY → `GSM.read<Mood>()` / `GSM.write<Mood>()`
- **INTRA-subsystem** (same chip): File-scoped globals OK (`.src` files aggregated into one `.cpp`)
- **NEVER**: Direct component function calls - components are isolated, dispatch system calls them

**Three-Level Hierarchy**:
- **System**: WiFi, Serial, Core (always present, hitCount 50-500)
- **Family**: Personality, Mood, Behavior (shared across bot types, hitCount 10-50)
- **Bot-Specific**: Eyes, Sensors, Motors (positioned hardware, hitCount 1-30)

**JSON-Driven Composition**: `goblin_full.json` → `goblin_head.json` → `goblin_left_eye.json` → `goblin_eye.json` → `gc9a01.json` (recursive includes)

## Essential Workflows (Exact Commands)

**1. Generate Dispatch Tables** (ALWAYS FIRST):
```powershell
python tools/generate_tables.py goblin_full src
```
Reads JSON configs → aggregates `.src` files into `p32_component_functions.cpp` → generates `src/subsystems/*/dispatch_tables.cpp`

**2. Validate Configuration**:
```powershell
.\generate_file_structure.ps1
```

**3. Build & Flash**:
```powershell
# Standard build (single env)
pio run -e goblin_head -t upload -t monitor

# Multi-variant build (multiple ESPs)
pio run -c platformio_multi_variant.ini -e goblin_head
```

**Environments**: `goblin_head`, `goblin_torso`, `left_arm`, `right_arm` (see `platformio.ini`)

## Golden Rules (NEVER BREAK THESE - THEY CAUSE IMMEDIATE BREAKAGE)

**DO NOT EDIT** (generated/temporary - editing these WILL break the build):
- `src/components/*.cpp` (generated from `.src`)
- `include/components/*.hpp` (generated from `.hdr`)
- `src/subsystems/*/dispatch_tables.cpp` (auto-generated)
- `src/p32_component_functions.cpp` (aggregated `.src` files)
- `include/p32_component_functions.hpp` (auto-generated)

**DO EDIT** (permanent sources - these are the source of truth):
- `config/components/**/*.src` (component implementations)
- `config/components/**/*.hdr` (component headers)
- `config/bots/**/*.json` (bot/component configurations)

**ENCODING RULE - BREAKS JSON PARSER**: ALL files must be ASCII-only (NO UTF-8 BOM)
- UTF-8 BOM (`0xEF 0xBB 0xBF`) breaks Python JSON parser
- First byte of JSON must be `{` (`0x7B`), not `239`
- Check: `$bytes = [IO.File]::ReadAllBytes($f); $bytes[0..2]` should be `123, ...`
- Fix: `[IO.File]::WriteAllText($path, $content, [System.Text.Encoding]::ASCII)`

**WINDOWS DEVELOPMENT RULE - BREAKS ON LINUX COMMANDS**:
- Use Windows PowerShell syntax ONLY
- Use backslash paths: `F:\GitHub\p32-animatronic-bot\config\file.json`
- NO Linux/Unix commands (no `ls`, `grep`, `find`, etc.)
- NO cross-platform assumptions

**CODE GENERATION RULE - BREAKS IF SKIPPED**:
- NEVER manually create `.cpp`/`.hpp` files in `src/components/`
- ALWAYS use `python tools/generate_tables.py` to create component code
- Manual creation = missing headers, inconsistent naming, linker errors

**ALLMAN BRACES RULE - REQUIRED FOR SAFETY-CRITICAL CODE**:
```cpp
// CORRECT - braces on own line, ALWAYS use braces
if (condition)
{
    doAction();
}

// WRONG - no braces, K&R style
if (condition) doAction();           // ❌
if (condition) { doAction(); }       // ❌
```

## Adding/Modifying Components

**Add New Component**:
1. Create `config/components/{category}/{name}.json` (define metadata, timing)
2. Create `config/components/{category}/{name}.src` (implement `{name}_init()` / `{name}_act()`)
3. Create `config/components/{category}/{name}.hdr` (declare functions, optional)
4. Reference in parent JSON: `"components": ["config/components/{category}/{name}.json"]`
5. Run `python tools/generate_tables.py goblin_full src`
6. Build: `pio run -e <env>`

**Modify Existing**:
1. Edit `.src` / `.hdr` in `config/components/`
2. Update JSON metadata if needed (hitCount, position, etc.)
3. Re-run `generate_tables.py` → rebuild

## Key Conventions

**JSON Structure** (minimal):
```json
{
  "relative_filename": "config/components/positioned/goblin_left_eye.json",
  "name": "goblin_left_eye",
  "type": "POSITIONED_COMPONENT",
  "timing": {"hitCount": 1},
  "software": {
    "init_function": "goblin_left_eye_init",
    "act_function": "goblin_left_eye_act"
  },
  "components": ["config/components/creature_specific/goblin_eye.json"]
}
```

**Naming**:
- Components: `{creature}_{side}_{part}` (e.g., `goblin_left_eye`)
- Functions: `{component_name}_init()` / `{component_name}_act()`
- Files: lowercase except classes (`SharedMemory.hpp` matches `class SharedMemory`)
- Bilateral: `goblin_left_eye`, `goblin_right_eye` (separate configs)

**SharedMemory API** (cross-chip sync):
```cpp
// Read shared state (any subsystem)
Mood* mood = GSM.read<Mood>();
Environment* env = GSM.read<Environment>();

// Write shared state (owner subsystem only)
Mood* mood = GSM.read<Mood>();
mood->anger = 70;
mood->fear = 30;
GSM.write<Mood>();  // Broadcasts to all ESP32s
```

**Encoding**: ASCII ONLY (no UTF-8 BOM) - JSON parser breaks on `0xEF 0xBB 0xBF`

## Troubleshooting (Common Breakage Patterns)

**Linker errors for component symbols**:
- ROOT CAUSE: Forgot to run `generate_tables.py` before build
- FIX: `python tools/generate_tables.py goblin_full src` then rebuild
- CHECK: Verify `.src` file referenced in JSON config tree
- VERIFY: Check `src/p32_component_functions.cpp` contains your component include

**JSON parsing fails with "Expecting value: line 1 column 1"**:
- ROOT CAUSE: UTF-8 BOM at start of JSON file
- FIX: `$content = Get-Content $f -Raw; [IO.File]::WriteAllText($f, $content, [Text.Encoding]::ASCII)`
- VERIFY: `$bytes = [IO.File]::ReadAllBytes($f); $bytes[0]` should be `123`, not `239`

**Behavior differs across ESP32 chips**:
- ROOT CAUSE: SharedMemory not broadcasting or wrong subsystem writing
- FIX: Verify `GSM.write<T>()` called ONLY by owner subsystem
- CHECK: `"controller"` field in JSON determines chip assignment
- NOTE: Duplicate components in dispatch tables = EXPECTED (reused hardware)

**Build filter issues**:
- ROOT CAUSE: New source directory not in `platformio.ini` filter
- FIX: Add `+<src/subsystems/{name}/>` to `build_src_filter`
- VERIFY: Check `platformio.ini` includes all subsystem paths

**Component not executing**:
- ROOT CAUSE: Component not referenced in bot's JSON hierarchy
- FIX: Add component reference to parent JSON's `"components": []` array
- VERIFY: Run `python tools/generate_tables.py` and check dispatch table

**Windows PowerShell script errors**:
- ROOT CAUSE: Unicode/emojis in script files
- FIX: Remove all non-ASCII characters, save as ASCII
- VERIFY: No emojis (🚨, 📁, etc.) in any script

**"Ask before assuming" violations**:
- ROOT CAUSE: Agent guessed instead of asking for clarification
- FIX: When unclear, ASK the human - don't improvise
- REMINDER: Better to ask than break code

## Examples & References

**Component pattern**: `config/components/positioned/goblin_left_eye.src`  
**JSON configs**: `config/bots/bot_families/goblins/goblin_full.json`  
**SharedMemory usage**: `src/subsystems/goblin_head/goblin_head_component_functions.cpp`  
**Generation script**: `tools/generate_tables.py` (see inline comments)  
**Pin allocation**: `PIN_ASSIGNMENT_RULES.md` (deterministic runtime assignment)

## Quick Reference

| Task | Command | Notes |
|------|---------|-------|
| Generate tables | `python tools/generate_tables.py goblin_full src` | Run BEFORE every build |
| Validate config | `.\generate_file_structure.ps1` | Check JSON integrity |
| Build single env | `pio run -e goblin_head` | Standard PlatformIO |
| Build multi-variant | `pio run -c platformio_multi_variant.ini -e goblin_head` | Multiple ESP32s |
| Upload & monitor | `pio run -t upload -t monitor` | Flash + serial output |
| Check for BOM | `$bytes = [IO.File]::ReadAllBytes($f); $bytes[0..2]` | Should be `123, ...` not `239, 187, 191` |

**Need more detail?** Check section-specific docs:
- Architecture decisions → `ARCHITECTURE_DECISION_LOG.md`
- Component JSON format → `docs/component-json-requirements.md`
- Pin assignment logic → `PIN_ASSIGNMENT_RULES.md`
- Multi-ESP32 design → `MULTI-ESP32-DESIGN.md` 

