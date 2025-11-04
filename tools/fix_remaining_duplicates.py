#!/usr/bin/env python3
"""
Fix remaining duplicate filenames:
1. Rename left/right arm components to include side in filename
2. Consolidate goblin head components to single location
"""

import json
from pathlib import Path
import sys
import shutil

root = Path("/home/runner/work/p32-animatronic-bot/p32-animatronic-bot")
bot_families_dir = root / "config" / "bots" / "bot_families"

def fix_arm_components():
    """Rename arm component files to include left/right in filename."""
    print("=== Fixing Arm Component Names ===\n")
    
    left_arm_dir = bot_families_dir / "goblins" / "head" / "goblin" / "arm_components" / "left"
    right_arm_dir = bot_families_dir / "goblins" / "head" / "goblin" / "arm_components" / "right"
    
    renamed_files = []
    
    # Process left arm components
    if left_arm_dir.exists():
        for json_file in left_arm_dir.glob("*.json"):
            name = json_file.stem
            if not name.startswith("left_"):
                new_name = f"left_{name}.json"
                new_path = json_file.parent / new_name
                print(f"Renaming: {json_file.relative_to(root)}")
                print(f"      to: {new_path.relative_to(root)}")
                
                # Update relative_filename inside the file
                try:
                    with open(json_file, 'r', encoding='utf-8') as f:
                        data = json.load(f)
                    
                    if 'relative_filename' in data:
                        old_rel = data['relative_filename']
                        new_rel = old_rel.replace(f"/{name}.json", f"/{new_name}")
                        data['relative_filename'] = new_rel
                        
                        with open(json_file, 'w', encoding='utf-8') as f:
                            json.dump(data, f, indent=4)
                except Exception as e:
                    print(f"  Warning: Could not update JSON content: {e}")
                
                json_file.rename(new_path)
                renamed_files.append((json_file, new_path))
    
    # Process right arm components
    if right_arm_dir.exists():
        for json_file in right_arm_dir.glob("*.json"):
            name = json_file.stem
            if not name.startswith("right_"):
                new_name = f"right_{name}.json"
                new_path = json_file.parent / new_name
                print(f"Renaming: {json_file.relative_to(root)}")
                print(f"      to: {new_path.relative_to(root)}")
                
                # Update relative_filename inside the file
                try:
                    with open(json_file, 'r', encoding='utf-8') as f:
                        data = json.load(f)
                    
                    if 'relative_filename' in data:
                        old_rel = data['relative_filename']
                        new_rel = old_rel.replace(f"/{name}.json", f"/{new_name}")
                        data['relative_filename'] = new_rel
                        
                        with open(json_file, 'w', encoding='utf-8') as f:
                            json.dump(data, f, indent=4)
                except Exception as e:
                    print(f"  Warning: Could not update JSON content: {e}")
                
                json_file.rename(new_path)
                renamed_files.append((json_file, new_path))
    
    print(f"\n✓ Renamed {len(renamed_files)} arm component files\n")
    return renamed_files

def fix_goblin_head_components():
    """Consolidate goblin head components to single location."""
    print("=== Consolidating Goblin Head Components ===\n")
    
    # Keep files in head/ directory, remove from head/goblin/head_components/
    head_dir = bot_families_dir / "goblins" / "head"
    head_components_dir = head_dir / "goblin" / "head_components"
    
    removed_files = []
    
    if head_components_dir.exists():
        for json_file in head_components_dir.glob("*.json"):
            # Check if corresponding file exists in head/ directory
            corresponding_file = head_dir / json_file.name
            
            if corresponding_file.exists():
                print(f"Removing duplicate: {json_file.relative_to(root)}")
                print(f"  (keeping: {corresponding_file.relative_to(root)})")
                json_file.unlink()
                removed_files.append(json_file)
            else:
                print(f"Moving: {json_file.relative_to(root)}")
                print(f"    to: {corresponding_file.relative_to(root)}")
                
                # Update relative_filename inside the file
                try:
                    with open(json_file, 'r', encoding='utf-8') as f:
                        data = json.load(f)
                    
                    if 'relative_filename' in data:
                        old_rel = data['relative_filename']
                        new_rel = old_rel.replace("/goblin/head_components/", "/")
                        data['relative_filename'] = new_rel
                        
                        with open(json_file, 'w', encoding='utf-8') as f:
                            json.dump(data, f, indent=4)
                except Exception as e:
                    print(f"  Warning: Could not update JSON content: {e}")
                
                shutil.move(str(json_file), str(corresponding_file))
                removed_files.append(json_file)
    
    print(f"\n✓ Processed {len(removed_files)} head component files\n")
    return removed_files

def fix_goblin_torso():
    """Consolidate goblin_torso to single location."""
    print("=== Consolidating Goblin Torso ===\n")
    
    # Keep in torso/ directory, remove from subsystems/
    torso_file = bot_families_dir / "goblins" / "torso" / "goblin_torso.json"
    subsystems_file = bot_families_dir / "goblins" / "subsystems" / "goblin_torso.json"
    
    if subsystems_file.exists():
        if torso_file.exists():
            print(f"Removing: {subsystems_file.relative_to(root)}")
            print(f"  (keeping: {torso_file.relative_to(root)})")
            subsystems_file.unlink()
            return [subsystems_file]
        else:
            print(f"Moving: {subsystems_file.relative_to(root)}")
            print(f"    to: {torso_file.relative_to(root)}")
            
            # Update relative_filename inside the file
            try:
                with open(subsystems_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                
                if 'relative_filename' in data:
                    data['relative_filename'] = str(torso_file.relative_to(root))
                    
                    with open(subsystems_file, 'w', encoding='utf-8') as f:
                        json.dump(data, f, indent=4)
            except Exception as e:
                print(f"  Warning: Could not update JSON content: {e}")
            
            # Ensure torso directory exists
            torso_file.parent.mkdir(parents=True, exist_ok=True)
            shutil.move(str(subsystems_file), str(torso_file))
            return [subsystems_file]
    
    return []

def main():
    print("=== Fixing Remaining Duplicate Filenames ===\n")
    
    # Fix arm components
    arm_files = fix_arm_components()
    
    # Fix goblin head components
    head_files = fix_goblin_head_components()
    
    # Fix goblin torso
    torso_files = fix_goblin_torso()
    
    print("=== Summary ===")
    print(f"Arm component files renamed: {len(arm_files)}")
    print(f"Head component files consolidated: {len(head_files)}")
    print(f"Torso files consolidated: {len(torso_files)}")
    print(f"\nTotal changes: {len(arm_files) + len(head_files) + len(torso_files)}")
    
    # Verify no duplicates remain
    print("\n=== Verifying No Duplicates Remain ===")
    all_files = list(bot_families_dir.rglob("*.json"))
    by_name = {}
    for f in all_files:
        name = f.name
        if name in by_name:
            by_name[name].append(f)
        else:
            by_name[name] = [f]
    
    duplicates = {name: files for name, files in by_name.items() if len(files) > 1}
    
    if duplicates:
        print(f"\n⚠️  WARNING: {len(duplicates)} duplicate filenames still exist:")
        for name in sorted(duplicates.keys()):
            print(f"\n  {name}:")
            for f in duplicates[name]:
                print(f"    - {f.relative_to(root)}")
    else:
        print("\n✓ SUCCESS: All duplicate filenames have been resolved!")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
