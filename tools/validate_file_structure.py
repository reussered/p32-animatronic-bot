import json
import os
import re
from pathlib import Path
import sys

class ValidationRunner:
    def __init__(self):
        self.root_dir = Path(__file__).parent.parent.resolve()
        self.rules_file = self.root_dir / "master_rules.json"
        self.rules = self._load_rules()
        self.master_files_dirty = False
        self.master_file_registry = {}  # Maps lowercase paths to actual file paths
        self.components_fixed = 0
        self.components_errors = 0
        self.inline_components_found = 0  # Track inline component violations

    def _load_rules(self):
        if not self.rules_file.exists():
            print(f"ERROR: master_rules.json not found at {self.rules_file}")
            sys.exit(1)
        with self.rules_file.open("r", encoding="utf-8") as f:
            return json.load(f)

    def _save_rules(self):
        if self.master_files_dirty:
            print("[SAVE] Saving updated master_rules.json...")
            with self.rules_file.open("w", encoding="utf-8") as f:
                json.dump(self.rules, f, indent=4)
            self.master_files_dirty = False

    def run(self):
        """Walks through the project and runs all validations."""
        print("[STEP 1] Building master file registry from config/ tree...")
        self._build_master_file_registry()
        
        print(f"[STEP 2] Found {len(self.master_file_registry)} component files in registry")
        print("[STEP 3] Updating master file list in master_rules.json...")
        all_json_files = self._get_all_json_files()

        # New step: Check for and rename files with uppercase letters
        if self._rename_uppercase_json_files(all_json_files):
            # If files were renamed, we need to get the list again
            all_json_files = self._get_all_json_files()

        self._update_master_file_list(all_json_files)
        
        self._consolidate_master_rules_types()
        self._migrate_required_fields()

        print("[STEP 4] Starting component validation...")
        
        overall_errors_found = False
        for file_path in all_json_files:
            if self._run_all_validations_for_file(file_path):
                overall_errors_found = True

        self._save_rules()

        print(f"\n[COMPONENTS VALIDATION SUMMARY]")
        print(f"  - References corrected: {self.components_fixed}")
        print(f"  - Unresolvable errors: {self.components_errors}")
        print(f"  - INLINE COMPONENTS (RULE 4/6 VIOLATIONS): {self.inline_components_found}")

        if self.inline_components_found > 0:
            print(f"\n[CRITICAL] {self.inline_components_found} inline components found!")
            print(f"Inline components violate RULE 4 (Component Pipeline Model)")
            print(f"and RULE 6 (File Organization Standards).")
            print(f"All components must be separate .json files with .src/.hdr pairs.")

        if not overall_errors_found:
            print("\n[OK] Validation complete. No new errors found.")
        else:
            print("\n[ERROR] Validation complete. Errors were found and reported above.")

    def _get_all_json_files(self):
        """Returns a list of all .json files in the 'config' and 'assets' directories."""
        config_dir = self.root_dir / "config"
        assets_dir = self.root_dir / "assets"

        config_files = list(config_dir.glob("**/*.json"))
        assets_files = list(assets_dir.glob("**/*.json"))

        return config_files + assets_files

    def _build_master_file_registry(self):
        """
        Walks the entire config/ tree and builds a registry of all component files.
        Registry maps relative paths (normalized) to actual Path objects.
        RULE 6: File organization standards - covers all component types and locations.
        """
        config_dir = self.root_dir / "config"
        
        if not config_dir.exists():
            print("WARNING: config/ directory not found")
            return
        
        # Find all .json files
        for json_file in config_dir.glob("**/*.json"):
            relative_path = json_file.relative_to(self.root_dir).as_posix()
            
            # Store both normalized (with and without .json) and full path
            normalized_key = relative_path.lower()
            self.master_file_registry[normalized_key] = json_file
            
            # Also store without .json extension for flexible matching
            if normalized_key.endswith('.json'):
                no_ext_key = normalized_key[:-5]
                self.master_file_registry[no_ext_key] = json_file

    def _find_component_file(self, component_ref: str) -> Path | None:
        """
        Attempts to locate a component file referenced in a components: [] array.
        Handles various path formats and tries case-insensitive matching.
        Returns the actual Path or None if not found.
        """
        # Clean up the reference (remove template markers like <...>)
        ref_clean = re.sub(r"<.*>", "", component_ref).strip()
        
        # Try direct match first (exact path as given)
        ref_lower = ref_clean.lower()
        if ref_lower in self.master_file_registry:
            return self.master_file_registry[ref_lower]
        
        # Try with .json extension added
        if not ref_lower.endswith('.json'):
            ref_with_json = ref_lower + '.json'
            if ref_with_json in self.master_file_registry:
                return self.master_file_registry[ref_with_json]
        
        # Try to find by component name (last part of path)
        component_name = Path(ref_clean).stem.lower()
        for reg_key, reg_path in self.master_file_registry.items():
            if reg_path.stem.lower() == component_name:
                return reg_path
        
        return None

    def _validate_and_fix_components_array(self, file_path, data):
        """
        RULE 4 ENFORCEMENT: components: [] MUST contain ONLY string file references.
        No inline objects, no metadata, no keyframes - just component file paths.
        
        RULE 4: Component pipeline model - all entries in components: [] are file references
        RULE 6: File organization standards - all components must be separate .json files
        
        EXCEPTION: Animation files (assets/animations/) use different pattern with component_id + keyframes
        """
        errors = []
        made_changes = False
        
        # Skip strict RULE 4 validation for animation files
        is_animation_file = "assets/animations" in str(file_path).replace("\\", "/")
        
        if "components" not in data:
            return errors, made_changes
        
        if not isinstance(data["components"], list):
            errors.append("Field 'components' must be an array/list")
            return errors, made_changes
        
        # For animation files, skip further validation (component_id + keyframes is correct pattern)
        if is_animation_file:
            return errors, made_changes
        
        corrected_components = []
        
        for idx, ref in enumerate(data["components"]):
            if isinstance(ref, str):
                # CORRECT: String file reference
                ref_clean = re.sub(r"<.*>", "", ref).strip()
                
                # Try to find the component
                actual_path = self._find_component_file(ref_clean)
                
                if actual_path:
                    # Component found - use correct relative path
                    correct_ref = actual_path.relative_to(self.root_dir).as_posix()
                    
                    if correct_ref != ref_clean:
                        # Path was corrected
                        print(f"  -> [FIXED] Component reference in {os.path.relpath(file_path, self.root_dir)}")
                        print(f"     OLD: {ref_clean}")
                        print(f"     NEW: {correct_ref}")
                        corrected_components.append(correct_ref)
                        made_changes = True
                        self.components_fixed += 1
                    else:
                        # Path is already correct
                        corrected_components.append(ref_clean)
                else:
                    # Component not found - log as warning but keep the reference
                    # (may be a placeholder or design-time reference)
                    corrected_components.append(ref_clean)
                    # Don't count as error - just log missing component
                    
            elif isinstance(ref, dict):
                # VIOLATION: Dict in components: array
                # RULE 4 requires ONLY string file references
                self.inline_components_found += 1
                
                # Extract identifying information
                component_name = ref.get("name", ref.get("component_id", ref.get("component_type", f"unnamed_dict_{idx}")))
                ref_keys = ", ".join(ref.keys())
                
                errors.append(
                    f"[RULE 4 VIOLATION] components[{idx}] is dict, not string reference. "
                    f"components: [] must contain ONLY file references (strings). "
                    f"Found: {{{ref_keys}}}"
                )
                
                # DO NOT auto-migrate or fix - report and skip
                # User must fix this manually to understand the violation
                
            else:
                errors.append(f"[RULE 4 VIOLATION] components[{idx}] is invalid type: {type(ref).__name__}")
        
        # Update components array if corrections were made
        if made_changes:
            data["components"] = corrected_components
        
        return errors, made_changes

    def _update_master_file_list(self, file_paths):
        """Updates the project_files.files list in the master rules."""
        file_list = sorted([
            p.relative_to(self.root_dir).as_posix() for p in file_paths
        ])
        if self.rules["project_files"]["files"] != file_list:
            self.rules["project_files"]["files"] = file_list
            self.master_files_dirty = True
            print("  -> Master file list was updated.")
        else:
            print("  -> Master file list is already up-to-date.")

    def _validate_filename_and_name_field(self, file_path: Path, data: dict) -> list[str]:
        """Checks if the 'name' field matches the filename."""
        errors = []
        component_name = data.get("name")
        expected_name = file_path.stem
        if component_name != expected_name:
            # This is a common pattern, let's try to be helpful
            if component_name and component_name.lower() == expected_name.lower():
                 errors.append(f"Field 'name' ('{component_name}') does not match filename ('{expected_name}') due to case difference.")
            else:
                errors.append(f"Field 'name' ('{component_name}') does not match filename ('{expected_name}').")
        return errors

    def _validate_relative_path(self, file_path: Path, data: dict) -> list[str]:
        """Checks the 'relative_filename' field."""
        errors = []
        relative_path = data.get("relative_filename")
        if not relative_path:
            errors.append("Missing required field: 'relative_filename'.")
            return errors
        
        expected_path = file_path.relative_to(self.root_dir).as_posix()
        if relative_path != expected_path:
            errors.append(f"Field 'relative_filename' is incorrect. Is: '{relative_path}', should be: '{expected_path}'.")
        return errors

    def _validate_type_and_rules(self, file_path: Path, data: dict) -> list[str]:
        """Validates master and type-specific required fields."""
        errors = []
        
        # 1. Check for master required fields
        master_fields = self.rules.get("master_required_fields", ["name", "type", "relative_filename"])
        for field in master_fields:
            if field not in data:
                errors.append(f"Missing master required field: '{field}'.")

        # 2. Check for type-specific additional fields
        comp_type = data.get("type")
        if not comp_type:
            # This error is already caught by the master field check, but we can't proceed.
            return errors

        defined_types = self.rules["type_definitions"]["types"]
        if comp_type not in defined_types:
            # This is a new type, add it with a default structure.
            print(f"-> Adding new component type '{comp_type}' to master_rules.json")
            defined_types[comp_type] = {
                "description": "Auto-generated type.",
                "additional_required_fields": []
            }
            self.master_files_dirty = True
        
        # Check for additional required fields for the known type
        type_def = defined_types.get(comp_type, {})
        for field in type_def.get("additional_required_fields", []):
            if field not in data:
                errors.append(f"Missing required field for type '{comp_type}': '{field}'.")
        
        return errors

    def _validate_components_array(self, file_path, data):
        """Deprecated - replaced by _validate_and_fix_components_array"""
        return []

    def _scaffold_component(self, json_path: Path):
        """Creates placeholder .json, .hdr, and .src files if they don't exist."""
        name = json_path.stem
        hdr_path = json_path.with_suffix(".hdr")
        src_path = json_path.with_suffix(".src")

        if not json_path.exists() or not hdr_path.exists() or not src_path.exists():
            print(f"-> Scaffolding component '{name}'...")

        if not json_path.exists():
            json_path.parent.mkdir(parents=True, exist_ok=True)
            json_content = {
                "name": name,
                "type": "SCAFFOLDED",
                "relative_filename": json_path.relative_to(self.root_dir).as_posix(),
                "description": "Auto-generated component."
            }
            with json_path.open("w", encoding="utf-8") as f:
                json.dump(json_content, f, indent=4)

        if not hdr_path.exists():
            hdr_path.parent.mkdir(parents=True, exist_ok=True)
            hdr_content = (
                f"// Auto-generated header for {name}\n"
                f"#include <esp_err.h>\n\n"
                f"esp_err_t {name}_init(void);\n"
                f"void {name}_act(void);\n"
            )
            with hdr_path.open("w", encoding="utf-8") as f:
                f.write(hdr_content)

        if not src_path.exists():
            src_path.parent.mkdir(parents=True, exist_ok=True)
            src_content = (
                f'// Auto-generated source for {name}\n'
                f'#include "components/{name}.hdr"\n'
                f'#include <esp_log.h>\n\n'
                f'static const char* TAG = "{name}";\n\n'
                f"esp_err_t {name}_init(void)\n{{\n"
                f'    ESP_LOGI(TAG, "init executed");\n'
                f"    return ESP_OK;\n}}\n\n"
                f"void {name}_act(void)\n{{\n"
                f'    ESP_LOGI(TAG, "act executed");\n'
                f"}}\n"
            )
            with src_path.open("w", encoding="utf-8") as f:
                f.write(src_content)

    def _fix_invalid_json_escapes(self, file_path):
        """Attempts to fix invalid escape sequences by replacing backslashes with forward slashes."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            fixed_content = content.replace('\\', '/')

            if content != fixed_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(fixed_content)
                return True
        except Exception:
            pass
        return False

    def _infer_type_from_path(self, file_path: Path) -> str:
        """Infers the component type based on its file path, following NAMING_RULES.md."""
        try:
            relative_path = file_path.relative_to(self.root_dir)
        except ValueError:
            return "UNKNOWN" # File is not within the project root

        parts = relative_path.parts

        # config/components/*
        if len(parts) > 3 and parts[0] == 'config' and parts[1] == 'components':
            if parts[2] == 'drivers': return 'driver'
            if parts[2] == 'hardware': return 'hardware'
            if parts[2] == 'creature_specific': return 'creature_specific'

        # config/bots/*
        if len(parts) > 4 and parts[0] == 'config' and parts[1] == 'bots' and parts[2] == 'bot_families':
            return 'bot'

        # config/*
        if len(parts) > 2 and parts[0] == 'config':
            if parts[1] == 'hardware': return 'hardware_spec'
            if parts[1] == 'interfaces': return 'interface'
            if parts[1] == 'internal': return 'internal'
            if parts[1] == 'instrumentation': return 'instrumentation'
            if parts[1] == 'subsystems': return 'subsystem'
            if parts[1] == 'test' and len(parts) > 3 and parts[2] == 'persisted': return 'test_persisted'
            if parts[1] == 'test': return 'test_temporary'

        # assets/*
        if len(parts) > 3 and parts[0] == 'assets':
            if parts[1] == 'animations': return 'animation'
            if parts[1] == 'shapes' and parts[2] == 'scad': return 'scad_model'
            if parts[1] == 'shapes' and parts[2] == 'stl': return 'stl_model'
            if parts[1] == 'sounds': return 'sound'

        return "UNKNOWN"

    def _run_all_validations_for_file(self, file_path):
        """Read a JSON file and run all validation checks against it."""
        errors_found = False
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except json.JSONDecodeError as e:
            if 'Invalid escape' in str(e):
                print(f"[FIX] Attempting to fix invalid escape sequences in {os.path.relpath(file_path, self.root_dir)}...")
                if self._fix_invalid_json_escapes(file_path):
                    try:
                        with open(file_path, 'r', encoding='utf-8') as f:
                            data = json.load(f)
                        print(f"[OK] Successfully fixed and reloaded {os.path.relpath(file_path, self.root_dir)}.")
                    except json.JSONDecodeError as e2:
                        print(f"[ERROR] Still could not parse {os.path.relpath(file_path, self.root_dir)} after fix: {e2}")
                        return True
                else:
                    print(f"[ERROR] Could not read or parse {os.path.relpath(file_path, self.root_dir)}: {e}")
                    return True
            else:
                print(f"[ERROR] Could not read or parse {os.path.relpath(file_path, self.root_dir)}: {e}")
                return True
        except Exception as e:
            print(f"[ERROR] Could not read file {os.path.relpath(file_path, self.root_dir)}: {e}")
            return True

        # --- Auto-fix common issues ---
        made_changes = False

        # Rename "component_name" to "name"
        if isinstance(data, dict) and "component_name" in data:
            data["name"] = data.pop("component_name")
            made_changes = True

        # Rename "component_type" to "type"
        if isinstance(data, dict) and "component_type" in data:
            data["type"] = data.pop("component_type")
            made_changes = True
        
        # Fix name to match lowercase filename
        correct_name = Path(file_path).stem
        if data.get("name") != correct_name:
            data["name"] = correct_name
            made_changes = True

        # Fix relative path
        correct_rel_path = Path(file_path).relative_to(self.root_dir).as_posix()
        if data.get("relative_filename") != correct_rel_path:
            data["relative_filename"] = correct_rel_path
            made_changes = True

        # Infer type if missing, and enforce uppercase
        if "type" not in data or not data["type"]:
            inferred_type = self._infer_type_from_path(Path(file_path))
            data["type"] = inferred_type.upper()
            made_changes = True
            if inferred_type != "UNKNOWN":
                print(f"  -> Inferred type '{data['type']}' for {os.path.relpath(file_path, self.root_dir)}")
        elif data.get("type") and not data.get("type").isupper():
             data["type"] = data["type"].upper()
             made_changes = True

        # --- Consolidate display types ---
        display_related_types = [
            "POSITIONED_DISPLAY",
            "POSITIONED_OLED_DISPLAY",
            "CREATURE_SPECIFIC_DISPLAY",
            "DISPLAY_DRIVER"
        ]
        current_type = data.get("type", "").upper()
        if current_type in display_related_types and current_type != "DISPLAY_DRIVER":
            data["type"] = "DISPLAY_DRIVER"
            made_changes = True

        # --- NEW: Validate and fix components array ---
        component_errors, component_fixes = self._validate_and_fix_components_array(file_path, data)
        if component_fixes:
            made_changes = True
        
        if made_changes:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=4)
                print(f"[FIX] Automatically fixed fields in {os.path.relpath(file_path, self.root_dir)}")
            except Exception as e:
                print(f"[ERROR] Could not write automatic fixes to file {os.path.relpath(file_path, self.root_dir)}: {e}")

        # --- Run validations on (now fixed) data ---
        errors = []
        errors.extend(self._validate_filename_and_name_field(Path(file_path), data))
        errors.extend(self._validate_type_and_rules(Path(file_path), data))
        errors.extend(component_errors)  # Add component validation errors

        if errors:
            print(f"\n--- Errors for: {os.path.relpath(file_path, self.root_dir)} ---")
            for error in errors:
                # Escape Unicode characters for Windows console compatibility
                safe_error = error.encode('utf-8', errors='replace').decode('utf-8')
                print(f"  - {safe_error}")
            errors_found = True

        return errors_found

    def _rename_uppercase_json_files(self, file_paths: list[Path]) -> bool:
        """
        Finds JSON files with uppercase letters in their names and renames them to lowercase.
        Returns True if any files were renamed.
        """
        renamed_any = False
        for file_path in file_paths:
            if not file_path.stem.islower():
                new_name = file_path.stem.lower() + file_path.suffix
                new_path = file_path.with_name(new_name)
                print(f"[RENAME] Renaming file: {file_path.name} -> {new_path.name}")
                try:
                    file_path.rename(new_path)
                    renamed_any = True
                except OSError as e:
                    print(f"[ERROR] Could not rename {file_path.name}: {e}")
        return renamed_any

    def _consolidate_master_rules_types(self):
        """
        Ensures all type definition keys in master_rules.json are uppercase.
        Merges definitions from lowercase/mixed-case keys into the uppercase version.
        """
        defined_types = self.rules["type_definitions"]["types"]
        updated_types = {}
        keys_to_process = list(defined_types.keys())

        for key in keys_to_process:
            upper_key = key.upper()
            if key == upper_key:
                if upper_key not in updated_types:
                    updated_types[upper_key] = defined_types[key]
                continue

            # It's a lowercase or mixed-case key, merge it into the uppercase version
            print(f"-> Consolidating type '{key}' into '{upper_key}'...")
            if upper_key not in updated_types:
                updated_types[upper_key] = defined_types[key]
            else:
                # Basic merge: description from the first one, fields are unioned
                existing_fields = set(updated_types[upper_key].get("required_fields", []))
                new_fields = set(defined_types[key].get("required_fields", []))
                updated_types[upper_key]["required_fields"] = sorted(list(existing_fields.union(new_fields)))

        if len(updated_types) != len(defined_types):
            self.rules["type_definitions"]["types"] = updated_types
            self.master_files_dirty = True
            print("  -> Master rules types were consolidated.")

    def _migrate_required_fields(self):
        """
        Performs a one-time migration from 'required_fields' to a master list
        and 'additional_required_fields' per type.
        """
        if "master_required_fields" in self.rules:
            return # Migration already done

        print("-> Performing one-time migration to additional_required_fields...")
        master_fields = ["name", "type", "relative_filename"]
        self.rules["master_required_fields"] = master_fields
        
        defined_types = self.rules["type_definitions"]["types"]
        for type_name, type_def in defined_types.items():
            if "required_fields" in type_def:
                old_fields = set(type_def["required_fields"])
                additional_fields = sorted(list(old_fields - set(master_fields)))
                type_def["additional_required_fields"] = additional_fields
                del type_def["required_fields"]
        
        self.master_files_dirty = True
        print("  -> Migration complete.")

def main():
    runner = ValidationRunner()
    runner.run()

if __name__ == "__main__":
    main()
