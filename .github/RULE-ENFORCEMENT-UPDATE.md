# AI Agent Rule Enforcement Tightening

**Date**: November 6, 2025

**Issue**: AI agent created documentation files with Unicode characters despite project rule requiring ASCII-only encoding. Root cause: Rule was not enforced at file creation time; cleanup happened retroactively.

**Solution**: Tightened enforcement in `.github/AI-AGENT-RULES.md`

---

## Changes Made

### 1. Added Rule 3b: Autonomous Refusal Authority

**Location**: Bounded Authority Framework section (Line ~109)

**New Rule**:
```
3b. REFUSE to create files with Unicode: IF creating ANY file (code, docs, config, scripts) 
AND that file would contain non-ASCII characters (°, ±, →, ⭐, ⚠️, etc.) 
REJECT the file creation and rewrite using only ASCII 
(RULE 25 enforcement - do not create files then cleanup Unicode afterward; 
write ASCII-only from the start)
```

**Authority**: Agent now has autonomous authority to REFUSE file creation if it contains Unicode, and must rewrite using only ASCII.

---

### 2. Enhanced Verification Gate Checklist

**Location**: Mandatory Verification Gate section (Line ~197)

**Added Item 4**:
```
4. Are you creating ANY new files (code, docs, config, scripts)?
   - If YES: STOP and verify the content is 100% ASCII-only BEFORE creation (RULE 25)
   - Check for: °, ±, →, ⭐, ⚠️, ≈, ×, µ, ∞, or any non-ASCII characters
   - If Unicode is needed, replace with ASCII: degrees, +/-, ->, [STARS], [WARNING], 
     approx, x, micro, infinity
   - Create files ASCII-only from the start. Do not create with Unicode then cleanup afterward.
```

**Authority**: This is now a mandatory gate before any file creation.

---

### 3. Expanded RULE 25: ASCII ENCODING ONLY

**Location**: RULE 25 (Line 1956)

**Previous Text**:
```
ALL files MUST BE ASCII encoded (NO BOM or UTF8)
Reference: See docs/component-json-requirements.md for complete JSON parsing rules...
```

**New Text** (Comprehensive):
```
ALL files MUST BE ASCII encoded (NO BOM or UTF8)

APPLIES TO:
- All C++ source files (.cpp, .hpp, .h, .src, .hdr)
- All JSON configuration files (.json)
- All documentation files (.md, .txt)
- All Python scripts (.py)
- All PowerShell scripts (.ps1)
- All generated files

NO EXCEPTIONS FOR:
- Special Unicode characters (degree symbol, arrows, checkmarks, emoji, box drawing)
- Mathematical symbols (±, ≈, ∞, µ, Ω)
- Non-ASCII punctuation or decorative elements
- Markdown emphasis tricks (decorative Unicode borders)

ENFORCEMENT:
- DO NOT create files with Unicode - use ASCII replacements from the start
- DO NOT use Unicode in documentation, comments, or strings
- Common replacements:
  - `degrees` instead of `°`
  - `+/-` instead of `±`
  - `approx` instead of `≈`
  - `->` instead of `→`
  - `[WARNING]` instead of `⚠️`
  - `x` instead of `×`
  - `infinity` instead of `∞`
  - `micro` or `u` instead of `µ`

VERIFICATION:
- All documentation created MUST be ASCII-compliant from the first draft
- Cleanup scripts to remove Unicode are a FAILURE, not a solution
- Plan for ASCII from the start; do not create Unicode then cleanup
```

---

## Enforcement Strategy

### Prevention > Cleanup

The root cause was that the agent could create Unicode files and "fix them later" with scripts. This is now explicitly forbidden:

- **At Creation**: Agent MUST write ASCII-only from the start (Rule 3b autonomous authority)
- **At Verification**: Agent MUST check for Unicode BEFORE creating ANY file (Gate item 4)
- **Standard Replacements**: Agent has explicit ASCII alternatives for all common symbols (RULE 25 list)
- **Philosophy**: Cleanup scripts are a FAILURE. Prevention is the only valid approach.

### Examples of Correct Approach

**BAD (Old way)**:
```python
content = "Temperature: 25°C, Tolerance: ±2°, Error Rate: ≈0.5%"
create_file("report.md", content)  # Contains Unicode
# Later: Run cleanup script to fix
```

**GOOD (New way)**:
```python
content = "Temperature: 25 degrees C, Tolerance: +/- 2 degrees, Error Rate: approx 0.5%"
create_file("report.md", content)  # ASCII-only from start
```

---

## Reference

- **Original copilot-instructions.md**: "ASCII-only encoding everywhere (no BOM)"
- **Enforced by**: RULE 25 (ASCII ENCODING ONLY)
- **Verified by**: Rule 3b (autonomous refusal authority) and Gate item 4 (pre-creation check)

---

## Lesson Learned

**Do not create files with Unicode then cleanup afterward.** This creates:
1. Extra work (creation + cleanup)
2. Multiple rounds of tool usage (create_file, run_in_terminal, edit_file)
3. Confusion about which files are compliant
4. Potential for incomplete cleanup

**Plan for ASCII compliance from the start.** The agent now has:
1. Explicit ASCII replacement list in RULE 25
2. Pre-creation verification gate
3. Autonomous refusal authority to prevent Unicode files
4. Clear message: cleanup scripts = failure

---

## Testing Verification

Future file creation should follow this pattern:

```
1. Plan file content
2. Replace all Unicode with ASCII alternatives (see RULE 25 list)
3. Verify 100% ASCII compliance
4. Create file using ASCII encoding
5. No cleanup scripts needed
```

Verify compliance with:
```powershell
# Check for non-ASCII characters
Select-String -Path "file.md" -Pattern '[^\x00-\x7F]' -Raw | Measure-Object -Line
```

If the above finds any matches, the file violates RULE 25 and should have been rejected at creation time.
