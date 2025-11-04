#!/usr/bin/env python3
"""
Fix duplicate filenames in bot_families by identifying and removing obsolete copies.

Strategy:
1. Keep files in specific family folders (goblins/, bears/, wolves/, etc.)
2. Remove duplicates from category folders (fantasy/, animals/, undead/)
3. Keep one authoritative location for subsystem files
4. Update any references to removed files
"""

import json
import hashlib
from pathlib import Path
from collections import defaultdict
import sys

root = Path("/home/runner/work/p32-animatronic-bot/p32-animatronic-bot")
bot_families_dir = root / "config" / "bots" / "bot_families"

# Category folders that typically contain obsolete copies
CATEGORY_FOLDERS = {"fantasy", "animals", "undead", "horror", "mechanical"}

# Specific family folders that should contain the primary versions
FAMILY_FOLDERS = {
    "goblins", "bears", "cats", "wolves", "vampires", "zombies", 
    "cyclops", "dragons", "elves", "orcs", "robots", "androids",
    "humanoids", "steampunk", "tests"
}

def analyze_duplicates():
    """Analyze all duplicate filenames and determine which to keep/remove."""
    
    # Find all JSON files
    all_files = list(bot_families_dir.rglob("*.json"))
    
    # Group by filename
    by_name = defaultdict(list)
    for f in all_files:
        by_name[f.name].append(f)
    
    # Find duplicates
    duplicates = {name: files for name, files in by_name.items() if len(files) > 1}
    
    print(f"=== Duplicate Filename Analysis ===\n")
    print(f"Found {len(duplicates)} duplicate filenames\n")
    
    to_remove = []
    to_keep = []
    uncertain = []
    
    for name in sorted(duplicates.keys()):
        files = duplicates[name]
        
        # Categorize files by location
        in_category = []
        in_family = []
        in_subsystems = []
        in_head = []
        
        for f in files:
            rel_path = f.relative_to(bot_families_dir)
            parts = rel_path.parts
            
            if len(parts) >= 1 and parts[0] in CATEGORY_FOLDERS:
                in_category.append(f)
            elif len(parts) >= 1 and parts[0] in FAMILY_FOLDERS:
                if "subsystems" in parts:
                    in_subsystems.append(f)
                elif "head" in parts:
                    in_head.append(f)
                else:
                    in_family.append(f)
        
        # Decision logic
        if in_category and (in_family or in_head or in_subsystems):
            # Keep family folder version, remove category version
            to_keep.extend(in_family + in_head + in_subsystems)
            to_remove.extend(in_category)
            decision = "REMOVE category copies, KEEP family folder copies"
        elif in_subsystems and in_head:
            # Both in subsystems and head folders - need manual review
            # Generally prefer head/ over subsystems/
            to_keep.extend(in_head)
            to_remove.extend(in_subsystems)
            decision = "KEEP head/ version, REMOVE subsystems/ version"
        else:
            # Uncertain - need manual review
            uncertain.extend(files)
            decision = "UNCERTAIN - needs manual review"
        
        print(f"\n{name}:")
        for f in files:
            rel_path = f.relative_to(root)
            checksum = hashlib.md5(f.read_bytes()).hexdigest()[:8]
            status = ""
            if f in to_remove:
                status = " [REMOVE]"
            elif f in to_keep:
                status = " [KEEP]"
            elif f in uncertain:
                status = " [REVIEW]"
            print(f"  {status:10} {rel_path} (md5:{checksum})")
        print(f"  Decision: {decision}")
    
    return to_keep, to_remove, uncertain

def check_references(file_path, all_json_files):
    """Check if any other files reference this file."""
    references = []
    rel_path = str(file_path.relative_to(root))
    
    for json_file in all_json_files:
        if json_file == file_path:
            continue
        
        try:
            content = json_file.read_text()
            if rel_path in content:
                references.append(json_file)
        except:
            pass
    
    return references

def main():
    print("=== Bot Files Duplicate Filename Fixer ===\n")
    
    # Step 1: Analyze duplicates
    to_keep, to_remove, uncertain = analyze_duplicates()
    
    print(f"\n\n=== Summary ===")
    print(f"Files to KEEP: {len(to_keep)}")
    print(f"Files to REMOVE: {len(to_remove)}")
    print(f"Files to REVIEW: {len(uncertain)}")
    
    if uncertain:
        print(f"\n⚠️  WARNING: {len(uncertain)} files need manual review")
        print("These files have ambiguous locations and should be reviewed manually:")
        for f in sorted(set(uncertain)):
            print(f"  - {f.relative_to(root)}")
    
    # Step 2: Check for references to files we plan to remove
    print(f"\n\n=== Checking References ===")
    all_json = list(bot_families_dir.rglob("*.json"))
    
    files_with_refs = []
    for f in to_remove:
        refs = check_references(f, all_json)
        if refs:
            files_with_refs.append((f, refs))
    
    if files_with_refs:
        print(f"\n⚠️  WARNING: {len(files_with_refs)} files to remove have references!")
        for f, refs in files_with_refs:
            print(f"\n  {f.relative_to(root)} is referenced by:")
            for ref in refs:
                print(f"    - {ref.relative_to(root)}")
        print("\n  These references will need to be updated before removal.")
    else:
        print("✓ No references found to files being removed")
    
    # Step 3: Show removal plan
    if to_remove:
        print(f"\n\n=== Removal Plan ===")
        print(f"The following {len(to_remove)} files will be removed:\n")
        
        # Group by category folder
        by_category = defaultdict(list)
        for f in to_remove:
            parts = f.relative_to(bot_families_dir).parts
            if parts[0] in CATEGORY_FOLDERS:
                by_category[parts[0]].append(f)
        
        for category in sorted(by_category.keys()):
            print(f"\n  From {category}/ ({len(by_category[category])} files):")
            for f in sorted(by_category[category]):
                print(f"    - {f.relative_to(bot_families_dir)}")
    
    print("\n\n=== Next Steps ===")
    print("1. Review the analysis above")
    print("2. If everything looks correct, run with --execute flag to perform removal")
    print("3. Update any references found (if any)")
    
    # Check if --execute flag was provided
    if "--execute" in sys.argv:
        print("\n\n=== EXECUTING REMOVAL ===")
        if files_with_refs:
            print("ERROR: Cannot execute - files have references that need to be updated first!")
            return 1
        
        removed_count = 0
        for f in to_remove:
            print(f"Removing: {f.relative_to(root)}")
            f.unlink()
            removed_count += 1
        
        print(f"\n✓ Successfully removed {removed_count} duplicate files")
    else:
        print("\nDry run complete. Run with --execute to perform actual removal.")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
