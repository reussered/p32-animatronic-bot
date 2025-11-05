#!/usr/bin/env python3
"""
Ensures every component JSON file has associated .src and .hdr files.
Creates scaffolding files if they don't exist.
Per copilot-instructions: "every single src and hdr file belong in the same directory as their json file"
"""

import json
import sys
from pathlib import Path
from typing import List, Tuple

PROJECT_ROOT = Path(__file__).parent.parent


def generate_scaffolding_hdr(component_name: str, json_path: Path) -> str:
    """Generate scaffolding .hdr file content."""
    guard = f"{component_name.upper()}_HDR"
    init_func = f"{component_name}_init"
    act_func = f"{component_name}_act"
    
    return f"""#ifndef {guard}
#define {guard}

#include <esp_err.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {{
#endif

/**
 * @brief Initialize {component_name} component
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t {init_func}(void);

/**
 * @brief Execute {component_name} component action
 * Called periodically by subsystem dispatcher
 */
void {act_func}(void);

#ifdef __cplusplus
}}
#endif

#endif // {guard}
"""


def generate_scaffolding_src(component_name: str, json_path: Path) -> str:
    """Generate scaffolding .src file content."""
    init_func = f"{component_name}_init"
    act_func = f"{component_name}_act"
    
    return f"""#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @file {json_path.stem}.src
 * @brief Implementation for {component_name} component
 * 
 * TODO: Implement component initialization and action logic
 */

/**
 * @brief Initialize {component_name}
 * Performs any setup needed for this component
 */
esp_err_t {init_func}(void)
{{
    // TODO: Implement initialization
    return ESP_OK;
}}

/**
 * @brief Execute {component_name} action
 * Called periodically during subsystem dispatch
 */
void {act_func}(void)
{{
    // TODO: Implement component behavior
}}
"""


def check_and_scaffold_component(json_path: Path) -> Tuple[bool, str]:
    """
    Check if a component JSON file has matching .src and .hdr files.
    Create scaffolding if missing.
    
    Returns: (all_exist, status_message)
    """
    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except (json.JSONDecodeError, UnicodeDecodeError) as e:
        return False, f"ERROR: Invalid JSON: {e}"
    
    component_name = data.get('name', json_path.stem)
    hdr_path = json_path.with_suffix('.hdr')
    src_path = json_path.with_suffix('.src')
    
    hdr_exists = hdr_path.exists()
    src_exists = src_path.exists()
    
    created = []
    
    # Create .hdr if missing
    if not hdr_exists:
        try:
            hdr_content = generate_scaffolding_hdr(component_name, json_path)
            hdr_path.write_text(hdr_content, encoding='utf-8')
            created.append('.hdr')
        except Exception as e:
            return False, f"ERROR: Failed to create .hdr: {e}"
    
    # Create .src if missing
    if not src_exists:
        try:
            src_content = generate_scaffolding_src(component_name, json_path)
            src_path.write_text(src_content, encoding='utf-8')
            created.append('.src')
        except Exception as e:
            return False, f"ERROR: Failed to create .src: {e}"
    
    if created:
        return True, f"CREATED: {', '.join(created)}"
    else:
        return True, "OK (all files exist)"


def main():
    """Main entry point."""
    print("Scanning component JSON files for missing .src and .hdr files...\n")
    
    components_dir = PROJECT_ROOT / "config" / "components"
    json_files = sorted(components_dir.rglob("*.json"))
    
    print(f"Found {len(json_files)} component JSON files\n")
    
    created_count = 0
    error_count = 0
    ok_count = 0
    
    for json_path in json_files:
        success, message = check_and_scaffold_component(json_path)
        
        if success:
            ok_count += 1
            if "CREATED" in message:
                created_count += 1
                rel_path = json_path.relative_to(PROJECT_ROOT)
                print(f"[OK] {rel_path}: {message}")
        else:
            error_count += 1
            rel_path = json_path.relative_to(PROJECT_ROOT)
            print(f"[ERROR] {rel_path}: {message}")
    
    print(f"\n{'='*80}")
    print(f"Summary:")
    print(f"  Total components: {len(json_files)}")
    print(f"  Created scaffolding: {created_count}")
    print(f"  Already complete: {ok_count - created_count}")
    print(f"  Errors: {error_count}")
    print(f"{'='*80}")
    
    return 0 if error_count == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
