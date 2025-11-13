#!/usr/bin/env python3
"""
Component Registration Tool
Validates component files, registers to component registry, and commits to git.

Usage:
    python tools/register_component.py <component_name> [--path <component_path>]

Example:
    python tools/register_component.py goblin_left_ear
    python tools/register_component.py my_sensor --path config/components/hardware/my_sensor.json
"""

import os
import sys
import json
import subprocess
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
CONFIG_ROOT = PROJECT_ROOT / "config"
COMPONENT_REGISTRY = PROJECT_ROOT / "config" / "component_registry.json"

def find_component_files(component_name, custom_path=None):
    """
    Find component .json, .src, and .hdr files.
    Returns: (json_path, src_path, hdr_path, is_controller)
    """
    if custom_path:
        json_path = Path(custom_path)
        if not json_path.exists():
            return None, None, None, False
    else:
        # Search common locations
        search_paths = [
            CONFIG_ROOT / "components" / "hardware",
            CONFIG_ROOT / "components" / "drivers",
            CONFIG_ROOT / "components" / "behaviors",
            CONFIG_ROOT / "components" / "functional",
            CONFIG_ROOT / "components" / "interfaces",
            CONFIG_ROOT / "bots" / "bot_families",
        ]
        
        json_path = None
        for base_path in search_paths:
            for candidate in base_path.rglob(f"{component_name}.json"):
                json_path = candidate
                break
            if json_path:
                break
        
        if not json_path:
            print(f"❌ ERROR: Could not find {component_name}.json in any standard location")
            return None, None, None, False
    
    # Load JSON to check for controller field
    with open(json_path, 'r') as f:
        component_data = json.load(f)
    
    is_controller = "controller" in component_data
    
    # Find .src and .hdr files
    base_dir = json_path.parent
    base_name = json_path.stem
    
    src_path = base_dir / f"{base_name}.src"
    hdr_path = base_dir / f"{base_name}.hdr"
    
    # Controllers don't need .src/.hdr files
    if is_controller:
        return json_path, None, None, True
    
    # Regular components must have .src and .hdr
    if not src_path.exists():
        print(f"❌ ERROR: Missing {src_path}")
        return None, None, None, False
    
    if not hdr_path.exists():
        print(f"❌ ERROR: Missing {hdr_path}")
        return None, None, None, False
    
    return json_path, src_path, hdr_path, False


def validate_component_dependencies(json_path):
    """
    Validate that all components in the components:[] array exist.
    """
    with open(json_path, 'r') as f:
        data = json.load(f)
    
    if "components" not in data:
        return True  # No dependencies to validate
    
    missing = []
    for dep_name in data["components"]:
        dep_json, dep_src, dep_hdr, dep_is_controller = find_component_files(dep_name)
        if not dep_json:
            missing.append(dep_name)
    
    if missing:
        print(f"❌ ERROR: Missing component dependencies: {', '.join(missing)}")
        return False
    
    return True


def register_to_component_registry(component_name, json_path):
    """
    Add component to component_registry.json
    """
    if not COMPONENT_REGISTRY.exists():
        registry = {
            "version": "1.0.0",
            "description": "Master component registry - maps component names to paths. Sorted alphabetically by name. NO duplicates.",
            "components": []
        }
    else:
        with open(COMPONENT_REGISTRY, 'r') as f:
            registry = json.load(f)
    
    if "components" not in registry:
        registry["components"] = []
    
    # Get relative path from project root
    rel_path = json_path.relative_to(PROJECT_ROOT).as_posix()
    
    # Check if component already exists
    existing_idx = None
    for idx, comp in enumerate(registry["components"]):
        if comp["name"] == component_name:
            existing_idx = idx
            break
    
    component_entry = {
        "name": component_name,
        "path": rel_path
    }
    
    if existing_idx is not None:
        # Update existing entry
        registry["components"][existing_idx] = component_entry
        print(f"✓ Updated {component_name} in component registry")
    else:
        # Add new entry
        registry["components"].append(component_entry)
        # Sort alphabetically by name
        registry["components"].sort(key=lambda x: x["name"])
        print(f"✓ Registered {component_name} in component registry")
    
    # Update timestamp
    from datetime import datetime
    registry["last_updated"] = datetime.now().strftime("%B %d, %Y at %I:%M %p")
    
    with open(COMPONENT_REGISTRY, 'w') as f:
        json.dump(registry, f, indent=2)



def git_add_and_commit(component_name, files):
    """
    Add files to git and commit
    """
    try:
        # Add files
        for file_path in files:
            if file_path:
                subprocess.run(["git", "add", str(file_path)], 
                             cwd=PROJECT_ROOT, check=True)
        
        # Add registry
        subprocess.run(["git", "add", str(COMPONENT_REGISTRY)], 
                      cwd=PROJECT_ROOT, check=True)
        
        # Commit
        commit_msg = f"Register component: {component_name}"
        subprocess.run(["git", "commit", "-m", commit_msg], 
                      cwd=PROJECT_ROOT, check=True)
        
        print(f"✓ Committed {component_name} to repository")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"❌ Git error: {e}")
        return False


def main():
    if len(sys.argv) < 2:
        print("Usage: python tools/register_component.py <component_name> [--path <path>]")
        sys.exit(1)
    
    component_name = sys.argv[1]
    custom_path = None
    
    if "--path" in sys.argv:
        path_idx = sys.argv.index("--path")
        if path_idx + 1 < len(sys.argv):
            custom_path = sys.argv[path_idx + 1]
    
    print(f"🔍 Validating component: {component_name}")
    
    # Find component files
    json_path, src_path, hdr_path, is_controller = find_component_files(component_name, custom_path)
    
    if not json_path:
        sys.exit(1)
    
    print(f"✓ Found {json_path}")
    if src_path:
        print(f"✓ Found {src_path}")
    if hdr_path:
        print(f"✓ Found {hdr_path}")
    if is_controller:
        print(f"ℹ Component is a controller (no .src/.hdr required)")
    
    # Validate dependencies
    print(f"🔍 Validating component dependencies...")
    if not validate_component_dependencies(json_path):
        sys.exit(1)
    
    print(f"✓ All dependencies valid")
    
    # Register to component registry
    register_to_component_registry(component_name, json_path)
    
    # Git add and commit
    files = [json_path, src_path, hdr_path]
    if git_add_and_commit(component_name, files):
        print(f"\n✅ Component {component_name} successfully registered and committed!")
    else:
        print(f"\n⚠️ Component registered but git commit failed")
        sys.exit(1)


if __name__ == "__main__":
    main()
