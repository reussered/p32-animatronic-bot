#!/usr/bin/env python3
"""
P32 Component Table Generator
Reads JSON bot configuration and generates initTable.h, actTable.h and component files
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any

class P32ComponentGenerator:
    def __init__(self, config_dir: str, output_dir: str):
        self.config_dir = Path(config_dir)
        self.output_dir = Path(output_dir)
        self.include_dir = Path(output_dir).parent / "include"
        self.components = []
        self.init_functions = []
        self.act_functions = []
        
    def load_bot_config(self, bot_name: str) -> Dict[str, Any]:
        """Load bot configuration from JSON file"""
        # Try multiple locations: bots/, subsystems/, families, or direct path
        possible_paths = [
            self.config_dir / "bots" / f"{bot_name}.json",
            self.config_dir / "subsystems" / f"{bot_name}.json",
            self.config_dir / "bots" / "bot_families" / "goblins" / f"{bot_name}.json",
            self.config_dir / "bots" / "bot_families" / "cats" / f"{bot_name}.json",
            self.config_dir / "bots" / "bot_families" / "bears" / f"{bot_name}.json",
            Path(bot_name)  # Direct path
        ]
        
        bot_file = None
        for path in possible_paths:
            if path.exists():
                bot_file = path
                break
        
        if bot_file is None:
            raise FileNotFoundError(f"Bot config not found: {bot_name} (tried bots/, subsystems/, families, and direct path)")
        
        print(f"DEBUG: Loading bot config from: {bot_file}")
        with open(bot_file, 'r') as f:
            return json.load(f)
    
    def load_positioned_components(self, bot_config: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Load all positioned components referenced in bot config - handles recursive composition"""
        components = []
        
        # Handle direct positioned_components (backward compatibility)
        if "positioned_components" in bot_config:
            for component_ref in bot_config["positioned_components"]:
                components.extend(self._load_component_recursive(component_ref))
        
        # Handle new subsystem_assemblies structure (recursive composition)
        if "subsystem_assemblies" in bot_config:
            for subsystem_ref in bot_config["subsystem_assemblies"]:
                components.extend(self._load_subsystem_recursive(subsystem_ref))
        
        return components
    
    def _load_component_recursive(self, component_ref: str) -> List[Dict[str, Any]]:
        """Recursively load a single component and its nested components"""
        components = []
        
        # Remove 'config/' prefix if present since config_dir already points to config/
        clean_ref = component_ref.replace("config/", "") if component_ref.startswith("config/") else component_ref
        component_file = self.config_dir / clean_ref
        
        if component_file.exists():
            print(f"DEBUG: Loading component: {component_file}")
            with open(component_file, 'r') as f:
                component_data = json.load(f)
                component_data['config_file'] = component_ref
                components.append(component_data)
                
                # Recursively load nested positioned_components if present
                if "positioned_components" in component_data:
                    for nested_ref in component_data["positioned_components"]:
                        components.extend(self._load_component_recursive(nested_ref))
        else:
            print(f"Warning: Component file not found: {component_file}")
        
        return components
    
    def _load_subsystem_recursive(self, subsystem_ref: str) -> List[Dict[str, Any]]:
        """Recursively load a subsystem and all its components"""
        components = []
        
        # Remove 'config/' prefix if present since config_dir already points to config/
        clean_ref = subsystem_ref.replace("config/", "") if subsystem_ref.startswith("config/") else subsystem_ref
        subsystem_file = self.config_dir / clean_ref
        
        if subsystem_file.exists():
            print(f"DEBUG: Loading subsystem: {subsystem_file}")
            with open(subsystem_file, 'r') as f:
                subsystem_data = json.load(f)
                subsystem_data['config_file'] = subsystem_ref
                
                # Add the subsystem itself as a component
                components.append(subsystem_data)
                
                # Recursively load positioned_components within the subsystem
                if "positioned_components" in subsystem_data:
                    for component_ref in subsystem_data["positioned_components"]:
                        components.extend(self._load_component_recursive(component_ref))
        else:
            print(f"Warning: Subsystem file not found: {subsystem_file}")
        
        return components
    
    def generate_function_name(self, component_name: str, func_type: str) -> str:
        """Generate standardized function names from component names"""
        # Convert component name to function name format
        # RULE 5: Functions should be {name}_init() and {name}_act() - NO p32_comp_ prefix
        name = component_name.lower().replace(' ', '_').replace('-', '_')
        return f"{name}_{func_type}"
    
    def extract_component_info(self, components: List[Dict[str, Any]]) -> None:
        """Extract component information and build function lists"""
        self.components = []
        self.init_functions = []
        self.act_functions = []
        
        # Add system-level components (always present)
        system_components = [
            {"name": "heartbeat", "hitCount": 1, "description": "System heartbeat"},
            {"name": "network_monitor", "hitCount": 1, "description": "Network monitoring and loop timing"}
        ]
        
        for comp in system_components:
            init_func = self.generate_function_name(comp["name"], "init")
            act_func = self.generate_function_name(comp["name"], "act")
            
            self.components.append({
                "name": comp["name"],
                "init_func": init_func,
                "act_func": act_func,
                "hitCount": comp["hitCount"],
                "description": comp["description"],
                "type": "system"
            })
        
        # Process positioned components from JSON
        for component in components:
            comp_name = component.get("component_name", "unknown")
            # Default hitCount = 1 if not specified (executes every loop)
            timing = component.get("timing", {})
            hit_count = timing.get("hitCount")
            if hit_count is None:
                print(f"Warning: Component '{comp_name}' has no hitCount defined, defaulting to 1 (every loop)")
                hit_count = 1
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
        """Generate unified p32_component_registry.hpp header file content"""
        content = [
            "#ifndef P32_COMPONENT_REGISTRY_HPP",
            "#define P32_COMPONENT_REGISTRY_HPP",
            "",
            '#include "esp_err.h"',
            '#include <stdint.h>',
            "",
            "// ============================================================================",
            "// P32 Component Registry - Function Declarations",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
            "",
            "// Forward Declarations - Init Functions (from individual component files)",
            ""
        ]

        for comp in self.components:
            content.append(f"esp_err_t {comp['init_func']}(void);")

        content.extend([
            "",
            "// Forward Declarations - Act Functions (from individual component files)",
            ""
        ])

        for comp in self.components:
            content.append(f"void {comp['act_func']}(void);")

        content.extend([
            "",
            "// Table size constants",
            f"#define INIT_TABLE_SIZE {len(self.components)}",
            f"#define ACT_TABLE_SIZE {len(self.components)}",
            "",
            "#endif // P32_COMPONENT_REGISTRY_HPP"
        ])

        return "\n".join(content)
    
    def generate_unified_implementation(self) -> str:
        """Generate unified component_tables.cpp implementation file content"""
        content = [
            "// ============================================================================",
            "// P32 Component Dispatch Tables - Implementation",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
            "",
            '#include "p32_component_registry.hpp"',
            ""
        ]

        # Include individual component headers (RULE 5 compliance)
        includes = set()
        for comp in self.components:
            include_file = f'"{comp["name"]}.hpp"'
            if include_file not in includes:
                content.append(f"#include {include_file}")
                includes.add(include_file)

        content.extend([
            "",
            "// ============================================================================",
            "// Initialization Table",
            "// ============================================================================",
            "",
            f"esp_err_t (*initTable[INIT_TABLE_SIZE])(void) = {{"
        ])

        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            content.append(f"    {comp['init_func']}{comma}")

        content.extend([
            "};",
            "",
            "// ============================================================================",
            "// Action Table",
            "// ============================================================================",
            "",
            f"void (*actTable[ACT_TABLE_SIZE])(void) = {{"
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
            f"uint32_t hitCountTable[ACT_TABLE_SIZE] = {{"
        ])

        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            comment = f"// [{i}] {comp['name']} - every {comp['hitCount']} loops"
            content.append(f"    {comp['hitCount']}{comma:1s}    {comment}")

        content.extend([
            "};",
            ""
        ])

        return "\n".join(content)
    
    def generate_component_stub(self, comp_type: str) -> str:
        """Generate component implementation stub file"""
        if comp_type == "system":
            components = [c for c in self.components if c['type'] == 'system']
            filename = "system_components.cpp"
        else:
            components = [c for c in self.components if c['type'] == 'positioned']
            filename = "bot_components.cpp"
        
        content = [
            '#include "p32_component_tables.h"',
            '#include "esp_log.h"',
            "",
            f"// {comp_type.title()} component implementations",
            f"// Generated from JSON bot configuration",
            ""
        ]
        
        # Generate TAG definitions
        tags = set()
        for comp in components:
            tag = comp['name'].upper()
            if tag not in tags:
                content.append(f'static const char *TAG_{tag} = "{tag}";')
                tags.add(tag)
        
        content.append("")
        
        # Generate function implementations
        for comp in components:
            tag = comp['name'].upper()
            
            # Init function - Allman style braces
            content.extend([
                f"esp_err_t {comp['init_func']}(void)",
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
            
            # Act function - NO ARGUMENTS - accesses g_loopCount from globals - Allman style
            content.extend([
                f"void {comp['act_func']}(void)",
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

        # Use correct filename per existing architecture
        header_file = ("p32_component_registry.hpp", self.generate_unified_header())

        # Source files - only generate the dispatch table, not individual components
        source_files = [
            ("component_tables.cpp", self.generate_unified_implementation())
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

    def generate_from_bot(self, bot_name: str) -> None:
        """Main generation process for a specific bot"""
        print(f"Generating P32 component tables for bot: {bot_name}")
        
        # Load bot configuration
        bot_config = self.load_bot_config(bot_name)
        print(f"Loaded bot config: {bot_config.get('bot_name', 'Unknown')}")
        
        # Load positioned components
        components = self.load_positioned_components(bot_config)
        print(f"Found {len(components)} positioned components")
        
        # Extract component information
        self.extract_component_info(components)
        print(f"Generated {len(self.components)} total components")
        
        # Write generated files
        self.write_files()
        print("Generation complete!")

def main():
    if len(sys.argv) != 3:
        print("Usage: python generate_tables.py <bot_name> <output_dir>")
        print("Example: python generate_tables.py goblin_full src/")
        sys.exit(1)
    
    bot_name = sys.argv[1]
    output_dir = sys.argv[2]
    config_dir = "config"  # Relative to current directory
    
    try:
        generator = P32ComponentGenerator(config_dir, output_dir)
        generator.generate_from_bot(bot_name)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()