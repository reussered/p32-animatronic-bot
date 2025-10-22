# ESP-NOW Communications Protocol - Architectural Rule

## CRITICAL RULE: ESP-NOW Never Used Explicitly

**The ESP-NOW communications protocol is NEVER used explicitly in any component code.**

## Mandatory Access Pattern

### RULE 1: SharedMemory Class Exclusive Gateway

- **ALL** ESP-NOW communication happens exclusively through the SharedMemory class
- **NO** component may directly call ESP-NOW APIs (esp_now_send, esp_now_register_recv_cb, etc.)
- **NO** direct access to ESP-NOW protocol functions is permitted

### RULE 2: Component Usage Protocol

Components must follow this exact pattern:

```cpp
// CORRECT: Component declares local copy
SharedMemory local_state;

// CORRECT: Read current state (purely local, no network traffic)
local_state.read();

// Component modifies local state as needed
// ... component logic operates on local_state ...

// CORRECT: Write updates (triggers ESP-NOW broadcast to all subsystems)
local_state.write();
```

## Data Transmission Characteristics

### Binary Protocol

- **Binary transmission**: ESP-NOW moves data in raw binary format with no encoding overhead
- **ASCII content compatibility**: Configuration file contents (which are ASCII) can be transmitted as-is if needed
- **No conversion required**: Direct binary transfer maintains efficiency

### Subsystem Distribution

- **Torso Subsystem** (50% CPU): Hosts ESP-NOW coordination, system-level components
- **Head Subsystem** (75% CPU): Receives state updates, focuses on real-time rendering
- **Mesh Network**: All subsystems maintain synchronized SharedMemory state

## Implementation Requirements

### For Component Developers

1. **Include SharedMemory**: `#include "p32_shared_state.h"`
2. **Use SharedMemory API**: Never call ESP-NOW functions directly
3. **Follow read/write pattern**: Always read before modifying, write after changes
4. **Local processing**: Work with local copy of shared state

### For System Architects

1. **SharedMemory encapsulation**: All ESP-NOW complexity hidden in SharedMemory class
2. **Clean abstraction**: Components remain unaware of ESP-NOW implementation details
3. **Unified protocol**: All inter-subsystem communication follows identical pattern

## Enforcement

### Code Review Checklist

- [ ] No direct ESP-NOW API calls in component code
- [ ] SharedMemory class used for all inter-subsystem communication
- [ ] No file transfer logic mixed with state synchronization
- [ ] Binary data handling preserves ASCII content integrity

### Build System Integration

The build system should reject any component code that directly references ESP-NOW functions outside of the SharedMemory class implementation.

## Rationale

This architectural rule ensures:

1. **Clean separation**: Component logic isolated from network protocol details
2. **Maintainability**: ESP-NOW implementation changes don't affect component code
3. **Performance**: No protocol overhead in component execution paths
4. **Reliability**: Consistent state synchronization across all subsystems
5. **Testability**: Components can be tested independently of network code

## Related Specifications

- `GLOBAL-SHARED-MEMORY-API.md` - SharedMemory class interface specification
- `THREE-LEVEL-COMPONENT-ATTACHMENT-SPEC.md` - Component architecture requirements
- `P32-DISTRIBUTED-MULTI-CHIP-ARCHITECTURE.md` - Overall system architecture

---
**Status**: IMMUTABLE ARCHITECTURAL RULE  
**Last Updated**: October 21, 2025  
**Enforcement Level**: CRITICAL - Build system rejection for violations