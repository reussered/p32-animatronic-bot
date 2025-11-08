"""
Duplicate Resolution Tool
Uses file modification dates to identify which duplicates are truly redundant.
Marks older files for deletion, keeps newest version.
"""

import json
import os
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Tuple


def get_file_mtime(filepath: str) -> float:
    """Get file modification time as timestamp."""
    try:
        return os.path.getmtime(filepath)
    except FileNotFoundError:
        return 0


def format_mtime(timestamp: float) -> str:
    """Format timestamp to readable date."""
    if timestamp == 0:
        return "FILE NOT FOUND"
    return datetime.fromtimestamp(timestamp).strftime("%Y-%m-%d %H:%M:%S")


def analyze_duplicates(registry_path: str = "config/component_registry.json") -> Dict:
    """Analyze duplicates by modification date."""
    
    with open(registry_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    components = data["components"]
    
    # Group by name
    by_name = {}
    for comp in components:
        name = comp["name"]
        if name not in by_name:
            by_name[name] = []
        by_name[name].append(comp)
    
    # Find duplicates
    duplicates = {k: v for k, v in by_name.items() if len(v) > 1}
    
    # Analyze each duplicate group
    analysis = {}
    for name, entries in sorted(duplicates.items()):
        # Get modification times
        timestamps = []
        for entry in entries:
            path = entry["path"]
            mtime = get_file_mtime(path)
            timestamps.append({
                "path": path,
                "mtime": mtime,
                "mtime_str": format_mtime(mtime),
                "entry": entry
            })
        
        # Sort by modification time (newest last)
        timestamps.sort(key=lambda x: x["mtime"])
        
        # Newest is the one to keep
        newest = timestamps[-1]
        obsolete = timestamps[:-1]
        
        analysis[name] = {
            "count": len(entries),
            "keep": newest,
            "delete": obsolete,
            "all": timestamps
        }
    
    return analysis


def report_duplicates(analysis: Dict) -> None:
    """Print detailed duplicate analysis report."""
    print("\n" + "=" * 80)
    print("DUPLICATE ANALYSIS - BY MODIFICATION DATE")
    print("=" * 80)
    
    total_to_delete = 0
    
    for name in sorted(analysis.keys()):
        info = analysis[name]
        count = info["count"]
        
        print(f"\n{name} ({count} copies):")
        print("-" * 80)
        
        # Show all versions with dates
        for i, item in enumerate(info["all"]):
            marker = "KEEP" if item == info["keep"] else "DELETE"
            print(f"  [{marker:6}] {item['mtime_str']}  {item['path']}")
        
        if info["delete"]:
            total_to_delete += len(info["delete"])
    
    print("\n" + "=" * 80)
    print(f"SUMMARY: {len(analysis)} component names with duplicates")
    print(f"FILES TO DELETE: {total_to_delete}")
    print(f"REGISTRY ENTRIES TO REMOVE: {total_to_delete}")
    print("=" * 80)


def generate_deletion_script(analysis: Dict, output_file: str = "tools/delete_obsolete_components.ps1") -> None:
    """Generate PowerShell script to delete obsolete files and update registry."""
    
    script = """# Delete Obsolete Component Duplicates
# Auto-generated - deletes older versions, keeps newest by modification date

$DeletedCount = 0
$FailedCount = 0

Write-Host "Deleting obsolete component files..." -ForegroundColor Yellow
Write-Host "=" * 80

"""
    
    deletion_entries = []
    
    for name in sorted(analysis.keys()):
        info = analysis[name]
        
        for item in info["delete"]:
            path = item["path"]
            deletion_entries.append({
                "path": path,
                "name": name,
                "mtime_str": item["mtime_str"]
            })
            
            script += f"""# {name} (older version from {item['mtime_str']})
if (Test-Path "{path}") {{
    Remove-Item -Force "{path}" -ErrorAction SilentlyContinue
    if ($?) {{
        Write-Host "✓ Deleted: {path}" -ForegroundColor Green
        $DeletedCount++
    }} else {{
        Write-Host "✗ Failed:  {path}" -ForegroundColor Red
        $FailedCount++
    }}
}} else {{
    Write-Host "⊘ Not found: {path}" -ForegroundColor Gray
}}

"""
    
    script += f"""
Write-Host "=" * 80
Write-Host "Deleted: $DeletedCount files" -ForegroundColor Green
Write-Host "Failed:  $FailedCount files" -ForegroundColor Red
Write-Host "Next step: Update config/component_registry.json to remove obsolete entries"
"""
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(script)
    
    print(f"\n✓ Deletion script generated: {output_file}")
    print(f"  Will delete {len(deletion_entries)} obsolete files")
    
    return deletion_entries


def generate_registry_update(analysis: Dict, output_file: str = "tools/update_registry_remove_duplicates.py") -> None:
    """Generate script to update registry after deletion."""
    
    entries_to_remove = []
    for name in analysis.keys():
        info = analysis[name]
        for item in info["delete"]:
            entries_to_remove.append({
                "name": item["entry"]["name"],
                "path": item["entry"]["path"]
            })
    
    script = f'''"""
Update registry to remove obsolete duplicate entries.
Auto-generated from duplicate analysis.
Run AFTER deletion script completes.
"""

import json
from pathlib import Path

registry_path = "config/component_registry.json"

# Load current registry
with open(registry_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

# Entries to remove (obsolete duplicates)
to_remove = {json.dumps(entries_to_remove, indent=2)}

print(f"Removing {{len(to_remove)}} obsolete entries from registry...")

# Remove each entry
removed_count = 0
for entry_info in to_remove:
    name = entry_info["name"]
    path = entry_info["path"]
    
    # Find and remove
    for i, comp in enumerate(data["components"]):
        if comp["name"] == name and comp["path"] == path:
            del data["components"][i]
            print(f"✓ Removed: {{name}} @ {{path}}")
            removed_count += 1
            break

# Save updated registry
with open(registry_path, 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=2)

print(f"\\nRegistry updated: {{removed_count}} entries removed")
print(f"Total entries now: {{len(data['components'])}}")
'''
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(script)
    
    print(f"✓ Registry update script generated: {output_file}")


def main():
    print("Component Duplicate Resolution Tool")
    print("Analyzes by file modification date")
    
    # Analyze
    analysis = analyze_duplicates()
    
    if not analysis:
        print("\nNo duplicates found!")
        return
    
    # Report
    report_duplicates(analysis)
    
    # Generate scripts
    print("\nGenerating cleanup scripts...")
    generate_deletion_script(analysis)
    generate_registry_update(analysis)
    
    print("\nNext steps:")
    print("1. Review deletion script: tools/delete_obsolete_components.ps1")
    print("2. Review registry update: tools/update_registry_remove_duplicates.py")
    print("3. Run deletion script: .\\tools\\delete_obsolete_components.ps1")
    print("4. Run registry update: python tools/update_registry_remove_duplicates.py")


if __name__ == "__main__":
    main()
