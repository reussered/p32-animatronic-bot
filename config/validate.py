#!/usr/bin/env python3
"""
Configuration Validator for p32-animatronic-bot

Validates that all JSON files in the config directory are:
1. Valid JSON
2. Consistent in structure within their respective folders
"""

import json
import sys
from pathlib import Path
from typing import Dict, Set, List, Tuple


def validate_json_syntax(file_path: Path) -> Tuple[bool, str]:
    """Validate JSON syntax."""
    try:
        with open(file_path, 'r') as f:
            json.load(f)
        return True, ""
    except json.JSONDecodeError as e:
        return False, f"JSON syntax error: {e}"
    except Exception as e:
        return False, f"Error reading file: {e}"


def get_json_keys(file_path: Path) -> Set[str]:
    """Get top-level keys from a JSON file."""
    with open(file_path, 'r') as f:
        data = json.load(f)
    return set(data.keys())


def check_folder_consistency(folder_path: Path) -> Tuple[bool, List[str]]:
    """Check if all JSON files in a folder have consistent structure."""
    json_files = list(folder_path.glob('*.json'))
    
    if not json_files:
        return True, []
    
    # Get reference keys from first file
    reference_file = json_files[0]
    reference_keys = get_json_keys(reference_file)
    
    errors = []
    
    # Check all other files
    for json_file in json_files[1:]:
        current_keys = get_json_keys(json_file)
        
        if current_keys != reference_keys:
            missing = reference_keys - current_keys
            extra = current_keys - reference_keys
            
            error_msg = f"  ✗ {json_file.name} has inconsistent keys"
            if missing:
                error_msg += f"\n    Missing keys: {sorted(missing)}"
            if extra:
                error_msg += f"\n    Extra keys: {sorted(extra)}"
            errors.append(error_msg)
    
    return len(errors) == 0, errors


def main():
    """Main validation function."""
    config_dir = Path(__file__).parent
    
    print("🤖 p32-animatronic-bot Configuration Validator\n")
    print("=" * 60)
    
    # Step 1: Validate JSON syntax
    print("\n📋 Step 1: Validating JSON syntax...")
    syntax_errors = []
    
    for json_file in config_dir.rglob('*.json'):
        is_valid, error = validate_json_syntax(json_file)
        relative_path = json_file.relative_to(config_dir)
        
        if is_valid:
            print(f"  ✓ {relative_path}")
        else:
            print(f"  ✗ {relative_path}")
            syntax_errors.append(f"    {error}")
    
    if syntax_errors:
        print("\n❌ JSON syntax errors found:")
        for error in syntax_errors:
            print(error)
        return 1
    
    print("\n✅ All JSON files have valid syntax")
    
    # Step 2: Check consistency within folders
    print("\n📋 Step 2: Checking consistency within folders...")
    
    folders = {
        'moods': config_dir / 'moods',
        'behaviors': config_dir / 'behaviors',
        'hardware': config_dir / 'hardware'
    }
    
    consistency_errors = []
    
    for folder_name, folder_path in folders.items():
        if not folder_path.exists():
            print(f"  ⚠ {folder_name}/ folder not found")
            continue
        
        is_consistent, errors = check_folder_consistency(folder_path)
        
        if is_consistent:
            json_count = len(list(folder_path.glob('*.json')))
            print(f"  ✓ {folder_name}/ ({json_count} files)")
        else:
            print(f"  ✗ {folder_name}/ has inconsistencies:")
            consistency_errors.extend(errors)
            for error in errors:
                print(error)
    
    if consistency_errors:
        print("\n❌ Consistency errors found")
        return 1
    
    print("\n✅ All folders have consistent structure")
    
    # Summary
    print("\n" + "=" * 60)
    print("✅ All validations passed!")
    print("\nConfiguration structure:")
    
    for folder_name, folder_path in folders.items():
        if folder_path.exists():
            json_files = list(folder_path.glob('*.json'))
            if json_files:
                reference_keys = sorted(get_json_keys(json_files[0]))
                print(f"\n  {folder_name}/ ({len(json_files)} files)")
                print(f"    Schema keys: {', '.join(reference_keys)}")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
