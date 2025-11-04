# Multi-Level AI Architecture Vision (TBD)

**Status:** This architecture is a forward-looking vision and is NOT yet implemented. It is documented here to preserve the design for future development.

## Overview

The system will use a multi-tiered AI architecture to provide scalable, context-aware intelligence for animatronic creatures. The design separates responsibilities across three levels:

---

## 1. Top Level: Cloud AI Dispatcher
- A wireless server (on PC or cloud) receives requests from the robot system.
- Dispatches requests to advanced AI models in the cloud (OpenAI, Azure, custom LLMs).
- Handles the highest-level reasoning, conversation, and planning.

## 2. Mid Level: Local AI/Advice Server
- Runs on a local machine (e.g., Raspberry Pi, PC).
- Receives requests from ESP32 subsystems/creatures for advice or behavior selection.
- Handles requests locally for fast response, or escalates to the cloud if needed.

## 3. Low Level: ESP32 Subsystems
- Each ESP32 runs real-time control, reflexes, and basic behaviors.
- Sends requests to the local AI server for higher-level advice.
- Local server decides if escalation to the cloud is needed.

---

## Key Design Points
- **Communication Protocols:** TBD (MQTT, HTTP, custom UDP/TCP, etc.)
- **Request/Response Format:** TBD (define advice, escalation, and context data)
- **Escalation Logic:** Local server determines when to escalate to cloud AI
- **Boundaries:** Each level has clear responsibilities and interfaces

---

## Status
- This architecture is not yet implemented.
- No code or infrastructure exists for these layers as of this writing.
- This document is to ensure the vision is not lost and can be referenced when development resumes.

---

*Document created automatically to preserve future system vision.*
