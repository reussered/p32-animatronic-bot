# Consistent Project Rules

This file contains the up-to-date, consistent rules for the p32-animatronic-bot project. All agents and contributors must read and apply these rules before performing any task.

1. All component functions must use NO ARGUMENTS 
2. The JSON file that defines each component must have a software subsection withboth init_function and act_function fields. Each field must be present: if the function is not needed, its value must be "STUB"; otherwise, the value is the exact name of the function. Remove all references to the hardware_only flag.
    for example:
        software :
        {
            init_function: {name}_init
            act_function: STUB
        }
3. All rules in this file supersede conflicting rules elsewhere in the documentation.
4. If a rule is unclear or inconsistent, report it immediately and halt further action until resolved.
5. The agent must always check this file first before executing any project-related task.
6. **DISPATCH TABLE GENERATION**: All dispatch table generation must follow the rules in `docs/DISPATCH-TABLE-GENERATION-RULES-SPEC.md`. The validation script MUST auto-generate exactly 4 files from JSON configurations: `src/p32_dispatch_tables.cpp`, `include/p32_dispatch_tables.hpp`, `src/p32_component_functions.cpp`, `include/p32_component_functions.hpp`. All 4 files MUST be in .gitignore as auto-generated content.

(Continue adding consistent rules as they are validated.)
