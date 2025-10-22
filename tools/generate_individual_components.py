#!/usr/bin/env python3
"""
P32 Individual Component Generator
Creates separate source files for each component to enable precise memory analysis
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any

class P32IndividualComponentGenerator:
    def __init__(self, config_dir: str, output_dir: str):
        self.config_dir = Path(config_dir)
        self.output_dir = Path(output_dir)
        self.include_dir = Path(output_dir).parent / "include"
        self.components_dir = self.output_dir / "components"
        self.components = []
        
        # Create components directory
        self.components_dir.mkdir(exist_ok=True)
        
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

    def generate_component_metadata(self, positioned_components: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Generate component metadata with function names and timing"""
        components = []
        
        # Add system components first
        components.append({
            'name': 'system_core',
            'type': 'system',
            'init_func': 'system_core_init',
            'act_func': 'system_core_act',
            'description': 'Core system initialization',
            'hitCount': 1000,  # Every 1000 loops
            'filename': 'system_core.cpp'
        })
        
        components.append({
            'name': 'network_monitor',
            'type': 'system', 
            'init_func': 'network_monitor_init',
            'act_func': 'network_monitor_act',
            'description': 'Network status monitoring',
            'hitCount': 5000,  # Every 5000 loops
            'filename': 'network_monitor.cpp'
        })
        
        # Add positioned components
        for comp_data in positioned_components:
            comp_name = comp_data.get('component_id', 'unknown_component')
            safe_name = comp_name.lower().replace('-', '_').replace(' ', '_')
            
            # Determine timing based on component type
            hitCount = 100  # Default timing
            if 'display' in comp_name.lower() or 'eye' in comp_name.lower():
                hitCount = 50   # Faster for displays
            elif 'sensor' in comp_name.lower():
                hitCount = 200  # Slower for sensors
            elif 'speaker' in comp_name.lower() or 'audio' in comp_name.lower():
                hitCount = 150  # Medium for audio
            
            components.append({
                'name': safe_name,
                'type': 'positioned',
                'init_func': f'{safe_name}_init',
                'act_func': f'{safe_name}_act', 
                'description': comp_data.get('description', f'{comp_name} component'),
                'hitCount': hitCount,
                'config_data': comp_data,
                'filename': f'{safe_name}.cpp'
            })
        
        return components

    def generate_individual_component_file(self, component: Dict[str, Any]) -> str:
        """Generate individual component source file"""
        name = component['name']
        tag = name.upper()
        
        content = [
            f'// P32 Component: {name}',
            f'// Auto-generated individual component file',
            f'// Memory footprint can be measured independently',
            '',
            '#include "esp_log.h"',
            '#include "esp_err.h"',
            '#include "p32_shared_state.h"',
            '',
            f'static const char *TAG = "{tag}";',
            '',
            f'// Component: {component["description"]}',
            f'esp_err_t {component["init_func"]}(void) {{',
            '#ifdef SIMPLE_TEST'
        ]
        
        # Simple test mode output
        content.extend([
            f'    printf("INIT: {name} - {component["description"]}\\n");',
            '    return ESP_OK;',
            '#endif',
            ''
        ])
        
        # Add component-specific initialization logic
        if component['type'] == 'system':
            if 'network' in name:
                content.extend([
                    '    // Network monitoring initialization',
                    '    ESP_LOGI(TAG, "Network monitor initialized");',
                ])
            else:
                content.extend([
                    '    // System component initialization', 
                    '    ESP_LOGI(TAG, "System component initialized");',
                ])
        else:
            # Positioned component - check hardware type
            config_data = component.get('config_data', {})
            hardware_ref = config_data.get('hardware_component', '')
            
            if 'display' in hardware_ref.lower():
                content.extend([
                    '    // Display initialization',
                    '    // TODO: Add SPI display setup',
                    '    ESP_LOGI(TAG, "Display component initialized");',
                ])
            elif 'sensor' in hardware_ref.lower():
                content.extend([
                    '    // Sensor initialization', 
                    '    // TODO: Add sensor GPIO setup',
                    '    ESP_LOGI(TAG, "Sensor component initialized");',
                ])
            elif 'speaker' in hardware_ref.lower():
                content.extend([
                    '    // Audio/Speaker initialization',
                    '    // TODO: Add I2S audio setup', 
                    '    ESP_LOGI(TAG, "Audio component initialized");',
                ])
            else:
                content.extend([
                    '    // Generic component initialization',
                    '    ESP_LOGI(TAG, "Component initialized");',
                ])
        
        content.extend([
            '    return ESP_OK;',
            '}',
            '',
            f'// Component action function - executes every {component["hitCount"]} loops',
            f'// NO ARGUMENTS - accesses g_loopCount from p32_shared_state.h',
            f'void {component["act_func"]}(void) {{',
            '#ifdef SIMPLE_TEST'
        ])
        
        # Simple test mode action
        if 'network' in name:
            content.extend([
                '    if (g_loopCount % 10000 == 0) {',
                f'        printf("ACT: {name} - checking network (loop %llu)\\n", g_loopCount);',
                '    }',
            ])
        else:
            content.extend([
                '    if (g_loopCount % 1000 == 0) {',
                f'        printf("ACT: {name} - active (loop %llu)\\n", g_loopCount);',
                '    }',
            ])
        
        content.extend([
            '    return;',
            '#endif',
            ''
        ])
        
        # Add component-specific action logic
        if component['type'] == 'system':
            if 'network' in name:
                content.extend([
                    '    // Network status check',
                    '    if (g_loopCount % 10000 == 0) {',
                    '        ESP_LOGD(TAG, "Network status check - loop %llu", g_loopCount);',
                    '    }',
                ])
            else:
                content.extend([
                    '    // System maintenance tasks',
                    '    if (g_loopCount % 5000 == 0) {',
                    '        ESP_LOGD(TAG, "System maintenance - loop %llu", g_loopCount);', 
                    '    }',
                ])
        else:
            # Positioned component actions
            config_data = component.get('config_data', {})
            hardware_ref = config_data.get('hardware_component', '')
            
            if 'display' in hardware_ref.lower():
                content.extend([
                    '    // Display update logic',
                    '    // TODO: Update display content based on mood/state',
                    '    ESP_LOGD(TAG, "Display update - loop %llu", g_loopCount);',
                ])
            elif 'sensor' in hardware_ref.lower():
                content.extend([
                    '    // Sensor reading logic',
                    '    // TODO: Read sensor data and process',
                    '    ESP_LOGD(TAG, "Sensor reading - loop %llu", g_loopCount);',
                ])
            elif 'speaker' in hardware_ref.lower():
                content.extend([
                    '    // Audio processing logic',
                    '    // TODO: Handle audio playback and effects',
                    '    ESP_LOGD(TAG, "Audio processing - loop %llu", g_loopCount);',
                ])
            else:
                content.extend([
                    '    // Generic component action',
                    '    ESP_LOGD(TAG, "Component action - loop %llu", loopCount);',
                ])
        
        content.append('}')
        
        return '\n'.join(content)

    def generate_component_registry_header(self) -> str:
        """Generate component registry header with all function declarations"""
        content = [
            '#ifndef P32_COMPONENT_REGISTRY_H',
            '#define P32_COMPONENT_REGISTRY_H',
            '',
            '#include "esp_err.h"',
            '#include <stdint.h>',
            '',
            '// Component initialization function type', 
            'typedef esp_err_t (*init_func_t)(void);',
            '',
            '// Component action function type',
            'typedef void (*act_func_t)(uint64_t loopCount);',
            '',
            '// Action table entry with timing',
            'typedef struct {',
            '    act_func_t act_func;',
            '    uint32_t hitCount;  // Execute every N loops',
            '    const char* name;   // Component name for debugging',
            '} act_table_entry_t;',
            '',
            '// Forward declarations for all component functions',
        ]
        
        # Add init function declarations
        content.append('// Initialization functions')
        for comp in self.components:
            content.append(f'esp_err_t {comp["init_func"]}(void);')
        
        content.append('')
        content.append('// Action functions') 
        for comp in self.components:
            content.append(f'void {comp["act_func"]}(uint64_t loopCount);')
        
        content.extend([
            '',
            '// Component tables',
            f'#define INIT_TABLE_SIZE {len(self.components)}',
            f'#define ACT_TABLE_SIZE {len(self.components)}',
            '',
            'extern init_func_t initTable[INIT_TABLE_SIZE];',
            'extern act_table_entry_t actTable[ACT_TABLE_SIZE];',
            '',
            '#endif // P32_COMPONENT_REGISTRY_H'
        ])
        
        return '\n'.join(content)

    def generate_component_tables(self) -> str:
        """Generate component table implementations"""
        content = [
            '// P32 Component Tables - Auto-generated',
            '// Links individual component files into unified tables',
            '',
            '#include "p32_component_registry.h"',
            '',
            '// Initialization table - all component init functions',
            'init_func_t initTable[INIT_TABLE_SIZE] = {'
        ]
        
        for comp in self.components:
            content.append(f'    {comp["init_func"]},')
        
        content.extend([
            '};',
            '',
            '// Action table - all component action functions with timing',
            'act_table_entry_t actTable[ACT_TABLE_SIZE] = {'
        ])
        
        for comp in self.components:
            content.append(f'    {{ {comp["act_func"]}, {comp["hitCount"]}, "{comp["name"]}" }},')
        
        content.extend([
            '};'
        ])
        
        return '\n'.join(content)

    def generate_all_components(self, bot_name: str = "goblin_simple"):
        """Generate all individual component files and tables"""
        print(f"Generating individual component files for {bot_name}...")
        
        # Load configuration
        bot_config = self.load_bot_config(bot_name)
        positioned_components = self.load_positioned_components(bot_config)
        
        # Generate component metadata
        self.components = self.generate_component_metadata(positioned_components)
        
        print(f"Found {len(self.components)} total components:")
        for comp in self.components:
            print(f"  - {comp['name']} ({comp['type']}) -> {comp['filename']}")
        
        # Generate individual component source files
        for comp in self.components:
            component_source = self.generate_individual_component_file(comp)
            component_file = self.components_dir / comp['filename']
            
            with open(component_file, 'w') as f:
                f.write(component_source)
            print(f"Generated: {component_file}")
        
        # Generate registry header
        registry_header = self.generate_component_registry_header()
        registry_file = self.include_dir / "p32_component_registry.h"
        
        with open(registry_file, 'w') as f:
            f.write(registry_header)
        print(f"Generated: {registry_file}")
        
        # Generate component tables
        tables_source = self.generate_component_tables()
        tables_file = self.output_dir / "p32_component_tables.cpp"
        
        with open(tables_file, 'w') as f:
            f.write(tables_source)
        print(f"Generated: {tables_file}")
        
        # Generate CMake component list for selective building
        cmake_content = [
            '# P32 Individual Component List - Auto-generated',
            '# Use this to selectively enable/disable components for memory analysis',
            '',
            'set(P32_COMPONENT_SOURCES'
        ]
        
        for comp in self.components:
            cmake_content.append(f'    components/{comp["filename"]}')
        
        cmake_content.extend([
            '    p32_component_tables.cpp',
            ')',
            '',
            '# Individual component targets for memory analysis',
            'foreach(component_file ${P32_COMPONENT_SOURCES})',
            '    get_filename_component(component_name ${component_file} NAME_WE)',
            '    # message(STATUS "P32 Component: ${component_name}")  ',
            'endforeach()',
        ])
        
        cmake_file = self.output_dir / "p32_components.cmake"
        with open(cmake_file, 'w') as f:
            f.write('\n'.join(cmake_content))
        print(f"Generated: {cmake_file}")
        
        print(f"\nGenerated {len(self.components)} individual component files!")
        print("Each component can now be measured independently for memory usage.")
        print("\nMemory analysis workflow:")
        print("1. Build with all components: `pio run`")  
        print("2. Build with single component: comment out others in CMake")
        print("3. Compare memory usage to isolate component overhead")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        bot_name = "goblin_simple"
        print(f"No bot specified, using default: {bot_name}")
    else:
        bot_name = sys.argv[1]
    
    config_dir = "config"
    output_dir = "src"
    
    generator = P32IndividualComponentGenerator(config_dir, output_dir)
    generator.generate_all_components(bot_name)