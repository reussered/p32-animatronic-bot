[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

# ASCII ENCODING MANDATE - Clear Guidelines

## The Rule (From RULE 3b and RULE 25)

**NO Unicode characters in ANY production file. EVER.**

Production files = any file that is:
- Included in the build system
- Part of the project repository
- Used by code (imported, included, referenced)
- Located in: `src/`, `include/`, `config/`, `tools/`, or any subdirectory thereof

---

## When Unicode is ABSOLUTELY FORBIDDEN

### Production Files (100% ASCII Required)

**C++ Code** - NEVER use Unicode
```cpp
// WRONG - contains degree symbol
float temp = 25°C;

// RIGHT - ASCII only
float temp = 25; // degrees C
```

**JSON Config** - NEVER use Unicode
```json
{
  "tolerance": "+/- 5",  // RIGHT - ASCII
  "rating": "[STAR] [STAR] [STAR] [STAR] [STAR]"  // RIGHT - emoji replaced
}
```

**Python Scripts** - NEVER use Unicode
```python
# WRONG
def get_angle():
    return "→ 90 degrees"

# RIGHT
def get_angle():
    return "-> 90 degrees"
```

**Documentation in src/, include/, config/, tools/** - NEVER use Unicode
```markdown
# WRONG - in src/components/README.md
This requires ±2 degrees tolerance

# RIGHT - in src/components/README.md
This requires +/- 2 degrees tolerance
```

---

## When Unicode is PERMITTED (Rare Exception)

**ONLY** if ALL 5 criteria are met:

### Criteria Checklist

1. **Is it standalone?** 
   - [ ] NOT included in any build process
   - [ ] NOT part of the build system
   - [ ] NOT compiled or processed

2. **Is it unincluded?**
   - [ ] NEVER imported by C++ code (no `#include`)
   - [ ] NEVER imported by Python (no `import`, `from`)
   - [ ] NEVER referenced by any code file
   - [ ] NEVER executed or evaluated

3. **Is it outside production directories?**
   - [ ] NOT in `src/`
   - [ ] NOT in `include/`
   - [ ] NOT in `config/`
   - [ ] NOT in `tools/`
   - [ ] Lives in root directory or dedicated docs folder

4. **Is it explicitly labeled?**
   - [ ] File starts with: `[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]`
   - [ ] This disclaimer is on line 1 or clearly visible at top

5. **Is it non-binding?**
   - [ ] Used only for human research/analysis/reference
   - [ ] Not version-controlled as part of production
   - [ ] Clear that it's informational, not prescriptive

### Examples That Qualify (Unicode Allowed)

**GOOD - Meets all 5 criteria:**
- `CAMERA_RESEARCH_FINDINGS.md` at root with disclaimer
  - Standalone research document
  - Never imported by code
  - Purely for human reference
  - Contains: "Temperature tolerance: ±2°C" (OK - Unicode allowed here)

- `FLYING_DRAGON_DECISION_LOG.md` at root with disclaimer
  - Historical design decisions
  - Never used in build
  - Research/analysis only
  - Contains: "Arrow direction: → northwest" (OK - Unicode allowed here)

- `.github/ARCHITECTURE_NOTES.md` with disclaimer
  - Design notes, not part of build
  - Human reference only
  - Contains: special symbols for readability (OK - Unicode allowed here)

### Examples That DO NOT Qualify (ASCII Only)

**BAD - Fails one or more criteria:**
- `README.md` in root (part of project identity, referenced)
- `src/components/README.md` (in production directory)
- Any file in `config/` (part of build)
- Any file in `tools/` (part of build system)
- Any Python script (executed, part of toolchain)
- Any JSON config (imported by code)
- Documentation for a component (referenced by component)

---

## The ASCII Replacement Table

If Unicode seems necessary, replace it with ASCII from this table:

| Symbol | ASCII Replacement | Example |
|--------|-------------------|---------|
| `°` | `degrees` | 25 degrees C |
| `±` | `+/-` | +/- 5 |
| `×` | `x` | 1024x768 |
| `≈` | `approx` | approx 3.14 |
| `→` | `->` | start -> end |
| `←` | `<-` | left <- right |
| `⟹` | `=>` | implies => result |
| `∞` | `infinity` | -infinity to +infinity |
| `∆` | `DELTA` | DELTA = change |
| `Ω` | `OMEGA` | R = OMEGA |
| `µ` | `micro` or `u` | microamps, uA |
| `⭐` | `[STAR]` | [STAR] excellent |
| `✓` | `[OK]` | [OK] passed |
| `✗` | `[FAIL]` | [FAIL] failed |
| `⚠️` | `[WARNING]` | [WARNING] unstable |
| `ℹ️` | `[INFO]` | [INFO] note |
| `❌` | `[REJECT]` | [REJECT] invalid |
| `✅` | `[ACCEPT]` | [ACCEPT] valid |
| `—` | `-` or `--` | text - more text |
| `…` | `...` | wait... |
| `«` | `"` | "quote" |
| `»` | `"` | "quote" |
| `━` | `-` or `--` | ---------- |
| `│` | `\|` | \| bar |
| `├` | `\|` | \|- junction |
| `└` | `\` | \ end |

---

## Decision Tree

When creating a file, follow this tree:

```
START: Creating a new file?

  ├─ Will it ever be imported/included/referenced by code?
  │  └─ YES → ASCII ONLY (RULE 25)
  │
  ├─ Will it be in src/, include/, config/, tools/?
  │  └─ YES → ASCII ONLY (RULE 25)
  │
  ├─ Will it be part of the build system?
  │  └─ YES → ASCII ONLY (RULE 25)
  │
  ├─ Is it standalone documentation in root or docs folder?
  │  ├─ YES + has disclaimer label
  │  │  ├─ YES → Unicode OK (research/reference only)
  │  │  └─ NO → Add disclaimer at top, then Unicode OK
  │  └─ NO → ASCII ONLY (RULE 25)
  │
  └─ UNSURE?
     └─ → ASCII ONLY (RULE 25) - Default to ASCII
```

---

## Common Mistakes

**Mistake 1: Adding Unicode to a file "just for readability"**
```
# WRONG
File: src/components/eye_control/README.md
Content: "Pan angle: 180° ± 5°"
Problem: In src/ directory = production file = ASCII only

# RIGHT
File: src/components/eye_control/README.md
Content: "Pan angle: 180 degrees +/- 5 degrees"
```

**Mistake 2: Using Unicode in documentation that will be referenced**
```
# WRONG
File: CAMERA_INTEGRATION.md
Problem: If this file is imported or referenced by code (e.g., in build script)
Content: "Field of view: ±45°"

# RIGHT
File: CAMERA_INTEGRATION.md (if NOT referenced by code)
Content: "Field of view: +/- 45 degrees"
OR
Content: "Field of view: +/- 45 degrees" (always use ASCII unless standalone exception applies)
```

**Mistake 3: Creating a Unicode file without the disclaimer**
```
# WRONG
File: RESEARCH_FINDINGS.md
Content: [No disclaimer]
Content: "Results showed ≈ 5% improvement"

# RIGHT
File: RESEARCH_FINDINGS.md
[STANDALONE DOCUMENTATION - NOT PART OF PROJECT BUILD - HUMAN REFERENCE ONLY]

Results showed approximately 5% improvement
(OR with Unicode if criteria met: ≈ 5% improvement)
```

---

## Why This Rule Exists

1. **Toolchain Compatibility**: Embedded ESP32 build system uses ASCII encoding (GCC, PlatformIO, ESP-IDF)
2. **Cross-platform Reliability**: ASCII guaranteed to work on Windows, Linux, macOS, embedded systems
3. **Git Compatibility**: UTF-8 can cause issues with Git diffs and merges
4. **Code Consistency**: Treating all code as ASCII prevents accidental Unicode in production
5. **Build Reproducibility**: Different systems might interpret Unicode differently

---

## Enforcement

- **Rule 3b**: Agent has autonomous authority to REFUSE to create files with Unicode (in production files)
- **Verification Gate Item 4**: Before creating ANY file, agent must verify ASCII compliance
- **RULE 25**: Comprehensive definition and exception criteria

If you create a production file with Unicode, you've violated RULE 3b and RULE 25. The file should have been rejected at creation time.

---

## Questions?

- Is the file in src/, include/, config/, or tools/? → ASCII ONLY
- Will code import/reference it? → ASCII ONLY  
- Is it standalone documentation at root with disclaimer? → Check all 5 criteria
- Unsure? → Default to ASCII
