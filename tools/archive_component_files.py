#!/usr/bin/env python3
"""
Safe Removal Script - Archive misclassified component files

This script moves component files from config/bots/ to a backup location.
It does NOT delete them, just moves them for later review/cleanup.
"""

import json
import sys
from pathlib import Path
from typing import List, Dict
import shutil


def has_bot_type(file_path: Path) -> bool:
    """Check if file has bot_type field (indicates it's a real bot)."""
    try:
        with open(file_path, 'r') as f:
            data = json.load(f)
        return "bot_type" in data
    except:
        return False


def identify_components(bots_dir: Path) -> List[Path]:
    """Find files that are components (no bot_type) in the bots directory."""
    bot_files = list(bots_dir.glob("**/[!.]*.json"))
    
    # Filter to only bot-like files (exclude intended components)
    bot_files = [f for f in bot_files if "components" not in str(f) and "subsystems" not in str(f)]
    
    component_files = []
    for bot_file in bot_files:
        # Quick heuristic: component names include "eye", "ear", "nose", etc.
        if any(pattern in bot_file.name.lower() for pattern in 
               ["eye", "ears", "mouth", "nose", "snout", "speaker", "sensor", "actuator"]):
            # Verify it's truly a component (no bot_type)
            if not has_bot_type(bot_file):
                component_files.append(bot_file)
    
    return component_files


def archive_components(component_files: List[Path], archive_dir: Path, dry_run: bool = False) -> Dict:
    """Move component files to archive directory."""
    if not dry_run:
        archive_dir.mkdir(parents=True, exist_ok=True)
    
    results = {
        "moved": [],
        "errors": [],
        "total": len(component_files)
    }
    
    for component_file in component_files:
        try:
            # Preserve relative path structure
            rel_path = component_file.relative_to(component_file.parent.parent.parent.parent)
            dest_file = archive_dir / rel_path
            dest_file.parent.mkdir(parents=True, exist_ok=True)
            
            if not dry_run:
                shutil.move(str(component_file), str(dest_file))
            
            results["moved"].append({
                "src": str(component_file),
                "dest": str(dest_file)
            })
        except Exception as e:
            results["errors"].append({
                "file": str(component_file),
                "error": str(e)
            })
    
    return results


def print_report(results: Dict, dry_run: bool = False):
    """Print archival report."""
    mode = "DRY RUN - " if dry_run else ""
    print("\n" + "="*80)
    print(f"{mode}BOT DIRECTORY CLEANUP - COMPONENT FILE ARCHIVAL")
    print("="*80)
    
    print(f"\nTotal files to process: {results['total']}")
    print(f"Successfully moved: {len(results['moved'])}")
    print(f"Errors: {len(results['errors'])}")
    
    if results["moved"]:
        print("\nFILES MOVED:")
        for item in results["moved"][:20]:
            src_name = Path(item['src']).name
            print(f"   {src_name}")
        if len(results["moved"]) > 20:
            print(f"  ... and {len(results['moved']) - 20} more")
    
    if results["errors"]:
        print("\nERRORS:")
        for item in results["errors"]:
            print(f"   {Path(item['file']).name}: {item['error']}")
    
    print("\n" + "="*80)


if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Archive misclassified component files from bots directory")
    parser.add_argument("--bots-dir", default="config/bots", help="Path to bots directory")
    parser.add_argument("--archive-dir", default="config/bots/.archived_components", 
                       help="Directory to archive component files to")
    parser.add_argument("--dry-run", action="store_true", help="Show what would be moved without moving")
    
    args = parser.parse_args()
    
    bots_dir = Path(args.bots_dir)
    if not bots_dir.exists():
        print(f"Error: Bots directory not found: {bots_dir}")
        sys.exit(1)
    
    archive_dir = Path(args.archive_dir)
    
    print(f"Scanning {bots_dir} for component files...")
    component_files = identify_components(bots_dir)
    
    if not component_files:
        print("No component files found to archive.")
        sys.exit(0)
    
    print(f"\nFound {len(component_files)} component files")
    
    results = archive_components(component_files, archive_dir, dry_run=args.dry_run)
    print_report(results, dry_run=args.dry_run)
    
    if args.dry_run:
        print("\n  DRY RUN MODE - No files were actually moved.")
        print("Run without --dry-run to perform the actual archival.")
