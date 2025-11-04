#!/usr/bin/env python3
import json
import sys
from pathlib import Path

root_dir = Path(__file__).parent.resolve()
config_dir = root_dir / "config"

print("Finding JSON files with old subsystems references...")
fixed_files = []

for json_file in config_dir.glob("**/*.json"):
    try:
        with json_file.open("r", encoding="utf-8") as f:
            content = f.read()
        
        if "config/subsystems" in content:
            print(f"\nFound in: {json_file.relative_to(root_dir)}")
            print(f"  Content preview: {content[0:200]}")
            
            # Load and fix
            with json_file.open("r", encoding="utf-8") as f:
                data = json.load(f)
            
            # Fix references in components array
            if "components" in data and isinstance(data["components"], list):
                for i, comp in enumerate(data["components"]):
                    if isinstance(comp, str) and "config/subsystems" in comp:
                        old_ref = comp
                        # Determine the new reference
                        if "goblin_head_left_eye_only" in comp:
                            new_ref = "config/bots/bot_families/goblins/subsystems/goblin_head_left_eye_only.json"
                        elif "goblin_pirate_head" in comp:
                            new_ref = "config/bots/bot_families/goblins/subsystems/goblin_pirate_head.json"
                        elif "goblin_head" in comp:
                            new_ref = "config/bots/bot_families/goblins/subsystems/goblin_head.json"
                        elif "goblin_torso" in comp:
                            new_ref = "config/bots/bot_families/goblins/subsystems/goblin_torso.json"
                        elif "test_head" in comp:
                            new_ref = "config/bots/bot_families/tests/subsystems/test_head.json"
                        else:
                            new_ref = comp  # unknown, leave as is
                        
                        data["components"][i] = new_ref
                        print(f"  Fixed: {old_ref} -> {new_ref}")
            
            # Write back
            with json_file.open("w", encoding="utf-8") as f:
                json.dump(data, f, indent=4)
            
            fixed_files.append(json_file.relative_to(root_dir))
    except Exception as e:
        pass

print(f"\n\nFixed {len(fixed_files)} files")
