# Cross-Document Consistency Report
**Generated after comprehensive review of all 5 .github rule documents**

## Summary
 **CRITICAL FINDING**: All documents are substantially consistent. However, **three significant documentation issues** require correction:

1. **CONSOLIDATED-RULES.md is deliberately incomplete** - Headers for obsolete sections without content
2. **NAMING_RULES.md and COMPONENT-JSON-REQUIREMENTS.md both contain deprecated headers** - They explicitly state consolidation but provide full content anyway
3. **Minor inconsistency in subsystem file naming terminology** - Some docs say "six files" vs. actual delivery patterns

---

## Issue 1: Deprecated Headers in Multiple Documents 

### Location
- `NAMING_RULES.md` - Line 1-2
- `component-json-requirements.md` - Line 1-2

### Finding
**Both files BEGIN with:**
```markdown
**Note**: This document has been consolidated into `consolidated-rules.md`. 
Please refer to that file for the latest rules and guidelines.
```

**BUT THEN BOTH PROVIDE COMPLETE CONTENT**

### Impact
-  **Confusing to users**: "Go read consolidated-rules" then 300+ lines of the same content
-  **Maintenance burden**: Three places to update instead of one
-  **Version control noise**: Multiple files track same information

### Recommendation
**EITHER:**
- **Option A (Recommended)**: Delete NAMING_RULES.md and COMPONENT-JSON-REQUIREMENTS.md entirely. Move any unique content to consolidated-rules.md
- **Option B (Alternative)**: Remove the "consolidated" headers since the documents still serve as authoritative sources

---

## Issue 2: CONSOLIDATED-RULES.md is Incomplete 

### Location
`consolidated-rules.md` - Entire document

### Finding
**The document has structural headers but NO CONTENT under key sections:**

```markdown
## Component Rules

### JSON Structure

- **Mandatory Fields**:

  ```json
  {
    "relative_filename": "config/components/positioned/component.json",
    ...
  }
  ```

- **Encoding**: ASCII-only, no UTF-8 BOM.
- **Validation**: Use `python config/validate.py` to ensure compliance.

### Function Signatures
...
```

**PROBLEM**: Sections are collapsed/summarized. Critical information like:
-  Component categories (System/Family/Bot-specific levels)
-  Timing guidelines (hitCount values)
-  Communication patterns (detailed intra vs inter-subsystem)
-  Dynamic pin assignment rules
-  Component traversal semantics
-  Subsystem boundaries and controller keyword

**ARE NOT PRESENT** in consolidated-rules.md but **ARE FULLY DOCUMENTED** in component-json-requirements.md

### Impact
-  Users directed to "single source of truth" but it's incomplete
-  Code generation rules are in TWO places
-  Risk of generating code from outdated incomplete specification

### Recommendation
**Expand consolidated-rules.md to be truly comprehensive**, including all sections from both component-json-requirements.md AND NAMING_RULES.md, then deprecate the original documents.

---

## Issue 3: Subsystem File Naming - "Six Files" vs. Actual Pattern

### Locations
- `AI-AGENT-RULES.md` - Line 555-575 (RULE 9)
- `consolidated-rules.md` - No explicit count
- `copilot-instructions.md` - Aligns with six-file pattern

### Finding
**AI-AGENT-RULES.md explicitly states SIX files:**
```
1. **`src/{bot}_{subsystem}_dispatch_tables.cpp`**
2. **`include/{bot}_{subsystem}_dispatch_tables.hpp`**
3. **`src/{bot}_{subsystem}_component_functions.cpp`**
4. **`include/{bot}_{subsystem}_component_functions.hpp`**
5. **`src/{bot}_{subsystem}_main.cpp`**
6. **`include/{bot}_{subsystem}_main.hpp`**
```

**component-json-requirements.md also specifies SIX files (section 5):**
1. `src/subsystems/{name}/{name}_component_functions.cpp`
2. `include/subsystems/{name}/{name}_component_functions.hpp`
3. `src/subsystems/{name}/{name}_dispatch_tables.cpp`
4. `include/subsystems/{name}/{name}_dispatch_tables.hpp`
5. `src/subsystems/{name}/{name}_main.cpp`
6. `include/subsystems/{name}/{name}_main.hpp`

**BUT PATH PATTERNS DIFFER:**
- AI-AGENT-RULES: `src/{bot}_{subsystem}_*.cpp` (flat structure)
- component-json-requirements: `src/subsystems/{name}/*.cpp` (nested structure)

### Impact
-  **Ambiguity**: Which path pattern is correct?
-  **Build system confusion**: CMakeLists.txt may not find files in expected location
-  **Generation tool alignment**: Code generator must match one of these patterns

### Recommendation
**Clarify and normalize:**
- Verify actual file generation in `tools/generate_tables.py` to see what's actually generated
- Update ONE document to match reality
- Update the other documents to match
- Add explicit path examples to consolidated-rules.md

---

## Issue 4: "insectoid" vs. No Multi-Family Subsystem Type Definitions 

### Locations
- `AI-AGENT-RULES.md` - Line 365 (RULE 6)
- `consolidated-rules.md` - Line 24
- `copilot-instructions.md` - Line 13
- `NAMING_RULES.md` - Line 138

### Finding
**AI-AGENT-RULES.md specifies:**
```
- Types: `humanoid/`, `quadruped/`, `insectoid/`
```

**BUT consolidated-rules.md specifies:**
```
- Types: `humanoid/`, `quadruped/`, `insectoid/`
```

**AND copilot-instructions.md specifies:**
```
- `config/bots/multi_family/{humanoid|quadruped|insectoid}/`
```

**CONFLICT**: Are multi-family types limited to these three, or are others allowed?

**ACTUAL FILESYSTEM** shows:
- `config/bots/multi_family/humanoid/`
- `config/bots/multi_family/quadruped/`
- `config/bots/multi_family/tentacles/`  (EXISTS but not documented)
- `config/bots/multi_family/wings/`  (EXISTS but not documented)

### Impact
-  **Documentation doesn't match reality**: Two types are missing from rules
-  **Generation tool confusion**: May not recognize valid multi-family types
-  **Validation errors**: Consistency checks will fail on valid components

### Recommendation
**UPDATE ALL DOCUMENTS to include complete list:**
```
- Types: `humanoid/`, `quadruped/`, `insectoid/`, `tentacles/`, `wings/`
```

---

## Issue 5: Subsystem Versioning Clarity in RULE 11B

### Location
`AI-AGENT-RULES.md` - Line 620-670 (RULE 11B)

### Finding
**RULE 11B is well-defined BUT:**
- Defines versioning system with major.minor.patch
- Requires upconversion when versions become obsolete
- Does NOT specify where version is stored in generated files

**Question**: Is version stored in:
-  `{subsystem}_main.hpp` as #define?
-  Dispatch tables as comment?
-  Component function struct?

### Current Status
 **Not a blocker** - The rule is clear enough for implementation, but implementation documentation would help

### Recommendation
Add example to RULE 11B showing version declaration in main.hpp:
```cpp
// In goblin_head_main.hpp
#define GOBLIN_HEAD_VERSION "1.0.0"
```

---

## Issue 6: Creature Family List - Missing Documentation 

### Locations
- `AI-AGENT-RULES.md` - Line 372 (partial list)
- `consolidated-rules.md` - Line 28 (partial list)
- `NAMING_RULES.md` - Line 144 (partial list)

### Finding
**All three documents list creature families but use different formats:**

AI-AGENT-RULES.md:
```
- Families: `goblins/`, `bears/`, `cats/`, `dragons/`, etc.
```

consolidated-rules.md:
```
- Families: `goblins/`, `bears/`, `cats/`, `dragons/`, `elves/`, `wolves/`, 
  `vampires/`, `zombies/`, `orcs/`, `horror/`, `steampunk/`, `androids/`, 
  `fantasy/`, `undead/`, `humanoids/`, `robots/`, `tests/`
```

NAMING_RULES.md:
```
Same as consolidated-rules.md
```

**ACTUAL FILESYSTEM COUNT**: 19 families verified:
- androids, animals, bears, cats, dragons, elves, fantasy, goblins, horror, humanoids, 
  mechanical, orcs, robots, steampunk, tests, undead, vampires, wolves, zombies

### Impact
-  **Incomplete examples**: AI-AGENT-RULES uses "etc." instead of full list
-  **No functional problem**: Both lists are correct, just different detail levels

### Recommendation
Update AI-AGENT-RULES.md line 372 to match consolidated-rules.md full list for consistency

---

## Cross-Document Verification Matrix

| Topic | AI-AGENT-RULES | NAMING_RULES | component-json-requirements | consolidated-rules | copilot-instructions | Status |
|-------|---|---|---|---|---|---|
| ASCII Encoding |  Yes |  Yes |  Yes |  Yes |  Yes | **CONSISTENT** |
| File organization paths |  Documented |  Documented |  Documented |  Documented |  Documented | **CONSISTENT** |
| Six-file generation |  Yes |  Implied |  Yes |  Abbreviated |  Yes | **INCONSISTENT PATH PATTERNS** |
| Multi-family types |  Partial |  Incomplete |  Not mentioned |  Partial |  Partial | **MISSING TYPES (tentacles, wings)** |
| Component uniqueness |  Yes |  Yes |  Yes |  Yes |  Yes | **CONSISTENT** |
| Communication patterns |  Full |  Brief |  Full |  Brief |  Full | **CONSISTENT** |
| Subsystem versioning |  Defined (RULE 11B) |  Not mentioned |  Implied |  Not mentioned |  Not mentioned | **INCONSISTENT** |
| Creature families |  Partial ("etc.") |  Complete |  Not listed |  Complete |  Partial | **INCONSISTENT DETAIL** |
| JSON structure |  Yes |  Brief |  Full |  Brief |  Not detailed | **INCONSISTENT DETAIL** |
| Bounded authority |  Yes |  No |  No |  No |  No | **UNIQUE TO AI-AGENT-RULES** |

---

## Recommendations (Priority Order)

###  CRITICAL - Must Fix
1. **Verify and normalize path patterns** for the six generated files
   - Confirm what `tools/generate_tables.py` actually generates
   - Update both AI-AGENT-RULES.md and component-json-requirements.md to match reality
   - Estimated effort: 15 minutes

2. **Add tentacles and wings to multi-family types** in all four rule documents
   - Update: AI-AGENT-RULES.md, consolidated-rules.md, NAMING_RULES.md, copilot-instructions.md
   - Change `insectoid` specification if tentacles/wings are newer
   - Estimated effort: 10 minutes

###  HIGH - Should Fix
3. **Expand consolidated-rules.md to include ALL critical sections**
   - Move complete content from component-json-requirements.md
   - Move complete content from NAMING_RULES.md
   - Then deprecate the source files
   - Estimated effort: 30 minutes

4. **Remove or update deprecated headers** in NAMING_RULES.md and component-json-requirements.md
   - Either delete the files completely OR remove the "consolidated" headers
   - Don't tell users to go to consolidated-rules then provide full content anyway
   - Estimated effort: 5 minutes

###  MEDIUM - Nice to Have
5. **Add subsystem versioning documentation** to NAMING_RULES.md and consolidated-rules.md
   - Reference RULE 11B from AI-AGENT-RULES.md
   - Show version declaration examples
   - Estimated effort: 10 minutes

6. **Complete creature family lists** across all documents
   - Update AI-AGENT-RULES.md to show full 19-family list instead of "etc."
   - Consistency check
   - Estimated effort: 5 minutes

---

## Conclusion

 **Overall Assessment**: The rule documents are **fundamentally consistent and well-organized**. The identified issues are primarily:
- Documentation presentation problems (redundancy, incomplete consolidation)
- Missing information on recently-added multi-family types
- Path pattern ambiguity that needs reality-check against code generator

**No conflicting architectural guidance found** - all differences are detail-level or presentation-style variations. The bounded authority framework, communication patterns, and component architecture are uniformly described across all documents.

**Recommended next step**: Verify file path patterns in `tools/generate_tables.py`, then update the path-related inconsistencies. These are blocking accurate code generation documentation.
