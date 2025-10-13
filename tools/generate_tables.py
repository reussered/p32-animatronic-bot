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
        bot_file = self.config_dir / "bots" / f"{bot_name}.json"
        
        if not bot_file.exists():
            raise FileNotFoundError(f"Bot config not found: {bot_file}")
            
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
            hit_count = component.get("timing", {}).get("hitCount", 10)
            description = component.get("description", f"{comp_name} component")
            
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
    
    def generate_init_table_header(self) -> str:
        """Generate initTable.h header file content"""
        content = [
            "#ifndef INIT_TABLE_H",
            "#define INIT_TABLE_H",
            "",
            '#include "esp_err.h"',
            "",
            "// Component initialization function type",
            "typedef esp_err_t (*init_func_t)(void);",
            "",
            "// Forward declarations for component init functions",
            "// Generated from JSON bot configuration"
        ]
        
        for comp in self.components:
            content.append(f"esp_err_t {comp['init_func']}(void);")
        
        content.extend([
            "",
            "// JSON-generated initialization table",
            f"#define INIT_TABLE_SIZE {len(self.components)}",
            "extern init_func_t initTable[INIT_TABLE_SIZE];",
            "",
            "#endif // INIT_TABLE_H"
        ])
        
        return "\n".join(content)
    
    def generate_act_table_header(self) -> str:
        """Generate actTable.h header file content"""
        content = [
            "#ifndef ACT_TABLE_H",
            "#define ACT_TABLE_H",
            "",
            '#include <stdint.h>',
            "",
            "// Component action function type",
            "typedef void (*act_func_t)(uint32_t loopCount);",
            "",
            "// Action table entry with timing",
            "typedef struct {",
            "    act_func_t act_func;",
            "    uint32_t hitCount;  // Execute every N loops (from JSON config)",
            "} act_table_entry_t;",
            "",
            "// Forward declarations for component action functions",
            "// Generated from JSON bot configuration"
        ]
        
        for comp in self.components:
            content.append(f"void {comp['act_func']}(uint32_t loopCount);")
        
        content.extend([
            "",
            "// JSON-generated action table with timing from bot configuration",
            f"#define ACT_TABLE_SIZE {len(self.components)}",
            "extern act_table_entry_t actTable[ACT_TABLE_SIZE];",
            "",
            "#endif // ACT_TABLE_H"
        ])
        
        return "\n".join(content)
    
    def generate_init_table_implementation(self) -> str:
        """Generate initTable.c implementation file content"""
        content = [
            '#include "initTable.h"',
            "",
            "// JSON-generated initialization table",
            "// Auto-generated from bot JSON configuration",
            f"init_func_t initTable[INIT_TABLE_SIZE] = {{"
        ]
        
        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            content.append(f"    {comp['init_func']}{comma}")
        
        content.append("};")
        return "\n".join(content)
    
    def generate_act_table_implementation(self) -> str:
        """Generate actTable.c implementation file content"""
        content = [
            '#include "actTable.h"',
            "",
            "// JSON-generated action table with timing from bot configuration",
            "// Auto-generated from bot JSON with hitCount values from config",
            f"act_table_entry_t actTable[ACT_TABLE_SIZE] = {{"
        ]
        
        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            comment = f"// {comp['description']}"
            content.append(f"    {{ {comp['act_func']}, {comp['hitCount']:2d} }}{comma:1s}    {comment}")
        
        content.append("};")
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
            '#include "initTable.h"',
            '#include "actTable.h"',
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
            
            # Init function
            content.extend([
                f"esp_err_t {comp['init_func']}(void) {{",
                "#ifdef SIMPLE_TEST",
                f'    printf("INIT: {comp["name"]} - {comp["description"]}\\n");',
                "    return ESP_OK;",
                "#endif",
                f'    ESP_LOGI(TAG_{tag}, "{comp["description"]} initialized");',
                "    return ESP_OK;",
                "}",
                ""
            ])
            
            # Act function - special handling for network_monitor
            if comp['name'] == 'network_monitor':
                content.extend([
                    f"void {comp['act_func']}(uint32_t loopCount) {{",
                    "#ifdef SIMPLE_TEST",
                    "    // Send timing packet to server every 100,000 loops for server-side timing measurement",
                    "    static uint32_t packet_counter = 0;", 
                    "    if (loopCount % 100000 == 0 && loopCount > 0) {",
                    "        printf(\"TIMING_PACKET: loop=%lu, packet=%lu\\n\", loopCount, packet_counter++);",
                    "        // TODO: Send actual network packet to server with loopCount and packet_counter",
                    "        // Server will measure time between packets to calculate loop performance",
                    "    }",
                    f'    if (loopCount % {comp["hitCount"]} == 0) printf("ACT[%lu]: {comp["name"]} - hitCount:{comp["hitCount"]}\\n", loopCount);',
                    "    return;",
                    "#endif",
                    f'    ESP_LOGI(TAG_{tag}, "Loop %lu: {comp["description"]}", loopCount);',
                    "    // TODO: Implement actual network monitoring and server communication",
                    "}",
                    ""
                ])
            else:
                content.extend([
                    f"void {comp['act_func']}(uint32_t loopCount) {{",
                    "#ifdef SIMPLE_TEST",
                    f'    printf("ACT[%lu]: {comp["name"]} - hitCount:{comp["hitCount"]}\\n", loopCount);',
                    "    return;",
                    "#endif",
                    f'    ESP_LOGI(TAG_{tag}, "Loop %lu: {comp["description"]}", loopCount);',
                    "}",
                    ""
                ])
        
        return "\n".join(content)
    
    def write_files(self) -> None:
        """Write all generated files to appropriate directories"""
        self.output_dir.mkdir(exist_ok=True)
        self.include_dir.mkdir(exist_ok=True)
        
        # Header files go in include/, source files go in src/
        header_files = [
            ("initTable.h", self.generate_init_table_header()),
            ("actTable.h", self.generate_act_table_header())
        ]
        
        source_files = [
            ("initTable.c", self.generate_init_table_implementation()),
            ("actTable.c", self.generate_act_table_implementation()),
            ("system_components.c", self.generate_component_stub("system")),
            ("bot_components.c", self.generate_component_stub("positioned"))
        ]
        
        # Write header files to include/
        for filename, content in header_files:
            filepath = self.include_dir / filename
            with open(filepath, 'w') as f:
                f.write(content)
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