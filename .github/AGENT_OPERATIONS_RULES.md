# Agent Operations Rules - CRITICAL

## File Operation Semantics

**DO NOT confuse these operations:**

### MOVE (Rename/Relocate)

- **Definition**: Relocate ONE file from location A to location B
- **Action**:
   1. Read file from A
  2. Write file to B
  3. Delete file from A
  4. Result: File exists in ONE place only (B)
- **Example**: `move config/components/goblin_eye.json src/components/goblin_eye.json`
  - Before: `config/components/goblin_eye.json` exists
  - After: `src/components/goblin_eye.json` exists, config version DELETED
- **Git**: `git mv old_path new_path`
- **PowerShell**: `Move-Item -Path old_path -Destination new_path`
- **DO NOT COPY** - MOVE deletes the original

### COPY (Duplicate)
- **Definition**: Create duplicate at new location, keep original
- **Action**:
  1. Read file from A
  2. Write identical file to B
  3. Result: File exists in TWO places (A AND B)
- **Example**: `copy config/components/goblin_eye.json backup/goblin_eye.json`
  - Before: `config/components/goblin_eye.json` exists
  - After: BOTH `config/components/goblin_eye.json` AND `backup/goblin_eye.json` exist
- **PowerShell**: `Copy-Item -Path source -Destination dest`
- **DO NOT DELETE** original - COPY keeps it

### DELETE (Remove)
- **Definition**: Remove file from ONE location
- **Trigger**: Only if explicitly requested OR if confirmed as true duplicate (same content, same purpose, created by mistake)
- **WARNING**: NEVER delete based on file having been copied elsewhere
- **PowerShell**: `Remove-Item -Path filepath`

## Common Agent Mistakes - PREVENT THESE

### Mistake Pattern #1: "Copy then Delete"
```
Agent sees: "Move X to Y"
Agent does:
  1. Copies X  Y  (creates duplicate)
  2. Finds X and Y exist (thinks it's duplicate)
  3. Deletes X  (wrong - should have just moved)
Result:  Correct by accident, but for wrong reason
```
**FIX**: If moving, use MOVE operation directly - don't COPY then DELETE

### Mistake Pattern #2: "Copy to Multiple Places then Delete Originals"
```
Agent sees: "Use X in subsystems A, B, C"
Agent does:
  1. Copies X  A/X, B/X, C/X  (creates 3 duplicates + original)
  2. Finds 4 copies of X
  3. Deletes X everywhere thinking they're all duplicates
  4. Deletes original too
Result:  File destroyed in all locations
```
**FIX**: Keep ONE source file. Reference it from other locations via JSON `components` arrays or `#include` statements. Do NOT copy files to create local versions.

### Mistake Pattern #3: "Consolidate by Copying"
```
Agent sees: "goblin_left_eye.json, goblin_right_eye.json are similar"
Agent does:
  1. Creates shared/goblin_eye_shared.json
  2. Copies parts of left_eye into shared
  3. Copies parts of right_eye into shared
  4. Sees duplicated content now exists in 3 places
  5. Deletes one or more versions to "remove duplicates"
  6. Breaks references because deleted file was being used
Result:  Architecture breaks, references now point to deleted files
```
**FIX**: Consolidate means refactor ONCE into shared location. Use JSON `components` arrays to reference. Do NOT create copies then delete to "clean up duplicates."

## Core Principles

1. **One Source of Truth**: Every piece of data should live in exactly ONE canonical location
2. **Reference via Components**: Other subsystems reference shared data through JSON `components` arrays (depth-first traversal)
3. **No Duplicate Content**: If same data appears in multiple files, that's a BUG - consolidate at source
4. **File-Scoped Statics**: Within single compilation unit, use `static` variables to share data between components
5. **Don't Copy-Consolidate**: Never "consolidate" by copying content to new location then deleting originals
   - Instead: Move original to canonical location, update all references

## When to MOVE vs COPY vs DELETE

| Situation | Action | Reason |
|-----------|--------|--------|
| "Relocate X from A to B" | **MOVE** (mv or git mv) | Single location, one source of truth |
| "Add backup of X" | **COPY** to backup/ | Keep original, create backup |
| "X is duplicate of Y" | **DELETE** duplicate, keep original | Remove redundancy |
| "Consolidate X, Y, Z into shared" | **MOVE** to canonical location, update refs | One source, referenced via `components` |
| "Remove obsolete X" | **DELETE** only if no references exist | Prevent broken references |

## Verification Before File Deletion

**NEVER delete a file without checking:**

1. Does any other file reference this file?
   - Search: `grep -r "filename" .`
   - Search JSON: `grep -r "filename.json" .`
   - Search includes: `grep -r "#include.*filename" .`
2. Is this the only copy, or are duplicates elsewhere?
   - If duplicates exist, ask which to keep before deleting any
3. Was this file recently modified or added by previous operations?
   - Check git log for recent changes
4. If deletion would break build/references  STOP, don't delete

## For Consolidation Tasks

**IF asked to "consolidate" component definitions:**

1.  Identify duplicated content
2.  Create ONE canonical file at shared location (e.g., `config/components/shared/`)
3.  Make that canonical file authoritative
4.  Update ALL references to point to canonical
5.  DO NOT copy content to multiple places
6.  DO NOT delete originals to "remove duplicates" created by your copies
7.  AFTER all references updated, then safe to delete old versions
8.  VERIFY no broken references before committing

---

**BOTTOM LINE**: MOVE means one file in one place. COPY means two files in two places. DELETE means one file in zero places. Don't confuse them.
