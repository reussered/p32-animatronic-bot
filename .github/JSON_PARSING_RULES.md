[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# JSON Parsing Rules - Complete Reference

## Overview

This document consolidates all the rules about how JSON files are parsed, processed, and validated in the P32 Animatronic Bot project. This is essential understanding for component generation, bot assembly, and system architecture.

---

## RULE 11D: JSON FILE ENCODING AND VALIDATION

### ASCII Encoding is MANDATORY - No UTF-8 BOM

All JSON configuration files MUST be saved as pure ASCII without UTF-8 BOM.

**Why This Matters:**
- UTF-8 BOM (bytes `EF BB BF` at file start) breaks Python JSON parser
- Error: "Expecting value: line 1 column 1 (char 0)"
- This is a known recurring issue in the project

**Correct File Format (PowerShell):**
```powershell
# Save JSON as ASCII without BOM
[System.IO.File]::WriteAllText($path, $content, [System.Text.Encoding]::ASCII)
```

**Detection of BOM Corruption (PowerShell):**
```powershell
# Check for BOM corruption
$bytes = [System.IO.File]::ReadAllBytes($jsonFile)
if ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) {
    Write-Host "UTF-8 BOM DETECTED - File is corrupted!" -ForegroundColor Red
    # Fix it:
    $content = Get-Content $jsonFile -Raw
    [System.IO.File]::WriteAllText($jsonFile, $content, [System.Text.Encoding]::ASCII)
}
```

**Validation Rules:**
- Always use ASCII encoding
- Never use UTF-8 with BOM
- Verify first 3 bytes are NOT `239, 187, 191`
- First byte should be `123` (the `{` character)
- Run validation: `python config/validate.py` after creating JSON files

---

## RULE 4: COMPONENT PIPELINE EXECUTION MODEL

### The `components: []` Array - CRITICAL RULE

Each JSON component defines a `components: []` array that specifies child components for execution. This array defines the execution sequence and data pipeline.

**The components array contains file paths or component names that must resolve to actual JSON files.**

**Example Component Pipeline:**

```json
goblin_left_eye.json:
  {
    "name": "goblin_left_eye",
    "components": ["goblin_eye.json"]
  }

goblin_eye.json:
  {
    "name": "goblin_eye",
    "components": ["gc9a01.json"]
  }

gc9a01.json:
  {
    "name": "gc9a01",
    "components": ["spi_display_bus.json"]
  }

spi_display_bus.json:
  {
    "name": "spi_display_bus",
    "components": ["generic_spi_display.json"]
  }

generic_spi_display.json:
  {
    "name": "generic_spi_display",
    "components": []  // Leaf node - no further dependencies
  }
```

### Key Rules for components array

1. **Order matters** - Components execute in the order they appear in the array
2. **All must exist** - Every entry must reference an actual JSON file that can be found and loaded
3. **Can be empty** - For leaf components (components with no children), `components: []` must be an empty array
4. **Depth-first traversal** - Parser walks deep into the tree before going wide
5. **No circular references** - A component cannot indirectly reference itself through its children
6. **References must be resolvable** - File paths must be correct or component names must be globally unique

---

## RULE 4: PARSER JOB - DEPTH-FIRST TRAVERSAL WITH AGGREGATION ON FIRST ENCOUNTER

### The Complete Parsing Algorithm

When the generator processes a component JSON, it performs a **depth-first tree walk** of the `components: []` hierarchy.

#### Step 1: DEPTH-FIRST TRAVERSAL

Walk the tree in order, going deep before going wide.

**Example tree visualization:**
```
goblin_left_eye
   goblin_eye
     gc9a01
        spi_display_bus
           generic_spi_display
   (end of left_eye branch)

Depth-first order encountered:
goblin_left_eye  goblin_eye  gc9a01  spi_display_bus  generic_spi_display
```

#### Step 2: FOR EACH COMPONENT ENCOUNTERED

**a) Load component.json file**
- Read the JSON file from disk
- Parse and validate the JSON structure
- Must contain required fields: name, type, version, etc.

**b) AGGREGATION CHECK - Is this the FIRST TIME we've seen this component name?**
- **FIRST ENCOUNTER**: 
  - Aggregate .src file into `{component_name}_component_functions.cpp`
  - Aggregate .hdr file into `{component_name}_component_functions.hpp`
  - These files are generated ONCE, never again for this component
- **DUPLICATE ENCOUNTER**: 
  - Skip aggregation (already aggregated the first time)
  - Code is reused from first encounter

**c) DISPATCH TABLE ENTRY**
- Add component to dispatch table (ALWAYS, even if duplicate)
- First encounter: dispatch_table entry 1
- Duplicate encounter: dispatch_table entry 2, 3, etc. (all are added)
- Order in dispatch table = order of tree traversal

#### Step 3: RESULT

- **Aggregated files**: Only contain code from FIRST encounter of each unique component
- **Dispatch tables**: Contain EVERY component encountered (including duplicates)
- **Execution order**: Determined by order of tree traversal (depth-first)

### CRITICAL IMPLICATIONS

- **Duplicates are expected and correct**: If the same component appears twice in a tree, both get dispatch entries
- **Only first .src/.hdr aggregated**: `gc9a01.src` is aggregated once (at first encounter), reused for all subsequent references
- **Dispatch table has duplicates**: If `gc9a01` appears in both left_eye and right_eye trees, it gets two dispatch table entries
- **Execution frequency determined by position**: Lower `hitCount` components execute more frequently
- **Same code, different data contexts**: `gc9a01` code executes twice per loop if it's in two different pipelines, but operates on different buffers (managed by file-scoped statics)

### Visual Example

```
Bot tree structure:

goblin_head
   goblin_left_eye
     goblin_eye (first encounter)
        gc9a01 (first encounter)
           spi_display_bus (first encounter)
              generic_spi_display (first encounter)
  
   goblin_right_eye
      goblin_eye (duplicate - skip aggregation)
         gc9a01 (duplicate - skip aggregation)
            spi_display_bus (duplicate - skip aggregation)
               generic_spi_display (duplicate - skip aggregation)

AGGREGATED FILES CREATED:
- goblin_left_eye_component_functions.cpp (aggregates: goblin_left_eye.src)
- goblin_eye_component_functions.cpp (aggregates: goblin_eye.src - ONLY ONCE)
- gc9a01_component_functions.cpp (aggregates: gc9a01.src - ONLY ONCE)
- spi_display_bus_component_functions.cpp (aggregates: spi_display_bus.src - ONLY ONCE)
- generic_spi_display_component_functions.cpp (aggregates: generic_spi_display.src - ONLY ONCE)
(Same for .hpp files)

DISPATCH TABLE ENTRIES (IN ORDER):
1. goblin_left_eye_init() + goblin_left_eye_act()
2. goblin_eye_init() + goblin_eye_act()         [DUPLICATE ENTRY 1]
3. gc9a01_init() + gc9a01_act()                 [DUPLICATE ENTRY 1]
4. spi_display_bus_init() + spi_display_bus_act() [DUPLICATE ENTRY 1]
5. generic_spi_display_init() + generic_spi_display_act() [DUPLICATE ENTRY 1]
6. goblin_right_eye_init() + goblin_right_eye_act()
7. goblin_eye_init() + goblin_eye_act()         [DUPLICATE ENTRY 2]
8. gc9a01_init() + gc9a01_act()                 [DUPLICATE ENTRY 2]
9. spi_display_bus_init() + spi_display_bus_act() [DUPLICATE ENTRY 2]
10. generic_spi_display_init() + generic_spi_display_act() [DUPLICATE ENTRY 2]

RUNTIME RESULT:
- goblin_eye code runs TWICE per loop (once for left eye, once for right eye)
- gc9a01 code runs TWICE per loop (once for left eye, once for right eye)
- Each call operates on different static buffers (managed by component file-scoping)
- No code duplication (same .src/.hdr used for both)
```

---

## RULE 6B: COMPONENT REFERENCES IN COMPONENTS ARRAY

### How Components Are Referenced

JSON `components: []` arrays specify the next component in the chain using **file paths or relative paths to JSON files**.

**Parser Requirements:**
- Every entry in `components: []` must be a valid file path to an actual component JSON file
- File paths can be relative (to the current component's directory) or full paths from project root
- Parser loads component JSON file from the specified path
- Component JSON files contain `relative_filename` field documenting the full path from project root

**Example valid JSON:**
```json
{
  "name": "bear_left_eye",
  "relative_filename": "config/bots/bot_families/bears/head/bear_left_eye.json",
  "components": ["bear_eye.json"]
}

{
  "name": "bear_eye",
  "relative_filename": "config/bots/bot_families/bears/head/bear_eye.json",
  "components": ["gc9a01.json"]
}

{
  "name": "gc9a01",
  "relative_filename": "config/components/drivers/gc9a01.json",
  "components": []
}
```

**Parser resolution process:**
1. Read file path from `components: []` array (e.g., "bear_eye.json")
2. Resolve path relative to current component's directory (or use as-is if absolute)
3. Load component's JSON file from resolved path
4. If file not found at specified path: ERROR
5. Continue depth-first traversal with loaded component

---

## MANDATORY JSON FIELDS FOR ALL COMPONENTS

Every component JSON file MUST include these fields:

```json
{
  "relative_filename": "config/components/positioned/component.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "name": "unique_component_name",
  "type": "DISPLAY_DRIVER",
  "description": "Human readable description",
  "timing": {
    "hitCount": 10
  },
  "components": []
}
```

**Field Definitions:**
- `relative_filename`: Path from project root to this JSON file
- `version`: Semantic version of this component (major.minor.patch)
- `author`: Reference to author configuration file
- `name`: Globally unique component identifier
- `type`: Component category (DISPLAY_DRIVER, SENSOR, BEHAVIOR, etc.)
- `description`: Human-readable purpose of this component
- `timing.hitCount`: Execution frequency (lower = more frequent)
- `components`: Array of child components to execute

---

## FILE ORGANIZATION & RESOLUTION PATHS

### Component Organization

**Generic/Reusable Components:**
- Hardware: `config/components/hardware/{name}.json|src|hdr`
- Drivers: `config/components/drivers/{name}.json|src|hdr`
- Interfaces: `config/components/interfaces/{name}.json|src|hdr`
- Behaviors: `config/components/behaviors/{name}.json|src|hdr`
- Functional: `config/components/functional/{name}.json|src|hdr`

**Creature-Specific Components:**
- Location: `config/bots/bot_families/{family}/{subsystem}/{component_name}.json|src|hdr`
- Families: `androids/`, `animals/`, `bears/`, `cats/`, `dragons/`, `goblins/`, etc.
- Subsystems: `head/`, `torso/`, `arms/`, `legs/`, `hands/`, `feet/`

### EXCEPTION: Subsystem Components Do NOT Have Src/Hdr Files

**Subsystem-level components are an exception to the general rule that all components have .src and .hdr files.**

Subsystem components (e.g., `goblin_head`, `goblin_torso`) represent the aggregation point for an entire subsystem running on a specific ESP32 chip. Their .src and .hdr files are **generated** by `generate_tables.py`, not hand-written.

**Example Structure:**
```
config/bots/bot_families/goblins/head/goblin_head.json  SOURCE (hand-written)
config/bots/bot_families/goblins/head/goblin_head.json can optionally reference:
  - config/bots/bot_families/goblins/head/goblin_head.hdr (optional)
  - config/bots/bot_families/goblins/head/goblin_head.src (optional)

BUT the real generated files are:
components/goblin_full/goblin_head.src  GENERATED (do not edit)
include/components/goblin_head.hdr  GENERATED (do not edit)
```

**Key Points:**
- Subsystem component JSON files ARE hand-written in `config/bots/bot_families/{family}/{subsystem}/`
- The `.src` and `.hdr` files for subsystems are GENERATED by `generate_tables.py`
- Generated files go to `components/{subsystem_name}/` and `include/components/`
- Never hand-edit generated subsystem files
- When you modify the subsystem's component JSON, regenerate with: `python tools/generate_tables.py config/bots/bot_families/{family}/{subsystem}/{subsystem_name}.json src`

---

## SPECIAL JSON PATTERNS

### Animation Files (NOT Component Pipeline)

**Location:** `assets/animations/{creature}/*.json`

Animations use a different pattern than component pipelines:

```json
{
  "animation_name": "goblin_blink",
  "components": [
    {
      "component_id": "goblin_eye_left",
      "keyframes": [
        {
          "time_ms": 0,
          "display_content": { "eye_openness": 0.8 }
        },
        {
          "time_ms": 100,
          "display_content": { "eye_openness": 0.0 }
        }
      ]
    }
  ]
}
```

**Key Differences:**
- Uses `component_id` (not file paths) to reference components by name
- Contains `keyframes` array (animation data, not executables)
- These are metadata/data files, not executable components
- Animation system reads these and applies keyframe data to named components at runtime
- NOT subject to RULE 4 component pipeline requirements

### Mounting Architecture Specification

**Every positioned component JSON MUST include:**

```json
{
  "mounting_architecture": {
    "type": "two_tier | integrated | hardware_direct",
    "hardware_mount": "basic_mount_name (if two_tier)",
    "decorative_shell": "shell_name (if two_tier)",
    "integrated_shape": "shape_name (if integrated)",
    "shape_generation": "auto | manual | template"
  }
}
```

---

## ERROR HANDLING IN PARSING

### Common JSON Parsing Errors

1. **UTF-8 BOM Present**
   - Symptom: "Expecting value: line 1 column 1 (char 0)"
   - Cause: File saved with UTF-8 BOM instead of ASCII
   - Fix: Resave as ASCII without BOM

2. **Missing or Invalid components: [] Path**
   - Symptom: Parser can't find referenced component file
   - Cause: Typo in file path or file doesn't exist at specified location
   - Fix: Check file path spelling and verify file exists at that path

3. **Missing relative_filename Field**
   - Symptom: Documentation incomplete (doesn't prevent parsing, but metadata missing)
   - Cause: Component JSON missing `relative_filename` field
   - Fix: Add `relative_filename` field with full path from project root for documentation

4. **Invalid JSON Syntax**
   - Symptom: "JSON decode error"
   - Cause: Missing comma, bracket, or quotes
   - Fix: Validate JSON with `python config/validate.py`

5. **Circular References**
   - Symptom: Infinite loop during traversal
   - Cause: ABCA or ABA in components arrays
   - Fix: Break the cycle in components arrays

6. **Duplicate Component Names**
   - Symptom: "Component name not unique"
   - Cause: Two different JSON files have same "name" field
   - Fix: Ensure all component names are globally unique

---

## VALIDATION WORKFLOW

**After creating or modifying JSON files:**

1. **Check encoding:**
   ```powershell
   $bytes = [System.IO.File]::ReadAllBytes($jsonFile)
   if ($bytes[0] -eq 239) { Write-Host "BOM DETECTED!" }
   ```

2. **Validate JSON syntax:**
   ```powershell
   python config/validate.py
   ```

3. **Check component references:**
   - All entries in `components: []` must be resolvable
   - Component names must be globally unique

4. **Regenerate affected subsystems:**
   ```powershell
   python tools/generate_tables.py {bot_name} src
   ```

5. **Build and test:**
   ```powershell
   pio run -e {environment}
   ```

---

## SUMMARY: JSON PARSING RULES AT A GLANCE

| Rule | Details |
|------|---------|
| **Encoding** | ASCII-only, no UTF-8 BOM, no Unicode |
| **components array** | Ordered list of file paths to component JSON files |
| **File paths** | Can be relative (to component directory) or full paths |
| **relative_filename** | Metadata field documenting full path from project root |
| **Traversal** | Depth-first, processing parents before children |
| **Aggregation** | Code aggregated only on FIRST encounter of each component |
| **Dispatch table** | Contains entry for EVERY component encounter (duplicates allowed) |
| **Component names** | Must be globally unique (fermionic - no duplicates) |
| **Mandatory fields** | name, type, version, timing, components, relative_filename |
| **Leaf components** | Have empty `components: []` array |
| **Circular refs** | NOT allowed - will cause infinite loop |
| **Error handling** | Check file paths first, then validate JSON syntax |

---

## References

- **AI-AGENT-RULES.md** - RULE 4 (Component Pipeline), RULE 11D (Encoding)
- **consolidated-rules.md** - Complete rules reference
- **naming_rules.md** - Component naming conventions
- **ASCII-ENCODING-GUIDE.md** - Encoding rules and ASCII replacements
