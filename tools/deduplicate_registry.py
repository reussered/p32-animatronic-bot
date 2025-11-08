"""
Deduplicate Registry - Keep bot_families, Delete Others
Rule: For each duplicate component name, keep the one in config/bots/bot_families/
      Delete all other versions.
"""

import json
from pathlib import Path


def deduplicate_registry(registry_path: str = "config/component_registry.json") -> None:
    """
    Find all duplicates and mark non-bot_families versions for deletion.
    """
    
    with open(registry_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    components = data["components"]
    
    # Group by name
    by_name = {}
    for i, comp in enumerate(components):
        name = comp["name"]
        if name not in by_name:
            by_name[name] = []
        by_name[name].append((i, comp))
    
    # Find duplicates
    duplicates = {k: v for k, v in by_name.items() if len(v) > 1}
    
    if not duplicates:
        print("No duplicates found")
        return
    
    print("=" * 80)
    print("DEDUPLICATION - KEEP bot_families, DELETE others")
    print("=" * 80)
    
    to_delete = []
    
    for name, entries in sorted(duplicates.items()):
        print(f"\n{name} ({len(entries)} versions):")
        
        # Find which one is in bot_families
        bot_families_entry = None
        other_entries = []
        
        for idx, entry in entries:
            path = entry["path"]
            if "bot_families" in path:
                bot_families_entry = (idx, entry)
                print(f"  [KEEP] {path}")
            else:
                other_entries.append((idx, entry))
                print(f"  [DELETE] {path}")
        
        if not bot_families_entry:
            # No bot_families version - keep first, delete rest
            print("  ⚠ No bot_families version found - keeping first, deleting rest")
            bot_families_entry = other_entries[0]
            other_entries = other_entries[1:]
        
        # Add others to deletion list
        for idx, entry in other_entries:
            to_delete.append((idx, entry))
    
    print("\n" + "=" * 80)
    print(f"SUMMARY:")
    print(f"  Total duplicates: {len(duplicates)}")
    print(f"  Files to delete: {len(to_delete)}")
    print("=" * 80)
    
    if to_delete:
        # Generate deletion list
        deletion_entries = []
        for idx, entry in sorted(to_delete, key=lambda x: x[1]["path"]):
            deletion_entries.append({
                "name": entry["name"],
                "path": entry["path"],
                "index": idx
            })
        
        # Save deletion list
        with open("tools/duplicates_to_delete.json", 'w', encoding='utf-8') as f:
            json.dump(deletion_entries, f, indent=2)
        
        print(f"\n✓ Deletion list saved to: tools/duplicates_to_delete.json")
        print(f"  Review it before running cleanup")


if __name__ == "__main__":
    deduplicate_registry()
