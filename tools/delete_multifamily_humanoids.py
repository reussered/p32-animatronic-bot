"""
Delete multi_family humanoid duplicates and update registry
Keep bot_families versions, delete multi_family versions.
"""

import json
import os
from pathlib import Path


def main():
    # List of multi_family humanoid files to delete
    files_to_delete = [
        "config/bots/multi_family/humanoid/humanoid_ankle_left.json",
        "config/bots/multi_family/humanoid/humanoid_ankle_right.json",
        "config/bots/multi_family/humanoid/humanoid_elbow_left.json",
        "config/bots/multi_family/humanoid/humanoid_elbow_right.json",
        "config/bots/multi_family/humanoid/humanoid_head_assembly.json",
        "config/bots/multi_family/humanoid/humanoid_hip_left.json",
        "config/bots/multi_family/humanoid/humanoid_hip_right.json",
        "config/bots/multi_family/humanoid/humanoid_knee_left.json",
        "config/bots/multi_family/humanoid/humanoid_knee_right.json",
        "config/bots/multi_family/humanoid/humanoid_neck_assembly.json",
        "config/bots/multi_family/humanoid/humanoid_pelvis_assembly.json",
        "config/bots/multi_family/humanoid/humanoid_shoulder_left.json",
        "config/bots/multi_family/humanoid/humanoid_shoulder_right.json",
        "config/bots/multi_family/humanoid/humanoid_spine_vertebrae.json",
        "config/bots/multi_family/humanoid/humanoid_wrist_left.json",
        "config/bots/multi_family/humanoid/humanoid_wrist_right.json",
    ]
    
    print("=" * 80)
    print("DELETE multi_family HUMANOID DUPLICATES")
    print("=" * 80)
    
    deleted_count = 0
    failed_count = 0
    
    for path in files_to_delete:
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
    
    # Update registry
    with open("config/component_registry.json", 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    print("\nUPDATE REGISTRY - REMOVE DELETED ENTRIES")
    print("=" * 80)
    
    removed_count = 0
    for path in files_to_delete:
        # Find and remove from registry
        for i, comp in enumerate(data["components"]):
            if comp["path"] == path:
                name = comp["name"]
                del data["components"][i]
                print(f"✓ Removed: {name} @ {path}")
                removed_count += 1
                break
    
    # Save updated registry
    with open("config/component_registry.json", 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2)
    
    print("\n" + "=" * 80)
    print(f"Registry updated: {removed_count} entries removed")
    print(f"Total entries now: {len(data['components'])}")
    print("=" * 80)


if __name__ == "__main__":
    main()
