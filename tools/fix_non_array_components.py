"""
Fix files where 'components' field is not an array.
Rename/remove to align with RULE 4.
"""
import json
from pathlib import Path

fixes = [
    {
        'path': 'config/components/functional/goblin_mood.json',
        'action': 'rename_field',
        'old_field': 'components',
        'new_field': 'moods'
    },
    {
        'path': 'config/components/hardware/esp32_c3_mini_controller.json',
        'action': 'remove_field',
        'field': 'components'
    },
    {
        'path': 'config/components/hardware/hardware/esp32_c3_mini_controller.json',
        'action': 'remove_field',
        'field': 'components'
    },
    {
        'path': 'config/components/interfaces/max98357a_i2s_amplifier.json',
        'action': 'remove_field',
        'field': 'components'
    }
]

root = Path('.')
fixed_count = 0

for fix_spec in fixes:
    file_path = fix_spec['path']
    p = root / file_path
    
    if not p.exists():
        print(f'[MISSING] {file_path}')
        continue
    
    try:
        with open(p, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        made_change = False
        
        if fix_spec['action'] == 'rename_field':
            old_field = fix_spec['old_field']
            new_field = fix_spec['new_field']
            
            if old_field in data:
                data[new_field] = data.pop(old_field)
                print(f'[FIXED] {file_path}')
                print(f'        Renamed: {old_field} -> {new_field}')
                made_change = True
            else:
                print(f'[OK] {file_path} (field not present)')
        
        elif fix_spec['action'] == 'remove_field':
            field = fix_spec['field']
            
            if field in data:
                del data[field]
                print(f'[FIXED] {file_path}')
                print(f'        Removed: {field}')
                made_change = True
            else:
                print(f'[OK] {file_path} (field not present)')
        
        if made_change:
            with open(p, 'w', encoding='utf-8') as f:
                json.dump(data, f, indent=4)
            fixed_count += 1
    
    except Exception as e:
        print(f'[ERROR] {file_path}: {e}')

print(f'\n[SUMMARY] Fixed {fixed_count} files')
