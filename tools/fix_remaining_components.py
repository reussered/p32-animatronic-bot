"""
Fix remaining files where 'components' field is not an array.
"""
import json
from pathlib import Path

files_to_fix = [
    'config/bots/bot_families/undead/zombie_runner_001.json',
    'config/bots/bot_families/undead/zombie_shambler_001.json',
    'config/components/creature_specific/esp32c3_hand_controller.json'
]

root = Path('.')
fixed_count = 0

for file_path in files_to_fix:
    p = root / file_path
    
    if not p.exists():
        print(f'[MISSING] {file_path}')
        continue
    
    try:
        with open(p, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # Check what components contains
        if 'components' in data:
            if isinstance(data['components'], dict):
                # It's a dict - rename to subsystems or implementation_details
                if any(key in str(data['components']).lower() for key in ['movement', 'system', 'indicator', 'control']):
                    data['subsystems'] = data.pop('components')
                    print(f'[FIXED] {file_path}')
                    print(f'        Renamed: components -> subsystems')
                    with open(p, 'w', encoding='utf-8') as f:
                        json.dump(data, f, indent=4)
                    fixed_count += 1
            elif isinstance(data['components'], list):
                print(f'[OK] {file_path} (components is already an array)')
            else:
                print(f'[WARN] {file_path} (components is {type(data["components"]).__name__})')
        else:
            # Add empty components array
            data['components'] = []
            print(f'[FIXED] {file_path}')
            print(f'        Added: empty components array')
            with open(p, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=4)
            fixed_count += 1
    
    except Exception as e:
        print(f'[ERROR] {file_path}: {e}')

print(f'\n[SUMMARY] Fixed {fixed_count} files')
