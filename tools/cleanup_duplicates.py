"""
Delete Duplicate Files and Update Registry
Deletes individual files from duplicates_to_delete.json
Does NOT delete directories even if they become empty.
"""

import json
import os
from pathlib import Path


def delete_duplicate_files(deletion_list_path: str = "tools/duplicates_to_delete.json") -> None:
    """Delete files listed in duplicates_to_delete.json"""
    
    with open(deletion_list_path, 'r', encoding='utf-8') as f:
        to_delete = json.load(f)
    
    print("=" * 80)
    print("DELETE DUPLICATE FILES")
    print("=" * 80)
    
    deleted_count = 0
    failed_count = 0
    
    for entry in to_delete:
        path = entry["path"]
        name = entry["name"]
        
        filepath = Path(path)
        
        if not filepath.exists():
            print(f"⊘ Not found: {path}")
            continue
        
        try:
            os.remove(filepath)
            print(f"✓ Deleted: {path}")
            deleted_count += 1
        except Exception as e:
            print(f"✗ Failed: {path} - {e}")
            failed_count += 1
    
    print("\n" + "=" * 80)
    print(f"Deleted: {deleted_count}")
    print(f"Failed:  {failed_count}")
    print("=" * 80)


def update_registry_after_deletion(registry_path: str = "config/component_registry.json",
                                   deletion_list_path: str = "tools/duplicates_to_delete.json") -> None:
    """Remove deleted entries from registry"""
    
    with open(deletion_list_path, 'r', encoding='utf-8') as f:
        to_delete = json.load(f)
    
    with open(registry_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    print("\n" + "=" * 80)
    print("UPDATE REGISTRY - REMOVE DELETED ENTRIES")
    print("=" * 80)
    
    removed_count = 0
    
    for entry_info in to_delete:
        name = entry_info["name"]
        path = entry_info["path"]
        
        # Find and remove from registry
        for i, comp in enumerate(data["components"]):
            if comp["name"] == name and comp["path"] == path:
                del data["components"][i]
                print(f"✓ Removed from registry: {name} @ {path}")
                removed_count += 1
                break
    
    # Save updated registry
    with open(registry_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2)
    
    print("\n" + "=" * 80)
    print(f"Registry updated: {removed_count} entries removed")
    print(f"Total entries now: {len(data['components'])}")
    print("=" * 80)


if __name__ == "__main__":
    delete_duplicate_files()
    update_registry_after_deletion()
