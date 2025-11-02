# P32 Animatronic Bot - AI Coding Agent Instructions

## CRITICAL SETUP (READ FIRST)
**Project Root**: `f:\GitHub\p32-animatronic-bot` (fixed location, use absolute paths)

**MANDATORY**: Read `.github/AI-AGENT-RULES.md` and `NAMING_RULES.md` before any changes.

## Architecture Overview
**Pure Component System**: Everything is a component with `init()`/`act()` functions. Main loop iterates components only - no application logic in `app_main()`.

**Component Pattern**:
```cpp
esp_err_t component_name_init(void);  // Returns ESP_OK
void component_name_act(void);        // No args, accesses g_loopCount/g_shared_state
```

**Communication**: Components NEVER call each other. Use `SharedMemory::read()`/`write()` for multi-chip coordination.

**Three-Level Component Hierarchy**:
- **System Level**: Core platform (WiFi, Serial, Watchdog) - always present
- **Family Level**: Behavior/personality shared across bot family (Goblin, Cat, Bear)
- **Bot-Specific Level**: Positioned hardware components (eyes, nose, mouth, sensors)

**Recursive JSON Composition**: `goblin_head.json` contains `goblin_left_eye.json` → `goblin_eye.json` → `gc9a01.json`

## Essential Workflows
**Generate Components**: `python tools/generate_tables.py goblin_full src` (reads JSON, creates dispatch tables)

**Build & Flash**: `pio run -t upload -t monitor`

**Validate Config**: `.\generate_file_structure.ps1`

**Build Environments**:
- `goblin_head`: Complete head assembly
- `goblin_torso`: Torso components
- `left_arm`, `right_arm`: Bilateral limb control

## Critical Workflow Rules
- **Never compile .src or .hpp files directly**: These are aggregated into `p32_component_functions.cpp` by `generate_tables.py`. The original `.src` and `.hdr` files live in `config/` locations (these are permanent source files); files in `src/` and `include/` are generated/temporary and should not be edited manually. The `generate_tables.py` script only modifies the aggregated `p32_component_functions.cpp` and `p32_component_functions.hpp` files; individual component headers are generally left as-is.
- **Always run generate_tables.py first**: Execute `python tools/generate_tables.py goblin_full src` before any build to generate dispatch tables from JSON configs.

**Pre-Build Checklist**:
- [ ] Execute `python tools/generate_tables.py goblin_full src` to generate dispatch tables from JSON configs
- [ ] Verify JSON configs are updated and mesh references removed if applicable
- [ ] Check CMakeLists.txt includes correct file paths and include directories
- [ ] Confirm .src files are included in p32_component_functions.cpp
- [ ] Validate component headers exist in include/components/

**Common Oversights**:
- Forgetting to run generate_tables.py before builds (generates component_tables.cpp, etc.)
- Using incorrect PlatformIO config (use platformio_multi_variant.ini for goblin_head)
- Assuming .src files are compiled separately (they are included in p32_component_functions.cpp)
- Missing include paths in CMakeLists.txt (add INCLUDE_DIRS for include/ and shared/)
- Forgetting to update `build_src_filter` in `platformio.ini` when adding components in new source directories.

**Validation Steps**:
- Run `.\generate_file_structure.ps1` to check config integrity
- Use `pio run -c platformio_multi_variant.ini -e <env>` for builds
# P32 Animatronic Bot — AI agent quick guide

This file condenses the essential, discoverable rules an AI coding agent needs to be productive in this repository.

1) Project entry points & mandatory reads
- Project root (absolute): f:\GitHub\p32-animatronic-bot — many scripts expect absolute Windows paths.
- Read before edits: `.github/AI-AGENT-RULES.md` and `NAMING_RULES.md`.

2) Big picture (what to know fast)
- Architecture: pure component system — every component implements `{name}_init()` and `{name}_act()` and is wired via generated dispatch tables. See `config/components/*.src`, `config/components/*.hdr` and generated `src/p32_component_functions.cpp`.
- No direct component calls — use SharedMemory (GSM.read<T>() / GSM.write<T>()) for cross-chip/state sync. Example:
	- Mood *m = GSM.read<Mood>(); m->anger = 50; GSM.write<Mood>();
- Constraints: targets ESP32-S3 (≈512KB RAM, 8MB flash). Components must be non-blocking and return quickly (<10ms). Avoid heap allocation in hot paths.

3) What you must never edit (golden rule)
- Do NOT edit generated sources or dispatch tables: `src/components/*.cpp` (generated), `include/components/*.hpp` (generated), `src/subsystems/*_dispatch_tables.cpp`, `src/p32_component_functions.cpp`, `include/p32_component_functions.hpp`.
- Edit the permanent sources under `config/components/*.src` and `config/components/*.hdr` and JSON configs in `config/bots/...` instead.

4) Typical developer workflow (exact commands)
- Generate dispatch tables (always run before a build):
	- PowerShell: `python tools/generate_tables.py goblin_full src`
	- Confirm structure: `.\\generate_file_structure.ps1`
- Build / flash (PlatformIO):
	- Quick test: `pio run -e <env>`
	- Full (variant) build: `pio run -c platformio_multi_variant.ini -e <env>`
	- Upload + monitor: `pio run -t upload -t monitor`
- Helpers: `build.ps1`, `build_bot.bat` and `build_multi_esp32.ps1` exist — inspect before use.

5) How to add or update a component (concrete steps)
- Edit `config/components/{new}.src` and `config/components/{new}.hdr` and reference it in the JSON bot config (e.g., `config/bots/bot_families/goblin/...`).
- Run `python tools/generate_tables.py <config_name> src`.
- Run `pio run -e <env>` for the target environment.

6) Key repo-specific conventions (examples)
- JSON keys: use short keys — `"name"`, `"type"`, `"relative_filename"`, `"author"` (see any `config/bots/.../*.json`).
- Filenames: lowercase for most files; class header names (e.g., `SharedMemory.hpp`) must match class names.
- Pin allocation: deterministic at runtime — see `PIN_ASSIGNMENT_RULES.md`.

7) Integration & toolchain notes
- Build system: PlatformIO + ESP-IDF. Use PlatformIO CLI on Windows PowerShell (scripts assume PowerShell semantics and absolute paths).
- Required tools: Python 3.x, PlatformIO CLI, PowerShell. ESP-IDF is handled via PlatformIO environments.

8) Quick troubleshooting checklist
- If linker errors reference component symbols: you probably forgot to run `generate_tables.py` or a `.src` was not included in the selected JSON config.
- If behavior differs across chips: check `GSM.write<T>()` usage and JSON composition (duplicate component references are expected in dispatch tables).

9) Where to look for examples
- Component pattern: search `config/components/*.src` for `*_init` / `*_act` examples.
- Generated aggregation: `src/p32_component_functions.cpp` demonstrates how `.src` files are combined.
- Build scripts: `build.ps1`, `build_bot.bat`, `platformio_multi_variant.ini`.

If any section above is unclear or you want the agent to expand examples (e.g., a sample component add/change + test run), tell me which area to expand and I'll update this file. 

