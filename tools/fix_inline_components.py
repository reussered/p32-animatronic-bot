#!/usr/bin/env python3
"""
Fix inline components by converting them to proper file-based components.
RULE 4: Component pipeline model - all components must be separate .json files
RULE 6: File organization standards - components must have .src and .hdr files
"""

import json
from pathlib import Path
import sys


def fix_inline_components(config_dir: Path) -> dict:
    """
    Walks config/ and fixes inline components by extracting them to separate files.
    Returns summary of changes.
    """
    summary = {
        "files_processed": 0,
        "inline_components_fixed": 0,
        "files_modified": 0,
        "errors": [],
    }
    
    # Find all JSON files in config/
    for json_file in sorted(config_dir.glob("**/*.json")):
        try:
            with json_file.open("r", encoding="utf-8") as f:
                data = json.load(f)
        except Exception as e:
            summary["errors"].append(f"Could not read {json_file}: {e}")
            continue
        
        summary["files_processed"] += 1
        
        if "components" not in data or not isinstance(data["components"], list):
            continue
        
        # Check for inline components
        has_inline = any(isinstance(c, dict) for c in data["components"])
        if not has_inline:
            continue
        
        print(f"\n[PROCESSING] {json_file.relative_to(config_dir.parent)}")
        
        # Extract inline components to separate files
        new_components = []
        for idx, component in enumerate(data["components"]):
            if isinstance(component, dict):
                # This is an inline component - extract it
                component_name = component.get("name", component.get("component_type", f"inline_component_{idx}"))
                
                # Determine target directory
                target_dir = json_file.parent / "components"
                target_dir.mkdir(parents=True, exist_ok=True)
                
                json_path = target_dir / f"{component_name}.json"
                
                # Don't overwrite existing files
                if json_path.exists():
                    print(f"  -> SKIP: {component_name} (file exists)")
                    new_components.append(component)  # Keep original
                    continue
                
                # Create component JSON
                component_data = {
                    "name": component_name,
                    "type": component.get("component_type", "MIGRATED_INLINE"),
                    "description": f"Auto-migrated from inline component",
                    **{k: v for k, v in component.items() if k not in ["name", "component_type"]}
                }
                
                # Write JSON file
                with json_path.open("w", encoding="utf-8") as f:
                    json.dump(component_data, f, indent=4)
                
                print(f"  -> CREATED: {component_name}.json")
                
                # Create stub .hdr file
                hdr_path = json_path.with_suffix(".hdr")
                hdr_content = (
                    f"// Auto-generated header for {component_name}\n"
                    f"// Migrated from inline component definition\n"
                    f"#include <esp_err.h>\n\n"
                    f"esp_err_t {component_name}_init(void);\n"
                    f"void {component_name}_act(void);\n"
                )
                with hdr_path.open("w", encoding="utf-8") as f:
                    f.write(hdr_content)
                
                # Create stub .src file
                src_path = json_path.with_suffix(".src")
                src_content = (
                    f"// Auto-generated source for {component_name}\n"
                    f"// Migrated from inline component definition\n"
                    f"#include \"{component_name}.hdr\"\n"
                    f"#include <esp_log.h>\n\n"
                    f"static const char* TAG = \"{component_name}\";\n\n"
                    f"esp_err_t {component_name}_init(void)\n{{\n"
                    f"    ESP_LOGI(TAG, \"init executed\");\n"
                    f"    return ESP_OK;\n"
                    f"}}\n\n"
                    f"void {component_name}_act(void)\n{{\n"
                    f"    ESP_LOGI(TAG, \"act executed\");\n"
                    f"}}\n"
                )
                with src_path.open("w", encoding="utf-8") as f:
                    f.write(src_content)
                
                # Add file reference to new_components array
                new_components.append(str(json_path.relative_to(config_dir.parent)).replace("\\", "/"))
                summary["inline_components_fixed"] += 1
            else:
                # Keep string references as-is
                new_components.append(component)
        
        # Update the parent JSON if any inline components were fixed
        if len(new_components) != len(data["components"]) or any(
            isinstance(new_components[i], str) and isinstance(data["components"][i], dict)
            for i in range(len(data["components"]))
        ):
            data["components"] = new_components
            
            with json_file.open("w", encoding="utf-8") as f:
                json.dump(data, f, indent=4)
            
            print(f"  -> UPDATED: Parent JSON with new component references")
            summary["files_modified"] += 1
    
    return summary


def main():
    root_dir = Path(__file__).parent.parent.resolve()
    config_dir = root_dir / "config"
    
    if not config_dir.exists():
        print(f"ERROR: config/ directory not found at {config_dir}")
        sys.exit(1)
    
    print("[INFO] Converting inline components to file-based components...")
    print(f"[INFO] Processing: {config_dir}")
    
    summary = fix_inline_components(config_dir)
    
    print(f"\n[SUMMARY]")
    print(f"  - Files processed: {summary['files_processed']}")
    print(f"  - Inline components fixed: {summary['inline_components_fixed']}")
    print(f"  - Parent files modified: {summary['files_modified']}")
    
    if summary["errors"]:
        print(f"\n[ERRORS]")
        for error in summary["errors"]:
            print(f"  - {error}")
    
    if summary["inline_components_fixed"] > 0:
        print(f"\n[SUCCESS] Fixed {summary['inline_components_fixed']} inline components!")
        print(f"[NEXT] Run: python tools/validate_file_structure.py")
    else:
        print(f"\n[INFO] No inline components found to fix.")


if __name__ == "__main__":
    main()
