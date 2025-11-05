#!/usr/bin/env python3
"""
Find duplicate bots and files across the project
"""
import json
from pathlib import Path
from collections import defaultdict

root = Path(__file__).parent.resolve()
config_dir = root / "config"

print("=== Scanning for Duplicate Bot Definitions ===\n")

# Track bot names and their locations
bot_names = defaultdict(list)
file_hashes = defaultdict(list)

for json_file in sorted(config_dir.glob("**/bot*.json")):
    # Skip if it's a component, not a bot
    if "components" in str(json_file) or "hardware" in str(json_file):
        continue
    
    rel_path = json_file.relative_to(root).as_posix()
    bot_name = json_file.stem
    
    try:
        with json_file.open("r", encoding="utf-8") as f:
            content = f.read()
        
        # Simple hash to detect duplicates
        file_size = len(content)
        bot_names[bot_name].append(rel_path)
        file_hashes[file_size].append(rel_path)
    except:
        pass

print("=== Bot Names Defined in Multiple Locations ===")
duplicates_found = 0
for bot_name, locations in sorted(bot_names.items()):
    if len(locations) > 1:
        duplicates_found += 1
        print(f"\n{bot_name} appears {len(locations)} times:")
        for loc in sorted(locations):
            print(f"  - {loc}")

if duplicates_found == 0:
    print("No duplicate bot names found")

print(f"\n\nTotal bots scanned: {sum(len(v) for v in bot_names.values())}")
print(f"Unique bot names: {len(bot_names)}")
