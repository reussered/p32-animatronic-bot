# Component Scaffolding Tool

## Overview

The `ensure_component_scaffolding.py` tool ensures that every component JSON file in `config/components/` has associated `.src` (implementation) and `.hdr` (header) files in the same directory.

**Key Principle**: Per the project's copilot-instructions, *"every single src and hdr file belong in the same directory as their json file"*.

## What It Does

1. **Scans** all JSON files in `config/components/` recursively
2. **Checks** for matching `.src` and `.hdr` files in the same directory
3. **Creates** scaffolding files if they don't exist
4. **Reports** summary statistics

## Scaffolding Generated

### `.hdr` Files (Header Template)

Generated headers include:

- Include guards using component name: `#ifndef COMPONENT_NAME_HDR`
- Standard ESP-IDF includes (`esp_err.h`, `stdint.h`)
- C++ compatibility guards (`extern "C"`)
- Function declarations:
  - `esp_err_t component_name_init(void)` - Initialization function
  - `void component_name_act(void)` - Action/dispatch function
- Doxygen-style documentation comments

### `.src` Files (Implementation Template)

Generated implementations include:

- Required includes: `esp_err.h`, `freertos/FreeRTOS.h`, `freertos/task.h`
- Standard function signatures matching the `.hdr`
- TODO comments prompting developers to fill in logic
- Minimal skeleton that compiles without errors

## Usage

### Run Once to Create All Missing Files
```powershell
python tools/ensure_component_scaffolding.py
```

### Sample Output
```
Scanning component JSON files for missing .src and .hdr files...

Found 421 component JSON files

[OK] config\components\creature_specific\android_emotion_display.json: CREATED: .hdr, .src
[OK] config\components\creature_specific\android_eye.json: CREATED: .hdr
...

================================================================================
Summary:
  Total components: 421
  Created scaffolding: 41
  Already complete: 380
  Errors: 0
================================================================================
```

## Integration with Build System

### Phase 1: Complete Scaffolding (DONE )
- Created 41 missing scaffolding files
- 380 components already had files
- Total: 421 components now have `.json`, `.src`, and `.hdr` triplets

### Phase 2: Validation Integration (PLANNED)
The validator tool (`component_template_validator.py`) should:
1. Check that `.hdr` and `.src` exist for each JSON component
2. Verify function signatures match between `.hdr` and `.src`
3. Validate that init/act functions are properly declared

### Phase 3: Generation Integration (PLANNED)
The generator tool (`template_expander.py`) should:
1. Check for template specs in component JSON
2. Generate config structs based on template args
3. Pass config structs to init/act functions via Global Shared Memory

## File Structure

Each component typically has this structure:
```
config/components/creature_specific/
 goblin_left_eye.json              (Metadata, templates, config)
 goblin_left_eye.hdr               (Public interface)
 goblin_left_eye.src               (Implementation)
```

## Important Notes

### Naming Conventions
- Component name derived from JSON's `"name"` field, or falls back to filename
- Function names: `{component_name}_init()` and `{component_name}_act()`
- Header guard: `{COMPONENT_NAME}_HDR`

### Generated Code Quality
Scaffolding is intentionally minimal:
- **Compiles** without errors
- **Runs** without segfaults (returns ESP_OK, does nothing)
- **Requires** developer implementation of actual logic
- **Marked** with TODO comments indicating what needs done

### Running Tool Multiple Times
The tool is **idempotent**:
- Won't overwrite existing `.src` or `.hdr` files
- Won't recreate already-present scaffolding
- Safe to run repeatedly in CI/CD

## Error Handling

The tool handles:
- Invalid JSON files (reports error, skips)
- Unicode encoding issues (uses UTF-8)
- File system permission errors (reports and continues)
- Missing JSON `"name"` field (falls back to filename)

## Future Enhancements

1. **Sync Validation**: Verify JSON component names match filename
2. **Function Signature Checking**: Ensure `.hdr` and `.src` match
3. **Template Aware Generation**: Create init/act signatures based on templates
4. **CI Integration**: Enforce scaffolding in pre-commit hooks
5. **Documentation Generation**: Create markdown docs from Doxygen comments

## Related Tools

- `component_template_validator.py` - Validates component JSON and templates
- `template_expander.py` - Expands template references to hardware configs
- `generate_tables.py` - Main build system orchestrator

## See Also

- `.github/copilot-instructions.md` - Project architecture rules
- `MULTI_HARDWARE_COMPONENT_SPEC.md` - Multi-hardware component design
- `PIN_ASSIGNMENT_RULES.md` - Dynamic pin assignment guidelines
