#!/usr/bin/env python3
"""
Reorganize bot and component structure:
1. Move bots from config/bots/*.json to family folders
2. Move misplaced components from bot_families root to components/creature_specific
3. Ensure bot_families has ONLY family folders
"""
import json
import shutil
from pathlib import Path

root = Path(__file__).parent.resolve()
bots_dir = root / "config" / "bots"
bot_families_dir = bots_dir / "bot_families"
components_dir = root / "config" / "components" / "creature_specific"

print("=== Bot Structure Reorganization ===\n")

# Known family folders
known_families = {
    "androids", "animals", "bears", "cats", "cyclops", "dragons", "elves",
    "fantasy", "goblins", "horror", "humanoids", "mechanical", "orcs",
    "robots", "steampunk", "tests", "undead", "vampires", "wolves", "zombies"
}

# Step 1: Move bot files from config/bots to families
print("[Step 1] Moving bot files to families...")
bot_files_moved = 0

for json_file in sorted(bots_dir.glob("*.json")):
    name = json_file.stem
    
    # Determine family from filename
    if name.startswith("goblin"):
        family = "goblins"
    elif name.startswith("test"):
        family = "tests"
    elif name.startswith("android"):
        family = "androids"
    else:
        print(f"  WARNING: Cannot determine family for {name}")
        continue
    
    family_dir = bot_families_dir / family
    target = family_dir / json_file.name
    
    shutil.move(str(json_file), str(target))
    print(f"  Moved: {name} -> {family}/")
    bot_files_moved += 1

# Step 2: Move misplaced component files from bot_families root
print("\n[Step 2] Moving misplaced component files...")
component_files_moved = 0

for file_path in sorted(bot_families_dir.glob("*")):
    # Skip family directories
    if file_path.is_dir() and file_path.name in known_families:
        continue
    
    if file_path.is_file():
        # Skip README
        if file_path.name == "README.md":
            continue
        
        # Move to components/creature_specific
        target = components_dir / file_path.name
        shutil.move(str(file_path), str(target))
        print(f"  Moved: {file_path.name} -> components/creature_specific/")
        component_files_moved += 1

print(f"\n=== Summary ===")
print(f"Bot files moved: {bot_files_moved}")
print(f"Component files moved: {component_files_moved}")
print(f"\nVerifying structure...")

# Verify
remaining_files = list(bot_families_dir.glob("*"))
remaining_files = [f for f in remaining_files if f.is_file() and f.name != "README.md"]

if remaining_files:
    print(f"ERROR: {len(remaining_files)} files still in bot_families root:")
    for f in remaining_files:
        print(f"  - {f.name}")
else:
    print("SUCCESS: bot_families root now contains ONLY family directories")

print(f"\nFamilies in bot_families:")
families = sorted([d.name for d in bot_families_dir.iterdir() if d.is_dir()])
for fam in families:
    print(f"  - {fam}")
