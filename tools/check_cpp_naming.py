#!/usr/bin/env python3
"""
CPP Filename/Function Name Correspondence Checker
Ensures CPP filenames match their init/act function prefixes
"""

import re
import os
import sys
from pathlib import Path

def extract_function_prefix(cpp_file):
    """Extract the component name from _init() and _act() function names"""
    try:
        with open(cpp_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Look for _init() and _act() function definitions
        init_pattern = r'(\w+)_init\s*\('
        act_pattern = r'(\w+)_act\s*\('
        
        init_matches = re.findall(init_pattern, content)
        act_matches = re.findall(act_pattern, content)
        
        # Find common prefixes (component names)
        prefixes = []
        
        for match in init_matches:
            if match.endswith('_init'):
                # Remove _init suffix to get component name
                prefix = match[:-5]
            else:
                prefix = match
            prefixes.append(prefix)
        
        for match in act_matches:
            if match.endswith('_act'):
                # Remove _act suffix to get component name
                prefix = match[:-4]
            else:
                prefix = match
            prefixes.append(prefix)
        
        # Return the most common prefix (should be the component name)
        if prefixes:
            return max(set(prefixes), key=prefixes.count)
        
        return None
        
    except Exception as e:
        print(f"Error reading {cpp_file}: {e}")
        return None

def check_cpp_naming(src_dir):
    """Check that CPP filenames match their function prefixes"""
    mismatches = []
    
    components_dir = Path(src_dir) / "components"
    
    if not components_dir.exists():
        print(f"Directory not found: {components_dir}")
        return []
    
    for cpp_file in components_dir.glob("*.cpp"):
        filename_stem = cpp_file.stem  # filename without .cpp extension
        
        # Extract component name from function names
        component_name = extract_function_prefix(cpp_file)
        
        if component_name and component_name != filename_stem:
            mismatches.append({
                'file': cpp_file,
                'filename_stem': filename_stem,
                'component_name': component_name,
                'suggested_filename': f"{component_name}.cpp"
            })
    
    return mismatches

def main():
    src_dir = Path(__file__).parent.parent / "src"
    
    print("Checking CPP filename/function name correspondence...")
    print(f"Scanning: {src_dir}/components")
    print()
    
    mismatches = check_cpp_naming(src_dir)
    
    if not mismatches:
        print("✅ All CPP files have matching filenames and function prefixes!")
        return 0
    
    print(f"❌ Found {len(mismatches)} mismatches:")
    print()
    
    for mismatch in mismatches:
        print(f"File: {mismatch['file']}")
        print(f"  Filename: {mismatch['filename_stem']}.cpp")
        print(f"  Function prefix: {mismatch['component_name']}")
        print(f"  Should be: {mismatch['suggested_filename']}")
        print()
    
    return len(mismatches)

if __name__ == "__main__":
    sys.exit(main())