"""
Add missing 'components': [] field to JSON files that need it.
"""
import json
from pathlib import Path

files_to_fix = [
    'config/bots/bot_families/robots/robot_combat_unit_001.json',
    'config/bots/bot_families/steampunk/steampunk_aristocrat_001.json',
    'config/bots/bot_families/undead/ghost_poltergeist_001.json',
    'config/bots/bot_families/undead/vampire_ancient_elder_001.json',
    'config/bots/bot_families/undead/zombie_runner_001.json',
    'config/bots/bot_families/undead/zombie_shambler_001.json',
    'config/components/creature_specific/esp32c3_hand_controller.json',
    'config/components/functional/goblin_mood.json',
    'config/components/hardware/esp32_c3_mini_controller.json',
    'config/components/hardware/hardware/esp32_c3_mini_controller.json',
    'config/components/interfaces/max98357a_i2s_amplifier.json'
]

root = Path('.')
fixed_count = 0

for file_path in files_to_fix:
    p = root / file_path
    if p.exists():
        try:
            with open(p, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            if 'components' not in data:
                data['components'] = []
                with open(p, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=4)
                print(f'[FIXED] {file_path}')
                fixed_count += 1
            else:
                print(f'[OK] {file_path} (already has components field)')
        except Exception as e:
            print(f'[ERROR] {file_path}: {e}')
    else:
        print(f'[MISSING] {file_path}')

print(f'\n[SUMMARY] Fixed {fixed_count} files')
