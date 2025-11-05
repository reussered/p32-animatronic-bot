#!/usr/bin/env python3
"""
Add creature keyword to all 79 bots based on their family and name patterns
"""
import json
from pathlib import Path

root = Path(__file__).parent.resolve()
bot_families_dir = root / "config" / "bots" / "bot_families"

def infer_creature_name(family_name, bot_name):
    """Infer creature type from family and bot name"""
    # Strip family suffix from bot name if present
    if bot_name.endswith("_family"):
        return bot_name[:-7]  # Remove "_family"
    
    # For specific instances, try to extract creature type
    # Example: "android_companion_unit_001" -> "android"
    parts = bot_name.split("_")
    first_part = parts[0]
    
    # Check if first part is a known creature type
    creature_types = {
        "android", "bear", "cat", "dragon", "elf", "goblin", "ghost", "orc",
        "pixie", "robot", "skeleton", "steampunk", "vampire", "wolf", "zombie",
        "humanoid", "cyclops"
    }
    
    if first_part in creature_types:
        return first_part
    
    # Check if family name is singular (likely the creature type)
    if family_name.endswith("s"):
        singular = family_name[:-1]  # Remove trailing 's'
        if singular in creature_types:
            return singular
    
    # Default to family name (without plural 's')
    return family_name.rstrip("s")

print("=== Adding 'creature' keyword to all bots ===\n")

updated_count = 0
skipped_count = 0

for family_dir in sorted(bot_families_dir.iterdir()):
    if not family_dir.is_dir():
        continue
    
    family_name = family_dir.name
    
    for json_file in sorted(family_dir.glob("*.json")):
        bot_name = json_file.stem
        
        # Skip body parts and subsystems
        if any(x in bot_name.lower() for x in ["head", "torso", "arm", "leg", "foot", "hand", "subsystems"]):
            continue
        
        try:
            with json_file.open("r", encoding="utf-8") as f:
                data = json.load(f)
            
            # Only update if creature keyword is missing
            if "creature" not in data or data["creature"] == "UNKNOWN" or not data["creature"]:
                creature = infer_creature_name(family_name, bot_name)
                data["creature"] = creature
                
                with json_file.open("w", encoding="utf-8") as f:
                    json.dump(data, f, indent=4)
                
                print(f"Updated: {family_name}/{bot_name} -> creature: '{creature}'")
                updated_count += 1
            else:
                skipped_count += 1
        
        except Exception as e:
            print(f"ERROR: {family_name}/{bot_name}: {e}")

print(f"\n=== Summary ===")
print(f"Bots updated with creature keyword: {updated_count}")
print(f"Bots already had creature keyword: {skipped_count}")
print(f"Total processed: {updated_count + skipped_count}")
