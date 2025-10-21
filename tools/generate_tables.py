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
        # Try multiple locations: bots/, subsystems/, or direct path
        possible_paths = [
            self.config_dir / "bots" / f"{bot_name}.json",
            self.config_dir / "subsystems" / f"{bot_name}.json",
            Path(bot_name)  # Direct path
        ]
        
        bot_file = None
        for path in possible_paths:
            if path.exists():
                bot_file = path
                break
        
        if bot_file is None:
            raise FileNotFoundError(f"Bot config not found: {bot_name} (tried bots/, subsystems/, and direct path)")
        
        print(f"DEBUG: Loading bot config from: {bot_file}")
        with open(bot_file, 'r') as f:
            return json.load(f)
    
    def load_positioned_components(self, bot_config: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Load all positioned components referenced in bot config"""
        components = []
        
        if "positioned_components" in bot_config:
            for component_ref in bot_config["positioned_components"]:
                # Remove 'config/' prefix if present since config_dir already points to config/
                clean_ref = component_ref.replace("config/", "") if component_ref.startswith("config/") else component_ref
                component_file = self.config_dir / clean_ref
                if component_file.exists():
                    print(f"DEBUG: Loading component: {component_file}")
                    with open(component_file, 'r') as f:
                        component_data = json.load(f)
                        component_data['config_file'] = component_ref
                        components.append(component_data)
                else:
                    print(f"Warning: Component file not found: {component_file}")
        
        return components
    
    def generate_function_name(self, component_name: str, func_type: str) -> str:
        """Generate standardized function names from component names"""
        # Convert component name to function name format
        name = component_name.lower().replace(' ', '_').replace('-', '_')
        return f"p32_comp_{name}_{func_type}"
    
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
        """Generate unified p32_component_tables.h header file content"""
        content = [
            "#ifndef P32_COMPONENT_TABLES_H",
            "#define P32_COMPONENT_TABLES_H",
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
            "// Forward Declarations - Init Functions",
            "// ============================================================================",
            ""
        ]
        
        for comp in self.components:
            content.append(f"esp_err_t {comp['init_func']}(void);")
        
        content.extend([
            "",
            "// ============================================================================",
            "// Forward Declarations - Act Functions (NO ARGUMENTS)",
            "// ============================================================================",
            ""
        ])
        
        for comp in self.components:
            content.append(f"void {comp['act_func']}(void);")
        
        content.extend([
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
            "#endif // P32_COMPONENT_TABLES_H"
        ])
        
        return "\n".join(content)
    
    def generate_unified_implementation(self) -> str:
        """Generate unified p32_component_tables.c implementation file content"""
        content = [
            '#include "p32_component_tables.h"',
            "",
            "// ============================================================================",
            "// P32 Component Dispatch Tables - Implementation",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
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
            # Calculate frequency: 120,000 iterations/sec ? hitCount
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
        """Generate component implementation stub file"""
        if comp_type == "system":
            components = [c for c in self.components if c['type'] == 'system']
            filename = "system_components.c"
        else:
            components = [c for c in self.components if c['type'] == 'positioned']
            filename = "bot_components.c"
        
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
        
        # Single unified header file
        header_file = ("p32_component_tables.h", self.generate_unified_header())
        
        # Source files
        source_files = [
            ("p32_component_tables.c", self.generate_unified_implementation()),
            ("system_components.c", self.generate_component_stub("system")),
            ("bot_components.c", self.generate_component_stub("positioned"))
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