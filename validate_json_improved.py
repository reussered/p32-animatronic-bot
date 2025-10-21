#!/usr/bin/env python3
"""
IMPROVED JSON Path Validation and Component Compliance Script
Fixes JSON encoding issues, creates missing components, handles file names properly

Usage:
    python validate_json_improved.py [component_name] [root_directory]
    
Arguments:
    component_name (optional): Specific component to process (e.g., "goblin_pirate")
    root_directory (optional): Root directory containing the project (defaults to current directory)
    
Examples:
    python validate_json_improved.py                           # Process all components in current directory
    python validate_json_improved.py goblin_pirate             # Process only goblin_pirate component
    python validate_json_improved.py goblin_pirate /path/to/project  # Process goblin_pirate in specific directory
"""

import os
import json
import re
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple

class ImprovedJsonValidator:
    def __init__(self, root_dir: str, target_component: str = None):
        self.root_dir = Path(root_dir)
        self.target_component = target_component  # Only process this component if specified
        self.exclude_patterns = ['.venv', 'node_modules', '__pycache__', '.git']
        self.missing_files = []
        self.invalid_json_files = []
        self.components_to_create = []
        self.json_files_fixed = []
        self.changes_log = []
        self.created_components = set()  # Track to avoid duplicates
        self.all_components = []  # Track ALL components for dispatch table generation
        
    def log_change(self, change_type: str, details: str):
        """Track all changes made"""
        self.changes_log.append(f"{change_type}: {details}")
        print(f" {change_type}: {details}")
        
    def should_exclude_path(self, path: Path) -> bool:
        """Check if path should be excluded from processing"""
        path_str = str(path)
        for pattern in self.exclude_patterns:
            if pattern in path_str:
                return True
        return False
        
    def find_json_files(self) -> List[Path]:
        """Find all JSON files excluding certain directories"""
        json_files = []
        for json_file in self.root_dir.rglob("*.json"):
            if not self.should_exclude_path(json_file):
                json_files.append(json_file)
        print(f"Found {len(json_files)} JSON files (excluding .venv, etc.)")
        return json_files
        
    def fix_json_encoding(self, json_path: Path) -> bool:
        """Fix UTF-8 BOM and other encoding issues"""
        try:
            # Try reading with UTF-8-BOM first
            with open(json_path, 'r', encoding='utf-8-sig') as f:
                content = f.read()
                
            # Parse to validate JSON
            json_data = json.loads(content)
            
            # Write back without BOM
            with open(json_path, 'w', encoding='utf-8') as f:
                json.dump(json_data, f, indent=2, ensure_ascii=False)
                
            self.log_change("JSON_FIXED", f"Fixed encoding: {json_path.relative_to(self.root_dir)}")
            return True
        except Exception as e:
            self.invalid_json_files.append((json_path, str(e)))
            print(f" Could not fix JSON: {json_path.relative_to(self.root_dir)} - {e}")
            return False
            
    def validate_json_file(self, json_path: Path) -> Tuple[bool, Dict]:
        """Validate a JSON file and return success status and content"""
        try:
            with open(json_path, 'r', encoding='utf-8') as f:
                content = json.load(f)
            return True, content
        except json.JSONDecodeError:
            # Try fixing encoding first
            if self.fix_json_encoding(json_path):
                try:
                    with open(json_path, 'r', encoding='utf-8') as f:
                        content = json.load(f)
                    return True, content
                except Exception as e:
                    self.invalid_json_files.append((json_path, str(e)))
                    return False, {}
            else:
                return False, {}
        except Exception as e:
            self.invalid_json_files.append((json_path, str(e)))
            return False, {}
            
    def sanitize_component_name(self, name: str) -> str:
        """Convert component name to valid C++ identifier"""
        # Replace spaces and special chars with underscores
        sanitized = re.sub(r'[^a-zA-Z0-9_]', '_', name)
        # Remove multiple consecutive underscores
        sanitized = re.sub(r'_+', '_', sanitized)
        # Remove leading/trailing underscores
        sanitized = sanitized.strip('_')
        # Convert to lowercase
        sanitized = sanitized.lower()
        # Ensure it doesn't start with a number (C++ requirement)
        if sanitized and sanitized[0].isdigit():
            sanitized = 'component_' + sanitized
        return sanitized
        
    def determine_component_files_needed(self, component_name: str, json_path: Path, json_data: Dict) -> List[Tuple[str, str]]:
        """Determine what .cpp/.hpp files should exist for a component"""
        files_needed = []
        
        # Check if this has hardware_only flag
        hardware_only = json_data.get('hardware_only', False)
        
        if not hardware_only:
            # Sanitize component name for file system
            safe_name = self.sanitize_component_name(component_name)
            
            # Regular component needs .cpp and .hpp files
            cpp_file = f"src/components/{safe_name}.cpp"
            hpp_file = f"include/components/{safe_name}.hpp"
            files_needed.append((cpp_file, 'implementation', safe_name))
            files_needed.append((hpp_file, 'header', safe_name))
            
        return files_needed
        
    def create_component_template(self, component_name: str, file_type: str, json_data: Dict) -> str:
        """Create template content for component files"""
        
        safe_name = self.sanitize_component_name(component_name)
        
        if file_type == 'header':
            return f'''#ifndef {safe_name.upper()}_HPP
#define {safe_name.upper()}_HPP

/**
 * @file {safe_name}.hpp
 * @brief {json_data.get('description', f'{component_name} component')}
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize {safe_name} component
 * Called once during system startup
 */
void {safe_name}_init(void);

/**
 * @brief Execute {safe_name} component logic
 * Called every loop iteration based on hitCount
 */
void {safe_name}_act(void);

#endif // {safe_name.upper()}_HPP
'''
        elif file_type == 'implementation':
            return f'''/**
 * @file {safe_name}.cpp
 * @brief {json_data.get('description', f'{component_name} component implementation')}
 * @author Auto-generated from JSON specification
 */

#include "components/{safe_name}.hpp"
#include "core/memory/SharedMemory.hpp"
#include "p32_shared_state.h"

// External GSM instance
extern SharedMemory GSM;

/**
 * @brief Initialize {safe_name} component
 * Called once during system startup
 */
void {safe_name}_init(void) {{
    // Component initialization logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Read shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    
    // TODO: Implement component-specific initialization
}}

/**
 * @brief Execute {safe_name} component logic  
 * Called every loop iteration based on hitCount: {json_data.get('hitCount', 1)}
 */
void {safe_name}_act(void) {{
    // Component execution logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Update shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    // // Modify shared_data...
    // GSM.write<SomeSharedDataType>(shared_data);
    
    // TODO: Implement component-specific behavior
}}
'''
        
        return ""
        
    def validate_all_json_files(self):
        """Main validation function"""
        if self.target_component:
            print(f" Starting improved JSON validation for component: {self.target_component}")
        else:
            print(" Starting improved JSON validation for all components...")
        
        json_files = self.find_json_files()
        
        for json_file in json_files:
            print(f"\n Processing: {json_file.relative_to(self.root_dir)}")
            
            # Validate JSON syntax
            is_valid, data = self.validate_json_file(json_file)
            if not is_valid:
                continue
                
            # Check if this is a component definition
            if 'component_name' in data:
                component_name = data['component_name']
                safe_name = self.sanitize_component_name(component_name)
                
                # If target_component is specified, only process matching components
                if self.target_component:
                    # Check if this component or its dependencies match the target
                    if not self.should_process_component(component_name, data):
                        print(f"    Skipping non-target component: {component_name}")
                        continue
                
                # Avoid duplicates
                if safe_name in self.created_components:
                    print(f"    Skipping duplicate component: {component_name} (already processed as {safe_name})")
                    continue
                    
                # Add to complete component list for dispatch table generation
                # Only include components that have software functions (not hardware_only)
                if not data.get('hardware_only', False):
                    hit_count = data.get('hitCount', 1)
                    description = data.get('description', f'{component_name} component')
                    self.all_components.append((safe_name, hit_count, description, component_name))
                
                files_needed = self.determine_component_files_needed(component_name, json_file, data)
                
                for file_path, file_type, sanitized_name in files_needed:
                    full_path = self.root_dir / file_path
                    if not full_path.exists():
                        self.components_to_create.append((component_name, file_path, file_type, data, sanitized_name))
                        print(f"   Need to create: {file_path}")
                        
                self.created_components.add(safe_name)
                        
    def should_process_component(self, component_name: str, data: Dict) -> bool:
        """Determine if this component should be processed based on target_component filter"""
        if not self.target_component:
            return True  # Process all components if no target specified
            
        # Direct match
        if component_name == self.target_component:
            return True
            
        # Check if this component contains the target (for bots that contain components)
        if 'contains' in data:
            contains_list = data['contains']
            for contained_item in contains_list:
                if isinstance(contained_item, str):
                    if contained_item == self.target_component:
                        return True
                elif isinstance(contained_item, dict):
                    if contained_item.get('component_name') == self.target_component:
                        return True
                        
        # Check if component name contains target as substring (e.g., goblin_pirate_head contains goblin_pirate)
        if self.target_component in component_name:
            return True
            
        # Check if target contains this component (dependency relationship)
        if component_name in self.target_component:
            return True
            
        return False
                        
    def execute_fixes(self):
        """Execute all fixes and create missing files"""
        print(f"\n[TOOL] Executing fixes...")
        
        # Create missing component files
        for component_name, file_path, file_type, json_data, sanitized_name in self.components_to_create:
            full_path = self.root_dir / file_path
            full_path.parent.mkdir(parents=True, exist_ok=True)
            
            content = self.create_component_template(component_name, file_type, json_data)
            
            with open(full_path, 'w', encoding='utf-8') as f:
                f.write(content)
                
            self.log_change("COMPONENT_CREATED", f"{file_path} ({file_type}) for '{component_name}' as '{sanitized_name}'")
            
        # Generate dispatch tables after creating components
        self.generate_dispatch_tables()
        
    def get_component_hierarchy_level(self, component_name: str) -> int:
        """Determine the hierarchy level of a component for proper initialization order"""
        # System Level (Level 1): Core platform components - always present
        system_components = [
            'system_core', 'watchdog', 'serial_console', 'power_manager', 
            'power_monitor', 'wifi_station', 'esp_now_mesh', 'network_monitor',
            'telemetry_hub', 'mesh_coordinator', 'mesh_software_coordinator',
            'master_controller', 'bluetooth_central'
        ]
        
        # Family Level (Level 2): Behavior/personality shared across bot family
        family_components = [
            'goblin_personality', 'goblin_mood', 'goblin_behavior_engine',
            'goblin_head', 'goblin_torso'
        ]
        
        # Hardware/Template Level (Level 3): Reusable hardware components
        hardware_components = [
            'gc9a01_circular_lcd_display', 'hc_sr04_ultrasonic_distance_sensor',
            'i2s_digital_speaker_module', 'audio', 'forehead_leds',
            'component_2_5x1_5_inch_tft_lcd_display'
        ]
        
        # Bot-Specific Level (Level 4): Positioned hardware instances
        # These are the actual eyes, ears, nose, mouth, speaker components
        
        if component_name in system_components:
            return 1
        elif component_name in family_components:
            return 2
        elif component_name in hardware_components:
            return 3
        else:
            return 4  # Bot-specific positioned components
            
    def get_bot_specific_order(self, component_name: str) -> int:
        """Define the order within Bot-Specific Level for logical initialization"""
        # Eyes should be initialized first (display components)
        eye_components = ['left_eye', 'right_eye', 'center_eye']
        # Then ears (audio input)
        ear_components = ['left_ear', 'right_ear', 'left_ear_microphone', 'right_ear_microphone']
        # Then nose (sensor input)
        nose_components = ['nose_sensor']
        # Then mouth (display output)
        mouth_components = ['mouth']
        # Then speaker (audio output)
        speaker_components = ['speaker']
        # Then body parts (movement components)
        body_components = [
            'shoulder_flexion', 'elbow_flexion', 'wrist_flexion', 'index_finger', 'thumb',
            'spine_lower', 'hip_flexion', 'knee_flexion'
        ]
        
        if component_name in eye_components:
            return 1
        elif component_name in ear_components:
            return 2
        elif component_name in nose_components:
            return 3
        elif component_name in mouth_components:
            return 4
        elif component_name in speaker_components:
            return 5
        elif component_name in body_components:
            return 6
        else:
            return 7  # Unknown components last
            
    def generate_dispatch_tables(self):
        """Generate the 4 dispatch table files"""
        print(f"\n[CHECKLIST] Generating dispatch tables...")
        
        # Use ALL discovered components (not just components being created)
        # Remove duplicates and sort by hierarchy level, then by bot-specific order, then by name
        unique_components = list(set(self.all_components))
        
        # Sort by hierarchy level first, then by bot-specific order for level 4, then alphabetically
        def sort_key(component_tuple):
            comp_name = component_tuple[0]
            hierarchy_level = self.get_component_hierarchy_level(comp_name)
            if hierarchy_level == 4:  # Bot-specific level
                bot_order = self.get_bot_specific_order(comp_name)
                return (hierarchy_level, bot_order, comp_name)
            else:
                return (hierarchy_level, 0, comp_name)
        
        unique_components.sort(key=sort_key)
        
        print(f"Including {len(unique_components)} components in dispatch tables (hierarchical order):")
        current_level = 0
        for comp_name, hit_count, description, original_name in unique_components:
            level = self.get_component_hierarchy_level(comp_name)
            if level != current_level:
                current_level = level
                level_names = {1: "System Level", 2: "Family Level", 3: "Hardware Level", 4: "Bot-Specific Level"}
                print(f"\n  === {level_names.get(level, f'Level {level}')} ===")
            print(f"  - {comp_name} (hitCount={hit_count}) [{original_name}]")
        
        # Generate p32_dispatch_tables.hpp
        self.create_dispatch_tables_header(unique_components)
        
        # Generate p32_dispatch_tables.cpp  
        self.create_dispatch_tables_implementation(unique_components)
        
        # Generate p32_component_functions.hpp
        self.create_component_functions_header(unique_components)
        
        # Generate p32_component_functions.cpp
        self.create_component_functions_implementation(unique_components)
        
        # Update CMakeLists.txt to include all component files
        self.update_cmake_lists()
        
    def update_cmake_lists(self):
        """Update src/CMakeLists.txt to include all component files"""
        cmake_file = self.root_dir / "src" / "CMakeLists.txt"
        
        if not cmake_file.exists():
            print(f"Warning: CMakeLists.txt not found at {cmake_file}")
            return
            
        # Get all component .cpp files
        components_dir = self.root_dir / "src" / "components"
        if not components_dir.exists():
            print(f"Warning: Components directory not found at {components_dir}")
            return
            
        # Find all .cpp files in components directory (excluding subdirectories for now)
        component_files = []
        for cpp_file in components_dir.glob("*.cpp"):
            relative_path = f"components/{cpp_file.name}"
            component_files.append(relative_path)
        
        component_files.sort()  # Sort for consistent output
        
        # Read current CMakeLists.txt
        content = cmake_file.read_text()
        
        # Create the new component sources section
        component_sources_section = """# P32 Individual Components - Auto-generated to include all components
set(P32_COMPONENT_SOURCES
    p32_dispatch_tables.cpp         # Generated dispatch tables (initTable, actTable, hitCountTable)
    p32_component_functions.cpp     # Aggregates all individual component implementations
    p32_eye_display.cpp             # Eye animation system
    p32_web_client.cpp              # Web client for PC streaming (test mode)
    Mood.cpp                        # C++ Mood system with efficient delta calculations
    FrameProcessor.cpp              # C++ Frame processing with mood-based optimization
    core/memory/SharedMemory.cpp    # SharedMemory ESP-NOW mesh communication"""

        # Add all component files
        for comp_file in component_files:
            component_sources_section += f"\n    {comp_file}"
            
        component_sources_section += "\n)"
        
        # Find the P32_COMPONENT_SOURCES section and replace it
        import re
        pattern = r'# P32 Individual Components.*?set\(P32_COMPONENT_SOURCES.*?\n\)'
        
        if re.search(pattern, content, re.DOTALL):
            new_content = re.sub(pattern, component_sources_section, content, flags=re.DOTALL)
            
            # Write the updated content
            cmake_file.write_text(new_content)
            print(f" CMAKE_UPDATED: {cmake_file}")
            print(f"   Added {len(component_files)} component files to build")
        else:
            print(f"Warning: Could not find P32_COMPONENT_SOURCES section in {cmake_file}")
        
    def create_dispatch_tables_header(self, components):
        """Create p32_dispatch_tables.hpp"""
        content = f'''#ifndef P32_DISPATCH_TABLES_H
#define P32_DISPATCH_TABLES_H

#include "esp_err.h"
#include <stdint.h>

// ============================================================================
// P32 Component Dispatch Tables
// Auto-generated from JSON bot configuration
// ============================================================================

// Component function type signatures - NO ARGUMENTS pattern
typedef void (*init_func_t)(void);
typedef void (*act_func_t)(void);

// Table size - all three tables have same size (one entry per component)
#define COMPONENT_TABLE_SIZE {len(components)}

// ============================================================================
// Forward Declarations - Init Functions
// ============================================================================

'''
        
        for sanitized_name, _, _, _ in components:
            content += f"void {sanitized_name}_init(void);\n"
            
        content += '''
// ============================================================================
// Forward Declarations - Act Functions (NO ARGUMENTS)
// ============================================================================

'''
        
        for sanitized_name, _, _, _ in components:
            content += f"void {sanitized_name}_act(void);\n"
            
        content += '''
// ============================================================================
// Dispatch Tables
// ============================================================================

// Initialization table - called once at startup in order
extern init_func_t initTable[COMPONENT_TABLE_SIZE];

// Action table - function pointers (NO ARGUMENTS)
// Parallel to hitCountTable - both indexed by same i
extern act_func_t actTable[COMPONENT_TABLE_SIZE];

// Timing table - execution frequency control
// actTable[i] executes when g_loopCount % hitCountTable[i] == 0
extern uint32_t hitCountTable[COMPONENT_TABLE_SIZE];

// Global loop counter - shared across all components
extern uint64_t g_loopCount;

#endif // P32_DISPATCH_TABLES_H
'''
        
        dispatch_header_path = self.root_dir / "include" / "p32_dispatch_tables.hpp"
        dispatch_header_path.parent.mkdir(parents=True, exist_ok=True)
        with open(dispatch_header_path, 'w', encoding='utf-8') as f:
            f.write(content)
        self.log_change("DISPATCH_TABLE_CREATED", "include/p32_dispatch_tables.hpp")
        
    def create_dispatch_tables_implementation(self, components):
        """Create p32_dispatch_tables.cpp"""
        content = f'''#include "p32_dispatch_tables.hpp"
#include "p32_component_functions.hpp"

// ============================================================================
// Global Loop Counter - Written by main.cpp, read by all components
// ============================================================================

uint64_t g_loopCount = 0;

// ============================================================================
// P32 Component Dispatch Tables - Implementation
// Auto-generated from JSON bot configuration
// ============================================================================

// ============================================================================
// Initialization Table
// ============================================================================

init_func_t initTable[COMPONENT_TABLE_SIZE] = {{
'''
        
        for i, (sanitized_name, _, _, _) in enumerate(components):
            comma = "," if i < len(components) - 1 else ""
            content += f"    {sanitized_name}_init{comma}\n"
            
        content += '''};

// ============================================================================
// Action Table - Function Pointers (NO ARGUMENTS)
// ============================================================================

act_func_t actTable[COMPONENT_TABLE_SIZE] = {
'''
        
        for i, (sanitized_name, _, description, _) in enumerate(components):
            comma = "," if i < len(components) - 1 else ""
            content += f"    {sanitized_name}_act{comma}    // [{i}] {description}\n"
            
        content += '''};

// ============================================================================
// Timing Table - Execution Frequency Control
// ============================================================================

uint32_t hitCountTable[COMPONENT_TABLE_SIZE] = {
'''
        
        for i, (sanitized_name, hit_count, _, _) in enumerate(components):
            comma = "," if i < len(components) - 1 else ""
            freq_hz = 120000 // hit_count if hit_count > 0 else 120000
            content += f"    {hit_count}{comma}    // [{i}] {sanitized_name} - every {hit_count} loops ({freq_hz} Hz)\n"
            
        content += '''};
'''
        
        dispatch_impl_path = self.root_dir / "src" / "p32_dispatch_tables.cpp"
        dispatch_impl_path.parent.mkdir(parents=True, exist_ok=True)
        with open(dispatch_impl_path, 'w', encoding='utf-8') as f:
            f.write(content)
        self.log_change("DISPATCH_TABLE_CREATED", "src/p32_dispatch_tables.cpp")
        
    def create_component_functions_header(self, components):
        """Create p32_component_functions.hpp"""
        content = '''#ifndef P32_COMPONENT_FUNCTIONS_H
#define P32_COMPONENT_FUNCTIONS_H

#include "esp_err.h"

// ============================================================================
// P32 Component Function Implementations
// Includes all individual component init() and act() functions
// ============================================================================

// Include all individual component headers
'''
        
        for sanitized_name, _, _, _ in components:
            content += f'#include "components/{sanitized_name}.hpp"\n'
            
        content += '''
// Additional component includes will be added here as components are implemented

#endif // P32_COMPONENT_FUNCTIONS_H
'''
        
        comp_funcs_header_path = self.root_dir / "include" / "p32_component_functions.hpp"
        comp_funcs_header_path.parent.mkdir(parents=True, exist_ok=True)
        with open(comp_funcs_header_path, 'w', encoding='utf-8') as f:
            f.write(content)
        self.log_change("COMPONENT_FUNCTIONS_CREATED", "include/p32_component_functions.hpp")
        
    def create_component_functions_implementation(self, components):
        """Create p32_component_functions.cpp"""
        content = '''#include "p32_component_functions.hpp"

// ============================================================================
// P32 Component Function Implementations
// This file aggregates all individual component implementations
// ============================================================================

// All component implementations are included via their individual headers
// The actual function definitions are in their respective component files:
//
'''
        
        for sanitized_name, _, _, _ in components:
            content += f"// - components/{sanitized_name}.cpp ({sanitized_name}_init, {sanitized_name}_act functions)\n"
            
        content += '''//
// This file serves as the central include point for all component functions
// that are referenced in the dispatch tables.
'''
        
        comp_funcs_impl_path = self.root_dir / "src" / "p32_component_functions.cpp"
        comp_funcs_impl_path.parent.mkdir(parents=True, exist_ok=True)
        with open(comp_funcs_impl_path, 'w', encoding='utf-8') as f:
            f.write(content)
        self.log_change("COMPONENT_FUNCTIONS_CREATED", "src/p32_component_functions.cpp")
            
    def generate_report(self):
        """Generate final report"""
        print(f"\n IMPROVED VALIDATION REPORT")
        print(f"=" * 60)
        print(f"Invalid JSON files: {len(self.invalid_json_files)}")
        print(f"Components created: {len(self.components_to_create)}")
        print(f"Unique component names: {len(self.created_components)}")
        print(f"Total changes: {len(self.changes_log)}")
        
        if self.changes_log:
            print(f"\n CHANGES MADE:")
            for change in self.changes_log:
                print(f"  {change}")
                
        if self.invalid_json_files:
            print(f"\n INVALID JSON FILES:")
            for file_path, error in self.invalid_json_files:
                print(f"  {file_path.relative_to(self.root_dir)}: {error}")
                
        if self.created_components:
            print(f"\n UNIQUE COMPONENTS PROCESSED:")
            for comp in sorted(self.created_components):
                print(f"  {comp}")

def print_usage():
    """Print usage information"""
    print("""
Usage: python validate_json_improved.py [component_name] [root_directory]

Arguments:
    component_name (optional): Specific component to process (e.g., "goblin_pirate")
    root_directory (optional): Root directory containing the project (defaults to current directory)

Examples:
    python validate_json_improved.py                           # Process all components in current directory
    python validate_json_improved.py goblin_pirate             # Process only goblin_pirate component
    python validate_json_improved.py goblin_pirate /path/to/project  # Process goblin_pirate in specific directory
    """)

if __name__ == "__main__":
    # Parse command line arguments
    component_name = None
    root_directory = os.getcwd()  # Default to current directory
    
    if len(sys.argv) > 1:
        if sys.argv[1] in ['-h', '--help', 'help']:
            print_usage()
            sys.exit(0)
        component_name = sys.argv[1]
        
    if len(sys.argv) > 2:
        root_directory = sys.argv[2]
        
    # Validate root directory exists
    if not os.path.exists(root_directory):
        print(f"Error: Root directory '{root_directory}' does not exist")
        sys.exit(1)
        
    # Print what we're doing
    if component_name:
        print(f"Processing component: {component_name}")
        print(f"Root directory: {root_directory}")
    else:
        print(f"Processing all components")
        print(f"Root directory: {root_directory}")
        
    # Execute validation and generation
    validator = ImprovedJsonValidator(root_directory, component_name)
    validator.validate_all_json_files()
    validator.execute_fixes()
    validator.generate_report()