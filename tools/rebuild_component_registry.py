"""
Rebuild Component Registry - No Duplicates
Scans config/ directory for all component triplets (.json/.src/.hdr)
Creates fresh registry with one entry per component name.
"""

import json
from pathlib import Path
from datetime import datetime
from typing import Dict, Set

def find_all_components(config_root: Path) -> Dict[str, Path]:
    """
    Find all component .json files in config/ tree.
    Returns dict mapping component name to path (no duplicates).
    If duplicate names found, keeps the first one encountered.
    """
    components = {}
    seen_names = set()
    
    # Recursively find all .json files
    for json_path in sorted(config_root.rglob("*.json")):
        # Skip archived components
        if "archived" in str(json_path).lower():
            continue
            
        try:
            with open(json_path, 'r', encoding='ascii') as f:
                data = json.load(f)
                
            # Check if this is a component (has a name field)
            name = data.get("name")
            if not name:
                continue
            
            # Handle duplicates - keep first occurrence
            if name in seen_names:
                print(f"WARNING: Duplicate '{name}' found at {json_path}, skipping (already have one)")
                continue
                
            seen_names.add(name)
            components[name] = json_path
            
        except (json.JSONDecodeError, UnicodeDecodeError) as e:
            print(f"Skipping {json_path}: {e}")
            continue
    
    return components


def build_registry(config_root: Path, output_path: Path, dry_run: bool = False) -> None:
    """Build fresh component registry with no duplicates."""
    print("Scanning for components...")
    components = find_all_components(config_root)
    
    print(f"\nFound {len(components)} unique components")
    
    if dry_run:
        print("\n=== DRY RUN MODE - NOT SAVING ===")
        print(f"Would create registry with {len(components)} components")
        return
    
    # Sort by name
    sorted_names = sorted(components.keys())
    
    # Build registry entries
    entries = []
    for name in sorted_names:
        json_path = components[name]
        # Convert to relative path from project root
        try:
            relative_path = json_path.relative_to(config_root.parent)
            path_str = str(relative_path).replace("/", "\\")
        except ValueError:
            path_str = str(json_path)
        
        entries.append({
            "name": name,
            "path": path_str,
            "git_repo": "p32-animatronic-bot"
        })
    
    # Create registry structure
    registry = {
        "version": "1.0.0",
        "description": "Master component registry - maps component names to paths and git repositories. Sorted alphabetically by name. NO duplicates.",
        "last_updated": datetime.now().isoformat(),
        "components": entries
    }
    
    # Save registry
    with open(output_path, 'w', encoding='ascii') as f:
        json.dump(registry, f, indent=2, ensure_ascii=True)
    
    print(f"\nRegistry saved to {output_path}")
    print(f"Total components: {len(entries)}")


if __name__ == "__main__":
    import sys
    
    project_root = Path(__file__).resolve().parents[1]
    config_root = project_root / "config"
    output_path = config_root / "component_registry.json"
    
    dry_run = "--dry-run" in sys.argv
    
    print(f"Project root: {project_root}")
    print(f"Config root: {config_root}")
    print(f"Output: {output_path}")
    if dry_run:
        print("MODE: DRY RUN (no changes will be made)")
    print()
    
    build_registry(config_root, output_path, dry_run=dry_run)
    
    if dry_run:
        print("\nDry run complete. Run without --dry-run to actually create registry.")
    else:
        print("\nDone! Registry rebuilt with no duplicates.")
