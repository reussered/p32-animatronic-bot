"""
Remove Doubled Directories
Deletes the outer directories when inner subdirectories contain newer versions.
Result: drivers/driver/* → drivers/* (moves up one level)
Result: hardware/hardware/* → hardware/* (moves up one level)
"""

import shutil
import os
from pathlib import Path


def move_and_cleanup(inner_dir: str, outer_dir: str, description: str) -> None:
    """Move files from inner to outer directory, delete inner directory."""
    inner_path = Path(inner_dir)
    outer_path = Path(outer_dir)
    
    if not inner_path.exists():
        print(f"⊘ {description}: {inner_dir} not found")
        return
    
    if not outer_path.exists():
        print(f"⊘ {description}: {outer_dir} not found")
        return
    
    print(f"\n{description}:")
    print(f"  From: {inner_dir}")
    print(f"  To:   {outer_dir}")
    
    try:
        # Get all files from inner directory
        files = list(inner_path.glob("*"))
        
        if not files:
            print(f"  ⊘ No files in inner directory")
            return
        
        moved_count = 0
        for file_path in files:
            target_path = outer_path / file_path.name
            
            if target_path.exists():
                # Remove old file first
                if target_path.is_file():
                    os.remove(target_path)
                    print(f"  ✓ Removed old: {target_path.name}")
                elif target_path.is_dir():
                    shutil.rmtree(target_path)
                    print(f"  ✓ Removed old dir: {target_path.name}")
            
            # Move file
            if file_path.is_file():
                shutil.move(str(file_path), str(target_path))
                print(f"  ✓ Moved: {file_path.name}")
                moved_count += 1
            elif file_path.is_dir():
                shutil.move(str(file_path), str(target_path))
                print(f"  ✓ Moved dir: {file_path.name}")
                moved_count += 1
        
        # Delete inner directory (should be empty now)
        if inner_path.exists() and not list(inner_path.glob("*")):
            shutil.rmtree(inner_path)
            print(f"  ✓ Deleted empty dir: {inner_dir}")
        
        print(f"  Total moved: {moved_count}")
        
    except Exception as e:
        print(f"  ✗ Error: {e}")


def main():
    print("=" * 80)
    print("REMOVE DOUBLED DIRECTORIES")
    print("=" * 80)
    
    # config/components/drivers/driver/ → config/components/drivers/
    move_and_cleanup(
        "config/components/drivers/driver",
        "config/components/drivers",
        "DRIVERS"
    )
    
    # config/components/hardware/hardware/ → config/components/hardware/
    move_and_cleanup(
        "config/components/hardware/hardware",
        "config/components/hardware",
        "HARDWARE"
    )
    
    # config/components/interfaces/interfaces/ → config/components/interfaces/
    move_and_cleanup(
        "config/components/interfaces/interfaces",
        "config/components/interfaces",
        "INTERFACES"
    )
    
    print("\n" + "=" * 80)
    print("Cleanup complete")
    print("=" * 80)


if __name__ == "__main__":
    main()
