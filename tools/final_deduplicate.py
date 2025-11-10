"""
Final Deduplication - Delete True Duplicates Outside bot_families
Rule: For each duplicate component name:
  - If any version is in bot_families/, keep ALL bot_families versions
  - Delete all versions NOT in bot_families/
  - Exception: If NONE are in bot_families/, keep the first, delete the rest
"""

import json
from pathlib import Path


def final_deduplicate(registry_path: str = "config/component_registry.json") -> None:
    """
    Find true duplicates and mark non-bot_families versions for deletion.
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
    print("FINAL DEDUPLICATION - DELETE TRUE DUPLICATES OUTSIDE bot_families")
    print("=" * 80)
    
    to_delete = []
    
    for name, entries in sorted(duplicates.items()):
        # Separate bot_families from others
        bot_families_entries = []
        other_entries = []
        
        for idx, entry in entries:
            path = entry["path"]
            if "bot_families" in path:
                bot_families_entries.append((idx, entry))
            else:
                other_entries.append((idx, entry))
        
        # Decision logic
        if bot_families_entries:
            # Has bot_families version(s) - keep all bot_families, delete others
            print(f"\n{name} ({len(entries)} total):")
            for idx, entry in bot_families_entries:
                print(f"  [KEEP] {entry['path']}")
            for idx, entry in other_entries:
                print(f"  [DELETE] {entry['path']}")
                to_delete.append((idx, entry))
        else:
            # No bot_families version - keep first, delete rest
            if entries:
                print(f"\n{name} ({len(entries)} total, NO bot_families):")
                first_idx, first_entry = entries[0]
                print(f"  [KEEP] {first_entry['path']}")
                for idx, entry in entries[1:]:
                    print(f"  [DELETE] {entry['path']}")
                    to_delete.append((idx, entry))
    
    print("\n" + "=" * 80)
    print(f"SUMMARY:")
    print(f"  Total duplicate names: {len(duplicates)}")
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
        with open("tools/final_duplicates_to_delete.json", 'w', encoding='utf-8') as f:
            json.dump(deletion_entries, f, indent=2)
        
        print(f"\n Deletion list saved to: tools/final_duplicates_to_delete.json")
        print(f"  Review it before running cleanup")


if __name__ == "__main__":
    final_deduplicate()
