"""
Component Registry Scanner
Scans config/ and assets/ directories to populate the registry.
Finds all .json component files and adds them to the registry.
"""

import json
import sys
from pathlib import Path
import os

# Add tools directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from component_registry import ComponentRegistry


def scan_components(base_dirs: list = None) -> list:
    """Scan directories for .json component files."""
    if base_dirs is None:
        base_dirs = ["config", "assets"]

    components = []

    for base_dir in base_dirs:
        base_path = Path(base_dir)
        if not base_path.exists():
            print(f"Warning: {base_dir} does not exist")
            continue

        print(f"Scanning {base_dir}/...")
        json_files = sorted(base_path.rglob("*.json"))

        for json_file in json_files:
            try:
                # Get component name from filename (without .json)
                component_name = json_file.stem

                # Convert to relative path
                relative_path = str(json_file.relative_to(Path(".")))

                components.append({
                    "name": component_name,
                    "path": relative_path,
                    "git_repo": "p32-animatronic-bot"
                })
            except Exception as e:
                print(f"Error processing {json_file}: {e}")

    return components


def populate_registry(registry: ComponentRegistry, components: list) -> None:
    """Add all components to registry."""
    for comp in components:
        registry.add(comp["name"], comp["path"], comp["git_repo"])


def main():
    print("Component Registry Scanner")
    print("=" * 60)

    # Scan
    print("\nScanning config/ and assets/...")
    components = scan_components(["config", "assets"])

    print(f"Found {len(components)} component files\n")

    # Load registry
    registry = ComponentRegistry("config/component_registry.json")

    # Clear existing entries
    registry.data["components"] = []

    # Add all components
    print("Adding to registry...")
    populate_registry(registry, components)

    # Save
    registry.save()
    print(f"Registry saved to config/component_registry.json")

    # Report
    print("\n" + "=" * 60)
    print("Registry Summary:")
    print(f"Total entries: {len(registry.data['components'])}")

    duplicates = registry.list_duplicates()
    if duplicates:
        print(f"\nDuplicates detected:")
        for name, count in sorted(duplicates.items()):
            print(f"  {name}: {count} entries")
            for entry in registry.get_all(name):
                print(f"    - {entry['path']}")
    else:
        print("\nNo duplicates detected")

    # Show first 10 entries
    print(f"\nFirst 10 entries:")
    for i, comp in enumerate(registry.data["components"][:10]):
        print(f"  {i+1}. {comp['name']}: {comp['path']}")

    if len(registry.data["components"]) > 10:
        print(f"  ... and {len(registry.data['components']) - 10} more")


if __name__ == "__main__":
    main()
