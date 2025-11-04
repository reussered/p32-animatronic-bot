#!/usr/bin/env python3
import json
import sys
from pathlib import Path

root_dir = Path(__file__).parent.resolve()
master_rules_path = root_dir / "master_rules.json"

# Read the current file as raw text
try:
    with master_rules_path.open("r", encoding="utf-8") as f:
        raw_content = f.read()
except UnicodeDecodeError:
    # Try with BOM
    with master_rules_path.open("r", encoding="utf-8-sig") as f:
        raw_content = f.read()

# Try to parse it
try:
    data = json.loads(raw_content)
except json.JSONDecodeError as e:
    print(f"ERROR: Cannot parse master_rules.json: {e}")
    print("Attempting cleanup...")
    
    # Remove BOM if present
    if raw_content.startswith('\ufeff'):
        raw_content = raw_content[1:]
    
    # Fix extra spaces in JSON
    raw_content = raw_content.replace('":  {', '": {')
    raw_content = raw_content.replace('":  [', '": [')
    
    try:
        data = json.loads(raw_content)
    except json.JSONDecodeError as e2:
        print(f"Still cannot parse: {e2}")
        sys.exit(1)

# Add subsystems_migration info
if "subsystems_migration" not in data:
    data["subsystems_migration"] = {
        "description": "Subsystems are now located in config/bots/bot_families/{family}/subsystems/",
        "migrated_on": "2025-11-04",
        "note": "The config/subsystems directory has been removed"
    }

# Write it back cleanly
with master_rules_path.open("w", encoding="utf-8") as f:
    json.dump(data, f, indent=4)

print("Fixed master_rules.json")
