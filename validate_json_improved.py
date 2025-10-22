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
        
        # Multi-ESP32 chip distribution
        self.chip_assignments = {}  # component_name -> chip_type mapping
        self.chip_components = {}   # chip_type -> [component_list] mapping
        self.component_hierarchy = {}  # component_name -> parent_component mapping
        
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
        
    def find_bot_json_files(self) -> List[Path]:
        """Find bot definition JSON files for tree analysis"""
        bot_files = []
        
        # Look for bot files in common locations
        bot_directories = [
            self.root_dir / 'config' / 'bots',
            self.root_dir / 'config' / 'creatures',
            self.root_dir / 'bots',
            self.root_dir / 'creatures'
        ]
        
        for bot_dir in bot_directories:
            if bot_dir.exists():
                for json_file in bot_dir.rglob('*.json'):
                    if not self.should_exclude_path(json_file):
                        # Check if this looks like a bot definition
                        try:
                            with open(json_file, 'r', encoding='utf-8') as f:
                                data = json.load(f)
                            
                            # Look for indicators this is a bot definition
                            if any(key in data for key in ['bot_type', 'creature_type', 'family']):
                                bot_files.append(json_file)
                                print(f"  Discovered bot file: {json_file.relative_to(self.root_dir)}")
                        except Exception:
                            continue
        
        print(f"Found {len(bot_files)} bot files total")
        return bot_files

    def generate_multi_esp32_files(self, subsystem_trees: Dict):
        """Generate all files for multi-ESP32 subsystem architecture"""
        print(f"\n=== GENERATING MULTI-ESP32 SUBSYSTEM FILES ===")
        
        for subsystem_name, tree_data in subsystem_trees.items():
            if not tree_data['components']:
                print(f"Skipping empty subsystem: {subsystem_name}")
                continue
                
            print(f"\nGenerating files for subsystem: {subsystem_name}")
            print(f"  Chip: {tree_data['chip_type']}")
            print(f"  Components: {sorted(tree_data['components'])}")
            print(f"  Dispatch Entries: {len(tree_data['dispatch_entries'])}")
            
            # Create subsystem directory
            subsystem_dir = self.root_dir / 'src' / 'subsystems' / subsystem_name
            subsystem_dir.mkdir(parents=True, exist_ok=True)
            
            # Convert components set to list format expected by existing methods
            components_list = []
            for component_name in sorted(tree_data['components']):
                # Use default values for components we don't have full data for
                sanitized_name = self.sanitize_component_name(component_name)
                hit_count = 1  # Default hit count
                description = f"{component_name} component"
                components_list.append((sanitized_name, hit_count, description, component_name))
            
            # Generate the 4 subsystem files
            self.create_subsystem_dispatch_files_by_name(subsystem_name, tree_data['chip_type'], components_list, subsystem_dir)
            
        print(f"\nMulti-ESP32 file generation complete!")

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
        
    def analyze_json_tree_for_subsystems(self, bot_json_path: Path):
        """
        Phase 1: Discover all subsystems by finding chip assignments in JSON tree
        Returns dict of {subsystem_name: chip_type}
        """
        print(f"\n=== ANALYZING JSON TREE FOR SUBSYSTEMS ===")
        print(f"Bot JSON: {bot_json_path}")
        
        # Load the top-level bot JSON
        try:
            with open(bot_json_path, 'r', encoding='utf-8') as f:
                bot_data = json.load(f)
        except Exception as e:
            print(f"Error loading bot JSON: {e}")
            return {}
        
        discovered_subsystems = {}
        
        def walk_component_tree(component_data, current_path="", parent_chip=None):
            """Recursively walk JSON tree to find chip assignments"""
            
            # Check if this component has a chip assignment (try multiple field names)
            assigned_chip = (component_data.get('chip') or 
                           component_data.get('controller_hardware') or 
                           parent_chip)
            
            # If this component has a new chip assignment, it becomes a subsystem
            if 'chip' in component_data or 'controller_hardware' in component_data:
                subsystem_name = self.normalize_chip_name_for_subsystem(current_path if current_path else 'main')
                chip_type = self.normalize_chip_name(assigned_chip)
                discovered_subsystems[subsystem_name] = chip_type
                print(f"  Found subsystem: {subsystem_name} -> {chip_type}")
                parent_chip = assigned_chip  # Update parent for children
            
            # Process component references
            if 'component' in component_data:
                component_ref = component_data['component']
                component_path = self.resolve_component_path(component_ref)
                if component_path and component_path.exists():
                    try:
                        with open(component_path, 'r', encoding='utf-8') as f:
                            sub_component = json.load(f)
                        walk_component_tree(sub_component, current_path, parent_chip)
                    except Exception as e:
                        print(f"Warning: Could not load component {component_path}: {e}")
            
            # Process child components
            for key, value in component_data.items():
                if isinstance(value, dict) and key not in ['chip', 'component', 'position', 'interface']:
                    child_path = f"{current_path}_{key}" if current_path else key
                    walk_component_tree(value, child_path, parent_chip)
        
        # Start analysis from bot root
        walk_component_tree(bot_data)
        
        print(f"\nDISCOVERED SUBSYSTEMS: {len(discovered_subsystems)}")
        for subsystem, chip in discovered_subsystems.items():
            print(f"  {subsystem}: {chip}")
        
        return discovered_subsystems

    def build_subsystem_component_trees(self, bot_json_path: Path, discovered_subsystems: Dict):
        """
        Phase 2: For each discovered subsystem, build its component tree
        Returns dict of {subsystem_name: {components: set, dispatch_entries: list}}
        """
        print(f"\n=== BUILDING SUBSYSTEM COMPONENT TREES ===")
        
        subsystem_trees = {}
        
        # Initialize each subsystem
        for subsystem_name in discovered_subsystems.keys():
            subsystem_trees[subsystem_name] = {
                'components': set(),  # Unique components
                'dispatch_entries': [],  # All dispatch entries (allows duplicates)
                'chip_type': discovered_subsystems[subsystem_name]
            }
        
        # Load the top-level bot JSON
        try:
            with open(bot_json_path, 'r', encoding='utf-8') as f:
                bot_data = json.load(f)
        except Exception as e:
            print(f"Error loading bot JSON: {e}")
            return subsystem_trees
        
        def assign_components_to_subsystems(component_data, current_path="", current_subsystem=None):
            """Recursively assign components to their subsystems"""
            
            # Determine which subsystem this component belongs to
            if 'chip' in component_data or 'controller_hardware' in component_data:
                # This component defines a new subsystem
                subsystem_name = self.normalize_chip_name_for_subsystem(current_path if current_path else 'main')
                current_subsystem = subsystem_name
            
            # Add component to its subsystem
            if current_subsystem and current_subsystem in subsystem_trees:
                # Add to dispatch table (allows duplicates)
                if current_path:
                    subsystem_trees[current_subsystem]['dispatch_entries'].append(current_path)
                
                # Add to unique components set
                if 'component' in component_data:
                    component_ref = component_data['component']
                    component_name = Path(component_ref).stem
                    subsystem_trees[current_subsystem]['components'].add(component_name)
            
            # Process component references
            if 'component' in component_data:
                component_ref = component_data['component']
                component_path = self.resolve_component_path(component_ref)
                if component_path and component_path.exists():
                    try:
                        with open(component_path, 'r', encoding='utf-8') as f:
                            sub_component = json.load(f)
                        assign_components_to_subsystems(sub_component, current_path, current_subsystem)
                    except Exception as e:
                        print(f"Warning: Could not load component {component_path}: {e}")
            
            # Process child components
            for key, value in component_data.items():
                if isinstance(value, dict) and key not in ['chip', 'component', 'position', 'interface']:
                    child_path = f"{current_path}_{key}" if current_path else key
                    assign_components_to_subsystems(value, child_path, current_subsystem)
        
        # Build trees for all subsystems
        assign_components_to_subsystems(bot_data)
        
        # Print results
        for subsystem_name, tree_data in subsystem_trees.items():
            print(f"\nSubsystem: {subsystem_name}")
            print(f"  Chip: {tree_data['chip_type']}")
            print(f"  Unique Components: {sorted(tree_data['components'])}")
            print(f"  Dispatch Entries: {tree_data['dispatch_entries']}")
        
        return subsystem_trees

    def normalize_chip_name_for_subsystem(self, path: str) -> str:
        """Convert component path to subsystem name"""
        if not path:
            return "main"
        
        # Use the full path as subsystem name (it's already the component with chip assignment)
        # Replace underscores with underscores for consistency and convert to lowercase
        subsystem_name = path.lower().replace('-', '_')
        
        # Ensure it's a valid C identifier
        if subsystem_name and subsystem_name[0].isdigit():
            subsystem_name = 'subsystem_' + subsystem_name
            
        return subsystem_name

    def resolve_component_path(self, component_ref: str) -> Path:
        """Resolve relative component reference to absolute path"""
        if component_ref.startswith('/'):
            # Absolute path from project root
            return self.root_dir / component_ref.lstrip('/')
        else:
            # Relative path - try common locations
            possible_paths = [
                self.root_dir / component_ref,
                self.root_dir / 'config' / component_ref,
                self.root_dir / 'config' / 'components' / component_ref
            ]
            
            for path in possible_paths:
                if path.exists():
                    return path
            
            print(f"Warning: Could not resolve component path: {component_ref}")
            return None
    
    def determine_chip_assignment(self, component_name: str, json_data: Dict, json_path: Path) -> str:
        """Determine which ESP32 chip controls this component"""
        # Check for direct chip assignment
        chip = json_data.get('chip') or json_data.get('controller_hardware')
        if chip:
            return self.normalize_chip_name(chip)
            
        # No direct chip assignment - walk up hierarchy to find controlling chip
        return self.find_parent_chip(component_name, json_path)
        
    def normalize_chip_name(self, chip: str) -> str:
        """Normalize chip names to standard format"""
        chip_upper = chip.upper()
        if 'ESP32_S3' in chip_upper or 'ESP32-S3' in chip_upper:
            return 'ESP32_S3'
        elif 'ESP32_C3' in chip_upper or 'ESP32-C3' in chip_upper:
            return 'ESP32_C3'
        elif 'ESP32' in chip_upper:
            return 'ESP32'
        else:
            return chip_upper
            
    def find_parent_chip(self, component_name: str, json_path: Path) -> str:
        """Walk up component hierarchy to find the controlling chip"""
        # For now, use path-based heuristics to determine subsystem
        path_str = str(json_path)
        
        # Check subsystem patterns from path
        if 'torso' in path_str.lower():
            return 'ESP32_S3'  # Torso uses ESP32-S3
        elif any(x in path_str.lower() for x in ['arm', 'hand', 'leg', 'foot']):
            return 'ESP32_C3'  # Limbs use ESP32-C3
        elif 'head' in path_str.lower():
            return 'ESP32_S3'  # Head uses ESP32-S3
        else:
            # Default to ESP32-S3 for system components
            return 'ESP32_S3'
            
    def build_component_hierarchy(self):
        """Build parent-child relationships for components"""
        # This will be expanded to parse 'contains' fields in JSON
        # For now, focus on basic chip assignment
        pass
        
    def generate_subsystem_dispatch_tables(self):
        """Generate separate dispatch tables for each ESP32 chip"""
        print(f"\n[CHECKLIST] Generating subsystem-specific dispatch tables...")
        
        # Group components by chip and analyze paths to determine subsystem names
        subsystem_names = self.determine_subsystem_names()
        
        # Group components by subsystem
        for subsystem_name, chip_type in subsystem_names.items():
            if chip_type not in self.chip_components:
                continue
                
            components = self.chip_components[chip_type]
            if not components:
                continue
                
            print(f"  Generating dispatch tables for {subsystem_name} ({chip_type}, {len(components)} components)")
            
            # Create subsystem directory
            subsystem_dir = self.root_dir / 'src' / 'subsystems' / subsystem_name
            subsystem_dir.mkdir(parents=True, exist_ok=True)
            
            # Generate the 4 files for this subsystem
            self.create_subsystem_dispatch_files_by_name(subsystem_name, chip_type, components, subsystem_dir)
            
    def determine_subsystem_names(self) -> Dict[str, str]:
        """Analyze components to determine subsystem names and their chip types"""
        subsystem_names = {}
        
        # Analyze component paths to determine subsystems
        for chip_type, components in self.chip_components.items():
            if chip_type == 'ESP32_S3':
                # ESP32-S3 chips: head and torso
                has_display = any('eye' in comp[0] or 'mouth' in comp[0] for comp in components)
                has_system = any('wifi' in comp[0] or 'mesh' in comp[0] or 'power' in comp[0] for comp in components)
                
                if has_display:
                    subsystem_names['head'] = 'ESP32_S3'
                if has_system:
                    subsystem_names['torso'] = 'ESP32_S3'
                    
            elif chip_type == 'ESP32_C3':
                # ESP32-C3 chips: arms, legs, hands, feet
                for comp_name, _, _, _ in components:
                    if 'left_arm' in comp_name or 'shoulder' in comp_name:
                        subsystem_names['left_arm'] = 'ESP32_C3'
                    elif 'right_arm' in comp_name:
                        subsystem_names['right_arm'] = 'ESP32_C3'  
                    elif 'left_hand' in comp_name or 'finger' in comp_name:
                        subsystem_names['left_hand'] = 'ESP32_C3'
                    elif 'right_hand' in comp_name:
                        subsystem_names['right_hand'] = 'ESP32_C3'
                    elif 'left_leg' in comp_name or 'hip' in comp_name:
                        subsystem_names['left_leg'] = 'ESP32_C3'
                    elif 'right_leg' in comp_name:
                        subsystem_names['right_leg'] = 'ESP32_C3'
                    elif 'left_foot' in comp_name or 'ankle' in comp_name:
                        subsystem_names['left_foot'] = 'ESP32_C3'
                    elif 'right_foot' in comp_name:
                        subsystem_names['right_foot'] = 'ESP32_C3'
        
        return subsystem_names
        
    def create_subsystem_dispatch_files_by_name(self, subsystem_name: str, chip_type: str, components: List, subsystem_dir: Path):
        """Create the 4 dispatch table files for a named subsystem"""
        # Sort components for this subsystem
        sorted_components = sorted(components, key=lambda x: (
            self.get_component_hierarchy_level(x[0]),
            self.get_bot_specific_order(x[0]) if self.get_component_hierarchy_level(x[0]) == 4 else 0,
            x[0]
        ))
        
        # Generate files with subsystem-specific names
        files_to_create = [
            (f'{subsystem_name}_dispatch_tables.hpp', self.create_dispatch_tables_header(sorted_components)),
            (f'{subsystem_name}_dispatch_tables.cpp', self.create_dispatch_tables_implementation(sorted_components)),
            (f'{subsystem_name}_component_functions.hpp', self.create_component_functions_header(sorted_components)),
            (f'{subsystem_name}_component_functions.cpp', self.create_component_functions_implementation(sorted_components))
        ]
        
        for filename, content in files_to_create:
            file_path = subsystem_dir / filename
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            self.log_change("SUBSYSTEM_FILE_CREATED", f"{subsystem_name} ({chip_type}): {filename}")
            
    def create_subsystem_dispatch_files(self, chip_type: str, components: List, subsystem_dir: Path):
        """Create the 4 dispatch table files for a specific subsystem"""
        # Sort components for this chip
        sorted_components = sorted(components, key=lambda x: (
            self.get_component_hierarchy_level(x[0]),
            self.get_bot_specific_order(x[0]) if self.get_component_hierarchy_level(x[0]) == 4 else 0,
            x[0]
        ))
        
        # Determine subsystem name from chip type and path
        subsystem_name = self.get_subsystem_name_from_chip(chip_type, subsystem_dir)
        
        # Generate files with subsystem-specific names
        files_to_create = [
            (f'{subsystem_name}_dispatch_tables.hpp', self.create_dispatch_tables_header(sorted_components)),
            (f'{subsystem_name}_dispatch_tables.cpp', self.create_dispatch_tables_implementation(sorted_components)),
            (f'{subsystem_name}_component_functions.hpp', self.create_component_functions_header(sorted_components)),
            (f'{subsystem_name}_component_functions.cpp', self.create_component_functions_implementation(sorted_components))
        ]
        
        for filename, content in files_to_create:
            file_path = subsystem_dir / filename
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            self.log_change("SUBSYSTEM_FILE_CREATED", f"{chip_type}: {filename}")
            
    def get_subsystem_name_from_chip(self, chip_type: str, subsystem_dir: Path) -> str:
        """Determine the subsystem name based on chip type and directory"""
        # Extract subsystem name from directory path
        dir_name = subsystem_dir.name.lower()
        
        # Map common patterns
        subsystem_map = {
            'esp32_s3': {
                'head': 'head',
                'torso': 'torso',
                'master': 'torso'
            },
            'esp32_c3': {
                'arm_left': 'left_arm',
                'arm_right': 'right_arm', 
                'hand_left': 'left_hand',
                'hand_right': 'right_hand',
                'leg_left': 'left_leg',
                'leg_right': 'right_leg',
                'foot_left': 'left_foot',
                'foot_right': 'right_foot'
            }
        }
        
        # Try to map based on chip type and directory
        if chip_type.lower() in subsystem_map:
            for pattern, name in subsystem_map[chip_type.lower()].items():
                if pattern in dir_name:
                    return name
                    
        # Fallback to directory name
        return dir_name
        
    def validate_all_json_files(self):
        """Main validation function"""
        if self.target_component:
            print(f" Starting improved JSON validation for component: {self.target_component}")
        else:
            print(" Starting improved JSON validation for all components...")
        
        # First, try to find a bot JSON file for tree analysis
        bot_json_files = self.find_bot_json_files()
        
        if bot_json_files:
            print(f"\n=== MULTI-ESP32 TREE ANALYSIS MODE ===")
            
            # Prefer test file if it exists
            test_file = None
            for bot_file in bot_json_files:
                if 'test_multi_esp32' in str(bot_file):
                    test_file = bot_file
                    break
            
            # Use the test file if found, otherwise use the first bot JSON file
            bot_json_path = test_file if test_file else bot_json_files[0]
            print(f"Using bot JSON: {bot_json_path.relative_to(self.root_dir)}")
            
            # Phase 1: Discover subsystems from JSON tree
            discovered_subsystems = self.analyze_json_tree_for_subsystems(bot_json_path)
            
            # Phase 2: Build component trees for each subsystem
            subsystem_trees = self.build_subsystem_component_trees(bot_json_path, discovered_subsystems)
            
            # Phase 3: Generate subsystem-specific files
            self.generate_multi_esp32_files(subsystem_trees)
            
        else:
            print(f"\n=== LEGACY SINGLE-CHIP MODE ===")
            print("No bot JSON files found, falling back to legacy component processing")
            
            # Legacy processing for individual components
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
                        
                        # Determine chip assignment for this component
                        chip_type = self.determine_chip_assignment(component_name, data, json_file)
                    self.chip_assignments[safe_name] = chip_type
                    
                    # Add to chip-specific component list
                    if chip_type not in self.chip_components:
                        self.chip_components[chip_type] = []
                    self.chip_components[chip_type].append((safe_name, hit_count, description, component_name))
                
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
        
        # Generate subsystem-specific dispatch tables for multi-ESP32 distribution
        self.generate_subsystem_dispatch_tables()
        
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
        
        return content
        
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
        
        return content
        
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
        
        return content
        
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
        
        return content
            
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