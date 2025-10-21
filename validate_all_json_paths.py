#!/usr/bin/env python3
"""
Complete JSON Path Validation and Component Compliance Script
Validates all JSON files, creates missing components, renames .c/.h to .cpp/.hpp
"""

import os
import json
import re
from pathlib import Path
from typing import Dict, List, Set, Tuple

class JsonValidator:
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir)
        self.missing_files = []
        self.invalid_json_files = []
        self.components_to_create = []
        self.files_to_rename = []
        self.changes_log = []
        
    def log_change(self, change_type: str, details: str):
        """Track all changes made"""
        self.changes_log.append(f"{change_type}: {details}")
        print(f" {change_type}: {details}")
        
    def find_all_json_files(self) -> List[Path]:
        """Find all JSON files in the project"""
        json_files = list(self.root_dir.rglob("*.json"))
        print(f"Found {len(json_files)} JSON files")
        return json_files
        
    def validate_json_file(self, json_path: Path) -> Tuple[bool, Dict]:
        """Validate a JSON file and return success status and content"""
        try:
            with open(json_path, 'r', encoding='utf-8') as f:
                content = json.load(f)
            return True, content
        except Exception as e:
            self.invalid_json_files.append((json_path, str(e)))
            print(f" Invalid JSON: {json_path} - {e}")
            return False, {}
            
    def check_file_reference(self, ref_path: str, base_dir: Path) -> bool:
        """Check if a referenced file exists, handle relative paths"""
        if not ref_path:
            return True  # Empty references are ok
            
        # Handle relative paths
        if ref_path.startswith('./') or ref_path.startswith('../'):
            full_path = (base_dir / ref_path).resolve()
        elif ref_path.startswith('/'):
            full_path = self.root_dir / ref_path.lstrip('/')
        else:
            # Try both relative to base_dir and relative to root
            rel_to_base = base_dir / ref_path
            rel_to_root = self.root_dir / ref_path
            if rel_to_base.exists():
                return True
            elif rel_to_root.exists():
                return True
            else:
                full_path = rel_to_base  # Use this for error reporting
                
        return full_path.exists()
        
    def extract_file_references(self, data: Dict, current_file: Path) -> List[str]:
        """Extract all file references from JSON data"""
        references = []
        
        def extract_refs(obj, path=""):
            if isinstance(obj, dict):
                for key, value in obj.items():
                    if key in ['source_file', 'header_file', 'implementation_file', 'shape_file', 'animation_file', 'sound_file']:
                        if isinstance(value, str) and value:
                            references.append(value)
                    elif key in ['contains', 'implements', 'extends']:
                        if isinstance(value, list):
                            for item in value:
                                if isinstance(item, str) and item.endswith('.json'):
                                    references.append(item)
                        elif isinstance(value, str) and value.endswith('.json'):
                            references.append(value)
                    elif isinstance(value, (dict, list)):
                        extract_refs(value, f"{path}.{key}" if path else key)
            elif isinstance(obj, list):
                for i, item in enumerate(obj):
                    if isinstance(item, (dict, list)):
                        extract_refs(item, f"{path}[{i}]" if path else f"[{i}]")
                        
        extract_refs(data)
        return references
        
    def determine_component_files_needed(self, component_name: str, json_path: Path, json_data: Dict) -> List[Tuple[str, str]]:
        """Determine what .cpp/.hpp files should exist for a component"""
        files_needed = []
        
        # Check if this has hardware_only flag
        hardware_only = json_data.get('hardware_only', False)
        
        if not hardware_only:
            # Regular component needs .cpp and .hpp files
            cpp_file = f"src/components/{component_name}.cpp"
            hpp_file = f"include/components/{component_name}.hpp"
            files_needed.append((cpp_file, 'implementation'))
            files_needed.append((hpp_file, 'header'))
            
        return files_needed
        
    def find_files_to_rename(self) -> List[Tuple[Path, Path]]:
        """Find .c/.h files that should be renamed to .cpp/.hpp"""
        renames = []
        
        # Check src/ directory for .c files
        for c_file in self.root_dir.rglob("src/**/*.c"):
            cpp_file = c_file.with_suffix('.cpp')
            renames.append((c_file, cpp_file))
            
        # Check include/ directory for .h files  
        for h_file in self.root_dir.rglob("include/**/*.h"):
            hpp_file = h_file.with_suffix('.hpp')
            renames.append((h_file, hpp_file))
            
        return renames
        
    def create_component_template(self, component_name: str, file_type: str, json_data: Dict) -> str:
        """Create template content for component files"""
        
        if file_type == 'header':
            return f'''#ifndef {component_name.upper()}_HPP
#define {component_name.upper()}_HPP

/**
 * @file {component_name}.hpp
 * @brief {json_data.get('description', f'{component_name} component')}
 * @author Auto-generated from JSON specification
 */

#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"

// Component-specific includes would go here

/**
 * @brief Initialize {component_name} component
 * Called once during system startup
 */
void {component_name}_init(void);

/**
 * @brief Execute {component_name} component logic
 * Called every loop iteration based on hitCount
 */
void {component_name}_act(void);

#endif // {component_name.upper()}_HPP
'''
        elif file_type == 'implementation':
            return f'''/**
 * @file {component_name}.cpp
 * @brief {json_data.get('description', f'{component_name} component implementation')}
 * @author Auto-generated from JSON specification
 */

#include "components/{component_name}.hpp"
#include "core/memory/SharedMemory.hpp"
#include "core/gsm.hpp"
#include "p32_shared_state.h"

// External GSM instance
extern SharedMemory GSM;

/**
 * @brief Initialize {component_name} component
 * Called once during system startup
 */
void {component_name}_init(void) {{
    // Component initialization logic
    // Access global state via g_loopCount and shared state via GSM
    
    // Example: Read shared memory
    // auto shared_data = GSM.read<SomeSharedDataType>();
    
    // TODO: Implement component-specific initialization
}}

/**
 * @brief Execute {component_name} component logic  
 * Called every loop iteration based on hitCount: {json_data.get('hitCount', 1)}
 */
void {component_name}_act(void) {{
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
        print(" Starting comprehensive JSON validation...")
        
        json_files = self.find_all_json_files()
        
        for json_file in json_files:
            print(f"\n Processing: {json_file.relative_to(self.root_dir)}")
            
            # Validate JSON syntax
            is_valid, data = self.validate_json_file(json_file)
            if not is_valid:
                continue
                
            # Extract and check file references
            references = self.extract_file_references(data, json_file.parent)
            
            for ref in references:
                if not self.check_file_reference(ref, json_file.parent):
                    self.missing_files.append((json_file, ref))
                    print(f"   Missing: {ref}")
                    
            # Check if this is a component definition
            if 'component_name' in data:
                component_name = data['component_name']
                files_needed = self.determine_component_files_needed(component_name, json_file, data)
                
                for file_path, file_type in files_needed:
                    full_path = self.root_dir / file_path
                    if not full_path.exists():
                        self.components_to_create.append((component_name, file_path, file_type, data))
                        print(f"   Need to create: {file_path}")
                        
        # Find files to rename
        print(f"\n Checking for .c/.h files to rename...")
        renames = self.find_files_to_rename()
        self.files_to_rename = renames
        
        for old_path, new_path in renames:
            print(f"   Rename: {old_path.relative_to(self.root_dir)}  {new_path.relative_to(self.root_dir)}")
            
    def execute_fixes(self):
        """Execute all fixes and create missing files"""
        print(f"\n  Executing fixes...")
        
        # Rename .c/.h files to .cpp/.hpp
        for old_path, new_path in self.files_to_rename:
            if old_path.exists():
                new_path.parent.mkdir(parents=True, exist_ok=True)
                old_path.rename(new_path)
                self.log_change("FILE_RENAMED", f"{old_path.relative_to(self.root_dir)}  {new_path.relative_to(self.root_dir)}")
                
        # Create missing component files
        for component_name, file_path, file_type, json_data in self.components_to_create:
            full_path = self.root_dir / file_path
            full_path.parent.mkdir(parents=True, exist_ok=True)
            
            content = self.create_component_template(component_name, file_type, json_data)
            
            with open(full_path, 'w', encoding='utf-8') as f:
                f.write(content)
                
            self.log_change("COMPONENT_CREATED", f"{file_path} ({file_type}) for {component_name}")
            
    def generate_report(self):
        """Generate final report"""
        print(f"\n VALIDATION REPORT")
        print(f"=" * 50)
        print(f"Invalid JSON files: {len(self.invalid_json_files)}")
        print(f"Missing file references: {len(self.missing_files)}")
        print(f"Components created: {len(self.components_to_create)}")
        print(f"Files renamed: {len(self.files_to_rename)}")
        print(f"Total changes: {len(self.changes_log)}")
        
        if self.changes_log:
            print(f"\n CHANGES MADE:")
            for change in self.changes_log:
                print(f"  {change}")
                
        if self.invalid_json_files:
            print(f"\n INVALID JSON FILES:")
            for file_path, error in self.invalid_json_files:
                print(f"  {file_path.relative_to(self.root_dir)}: {error}")
                
        if self.missing_files:
            print(f"\n MISSING REFERENCES:")
            for json_file, ref in self.missing_files:
                print(f"  {json_file.relative_to(self.root_dir)}  {ref}")

if __name__ == "__main__":
    validator = JsonValidator("f:/GitHub/p32-animatronic-bot")
    validator.validate_all_json_files()
    validator.execute_fixes()
    validator.generate_report()