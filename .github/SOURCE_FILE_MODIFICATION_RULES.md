# SOURCE FILE MODIFICATION RULES

## Overview
Rules for modifying source files (.src and .hdr)

## Core Principle
Changes to source files must be INTENTIONAL, not INCIDENTAL. The distinction is between direct, purposeful modifications and indirect side-effects of automation.

## 1. ALLOWED Modifications (Intentional Changes)

These are direct consequences of development work:

- Human developers implementing features
- AI agent implementing requested features
- AI agent fixing bugs
- AI agent refactoring code
- Any changes that are:
  - Directly related to a task
  - Visible in git diffs
  - Reviewable by users
  - Part of normal development flow

## 2. PROHIBITED Modifications (Automated Processes)

These are side-effects that must be prevented:

- NO modifications during build/aggregation
- NO changes during table generation
- NO side-effects from tooling
- Examples of prohibited modifications:
  - generate_tables.py modifying .src files when injecting use_fields
  - Build tools altering include paths
  - Automated formatting during aggregation
  - Any "temporary" changes for processing

## 3. Required Pattern for Automation

When automated processes need to modify content:

1. Create temporary working files
2. Copy source content to temp files
3. Make ALL modifications in temp space
4. Use modified temp files for processing
5. Delete temp files after use
6. NEVER modify original source files

## 4. Key Implementation Points

- Source files are "read-only" during automation
- All automated transformations work on copies
- Temporary files should be clearly marked (e.g., .tmp extension)
- Temp files should not be committed to git
- Build process must clean up all temp files

## 5. Rationale

This rule ensures:
- Source truth remains stable
- Builds are reproducible
- Git history stays clean
- No unintended side-effects
- Clear separation between development and automation

## 6. Example: generate_tables.py

```python
# INCORRECT:
with open(f"{component}.src", "r+") as f:
    content = f.read()
    modified = inject_use_fields(content)  # Modifies source file!
    f.write(modified)

# CORRECT:
with open(f"{component}.src", "r") as src:
    content = src.read()
    
with open(f"{component}.src.tmp", "w") as tmp:
    modified = inject_use_fields(content)
    tmp.write(modified)

# Use tmp file for aggregation
aggregate_files([f"{component}.src.tmp"])

# Clean up
os.remove(f"{component}.src.tmp")
```