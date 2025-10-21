#!/usr/bin/env python3
"""
IMPROVED JSON Path Validation and Component Compliance Script
Fixes JSON encoding issues, creates missing components, handles file names properly
"""

import os
import json
import re
from pathlib import Path
from typing import Dict, List, Set, Tuple

class ImprovedJsonValidator:
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir)
        self.exclude_patterns = ['.venv', 'node_modules', '__pycache__', '.git']
        self.missing_files = []
        self.invalid_json_files = []
        self.components_to_create = []
        self.json_files_fixed = []
        self.changes_log = []
        self.created_components = set()  # Track to avoid duplicates
        
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
        """Convert component name to valid file name"""
        # Replace spaces and special chars with underscores
        sanitized = re.sub(r'[^a-zA-Z0-9_]', '_', name)
        # Remove multiple consecutive underscores
        sanitized = re.sub(r'_+', '_', sanitized)
        # Remove leading/trailing underscores
        sanitized = sanitized.strip('_')
        # Convert to lowercase
        return sanitized.lower()
        
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
#include "core/gsm.hpp"

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
#include "core/gsm.hpp"
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
        print(" Starting improved JSON validation...")
        
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
                
                # Avoid duplicates
                if safe_name in self.created_components:
                    print(f"    Skipping duplicate component: {component_name} (already processed as {safe_name})")
                    continue
                    
                files_needed = self.determine_component_files_needed(component_name, json_file, data)
                
                for file_path, file_type, sanitized_name in files_needed:
                    full_path = self.root_dir / file_path
                    if not full_path.exists():
                        self.components_to_create.append((component_name, file_path, file_type, data, sanitized_name))
                        print(f"   Need to create: {file_path}")
                        
                self.created_components.add(safe_name)
                        
    def execute_fixes(self):
        """Execute all fixes and create missing files"""
        print(f"\n  Executing fixes...")
        
        # Create missing component files
        for component_name, file_path, file_type, json_data, sanitized_name in self.components_to_create:
            full_path = self.root_dir / file_path
            full_path.parent.mkdir(parents=True, exist_ok=True)
            
            content = self.create_component_template(component_name, file_type, json_data)
            
            with open(full_path, 'w', encoding='utf-8') as f:
                f.write(content)
                
            self.log_change("COMPONENT_CREATED", f"{file_path} ({file_type}) for '{component_name}' as '{sanitized_name}'")
            
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

if __name__ == "__main__":
    validator = ImprovedJsonValidator("f:/GitHub/p32-animatronic-bot")
    validator.validate_all_json_files()
    validator.execute_fixes()
    validator.generate_report()