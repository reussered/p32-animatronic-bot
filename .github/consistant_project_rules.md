# Consistent Project Rules

This file contains the up-to-date, consistent rules for the p32-animatronic-bot project. All agents and contributors must read and apply these rules before performing any task.

1. All component functions must use NO ARGUMENTS and access shared state via p32_shared_state.h directly.
2. Every component must have a software subsection in its JSON definition with both init_function and act_function fields. Each field must be present: if the function is not needed, its value must be "STUB"; otherwise, the value is the exact name of the function. Remove all references to the hardware_only flag.
3. All rules in this file supersede conflicting rules elsewhere in the documentation.
4. If a rule is unclear or inconsistent, report it immediately and halt further action until resolved.
5. The agent must always check this file first before executing any project-related task.

(Continue adding consistent rules as they are validated.)
