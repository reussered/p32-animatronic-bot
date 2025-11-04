#!/usr/bin/env python3
"""
Add 'creature' keyword to all bot JSON files based on their name pattern
"""
import json
from pathlib import Path

root_dir = Path(__file__).parent.resolve()
config_bots_dir = root_dir / "config" / "bots"

def infer_creature_from_name(bot_name: str) -> str:
    """Infer the creature type from bot name"""
    # Examples: goblin_full, android_medical_unit, test_bot_minimal
    if bot_name.startswith("goblin_"):
        return "goblin"
    elif bot_name.startswith("android_"):
        return "android"
    elif bot_name.startswith("test_"):
        return "test"
    elif "android" in bot_name.lower():
        return "android"
    elif "goblin" in bot_name.lower():
        return "goblin"
    else:
        return bot_name.split("_")[0]  # Use first part as creature type

print("=== Adding 'creature' keyword to bots ===\n")

updated_count = 0

# Process all bot JSON files
for json_file in sorted(config_bots_dir.glob("*.json")):
    try:
        with json_file.open("r", encoding="utf-8") as f:
            data = json.load(f)
        
        bot_name = json_file.stem
        creature_type = infer_creature_from_name(bot_name)
        
        # Add creature keyword if not present
        if "creature" not in data:
            data["creature"] = creature_type
            updated_count += 1
            
            # Write back
            with json_file.open("w", encoding="utf-8") as f:
                json.dump(data, f, indent=4)
            
            print(f"Updated: {bot_name}")
            print(f"  creature: '{creature_type}'")
        else:
            print(f"Already has creature: {bot_name} = '{data['creature']}'")
    
    except Exception as e:
        print(f"Error processing {json_file.name}: {e}")

print(f"\n=== Results ===")
print(f"Updated {updated_count} bot files with creature keyword")
