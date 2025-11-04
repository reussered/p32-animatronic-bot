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

    def _load_rules(self):
        if not self.rules_file.exists():
            print(f"ERROR: master_rules.json not found at {self.rules_file}")
            sys.exit(1)
        with self.rules_file.open("r", encoding="utf-8") as f:
            return json.load(f)

    def _save_rules(self):
        if self.master_files_dirty:
            print(" Saving updated master_rules.json...")
            with self.rules_file.open("w", encoding="utf-8") as f:
                json.dump(self.rules, f, indent=4)
            self.master_files_dirty = False

    def run(self):
        """Walks through the project and runs all validations."""
        print("[*] Updating master file list...")
        all_json_files = self._get_all_json_files()

        # New step: Check for and rename files with uppercase letters
        if self._rename_uppercase_json_files(all_json_files):
            # If files were renamed, we need to get the list again
            all_json_files = self._get_all_json_files()

        self._update_master_file_list(all_json_files)
        
        # Scan /shared directory for .hpp files
        self._scan_shared_classes()
        
        # Scan component .hdr files for STL includes
        self._scan_stl_includes(all_json_files)
        
        self._consolidate_master_rules_types()
        self._consolidate_component_types()
        self._migrate_required_fields()

        print("[*] Starting component validation...")
        
        overall_errors_found = False
        for file_path in all_json_files:
            if self._run_all_validations_for_file(file_path):
                overall_errors_found = True

        self._save_rules()

        if not overall_errors_found:
            print("\n Validation complete. No new errors found.")
        else:
            print("\n Validation complete. Errors were found and reported above.")

    def _get_all_json_files(self):
        """Returns a list of all .json files in the 'config' directory."""
        config_dir = self.root_dir / "config"

        config_files = list(config_dir.glob("**/*.json"))

        return config_files

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

    def _scan_shared_classes(self):
        """Scans /shared directory for .hpp files and updates required_includes.shared_classes."""
        shared_dir = self.root_dir / "shared"
        if not shared_dir.exists():
            return
        
        shared_classes = []
        for hpp_file in shared_dir.glob("*.hpp"):
            # Store relative path like "shared/ClassName.hpp"
            rel_path = hpp_file.relative_to(self.root_dir).as_posix()
            shared_classes.append(rel_path)
        
        shared_classes.sort()
        
        # Update master_rules.json
        if "required_includes" not in self.rules:
            self.rules["required_includes"] = {
                "static": [],
                "shared_classes": [],
                "stl_headers": []
            }
        
        if self.rules["required_includes"]["shared_classes"] != shared_classes:
            self.rules["required_includes"]["shared_classes"] = shared_classes
            self.master_files_dirty = True
            print(f"  -> Updated shared_classes list ({len(shared_classes)} files)")

    def _scan_stl_includes(self, json_files):
        """Scans component .hdr files for STL includes and updates required_includes.stl_headers."""
        stl_headers = set()
        stl_patterns = [
            "vector", "list", "map", "set", "unordered_map", "unordered_set",
            "queue", "deque", "stack", "string", "array", "tuple", "algorithm",
            "memory", "functional", "utility", "numeric", "chrono", "thread",
            "mutex", "condition_variable"
        ]
        
        for json_path in json_files:
            hdr_path = json_path.with_suffix(".hdr")
            if hdr_path.exists():
                try:
                    with hdr_path.open("r", encoding="ascii") as f:
                        content = f.read()
                        # Look for #include <stl_header>
                        import re
                        includes = re.findall(r'#include\s*<([^>]+)>', content)
                        for inc in includes:
                            # Check if it matches any STL pattern
                            if inc in stl_patterns:
                                stl_headers.add(inc)
                except Exception:
                    pass
        
        stl_headers_list = sorted(list(stl_headers))
        
        # Update master_rules.json
        if "required_includes" not in self.rules:
            self.rules["required_includes"] = {
                "static": [],
                "shared_classes": [],
                "stl_headers": []
            }
        
        if self.rules["required_includes"]["stl_headers"] != stl_headers_list:
            self.rules["required_includes"]["stl_headers"] = stl_headers_list
            self.master_files_dirty = True
            if stl_headers_list:
                print(f"  -> Updated stl_headers list ({len(stl_headers_list)} headers)")

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
        """Validates that all referenced component files exist."""
        errors = []
        if "components" in data and isinstance(data["components"], list):
            for ref in data["components"]:
                if isinstance(ref, str):
                    ref_path_str = re.sub(r"<.*>", "", ref)
                    ref_path = self.root_dir / ref_path_str
                    
                    if not ref_path.exists():
                        errors.append(f"Component reference '{ref_path_str}' not found.")
                        self._scaffold_component(ref_path)
                elif isinstance(ref, dict):
                    # This is an inline component definition, not a file reference.
                    pass
        return errors

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

    def _generate_display_driver_header(self, json_path: Path, component_name: str):
        """Generates a display driver .hdr file with namespace, Pixel struct, and CSS colors."""
        hdr_path = json_path.with_suffix(".hdr")
        name_upper = component_name.upper()
        
        hdr_content = f'''// {component_name} component header
// Defines data structures for {component_name} display
#ifndef {name_upper}_HDR
#define {name_upper}_HDR

#include <stdint.h>
#include <esp_err.h>

namespace {name_upper}
{{

// Forward declaration and global InitialPixel
struct Pixel;
extern Pixel InitialPixel;

struct Pixel
{{
    // Maximum values for each color channel (for mood calculations)
    static constexpr unsigned int maxRed = 0x1F;    // 5 bits = 31
    static constexpr unsigned int maxGreen = 0x3F;  // 6 bits = 63
    static constexpr unsigned int maxBlue = 0x1F;   // 5 bits = 31

    Pixel()
    {{
        *this = InitialPixel;
    }}
    unsigned int red : 5;
    unsigned int green : 6;
    unsigned int blue : 5;

    // Constructor from 8-bit RGB byte values (0-255 each)
    constexpr Pixel(uint8_t r8, uint8_t g8, uint8_t b8)
        : red(static_cast<unsigned int>((static_cast<float>(r8) / 255.0f) * maxRed)),
          green(static_cast<unsigned int>((static_cast<float>(g8) / 255.0f) * maxGreen)),
          blue(static_cast<unsigned int>((static_cast<float>(b8) / 255.0f) * maxBlue))
    {{}}

    // Assignment operator
    Pixel& operator=(const Pixel& other)
    {{
        red = other.red;
        green = other.green;
        blue = other.blue;
        return *this;
    }}

    // Add two pixels with saturation
    Pixel operator+(const Pixel& other) const
    {{
        Pixel result;
        unsigned int sum_r = red + other.red;
        result.red = (sum_r > maxRed) ? maxRed : sum_r;
        unsigned int sum_g = green + other.green;
        result.green = (sum_g > maxGreen) ? maxGreen : sum_g;
        unsigned int sum_b = blue + other.blue;
        result.blue = (sum_b > maxBlue) ? maxBlue : sum_b;
        return result;
    }}

    // Add-assign operator with saturation
    Pixel& operator+=(const Pixel& other)
    {{
        *this = *this + other;
        return *this;
    }}

    // Convert Pixel* buffer to uint8_t*
    static uint8_t* to_bytes(Pixel* pixel_buffer)
    {{
        return reinterpret_cast<uint8_t*>(pixel_buffer);
    }}

    // Convert uint8_t* buffer back to Pixel*
    static Pixel* from_bytes(uint8_t* byte_buffer)
    {{
        return reinterpret_cast<Pixel*>(byte_buffer);
    }}

    // Get size of a single Pixel in bytes
    static constexpr size_t getBytesPerPixel()
    {{
        return 2;  // RGB565 = 16 bits = 2 bytes
    }}
}};

// Display interface - compile-time sizing
static constexpr uint16_t getRowCount() {{ return 240; }}
static constexpr uint16_t getRowSize() {{ return 240; }}

// Init and act function prototypes
esp_err_t {component_name}_init(void);
void {component_name}_act(void);

}}  // namespace {name_upper}

#endif // {name_upper}_HDR
'''
        
        with hdr_path.open("w", encoding="utf-8") as f:
            f.write(hdr_content)
        
        print(f"  -> Generated display driver header for {component_name}")

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

        return "UNKNOWN"

    def _run_all_validations_for_file(self, file_path):
        """Read a JSON file and run all validation checks against it."""
        errors_found = False
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except json.JSONDecodeError as e:
            if 'Invalid escape' in str(e):
                print(f" Attempting to fix invalid escape sequences in {os.path.relpath(file_path, self.root_dir)}...")
                if self._fix_invalid_json_escapes(file_path):
                    try:
                        with open(file_path, 'r', encoding='utf-8') as f:
                            data = json.load(f)
                        print(f" Successfully fixed and reloaded {os.path.relpath(file_path, self.root_dir)}.")
                    except json.JSONDecodeError as e2:
                        print(f" ERROR: Still could not parse {os.path.relpath(file_path, self.root_dir)} after fix: {e2}")
                        return True
                else:
                    print(f" ERROR: Could not read or parse {os.path.relpath(file_path, self.root_dir)}: {e}")
                    return True
            else:
                print(f" ERROR: Could not read or parse {os.path.relpath(file_path, self.root_dir)}: {e}")
                return True
        except Exception as e:
            print(f" ERROR: Could not read file {os.path.relpath(file_path, self.root_dir)}: {e}")
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

        # --- Apply type equivalency mapping from master_rules ---
        type_equivalency_map = self.rules.get("type_equivalency_map", {})
        current_type = data.get("type", "").upper()
        
        # Check if current type is in any equivalency group
        for canonical_type, equivalent_types in type_equivalency_map.items():
            if current_type in equivalent_types:
                if current_type != canonical_type:
                    data["type"] = canonical_type
                    made_changes = True
                    print(f"  -> Consolidated type '{current_type}' to '{canonical_type}' for {os.path.relpath(file_path, self.root_dir)}")
                break

        # --- Generate display driver header if needed ---
        if data.get("type") == "DISPLAY_DRIVER":
            component_name = data.get("name")
            if component_name:
                self._generate_display_driver_header(Path(file_path), component_name)

        if made_changes:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=4)
                print(f" Automatically fixed fields in {os.path.relpath(file_path, self.root_dir)}")
            except Exception as e:
                errors.append(f"Could not write automatic fixes to file: {e}")

        # --- Run validations on (now fixed) data ---
        errors = []
        errors.extend(self._validate_filename_and_name_field(Path(file_path), data))
        errors.extend(self._validate_type_and_rules(Path(file_path), data))
        errors.extend(self._validate_components_array(file_path, data))

        if errors:
            print(f"\n--- Errors for: {os.path.relpath(file_path, self.root_dir)} ---")
            for error in errors:
                print(f"  - {error}")
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
                print(f"-> Renaming file: {file_path.name} -> {new_path.name}")
                try:
                    file_path.rename(new_path)
                    renamed_any = True
                except OSError as e:
                    print(f" ERROR: Could not rename {file_path.name}: {e}")
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

    def _consolidate_component_types(self):
        """
        Apply type equivalency mapping to all JSON files to consolidate duplicate types.
        This ensures all equivalent types map to their canonical forms.
        """
        type_equivalency_map = self.rules.get("type_equivalency_map", {})
        if not type_equivalency_map:
            return
        
        all_json_files = self._get_all_json_files()
        consolidated_count = 0
        
        for json_path in all_json_files:
            try:
                with json_path.open("r", encoding="utf-8") as f:
                    data = json.load(f)
            except Exception:
                continue
            
            current_type = data.get("type", "").upper()
            if not current_type:
                continue
            
            # Check if current type should be mapped to a canonical type
            for canonical_type, equivalent_types in type_equivalency_map.items():
                if current_type in equivalent_types and current_type != canonical_type:
                    data["type"] = canonical_type
                    try:
                        with json_path.open("w", encoding="utf-8") as f:
                            json.dump(data, f, indent=4)
                        consolidated_count += 1
                    except Exception:
                        pass
                    break
        
        if consolidated_count > 0:
            print(f"  -> Consolidated {consolidated_count} component types using equivalency map")

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
