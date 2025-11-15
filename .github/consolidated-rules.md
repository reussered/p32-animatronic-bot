# P32 Animatronic Bot - Consolidated Rules

## Context

- **Target Hardware**: ESP32-S3/ESP32-C3 microcontrollers.
- **Framework**: ESP-IDF native APIs.
- **Resource Constraints**: Memory and performance constrained embedded environment.
- **Encoding**: ASCII-only for toolchain compatibility (NO UTF-8 BOM).
- **Toolchain**: PlatformIO + ESP-IDF, GCC cross-compiler.
- **Debugging**: Serial console, hardware debugger.
- **Performance**: Real-time constraints, interrupt-driven.

## File Organization

### General Rules

- **ASCII Characters Only**: All filenames and content must use standard ASCII characters (32-126).
- **Case Sensitivity**: If there is only one class specification in the file, the filename must match the class name exactly (case-sensitive).
- **Global Uniqueness**: Component names must be unique across the entire system.
- **File Types**:

  - `.json`: Configuration files.
  - `.src`: Source code fragments.
  - `.hdr`: Header code fragments.

### Directory Structure

- **Components**: `config/components/{category}/{component_name}.json`.
- **Subsystems**: `config/bots/bot_families/{family}/{subsystem}/{component_name}.json`.
- **Generated Code**:

  - `src/components/{component_name}.cpp`
  - `include/components/{component_name}.hpp`

- **Assets**:

  - 3D Models: `assets/shapes/scad/{category}/{model}.scad`.
  - Animations: `assets/animations/{creature_family}/{animation_name}.json`.

## Component Rules

### JSON Structure

- **Recommended Fields** (not strictly required):

  ```json
  {
    "version": "1.0.0",
    "author": "config/author.json",
    "name": "unique_component_name",
    "type": "DISPLAY_DRIVER",
    "description": "Human readable description",
    "timing": {
      "hitCount": 10
    }
  }
  ```

  > Note: `relative_filename` may be present for human-readability and some tooling convenience, but it is no longer a required field for JSON validation.

- **Encoding**: ASCII-only, no UTF-8 BOM.
- **Validation**: Use `python config/validate.py` to ensure compliance.

### Function Signatures

- Init functions: `esp_err_t {component_name}_init(void);`
- Action functions: `void {component_name}_act(void);`

### Communication Patterns

- **Intra-Subsystem**: Use file-scoped `static` variables for communication.
- **Inter-Subsystem**: Use `SharedMemory` (`GSM.read<T>()` and `GSM.write<T>()`).

## Subsystem Rules

### Naming

- Subsystem names are derived from the `controller` field in JSON.
- Generated files:

  - `src/subsystems/{name}/{name}_component_functions.cpp`
  - `include/subsystems/{name}/{name}_component_functions.hpp`

### Communication

- **Intra-Subsystem**: File-scoped globals.
- **Inter-Subsystem**: `SharedMemory` only.

## Build and Deployment

### Workflow

1. Modify JSON configurations.
2. Run generation scripts:

   ```powershell
   python tools/generate_tables.py {bot_name} src
   ```

3. Build:

   ```powershell
   pio run -e {environment}
   ```

4. Flash and test.

### Critical Rules

- Never manually edit generated files.
- Regenerate tables after any JSON or `.src` change.

## Historical Documents

- The following documents are now obsolete and have been consolidated into this file:

  - `GLOBAL-SHARED-MEMORY-API.md`
  - `JSON-DRIVEN-PERSONALITY-SPEC.md`
  - `DISPATCH-TABLE-GENERATION-RULES-SPEC.md`

---

This document serves as the single source of truth for all rules and guidelines in the P32 Animatronic Bot project.