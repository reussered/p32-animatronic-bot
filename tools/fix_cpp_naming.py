#!/usr/bin/env python3
"""
CPP Filename/Function Name Auto-Fixer
Automatically renames CPP files to match their init/act function prefixes
"""

import re
import os
import sys
from pathlib import Path

def extract_function_prefix(cpp_file):
    """Extract the component name from _init() and _act() function definitions"""
    try:
        with open(cpp_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Look for _init() and _act() function definitions (not calls)
        # Match patterns like: esp_err_t component_name_init( or void component_name_act(
        init_pattern = r'(?:esp_err_t|void)\s+(\w+)_init\s*\('
        act_pattern = r'(?:esp_err_t|void)\s+(\w+)_act\s*\('
        
        init_matches = re.findall(init_pattern, content)
        act_matches = re.findall(act_pattern, content)
        
        # Combine and find most common prefix
        all_prefixes = init_matches + act_matches
        
        if all_prefixes:
            # Return the most common prefix (should be the component name)
            return max(set(all_prefixes), key=all_prefixes.count)
        
        return None
        
    except Exception as e:
        print(f"Error reading {cpp_file}: {e}")
        return None

def fix_cpp_naming(src_dir):
    """Fix CPP filenames to match their function prefixes"""
    fixes_made = []
    
    components_dir = Path(src_dir) / "components"
    
    if not components_dir.exists():
        print(f"Directory not found: {components_dir}")
        return []
    
    for cpp_file in components_dir.glob("*.cpp"):
        filename_stem = cpp_file.stem  # filename without .cpp extension
        
        # Extract component name from function names
        component_name = extract_function_prefix(cpp_file)
        
        if component_name and component_name != filename_stem:
            # Create new filename
            new_filename = cpp_file.parent / f"{component_name}.cpp"
            
            # Check if target already exists
            if new_filename.exists():
                print(f"⚠️  Cannot rename {cpp_file} - target {new_filename} already exists")
                continue
            
            # Rename the file
            cpp_file.rename(new_filename)
            fixes_made.append({
                'old': str(cpp_file),
                'new': str(new_filename),
                'component_name': component_name
            })
    
    return fixes_made

def main():
    src_dir = Path(__file__).parent.parent / "src"
    
    print("Auto-fixing CPP filename/function name correspondence...")
    print(f"Scanning: {src_dir}/components")
    print()
    
    fixes = fix_cpp_naming(src_dir)
    
    if not fixes:
        print("✅ No CPP files needed renaming!")
        return 0
    
    print(f"✅ Fixed {len(fixes)} CPP files:")
    print()
    
    for fix in fixes:
        old_name = Path(fix['old']).name
        new_name = Path(fix['new']).name
        print(f"  {old_name} → {new_name}")
    
    print()
    print("All CPP files now match their function prefixes!")
    return 0

if __name__ == "__main__":
    sys.exit(main())