#!/usr/bin/env python3
"""
Discovers all bot definitions and creates the 'bots' section in master_rules.json
"""
import json
from pathlib import Path

root_dir = Path(__file__).parent.resolve()
master_rules_path = root_dir / "master_rules.json"
config_bots_dir = root_dir / "config" / "bots"

print("=== Discovering Bot Definitions ===\n")

# Find all bot definition JSON files (those that reference subsystems or are full bot definitions)
bots = {}

# Check config/bots/*.json files for complete bot definitions
if config_bots_dir.exists():
    for json_file in sorted(config_bots_dir.glob("*.json")):
        try:
            with json_file.open("r", encoding="utf-8") as f:
                data = json.load(f)
            
            name = json_file.stem
            rel_path = json_file.relative_to(root_dir).as_posix()
            
            # Determine bot type and subsystems
            bot_type = data.get("bot_type", "UNKNOWN")
            subsystems = []
            
            if "components" in data and isinstance(data["components"], list):
                for comp in data["components"]:
                    if isinstance(comp, str) and "subsystems" in comp:
                        subsystems.append(comp)
            
            bots[name] = {
                "relative_filename": rel_path,
                "bot_type": bot_type,
                "subsystems": subsystems,
                "description": data.get("description", "")
            }
            print(f"Bot: {name}")
            print(f"  Type: {bot_type}")
            print(f"  Subsystems: {len(subsystems)}")
            print()
        except Exception as e:
            print(f"Error reading {json_file.name}: {e}\n")

# Load master_rules.json
print(f"Loading master_rules.json...")
with master_rules_path.open("r", encoding="utf-8") as f:
    master_rules = json.load(f)

# Add or update the bots section
master_rules["bots"] = {
    "description": "Complete bot definitions with subsystems. A bot is a complete creature instance combining one or more subsystems (head, torso, etc.). The 'creature' keyword identifies the creature type (e.g., 'goblin_full', 'android_medical_unit').",
    "bots": bots
}

# Save the updated master_rules.json
with master_rules_path.open("w", encoding="utf-8") as f:
    json.dump(master_rules, f, indent=4)

print(f"\n=== Results ===")
print(f"Added {len(bots)} bot definitions to master_rules.json")
print(f"Bot names: {', '.join(sorted(bots.keys()))}")
