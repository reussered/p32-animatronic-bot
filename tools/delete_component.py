#!/usr/bin/env python3
"""
Component Deletion Tool
Removes component from registry, deletes files, and commits to git.

Usage:
    python tools/delete_component.py <component_name> [--keep-files]

Example:
    python tools/delete_component.py goblin_left_ear
    python tools/delete_component.py my_sensor --keep-files  # Remove from registry but keep files
"""

import os
import sys
import json
import subprocess
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
CONFIG_ROOT = PROJECT_ROOT / "config"
COMPONENT_REGISTRY = PROJECT_ROOT / "config" / "component_registry.json"

def binary_search_component(registry, component_name):
    """
    Binary search on alphabetically sorted component registry.
    O(log n) complexity - much faster than linear search.
    """
    components = registry.get("components", [])
    
    left = 0
    right = len(components) - 1
    
    while left <= right:
        mid = (left + right) // 2
        mid_name = components[mid]["name"]
        
        if mid_name == component_name:
            return components[mid]["path"]
        elif mid_name < component_name:
            left = mid + 1
        else:
            right = mid - 1
    
    return None


def find_component_files(component_name):
    """
    Find component .json, .src, and .hdr files.
    Uses binary search on component registry for O(log n) lookup.
    Returns: (json_path, src_path, hdr_path)
    """
    # Use binary search on registry for fast lookup
    if COMPONENT_REGISTRY.exists():
        with open(COMPONENT_REGISTRY, 'r') as f:
            registry = json.load(f)
        
        rel_path = binary_search_component(registry, component_name)
        
        if rel_path:
            json_path = PROJECT_ROOT / rel_path
            if not json_path.exists():
                json_path = None
        else:
            json_path = None
    else:
        json_path = None
    
    if not json_path:
        print(f"⚠️ WARNING: Could not find {component_name} in component registry")
        return None, None, None
    
    # Find .src and .hdr files
    base_dir = json_path.parent
    base_name = json_path.stem
    
    src_path = base_dir / f"{base_name}.src"
    hdr_path = base_dir / f"{base_name}.hdr"
    
    # These may not exist for controller components
    if not src_path.exists():
        src_path = None
    if not hdr_path.exists():
        hdr_path = None
    
    return json_path, src_path, hdr_path


def check_component_usage(component_name):
    """
    Check if component is still referenced in any bot configurations.
    """
    used_in = []
    
    # Search all .json files for component references
    for json_file in CONFIG_ROOT.rglob("*.json"):
        try:
            with open(json_file, 'r') as f:
                data = json.load(f)
            
            # Check components array
            if "components" in data and component_name in data["components"]:
                used_in.append(json_file)
        except (json.JSONDecodeError, Exception):
            continue
    
    return used_in


def remove_from_component_registry(component_name):
    """
    Remove component from component_registry.json
    """
    if not COMPONENT_REGISTRY.exists():
        print(f"⚠️ WARNING: Component registry does not exist")
        return False
    
    with open(COMPONENT_REGISTRY, 'r') as f:
        registry = json.load(f)
    
    if "components" not in registry:
        print(f"⚠️ WARNING: Component registry has no components array")
        return False
    
    # Find component in array
    found_idx = None
    for idx, comp in enumerate(registry["components"]):
        if comp["name"] == component_name:
            found_idx = idx
            break
    
    if found_idx is None:
        print(f"⚠️ WARNING: Component {component_name} not found in registry")
        return False
    
    # Remove component
    del registry["components"][found_idx]
    
    # Update timestamp
    from datetime import datetime
    registry["last_updated"] = datetime.now().strftime("%B %d, %Y at %I:%M %p")
    
    with open(COMPONENT_REGISTRY, 'w') as f:
        json.dump(registry, f, indent=2)
    
    print(f"✓ Removed {component_name} from component registry")
    return True


def save_component_files(component_name, files):
    """
    Move component files to delete-saved folder with -saved extensions
    """
    saved_dir = CONFIG_ROOT / "delete-saved"
    saved_dir.mkdir(exist_ok=True)
    
    saved_files = []
    for file_path in files:
        if file_path and file_path.exists():
            # Get new filename with -saved extension
            if file_path.suffix == ".json":
                new_name = f"{file_path.stem}.json-saved"
            elif file_path.suffix == ".src":
                new_name = f"{file_path.stem}.src-saved"
            elif file_path.suffix == ".hdr":
                new_name = f"{file_path.stem}.hdr-saved"
            else:
                continue
            
            # Move file to saved directory
            saved_path = saved_dir / new_name
            import shutil
            shutil.copy2(file_path, saved_path)
            saved_files.append(saved_path)
            print(f"✓ Saved to {saved_path}")
    
    return saved_files


def git_rm_and_commit(component_name, files, keep_files=False):
    """
    Remove files from git and commit
    """
    try:
        if not keep_files:
            # Save component files first
            saved_files = save_component_files(component_name, files)
            
            # Remove original files from git
            for file_path in files:
                if file_path and file_path.exists():
                    subprocess.run(["git", "rm", str(file_path)], 
                                 cwd=PROJECT_ROOT, check=True)
                    print(f"✓ Removed {file_path} from git")
            
            # Add saved files to git
            for saved_path in saved_files:
                subprocess.run(["git", "add", str(saved_path)], 
                             cwd=PROJECT_ROOT, check=True)
            print(f"✓ Added saved files to git")
        
        # Update registry
        subprocess.run(["git", "add", str(COMPONENT_REGISTRY)], 
                      cwd=PROJECT_ROOT, check=True)
        
        # Commit
        action = "Unregister" if keep_files else "Archive"
        commit_msg = f"{action} component: {component_name}"
        subprocess.run(["git", "commit", "-m", commit_msg], 
                      cwd=PROJECT_ROOT, check=True)
        
        print(f"✓ Committed changes to repository")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Git error: {e}")
        return False


def main():
    if len(sys.argv) < 2:
        print("Usage: python tools/delete_component.py <component_name> [--keep-files]")
        sys.exit(1)
    
    component_name = sys.argv[1]
    keep_files = "--keep-files" in sys.argv
    
    print(f"🔍 Searching for component: {component_name}")
    
    # Find component files
    json_path, src_path, hdr_path = find_component_files(component_name)
    
    if not json_path:
        print(f"❌ ERROR: Component {component_name} not found")
        sys.exit(1)
    
    print(f"✓ Found {json_path}")
    if src_path:
        print(f"✓ Found {src_path}")
    if hdr_path:
        print(f"✓ Found {hdr_path}")
    
    # Check if component is still in use
    print(f"🔍 Checking component usage...")
    used_in = check_component_usage(component_name)
    
    if used_in:
        print(f"\n❌ ERROR: Component {component_name} is still referenced in:")
        for usage_file in used_in:
            rel_path = usage_file.relative_to(PROJECT_ROOT)
            print(f"  - {rel_path}")
        print(f"\nPlease remove these references first.")
        sys.exit(1)
    
    print(f"✓ Component not referenced in any configurations")
    
    # Confirm deletion
    if not keep_files:
        print(f"\n⚠️  WARNING: This will ARCHIVE the following files to config/delete-saved/:")
        print(f"  - {json_path} → {json_path.stem}.json-saved")
        if src_path:
            print(f"  - {src_path} → {src_path.stem}.src-saved")
        if hdr_path:
            print(f"  - {hdr_path} → {hdr_path.stem}.hdr-saved")
        print(f"\nOriginal files will be removed from their current location.")
        
        response = input("\nType 'ARCHIVE' to confirm: ")
        if response != "ARCHIVE":
            print("Cancelled.")
            sys.exit(0)
    
    # Remove from registry
    remove_from_component_registry(component_name)
    
    # Git remove and commit
    files = [json_path, src_path, hdr_path]
    if git_rm_and_commit(component_name, files, keep_files):
        if keep_files:
            print(f"\n✅ Component {component_name} unregistered (files kept)")
        else:
            print(f"\n✅ Component {component_name} archived to config/delete-saved/ and changes committed!")
    else:
        print(f"\n⚠️ Component removed but git commit failed")
        sys.exit(1)


if __name__ == "__main__":
    main()
