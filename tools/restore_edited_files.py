#!/usr/bin/env python3
"""
Restore all files that were EDITED in commit 3cc74ab from 3cc74ab~1
This recovers content that was modified during the reorganization.
"""

import subprocess
import sys
from pathlib import Path

# Files that were EDITED (modified) in commit 3cc74ab
# These need restoration from 3cc74ab~1
EDITED_FILES = [
    ".github/consolidated-rules.md",
    ".github/copilot-instructions.md",
    ".github/naming_rules.md",
    "config/bots/bot_families/androids/head/android_emotion_display.json",
    "config/bots/bot_families/androids/head/android_neural_interface.json",
    "config/bots/bot_families/androids/head/android_synthetic_skin.json",
    "config/bots/bot_families/bears/head/bear_arctic_fur.json",
    "config/bots/bot_families/bears/head/bear_massive_claws.json",
    "config/bots/bot_families/bears/head/bear_thick_fur.json",
    "config/bots/bot_families/cats/head/cat_purr_generator.json",
    "config/bots/bot_families/cats/head/cat_tail_servo.json",
    "config/bots/bot_families/dragons/head/dragon_nostril_left.json",
    "config/bots/bot_families/dragons/head/dragon_nostril_right.json",
    "config/bots/bot_families/elves/head/elf_ear_left.json",
    "config/bots/bot_families/elves/head/elf_ear_right.json",
    "config/bots/bot_families/goblins/head/goblin_authority_markings.json",
    "config/bots/bot_families/goblins/head/goblin_head.json",
    "config/bots/bot_families/goblins/head/goblin_left_ear.json",
    "config/bots/bot_families/goblins/head/goblin_left_ear.src",
    "config/bots/bot_families/goblins/head/goblin_left_eye.src",
    "config/bots/bot_families/goblins/head/goblin_mouth.src",
    "config/bots/bot_families/goblins/head/goblin_nose.src",
    "config/bots/bot_families/goblins/head/goblin_pirate_head.json",
    "config/bots/bot_families/goblins/head/goblin_right_ear.json",
    "config/bots/bot_families/goblins/head/goblin_right_ear.src",
    "config/bots/bot_families/goblins/head/goblin_speaker.src",
    "config/bots/bot_families/goblins/torso/goblin_torso.json",
    "config/bots/bot_families/horror/head/ghost_ectoplasm_generator.json",
    "config/bots/bot_families/horror/head/ghost_floating_mechanism.json",
    "config/bots/bot_families/horror/head/ghost_phase_lights.json",
    "config/bots/bot_families/robots/head/robot_back_mount.json",
    "config/bots/bot_families/robots/head/robot_chest_matrix.json",
    "config/bots/bot_families/robots/head/robot_led_strips.json",
    "config/bots/bot_families/robots/head/robot_solar_parasol.json",
    "config/bots/bot_families/robots/head/robot_status_oled.json",
]

def restore_file(file_path):
    """Restore a file from 3cc74ab~1 using git show"""
    try:
        # Get the content from 3cc74ab~1
        result = subprocess.run(
            ["git", "show", f"3cc74ab~1:{file_path}"],
            capture_output=True,
            text=True,
            check=True
        )
        
        # Write it to the current location
        full_path = Path(file_path)
        full_path.parent.mkdir(parents=True, exist_ok=True)
        full_path.write_text(result.stdout)
        print(f"✓ Restored: {file_path}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ Failed to restore {file_path}: {e.stderr}")
        return False
    except Exception as e:
        print(f"✗ Error restoring {file_path}: {e}")
        return False

def main():
    print("=" * 80)
    print("RESTORING FILES MODIFIED IN COMMIT 3cc74ab")
    print("=" * 80)
    print(f"Total files to restore: {len(EDITED_FILES)}")
    print()
    
    restored_count = 0
    failed_count = 0
    
    for file_path in EDITED_FILES:
        if restore_file(file_path):
            restored_count += 1
        else:
            failed_count += 1
    
    print()
    print("=" * 80)
    print(f"RESTORATION COMPLETE")
    print(f"Successfully restored: {restored_count}/{len(EDITED_FILES)}")
    print(f"Failed: {failed_count}/{len(EDITED_FILES)}")
    print("=" * 80)
    
    if failed_count > 0:
        sys.exit(1)

if __name__ == "__main__":
    main()
