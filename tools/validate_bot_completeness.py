#!/usr/bin/env python3
"""
Enhanced Bot Configuration Validator & Repair Tool

Validates and repairs bot JSON files by:
1. Detecting incomplete bots (head-only)
2. Adding missing subsystems based on creature type
3. Ensuring consistent component assembly
"""

import json
import sys
from pathlib import Path
from typing import Dict, List, Tuple, Optional
import re


# Map creature types to their standard subsystems
CREATURE_SUBSYSTEM_MAP = {
    "goblin": [
        "config/subsystems/goblin_head.json",
        "config/subsystems/goblin_torso.json"
    ],
    "pirate_goblin": [
        "config/subsystems/goblin_pirate_head.json",
        "config/subsystems/goblin_torso.json"
    ],
    "bear": [
        "config/subsystems/bear_head.json",
        "config/subsystems/bear_torso.json"
    ],
    "cat": [
        "config/subsystems/cat_head.json",
        "config/subsystems/cat_torso.json"
    ],
    "dragon": [
        "config/subsystems/dragon_head.json",
        "config/subsystems/dragon_torso.json"
    ],
    "elf": [
        "config/subsystems/elf_head.json",
        "config/subsystems/elf_torso.json"
    ],
    "vampire": [
        "config/subsystems/vampire_head.json",
        "config/subsystems/vampire_torso.json"
    ],
    "wolf": [
        "config/subsystems/wolf_head.json",
        "config/subsystems/wolf_torso.json"
    ],
    "cyclops": [
        "config/subsystems/cyclops_head.json",
        "config/subsystems/cyclops_torso.json"
    ],
    "android": [
        "config/subsystems/android_head.json",
        "config/subsystems/android_torso.json"
    ],
}


def extract_creature_type_from_filename(filename: str) -> Optional[str]:
    """Extract creature type from filename."""
    # Examples: goblin_head_only.json -> goblin, pirate_goblin.json -> pirate_goblin
    name = filename.replace(".json", "").lower()
    
    for creature_type in CREATURE_SUBSYSTEM_MAP.keys():
        if creature_type in name:
            return creature_type
    
    return None


def extract_creature_type_from_components(components: List[str]) -> Optional[str]:
    """Extract creature type from component references."""
    # Look for subsystem references to infer creature type
    for component in components:
        for creature_type in CREATURE_SUBSYSTEM_MAP.keys():
            if creature_type in component.lower():
                return creature_type
    return None


def is_head_only_bot(components: List) -> bool:
    """Check if bot only has a head subsystem."""
    head_subsystems = {
        "goblin_head",
        "goblin_head_left_eye_only",
        "goblin_pirate_head",
        "bear_head",
        "cat_head",
        "dragon_head",
        "elf_head",
        "vampire_head",
        "wolf_head",
        "cyclops_head",
        "android_head"
    }
    
    for component in components:
        # Handle both string and dict components
        component_str = component if isinstance(component, str) else str(component.get("name", ""))
        component_name = Path(component_str).stem
        if component_name in head_subsystems:
            return True
    
    has_torso = any("torso" in (str(c) if isinstance(c, str) else str(c.get("name", ""))).lower() for c in components)
    return not has_torso


def get_missing_subsystems(creature_type: str, current_components: List) -> List[str]:
    """Get list of missing subsystems for a creature."""
    if creature_type not in CREATURE_SUBSYSTEM_MAP:
        return []
    
    expected_subsystems = CREATURE_SUBSYSTEM_MAP[creature_type]
    
    # Normalize components to strings for comparison
    current_components_str = []
    for comp in current_components:
        if isinstance(comp, str):
            current_components_str.append(comp)
        elif isinstance(comp, dict):
            current_components_str.append(comp.get("name", ""))
    
    current_set = set(current_components_str)
    missing = [sub for sub in expected_subsystems if sub not in current_set]
    
    return missing


def validate_and_repair_bot(bot_file: Path, auto_fix: bool = False) -> Dict:
    """
    Validate a bot JSON file and optionally repair it.
    
    Returns dict with:
    - is_valid: bool
    - is_incomplete: bool
    - creature_type: str or None
    - missing_subsystems: list
    - issues: list of strings
    - fixed: bool (if auto_fix=True)
    """
    result = {
        "file": str(bot_file),
        "is_valid": False,
        "is_incomplete": False,
        "creature_type": None,
        "missing_subsystems": [],
        "issues": [],
        "fixed": False
    }
    
    # Read file
    try:
        with open(bot_file, 'r') as f:
            bot_data = json.load(f)
        result["is_valid"] = True
    except Exception as e:
        result["issues"].append(f"Invalid JSON: {e}")
        return result
    
    # Get components list
    components = bot_data.get("components", [])
    if not components:
        result["issues"].append("No 'components' field found")
        return result
    
    # Determine creature type
    creature_type = extract_creature_type_from_filename(bot_file.name)
    if not creature_type:
        creature_type = extract_creature_type_from_components(components)
    
    result["creature_type"] = creature_type
    
    # Check if head-only
    if is_head_only_bot(components):
        result["is_incomplete"] = True
        
        if creature_type:
            missing = get_missing_subsystems(creature_type, components)
            if missing:
                result["missing_subsystems"] = missing
                result["issues"].append(
                    f"Head-only bot detected. Missing subsystems: {', '.join([Path(m).stem for m in missing])}"
                )
                
                # Auto-fix if requested
                if auto_fix:
                    try:
                        # Add missing subsystems to components
                        for missing_sub in missing:
                            if missing_sub not in components:
                                components.append(missing_sub)
                        
                        bot_data["components"] = components
                        
                        # Write back to file
                        with open(bot_file, 'w') as f:
                            json.dump(bot_data, f, indent=4)
                        
                        result["fixed"] = True
                        result["issues"][-1] += " [FIXED]"
                    except Exception as e:
                        result["issues"][-1] += f" [FIX FAILED: {e}]"
        else:
            result["issues"].append("Head-only bot but creature type could not be determined")
    
    return result


def scan_all_bots(bots_dir: Path, auto_fix: bool = False) -> Dict:
    """Scan all bot JSON files and return summary."""
    bot_files = list(bots_dir.glob("**/[!.]*.json"))
    
    # Filter to only actual bot files (exclude component files)
    bot_files = [f for f in bot_files if "components" not in str(f) and "subsystems" not in str(f)]
    
    results = {
        "total_bots": len(bot_files),
        "incomplete_bots": [],
        "complete_bots": [],
        "invalid_bots": [],
        "summary": {}
    }
    
    for bot_file in sorted(bot_files):
        validation = validate_and_repair_bot(bot_file, auto_fix=auto_fix)
        
        if not validation["is_valid"]:
            results["invalid_bots"].append(validation)
        elif validation["is_incomplete"]:
            results["incomplete_bots"].append(validation)
        else:
            results["complete_bots"].append(validation)
    
    # Generate summary
    results["summary"] = {
        "total": len(bot_files),
        "complete": len(results["complete_bots"]),
        "incomplete": len(results["incomplete_bots"]),
        "invalid": len(results["invalid_bots"]),
        "fixed": sum(1 for b in results["incomplete_bots"] if b["fixed"]) if auto_fix else 0
    }
    
    return results


def print_report(results: Dict, verbose: bool = False):
    """Print validation report."""
    print("\n" + "="*70)
    print("BOT CONFIGURATION VALIDATION REPORT")
    print("="*70)
    
    summary = results["summary"]
    print(f"\nSUMMARY:")
    print(f"  Total bots found: {summary['total']}")
    print(f"  Complete bots: {summary['complete']}")
    print(f"  Incomplete bots (head-only): {summary['incomplete']}")
    print(f"  Invalid bots: {summary['invalid']}")
    if summary.get('fixed', 0) > 0:
        print(f"  Fixed during scan: {summary['fixed']}")
    
    if results["incomplete_bots"]:
        print(f"\nINCOMPLETE BOTS (Missing subsystems):")
        print("-" * 70)
        for bot in results["incomplete_bots"]:
            print(f"\n   {Path(bot['file']).name}")
            print(f"     Type: {bot.get('creature_type', 'unknown')}")
            if bot["missing_subsystems"]:
                for missing in bot["missing_subsystems"]:
                    stem = Path(missing).stem
                    status = "  ADDED" if bot["fixed"] else ""
                    print(f"     Missing: {stem}{status}")
    
    if results["invalid_bots"]:
        print(f"\nINVALID BOTS:")
        print("-" * 70)
        for bot in results["invalid_bots"]:
            print(f"\n   {Path(bot['file']).name}")
            for issue in bot["issues"]:
                print(f"     {issue}")
    
    if verbose and results["complete_bots"]:
        print(f"\nCOMPLETE BOTS:")
        print("-" * 70)
        for bot in results["complete_bots"]:
            print(f"   {Path(bot['file']).name} ({bot.get('creature_type', 'unknown')})")
    
    print("\n" + "="*70)


if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Validate and repair bot configurations")
    parser.add_argument("--bots-dir", default="config/bots", help="Path to bots directory")
    parser.add_argument("--auto-fix", action="store_true", help="Automatically fix incomplete bots")
    parser.add_argument("--verbose", action="store_true", help="Show detailed output")
    
    args = parser.parse_args()
    
    bots_dir = Path(args.bots_dir)
    if not bots_dir.exists():
        print(f"Error: Bots directory not found: {bots_dir}")
        sys.exit(1)
    
    results = scan_all_bots(bots_dir, auto_fix=args.auto_fix)
    print_report(results, verbose=args.verbose)
    
    # Exit with error code if there are issues
    if results["invalid_bots"] or (results["incomplete_bots"] and not args.auto_fix):
        sys.exit(1)
