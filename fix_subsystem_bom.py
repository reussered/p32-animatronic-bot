#!/usr/bin/env python3
import json
import sys
from pathlib import Path

root_dir = Path(__file__).parent.resolve()

# Find all JSON files with BOM issues in the migrated subsystems
subsystems_dir = root_dir / "config" / "bots" / "bot_families"

fixed_count = 0
for json_file in subsystems_dir.glob("**/subsystems/*.json"):
    try:
        # Try reading with utf-8-sig to handle BOM
        with json_file.open("r", encoding="utf-8-sig") as f:
            data = json.load(f)
        
        # Write back without BOM
        with json_file.open("w", encoding="utf-8") as f:
            json.dump(data, f, indent=4)
        
        print(f"Fixed BOM in: {json_file.relative_to(root_dir)}")
        fixed_count += 1
    except Exception as e:
        print(f"ERROR fixing {json_file.relative_to(root_dir)}: {e}")

print(f"\nFixed {fixed_count} files")
