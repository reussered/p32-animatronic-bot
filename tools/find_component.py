#!/usr/bin/env python3
"""
Fast Component Finder
Uses binary search on the sorted component registry to find component paths.

Usage:
    python tools/find_component.py <component_name>

Example:
    python tools/find_component.py goblin_left_ear
    # Output: config/bots/bot_families/goblins/head/goblin_left_ear.json
"""

import sys
import json
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
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


def find_component(component_name):
    """
    Find component path using binary search on registry.
    Returns full absolute path to component .json file.
    """
    if not COMPONENT_REGISTRY.exists():
        print(f"ERROR: Component registry not found at {COMPONENT_REGISTRY}", file=sys.stderr)
        return None
    
    with open(COMPONENT_REGISTRY, 'r') as f:
        registry = json.load(f)
    
    rel_path = binary_search_component(registry, component_name)
    
    if rel_path:
        full_path = PROJECT_ROOT / rel_path
        return full_path
    
    return None


def main():
    if len(sys.argv) < 2:
        print("Usage: python tools/find_component.py <component_name>", file=sys.stderr)
        sys.exit(1)
    
    component_name = sys.argv[1]
    
    path = find_component(component_name)
    
    if path:
        # Output just the path for easy piping/scripting
        print(path)
        sys.exit(0)
    else:
        print(f"ERROR: Component '{component_name}' not found in registry", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
