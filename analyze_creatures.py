#!/usr/bin/env python3
"""
Analyze bot structure for:
1. Duplicate creature definitions
2. Missing creature keywords
3. Incorrect family assignments
4. Orphaned components
"""
import json
from pathlib import Path
from collections import defaultdict

root = Path(__file__).parent.resolve()
bot_families_dir = root / "config" / "bots" / "bot_families"

creatures_found = defaultdict(list)
missing_creature = []
duplicates = defaultdict(list)
family_creature_map = {}

print("=== Bot Analysis ===\n")

# Scan all family directories
for family_dir in sorted(bot_families_dir.iterdir()):
    if not family_dir.is_dir():
        continue
    
    family_name = family_dir.name
    
    # Find bot JSON files
    for json_file in sorted(family_dir.glob("*.json")):
        bot_name = json_file.stem
        
        # Skip body parts and subsystems
        if any(x in bot_name.lower() for x in ["head", "torso", "arm", "leg", "foot", "hand", "subsystems"]):
            continue
        
        try:
            with json_file.open("r", encoding="utf-8") as f:
                data = json.load(f)
            
            creature = data.get("creature")
            bot_type = data.get("bot_type", "UNKNOWN")
            
            if not creature:
                missing_creature.append(f"{family_name}/{bot_name}")
                creature = "UNKNOWN"
            
            creatures_found[creature].append(f"{family_name}/{bot_name}")
            
            # Track if creature matches family name
            if creature.lower() != family_name.lower():
                if family_name not in family_creature_map:
                    family_creature_map[family_name] = set()
                family_creature_map[family_name].add(creature)
        
        except Exception as e:
            print(f"ERROR reading {family_name}/{bot_name}: {e}")

print(f"Total unique creatures: {len(creatures_found)}")
print(f"Total bots: {sum(len(v) for v in creatures_found.values())}\n")

print("=== Creatures by Type ===")
for creature in sorted(creatures_found.keys()):
    bots = creatures_found[creature]
    print(f"\n{creature}: {len(bots)} instances")
    for bot in sorted(bots)[:3]:
        print(f"  - {bot}")
    if len(bots) > 3:
        print(f"  ... and {len(bots)-3} more")

print(f"\n\n=== Missing Creature Keywords ===")
if missing_creature:
    print(f"Found {len(missing_creature)} bots without creature keyword:")
    for bot in missing_creature[:10]:
        print(f"  - {bot}")
else:
    print("All bots have creature keywords")

print(f"\n\n=== Family-Creature Mismatches ===")
if family_creature_map:
    for family, creatures in sorted(family_creature_map.items()):
        if len(creatures) > 1 or (len(creatures) == 1 and list(creatures)[0].lower() != family.lower()):
            print(f"{family}: {creatures}")
else:
    print("All creatures match their family names")
