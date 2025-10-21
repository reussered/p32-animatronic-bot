# Core Service Interface Checklist and Audit

This document summarizes the required core service interfaces for all creature/component subsystems in the P32 Animatronic Bot architecture, and verifies their presence and completeness as of this audit.

---

## 1. SharedMemory Protocol (Global State)
- **SharedMemory**: The only allowed mechanism for synchronized state (mood, sensors, system status, etc.) across all subsystems
- **g_loopCount**: Global loop counter, read-only for components
- **C++ Class Integration**: Mood and (future) Personality classes, with serialization helpers for SharedMemory sync

## 2. Component Registration and Execution
- **initTable[] / actTable[]**: Function pointer tables for component init/act, auto-generated from JSON
- **hitCountTable[]**: Timing for each component's act function
- **NO ARGUMENTS Pattern**: All component functions use void signatures, access state via globals

## 3. Main Loop
- **Centralized in main.c**: Iterates over all registered components, calling init/act based on timing
- **Loop Counter**: Increments g_loopCount, drives component execution

## 4. Change Detection and Synchronization
- **Class/Struct Change Detection**: Helpers for efficient state sync (memcmp, operator!=, etc.)
- **Serialization/Deserialization**: C++ ↔ C struct for SharedMemory transmission

---

## Audit Results
- **All required interfaces are present and functional.**
- No missing or incomplete core APIs for basic operation.
- All code follows the documented NO ARGUMENTS and global state access patterns.
- No direct core state logic in creature/component code; all core state is accessed and synchronized exclusively via SharedMemory.

---

**This document should be updated if new core service interfaces are added or the architecture changes. All subsystem communication and synchronization must use SharedMemory. No other protocol or mechanism is permitted.**
