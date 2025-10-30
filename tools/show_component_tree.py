#!/usr/bin/env python3
"""
show_component_tree.py - Display complete component tree from a given component JSON file

Usage: python show_component_tree.py <component_path>

This script reads a component JSON file and recursively traverses all referenced
components in the "components" arrays, displaying the complete hierarchical tree.

Arguments:
    component_path: Full path to the component JSON file to analyze

Example:
    python tools/show_component_tree.py config/bots/bot_families/goblins/goblin_full.json
"""

import json
import os
import sys
from pathlib import Path


class ComponentTreeWalker:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.visited = set()  # Track visited files to avoid infinite loops

    def load_component(self, component_path):
        """Load and parse a component JSON file."""
        full_path = self.project_root / component_path
        if not full_path.exists():
            return None

        try:
            with open(full_path, 'r', encoding='utf-8-sig') as f:  # Handle UTF-8 BOM
                return json.load(f)
        except (json.JSONDecodeError, IOError) as e:
            print(f"Error loading {component_path}: {e}")
            return None

    def get_component_name(self, component_data):
        """Extract component name from JSON data."""
        if not component_data:
            return "UNKNOWN"

        # Try different possible name fields
        for name_field in ['component_name', 'name', 'hardware_type', 'component_type']:
            if name_field in component_data:
                return component_data[name_field]

        # Fallback to filename
        if 'relative_filename' in component_data:
            return Path(component_data['relative_filename']).stem

        return "UNNAMED"

    def walk_component_tree(self, component_path, depth=0, prefix=""):
        """Recursively walk the component tree and display it."""
        indent = "  " * depth
        component_data = self.load_component(component_path)

        if not component_data:
            print(f"{indent}{prefix} {component_path} (NOT FOUND)")
            return

        component_name = self.get_component_name(component_data)

        # Check for cycles
        if component_path in self.visited:
            print(f"{indent}{prefix} {component_name} ({component_path}) [CYCLE DETECTED]")
            return

        self.visited.add(component_path)

        # Display current component
        component_type = component_data.get('component_type', 
                          component_data.get('type', 
                          component_data.get('hardware_type', 
                          component_data.get('subsystem_type', 'UNKNOWN'))))
        print(f"{indent}{prefix} {component_name} ({component_type})")

        # Process components array (only if it's actually an array of strings)
        if 'components' in component_data and isinstance(component_data['components'], list):
            for child_path in component_data['components']:
                if isinstance(child_path, str):  # Only process string paths
                    self.walk_component_tree(child_path, depth + 1, "  ")

        # Also check for nested components in software/hardware sections
        for nested_section in ['software', 'hardware', 'interfaces']:
            if nested_section in component_data and isinstance(component_data[nested_section], dict):
                nested_data = component_data[nested_section]
                if 'components' in nested_data and isinstance(nested_data['components'], list):
                    for child_path in nested_data['components']:
                        if isinstance(child_path, str):  # Only process string paths
                            self.walk_component_tree(child_path, depth + 1, "  ")

        self.visited.remove(component_path)


def main():
    if len(sys.argv) != 2:
        print("Usage: python show_component_tree.py <component_path>")
        print("Example: python tools/show_component_tree.py config/bots/bot_families/goblins/goblin_full.json")
        sys.exit(1)

    component_path = sys.argv[1]

    # Determine project root (assuming script is in tools/ subdirectory)
    script_dir = Path(__file__).parent
    if script_dir.name == 'tools':
        project_root = script_dir.parent
    else:
        project_root = script_dir

    print(f" Analyzing component tree starting from: {component_path}")
    print(f" Project root: {project_root}")
    print()

    walker = ComponentTreeWalker(project_root)
    walker.walk_component_tree(component_path)

    print()
    print(" Component tree analysis complete")


if __name__ == "__main__":
    main()
