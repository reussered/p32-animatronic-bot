# P32 Animatronic Bot - AI Agent Guide

## Context
- Embedded ESP32-S3/ESP32-C3 project; think tight RAM (~512KB) and real-time loops, not web stacks.
- Tooling is PlatformIO + ESP-IDF running on Windows PowerShell; paths are absolute like `F:\GitHub\p32-animatronic-bot\...`.
- ASCII-only encoding everywhere (no BOM); Allman braces with mandatory `{}` on every control flow block.
- Before touching code reread the rest of the files in       `.github`

## Architecture
- **CRITICAL: Single Compilation Unit**: The build script (`tools/generate_tables.py`) concatenates all component `.src` files for a subsystem (e.g., `goblin_head`) into a single, large `.cpp` file. This means all `.src` files within that subsystem share the **same file scope**.
- **Intra-Subsystem Communication**: Because of the single compilation unit, components within the same subsystem **must** communicate using file-scoped `static` variables (e.g., `static uint16_t* display_buffer = NULL;`). This is the standard way to pass data along a processing chain. You do **not** need to `#include` other component headers for this to work; the build system aggregates them.
- **Component System**: Every behavior lives in `config/components/**/*.src` (logic) and `.hdr` (definitions), exposing `{name}_init()` / `{name}_act()` functions.
- **JSON-Driven Composition**: A hierarchy of JSON files starting from `config/bots/...` defines the animatronic, composes components, and assigns them to controllers.
- **Inter-Subsystem Communication**: Data flow between different ESP32 chips (e.g., head to torso) happens **exclusively** through the Global Shared Memory system: `GSM.read<T>()` and `GSM.write<T>()`.

## Critical workflows
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
- **CRITICAL: .src/.hdr aggregation model**: `.hdr` and `.src` files have a DIFFERENT relationship than traditional `.hpp`/`.cpp` files:
  - ALL `.hdr` files aggregate into ONE subsystem `.hpp` file
  - ALL `.src` files aggregate into ONE subsystem `.cpp` file
  - The aggregated `.hpp` is `#include`d ONCE at the top of the aggregated `.cpp`
  - **NEVER `#include` a `.hdr` file from a `.src` file** - it makes no sense; the `.hdr` content is already in the generated `.hpp` that wraps the entire `.cpp`
  - `.hdr` files should contain ONLY declarations (function prototypes, struct definitions)
  - `.hdr` files must NOT contain `extern "C"` blocks (all code is C++ in the generated files)
  - **NEVER use `extern` declarations in `.src` files** - all `.src` files aggregate into the same compilation unit, so declare variables once at file scope using `static` at the top of the first `.src` file that needs them
  - **NEVER call functions from `.src` files in other subsystems** - inter-subsystem communication happens ONLY via GSM (Global Shared Memory)

## Patterns & data flow
- Component timing uses `timing.hitCount`; lower numbers execute more frequently (for example eyes at 1, mood components at 10+).
- SharedMemory writes incur ESP-NOW traffic, only perform after state changes; reads are free.
- Dynamic pin assignment lives in `include/esp32_s3_r8n16_pin_assignments.h`; SPI/I2C/ADC buses assign pins during their `init()` and rotate within `act()`.

## Reference map
- Component logic: `config/components/hardware/`, `config/components/drivers/`, `config/components/behaviors/`, `config/components/functional/`, `config/components/interfaces/`
- Creature-specific components: `config/bots/bot_families/{family}/{subsystem}/{component_name}.json|src|hdr`
- Bot assemblies: `config/bots/bot_families/goblins/goblin_full.json` (root for head/torso/etc.).
- Shared state classes: `shared/*.hpp` (Mood, Environment, Personality, SysTest, etc.)
- Shared header definitions: `config/shared_headers/*.hpp` (color_schema, etc.)
- Tooling helpers: `tools/generate_tables.py`, `tools/generate_file_structure.ps1`, pin guidance `PIN_ASSIGNMENT_RULES.md`.
- Build configs: `platformio.ini` (env filters), `.pio/**` for build outputs (never edit).

