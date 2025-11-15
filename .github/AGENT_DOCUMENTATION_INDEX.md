# P32 Animatronic Bot - Agent Documentation Index

**Purpose**: Agent onboarding and operational reference for autonomous development.

**Reading Strategy**: 
- Tier 1 (CRITICAL) at session start
- Tier 2 (OPERATIONAL) during active work
- Tier 3 (TROUBLESHOOTING) when errors occur
- Tier 4 (REFERENCE) as needed

---

## TIER 1: CRITICAL - Read First (Agent Onboarding)

### 1. Architecture Overview
**File**: `docs/01_ARCHITECTURE_OVERVIEW.md`
**Topics**: Single compilation unit, component isolation, multi-ESP32 distributed system, GSM communication
**Why Critical**: Violating these patterns breaks the entire build system

### 2. Build Workflow
**File**: `docs/02_BUILD_WORKFLOW.md`
**Topics**: `generate_tables.py` → build → flash cycle, PowerShell patterns, log capture
**Why Critical**: Wrong workflow = corrupted generated files = build failures

### 3. Component System Rules
**File**: `docs/03_COMPONENT_SYSTEM_RULES.md`
**Topics**: `.src`/`.hdr` aggregation, function signatures, component lifecycle
**Why Critical**: Most non-intuitive system behavior; breaks builds and runtime

### 4. Coding Standards
**File**: `docs/04_CODING_STANDARDS.md`
**Topics**: ASCII-only, Allman braces, no `extern`, no `#include` patterns, static variables
**Why Critical**: Violating these causes compilation errors and linker failures

---

## TIER 2: OPERATIONAL - Reference During Work

### 5. Communication Patterns
**File**: `docs/05_COMMUNICATION_PATTERNS.md`
**Topics**: Static variables (intra-subsystem), SharedMemory API (inter-subsystem)
**When Needed**: Writing components that share data

### 6. Timing & Execution
**File**: `docs/06_TIMING_EXECUTION.md`
**Topics**: `hitCount` mechanism, `g_loopCount`, execution frequency by component type
**When Needed**: Setting component timing, optimizing performance

### 7. JSON Configuration
**File**: `docs/07_JSON_CONFIGURATION.md`
**Topics**: Bot assembly structure, component definition format, inheritance patterns
**When Needed**: Creating new bots, modifying component hierarchies

### 8. Shared State Classes
**File**: `docs/08_SHARED_STATE_CLASSES.md`
**Topics**: Mood, Environment, Personality, BehaviorControl - fields and usage patterns
**When Needed**: Reading/writing specific state objects via SharedMemory

---

## TIER 3: TROUBLESHOOTING - When Things Break

### 9. Common Build Errors
**File**: `docs/09_BUILD_ERRORS.md`
**Topics**: Multiple definition errors, missing init/act functions, linker failures
**When Needed**: Build breaks after JSON or code changes

### 10. Memory & Performance
**File**: `docs/10_MEMORY_PERFORMANCE.md`
**Topics**: RAM constraints (~512KB), display buffer sizing, optimization patterns
**When Needed**: Out of memory errors, performance issues

---

## Quick Reference Cards

### Critical Rules Summary
**File**: `docs/CRITICAL_RULES_CARD.md`
**Content**: One-page reference of all build-breaking rules

### Component Quick Start
**File**: `docs/COMPONENT_QUICK_START.md`
**Content**: Template for creating new components (copy-paste ready)

### Build Commands
**File**: `docs/BUILD_COMMANDS_CARD.md`
**Content**: All PowerShell build commands with explanations

### Error → Fix Patterns
**File**: `docs/ERROR_FIX_PATTERNS.md`
**Content**: Common errors and their fixes (table format)

---

## Document Format Standards

All documents follow:
- **Dense format**: Tables > prose, code examples > explanations
- **DO/DON'T patterns**: Clear correct vs incorrect examples
- **Token efficiency**: Maximum information, minimum words
- **Critical warnings**: ⚠️ for build-breaking issues, ❌ for wrong patterns, ✅ for correct patterns

---

## Navigation

**Start Here**: `docs/01_ARCHITECTURE_OVERVIEW.md`
**Most Referenced**: `docs/03_COMPONENT_SYSTEM_RULES.md`, `docs/05_COMMUNICATION_PATTERNS.md`
**When Stuck**: `docs/09_BUILD_ERRORS.md`, `docs/10_RUNTIME_DEBUGGING.md`
