#!/usr/bin/env python3
"""
Find all complete bots in bot_families and count unique creatures
"""
import json
from pathlib import Path
from collections import defaultdict

root_dir = Path(__file__).parent.resolve()
bot_families_dir = root_dir / "config" / "bots" / "bot_families"

creatures = defaultdict(list)
subsystem_patterns = {"head", "torso", "arm", "leg", "foot", "hand", "subsystems", "hands", "feet"}

# Scan all bot family directories
for family_dir in sorted(bot_families_dir.iterdir()):
    if not family_dir.is_dir():
        continue
    
    family_name = family_dir.name
    
    # Find bot JSON files (not subsystem or body part directories)
    for json_file in sorted(family_dir.glob("*.json")):
        bot_name = json_file.stem
        
        # Skip if it's a body part or subsystem file
        if any(pattern in bot_name.lower() for pattern in subsystem_patterns):
            continue
        
        # Try to read and get creature info
        try:
            with json_file.open("r", encoding="utf-8") as f:
                data = json.load(f)
            
            creature = data.get("creature", family_name)  # Default to family name
            creatures[creature].append(f"{family_name}/{bot_name}")
        except:
            # If we can't read it, use family name as creature
            creatures[family_name].append(f"{family_name}/{bot_name}")

print(f"=== Creatures Found ===\n")
print(f"Total unique creatures: {len(creatures)}\n")

for creature_name in sorted(creatures.keys()):
    bots = creatures[creature_name]
    print(f"{creature_name}: {len(bots)} bots")
    for bot in sorted(bots)[:5]:  # Show first 5
        print(f"  - {bot}")
    if len(bots) > 5:
        print(f"  ... and {len(bots) - 5} more")
    print()

print(f"\nTotal bots across all creatures: {sum(len(b) for b in creatures.values())}")
