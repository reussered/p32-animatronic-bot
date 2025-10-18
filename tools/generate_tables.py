#!/usr/bin/env python3
"""
P32 Component Table Generator
Reads JSON bot configuration and generates C++ component tables and implementation stubs

CRITICAL: This project uses C++, NOT C
- All generated files use .cpp and .hpp extensions
- All functions use extern "C" for component table compatibility
- Never generate .c or .h files (except when interfacing with pure C libraries)
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any

class P32ComponentGenerator:
    def __init__(self, config_dir: str, output_dir: str):
        self.output_dir = Path(output_dir)
        self.include_dir = Path(output_dir).parent / "include"
        self.components = []
        self.init_functions = []
        self.act_functions = []
        
    def load_json_file(self, json_path: str) -> Dict[str, Any]:
        """Load any JSON file from given path"""
        json_file = Path(json_path)
        if not json_file.exists():
            raise FileNotFoundError(f"JSON file not found: {json_path}")
        
        with open(json_file, 'r') as f:
            return json.load(f)
    
    def load_system_components(self) -> List[Dict[str, Any]]:
        """Load system components from config/system.json"""
        system_config = self.load_json_file("config/system.json")
        components = []
        
        if "system_components" in system_config:
            for component_ref in system_config["system_components"]:
                component_data = self.load_json_file(component_ref)
                components.append(component_data)
        
        return components
    
    def load_family_components(self, family_path: str) -> List[Dict[str, Any]]:
        """Load family components from family JSON"""
        family_config = self.load_json_file(family_path)
        components = []
        
        if "default_components" in family_config:
            for component_ref in family_config["default_components"]:
                component_data = self.load_json_file(component_ref)
                components.append(component_data)
        
        return components
    
    def load_positioned_components(self, creature_config: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Load all positioned components referenced in creature config"""
        components = []
        
        if "positioned_components" in creature_config:
            for component_ref in creature_config["positioned_components"]:
                component_data = self.load_json_file(component_ref)
                components.append(component_data)
        
        if "subsystem_components" in creature_config:
            for subsystem_ref in creature_config["subsystem_components"]:
                subsystem_data = self.load_json_file(subsystem_ref)
                # Recursively load components from subsystem
                if "positioned_components" in subsystem_data:
                    subsystem_components = self.load_positioned_components(subsystem_data)
                    components.extend(subsystem_components)
        
        return components
    
    def generate_function_name(self, component_name: str, func_type: str) -> str:
        """Generate standardized function names from component names"""
        # Convert component name to function name format - ALL LOWERCASE
        name = component_name.lower().replace(' ', '_').replace('-', '_')
        return f"{name}_{func_type}"
    
    def extract_component_info(self, components: List[Dict[str, Any]]) -> None:
        """Extract component information and build function lists"""
        self.components = []
        self.init_functions = []
        self.act_functions = []
        
        # Process positioned components from JSON
        for component in components:
            comp_name = component.get("component_name", "unknown")
            # Default hitCount = 60000 if not specified (~500ms, reasonable default)
            timing = component.get("timing", {})
            hit_count = timing.get("hitCount")
            if hit_count is None:
                print(f"Warning: Component '{comp_name}' has no hitCount defined, defaulting to 60000 (~500ms)")
                hit_count = 60000
            description = component.get("description", f"{comp_name} component")
            
            # Check for explicit function declarations first
            function_declarations = component.get("function_declarations", {})
            if function_declarations:
                init_func = function_declarations.get("init_function")
                act_func = function_declarations.get("act_function")
                if not init_func or not act_func:
                    print(f"Warning: Incomplete function declarations in {comp_name}")
                    init_func = self.generate_function_name(comp_name, "init")
                    act_func = self.generate_function_name(comp_name, "act")
            else:
                # Fall back to auto-generated names
                init_func = self.generate_function_name(comp_name, "init")
                act_func = self.generate_function_name(comp_name, "act")
            
            self.components.append({
                "name": comp_name,
                "init_func": init_func,
                "act_func": act_func,
                "hitCount": hit_count,
                "description": description,
                "type": "positioned",
                "config": component
            })
    
    def generate_unified_header(self) -> str:
        """Generate unified p32_component_tables.hpp header file content"""
        content = [
            "#ifndef P32_COMPONENT_TABLES_HPP",
            "#define P32_COMPONENT_TABLES_HPP",
            "",
            '#include "esp_err.h"',
            '#include <stdint.h>',
            "",
            "// ============================================================================",
            "// P32 Component Dispatch Tables",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
            "",
            "// Component function type signatures - NO ARGUMENTS pattern",
            "typedef esp_err_t (*init_func_t)(void);",
            "typedef void (*act_func_t)(void);",
            "",
            "// Table size - all three tables have same size (one entry per component)",
            f"#define COMPONENT_TABLE_SIZE {len(self.components)}",
            "",
            "// ============================================================================",
            "// Forward Declarations - Init Functions (C linkage)",
            "// ============================================================================",
            "",
            'extern "C" {'
        ]
        
        for comp in self.components:
            content.append(f"    esp_err_t {comp['init_func']}(void);")
        
        content.extend([
            "}",
            "",
            "// ============================================================================",
            "// Forward Declarations - Act Functions (NO ARGUMENTS, C linkage)",
            "// ============================================================================",
            "",
            'extern "C" {'
        ])
        
        for comp in self.components:
            content.append(f"    void {comp['act_func']}(void);")
        
        content.extend([
            "}",
            "",
            "// ============================================================================",
            "// Dispatch Tables",
            "// ============================================================================",
            "",
            "// Initialization table - called once at startup in order",
            "extern init_func_t initTable[COMPONENT_TABLE_SIZE];",
            "",
            "// Action table - function pointers (NO ARGUMENTS)",
            "// Parallel to hitCountTable - both indexed by same i",
            "extern act_func_t actTable[COMPONENT_TABLE_SIZE];",
            "",
            "// Timing table - execution frequency control",
            "// actTable[i] executes when g_loopCount % hitCountTable[i] == 0",
            "extern uint32_t hitCountTable[COMPONENT_TABLE_SIZE];",
            "",
            "#endif // P32_COMPONENT_TABLES_HPP"
        ])
        
        return "\n".join(content)
    
    def generate_unified_implementation(self) -> str:
        """Generate unified p32_component_tables.cpp implementation file content"""
        content = [
            '#include "p32_component_tables.hpp"',
            "",
            "// ============================================================================",
            "// P32 Component Dispatch Tables - Implementation",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
            "",
            "// Global loop counter - incremented by main.cpp core loop",
            "uint64_t g_loopCount = 0;",
            "",
            "// ============================================================================",
            "// Initialization Table",
            "// ============================================================================",
            "",
            f"init_func_t initTable[COMPONENT_TABLE_SIZE] = {{"
        ]
        
        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            content.append(f"    {comp['init_func']}{comma}")
        
        content.extend([
            "};",
            "",
            "// ============================================================================",
            "// Action Table - Function Pointers (NO ARGUMENTS)",
            "// ============================================================================",
            "",
            f"act_func_t actTable[COMPONENT_TABLE_SIZE] = {{"
        ])
        
        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            comment = f"// [{i}] {comp['description']}"
            content.append(f"    {comp['act_func']}{comma:1s}    {comment}")
        
        content.extend([
            "};",
            "",
            "// ============================================================================",
            "// Timing Table - Execution Frequency Control",
            "// ============================================================================",
            "",
            f"uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {{"
        ])
        
        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            # Calculate frequency: 120,000 iterations/sec ÷ hitCount
            frequency = 120000.0 / comp['hitCount']
            if frequency >= 1000:
                freq_str = f"{frequency:.0f} Hz"
            elif frequency >= 1:
                freq_str = f"{frequency:.1f} Hz"
            else:
                period = comp['hitCount'] / 120000.0
                freq_str = f"{period*1000:.1f}ms period"
            comment = f"// [{i}] {comp['name']} - every {comp['hitCount']} loops ({freq_str})"
            content.append(f"    {comp['hitCount']}{comma:1s}    {comment}")
        
        content.extend([
            "};",
            ""
        ])
        
        return "\n".join(content)
    
    def generate_component_stub(self, comp_type: str) -> str:
        """Generate component implementation stub file - C++ style"""
        if comp_type == "system":
            components = [c for c in self.components if c['type'] == 'system']
            filename = "system_components.cpp"
        else:
            components = [c for c in self.components if c['type'] == 'positioned']
            filename = "bot_components.cpp"
        
        content = [
            '#include "p32_component_tables.hpp"',
            '#include "esp_log.h"',
            "",
            f"// {comp_type.title()} component implementations - C++",
            f"// Generated from JSON bot configuration",
            "",
            "namespace {"
        ]
        
        # Generate TAG constants in anonymous namespace
        tags = set()
        for comp in components:
            tag = comp['name'].upper()
            if tag not in tags:
                content.append(f'    constexpr const char* TAG_{tag} = "{tag}";')
                tags.add(tag)
        
        content.extend([
            "}",
            ""
        ])
        
        # Generate function implementations with extern "C" linkage
        for comp in components:
            tag = comp['name'].upper()
            
            # Init function - C linkage, C++ implementation
            content.extend([
                f'extern "C" esp_err_t {comp["init_func"]}(void)',
                "{",
                "#ifdef SIMPLE_TEST",
                f'    printf("INIT: {comp["name"]} - {comp["description"]}\\n");',
                "    return ESP_OK;",
                "#endif",
                f'    ESP_LOGI(TAG_{tag}, "{comp["description"]} initialized");',
                "    return ESP_OK;",
                "}",
                ""
            ])
            
            # Act function - C linkage, C++ implementation
            content.extend([
                f'extern "C" void {comp["act_func"]}(void)',
                "{",
                f'    // Component: {comp["name"]} - {comp["description"]}',
                f'    // Timing: Execute every {comp["hitCount"]} loops',
                "#ifdef SIMPLE_TEST",
                f'    printf("ACT: {comp["name"]} - hitCount:{comp["hitCount"]}\\n");',
                "    return;",
                "#endif",
                f'    ESP_LOGI(TAG_{tag}, "{comp["description"]}");',
                "    // TODO: Implement actual component logic",
                "}",
                ""
            ])
        
        return "\n".join(content)
    
    def write_files(self) -> None:
        """Write all generated files to appropriate directories"""
        self.output_dir.mkdir(exist_ok=True)
        self.include_dir.mkdir(exist_ok=True)
        
        # Single unified header file
        header_file = ("p32_component_tables.hpp", self.generate_unified_header())
        
        # Source files - ALL C++ now (.cpp extensions)
        source_files = [
            ("p32_component_tables.cpp", self.generate_unified_implementation()),
            ("system_components.cpp", self.generate_component_stub("system")),
            ("bot_components.cpp", self.generate_component_stub("positioned"))
        ]
        
        # Write unified header to include/
        filepath = self.include_dir / header_file[0]
        with open(filepath, 'w') as f:
            f.write(header_file[1])
        print(f"Generated: {filepath}")
        
        # Write source files to src/
        for filename, content in source_files:
            filepath = self.output_dir / filename
            with open(filepath, 'w') as f:
                f.write(content)
            print(f"Generated: {filepath}")
    
    def generate_from_creature_json(self, creature_json_path: str) -> None:
        """Main generation process from creature JSON file"""
        print(f"Generating P32 component tables from: {creature_json_path}")
        
        # Load creature configuration
        creature_config = self.load_json_file(creature_json_path)
        print(f"Loaded creature config: {creature_config.get('component_name', creature_config.get('bot_id', 'Unknown'))}")
        
        all_components = []
        
        # 1. Load system components first
        print("Loading system components...")
        system_components = self.load_system_components()
        all_components.extend(system_components)
        print(f"Loaded {len(system_components)} system components")
        
        # 2. Load family components if specified
        if "family" in creature_config:
            print(f"Loading family components from: {creature_config['family']}")
            family_components = self.load_family_components(creature_config["family"])
            all_components.extend(family_components)
            print(f"Loaded {len(family_components)} family components")
        
        # 3. Load creature-specific positioned components
        print("Loading creature-specific components...")
        creature_components = self.load_positioned_components(creature_config)
        all_components.extend(creature_components)
        print(f"Loaded {len(creature_components)} creature-specific components")
        
        # Extract component information and generate
        self.extract_component_info(all_components)
        print(f"Generated {len(self.components)} total components")
        
        # Write generated files
        self.write_files()
        print("Generation complete!")

def main():
    if len(sys.argv) != 3:
        print("Usage: python generate_tables.py <creature_json_path> <output_dir>")
        print("Example: python generate_tables.py config/bots/bot_families/goblins/goblin_full.json src/")
        sys.exit(1)
    
    creature_json_path = sys.argv[1]
    output_dir = sys.argv[2]
    
    try:
        generator = P32ComponentGenerator("", output_dir)  # No config_dir needed now
        generator.generate_from_creature_json(creature_json_path)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()