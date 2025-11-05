#!/usr/bin/env python3
"""
Bot Cleanup Script - Remove misclassified component files from bots directory

This script identifies files that are actually components (not bots) and removes them
or moves them to the appropriate location.
"""

import json
import sys
from pathlib import Path
from typing import List, Dict


# Known component-only patterns (not bots)
COMPONENT_PATTERNS = {
    "eye", "ears", "mouth", "nose", "snout", "nostril", "speaker",
    "sensor", "actuator", "servo", "led", "light", "strip",
    "joint", "elbow", "shoulder", "wrist", "hip", "knee", "ankle",
    "vertebrae", "spine", "pelvis", "neck", "assembly",
    "claw", "fang", "fur", "scale", "wing", "tail",
    "marking", "scar", "tattoo", "paint", "glow", "aura",
    "beam", "matrix", "array", "module", "unit",
    "generator", "mechanism", "mechanism", "mount", "bracket"
}


def is_likely_component(filename: str) -> bool:
    """Check if filename indicates this is a component, not a bot."""
    name_lower = filename.lower().replace(".json", "")
    
    for pattern in COMPONENT_PATTERNS:
        if pattern in name_lower:
            return True
    
    return False


def scan_and_categorize(bots_dir: Path) -> Dict:
    """Categorize all bot files."""
    bot_files = list(bots_dir.glob("**/[!.]*.json"))
    
    # Filter to only bot-like files (exclude components intentionally there)
    bot_files = [f for f in bot_files if "components" not in str(f) and "subsystems" not in str(f)]
    
    results = {
        "likely_components": [],
        "family_files": [],
        "actual_bots": [],
        "total": len(bot_files)
    }
    
    for bot_file in sorted(bot_files):
        filename = bot_file.name
        
        # Read to check if it has actual bot structure
        try:
            with open(bot_file, 'r') as f:
                data = json.load(f)
        except:
            results["actual_bots"].append({"file": str(bot_file), "reason": "JSON read error"})
            continue
        
        # Check filename patterns first
        if is_likely_component(filename):
            results["likely_components"].append({
                "file": str(bot_file),
                "reason": "filename pattern indicates component"
            })
            continue
        
        # Check if it's a family template
        if "family" in filename.lower():
            results["family_files"].append({"file": str(bot_file)})
            continue
        
        # Check if it has bot_type or bot_id (actual bots)
        if "bot_type" in data or "bot_id" in data:
            results["actual_bots"].append({"file": str(bot_file)})
            continue
        
        # Check if it looks like a component by structure
        has_bot_fields = any(key in data for key in ["bot_type", "bot_id", "components", "behavior_config"])
        if not has_bot_fields and ("component" in filename.lower() or "part" in filename.lower()):
            results["likely_components"].append({
                "file": str(bot_file),
                "reason": "structure indicates component"
            })
        else:
            results["actual_bots"].append({"file": str(bot_file)})
    
    return results


def print_report(results: Dict, show_deletable: bool = False):
    """Print categorization report."""
    print("\n" + "="*80)
    print("BOT DIRECTORY CLEANUP ANALYSIS")
    print("="*80)
    
    print(f"\nTOTAL FILES ANALYZED: {results['total']}")
    print(f"Likely components (misclassified): {len(results['likely_components'])}")
    print(f"Family template files: {len(results['family_files'])}")
    print(f"Actual bots: {len(results['actual_bots'])}")
    
    if results["likely_components"] and show_deletable:
        print("\n" + "-"*80)
        print("FILES LIKELY TO BE COMPONENTS (candidates for removal/relocation):")
        print("-"*80)
        for item in results["likely_components"][:50]:  # Show first 50
            filename = Path(item["file"]).name
            print(f"  • {filename}")
            print(f"    Path: {item['file']}")
            print(f"    Reason: {item.get('reason', 'unclear')}")
            print()
    
    print("\n" + "="*80)


if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Analyze and clean bot directory")
    parser.add_argument("--bots-dir", default="config/bots", help="Path to bots directory")
    parser.add_argument("--show-deletable", action="store_true", help="Show files recommended for deletion")
    parser.add_argument("--dry-run", action="store_true", help="Show what would be deleted (don't delete)")
    
    args = parser.parse_args()
    
    bots_dir = Path(args.bots_dir)
    if not bots_dir.exists():
        print(f"Error: Bots directory not found: {bots_dir}")
        sys.exit(1)
    
    results = scan_and_categorize(bots_dir)
    print_report(results, show_deletable=args.show_deletable)
