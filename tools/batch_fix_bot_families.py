"""
Batch fix all bot family JSON files that have non-array 'components' fields.
"""
import json
from pathlib import Path

root = Path('.')
bot_families_dir = root / 'config/bots/bot_families'

if not bot_families_dir.exists():
    print("[ERROR] bot_families directory not found")
    exit(1)

fixed_count = 0
total_checked = 0

# Walk all bot family JSON files
for json_file in bot_families_dir.glob('**/*.json'):
    total_checked += 1
    
    try:
        with open(json_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        made_change = False
        
        # If components exists and is NOT an array, rename it
        if 'components' in data and not isinstance(data['components'], list):
            rel_path = json_file.relative_to(root)
            
            # Determine what to rename it to
            if isinstance(data['components'], dict):
                # Check if it's bot-definition related or subsystem definitions
                data['subsystems'] = data.pop('components')
                print(f'[FIXED] {rel_path}')
                print(f'        Renamed: components -> subsystems')
                made_change = True
        
        # If components doesn't exist, add empty array
        elif 'components' not in data:
            # Only for files that should have components (bot families, not subsystems)
            if 'bot' in json_file.parent.name.lower() or 'bot_families' in str(json_file).lower():
                rel_path = json_file.relative_to(root)
                data['components'] = []
                print(f'[FIXED] {rel_path}')
                print(f'        Added: empty components array')
                made_change = True
        
        if made_change:
            with open(json_file, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=4)
            fixed_count += 1
    
    except Exception as e:
        rel_path = json_file.relative_to(root)
        print(f'[ERROR] {rel_path}: {e}')

print(f'\n[SUMMARY] Checked {total_checked} files, fixed {fixed_count}')
