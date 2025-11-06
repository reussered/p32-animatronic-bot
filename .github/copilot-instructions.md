# P32 Animatronic Bot - AI Agent Guide

## Context
- Embedded ESP32-S3/ESP32-C3 project; think tight RAM (~512KB) and real-time loops, not web stacks.
- Tooling is PlatformIO + ESP-IDF running on Windows PowerShell; paths are absolute like `F:\GitHub\p32-animatronic-bot\...`.
- ASCII-only encoding everywhere (no BOM); Allman braces with mandatory `{}` on every control flow block.
- Before touching code reread the rest of the files in       `.github`

## Architecture
- **CRITICAL: Single Compilation Unit**: The build script (`tools/generate_tables.py`) concatenates all component `.src` files for a subsystem (e.g., `goblin_head`) into a single, large `.cpp` file. This means all `.src` files within that subsystem share the **same file scope**.
- **Intra-Subsystem Communication**: Because of the single compilation unit, components within the same subsystem **must** communicate using file-scoped `static` variables (e.g., `static uint16_t* display_buffer = NULL;`). This is the standard way to pass data along a processing chain. You do **not** need to `#include` other component headers for this to work; the build system aggregates them.
- **Component System**: Every behavior lives in organized component folders:
  - `config/components/{hardware|drivers|interfaces|behaviors}/` (generic/reusable)
  - `config/bots/multi_family/{humanoid|quadruped|tentacles|wings}/` (shared multi-family)
  - `config/bots/bot_families/{family}/{subsystem}/` (creature-specific)
  - All expose `{name}_init()` / `{name}_act()` functions via `.json`, `.src`, `.hdr` files
- **JSON-Driven Composition**: A hierarchy of JSON files starting from `config/bots/bot_families/...` defines the animatronic, composes components, and assigns them to controllers.
- **Inter-Subsystem Communication**: Data flow between different ESP32 chips (e.g., head to torso) happens **exclusively** through the Global Shared Memory system: `GSM.read<T>()` and `GSM.write<T>()`.

## Critical workflows
- **VALIDATE BEFORE EVERY CODE GENERATION** - Auto-corrects component references and detects violations:
```powershell
python tools/validate_file_structure.py
```
This MUST run successfully before `generate_tables.py`.

- Regenerate tables after any JSON or `.src` change or builds will fail:
```powershell
python tools/generate_tables.py goblin_full src
.\generate_file_structure.ps1
```
- Build flashes must be log-captured; never stream directly:
```powershell
pio run -e goblin_head > build.log 2>&1
Select-String -Path build.log -Pattern "error|failed|success" -CaseSensitive
pio run -e goblin_head -t upload > upload.log 2>&1
Select-String -Path upload.log -Pattern "error|success|Writing"
```
- Serial capture for diagnostics: `Start-Job { pio device monitor } | Wait-Job -Timeout 60 | Receive-Job`.

## Editing rules
- Never hand-edit generated `src/components/*.cpp`, `include/components/*.hpp`, or subsystem dispatch files; source truth lives in `config/**`.
- Keep PowerShell syntax (`;` separators, backslash paths); avoid Unix commands.
- Maintain Allman-style formatting and component signatures exactly:
```cpp
esp_err_t goblin_eye_init(void);
void goblin_eye_act(void);
```

## Patterns & data flow
- Component timing uses `timing.hitCount`; lower numbers execute more frequently (for example eyes at 1, mood components at 10+).
- SharedMemory writes incur ESP-NOW traffic, only perform after state changes; reads are free.
- Dynamic pin assignment lives in `include/esp32_s3_r8n16_pin_assignments.h`; SPI/I2C/ADC buses assign pins during their `init()` and rotate within `act()`.

## Reference map
- **Generic Components**: `config/components/{hardware|drivers|interfaces|behaviors}/`
- **Multi-Family Components**: `config/bots/multi_family/{humanoid|quadruped|tentacles|wings}/`
- **Creature-Specific Components**: `config/bots/bot_families/{family}/{subsystem}/` (e.g., goblins/head/, bears/torso/)
- **Bot Assemblies**: `config/bots/bot_families/{family}/*.json` (root definitions for each creature)
- **Shared Structs** (`Mood`, `Environment`, `Personality`): `shared/`
- **Validation Tools** (RUN BEFORE CODE GENERATION):
  - `tools/validate_file_structure.py` - Auto-correct component references, enforce RULE 4/6
  - `tools/generate_tables.py` - Generate subsystem code from JSON (AFTER validation passes)
  - `tools/generate_file_structure.ps1` - Generate folder structure
- **Build configs**: `platformio.ini` (env filters), `.pio/**` (build outputs - never edit)

