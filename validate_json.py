#!/usr/bin/env python3
"""
JSON Validator for P32 Animatronic Bot Assets

This script validates all JSON files in the assets folder recursively,
checking for:
1. Valid JSON syntax
2. Consistent schema based on file type
3. Required fields presence
4. Data type validation
5. Value range validation
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Tuple


class JSONValidator:
    """Validator for animatronic bot JSON configuration files"""
    
    # Define schemas for different file types
    SCHEMAS = {
        "mood": {
            "required_fields": ["name", "type", "description", "priority", "triggers", "behaviors", "led_color", "servo_settings"],
            "field_types": {
                "name": str,
                "type": str,
                "description": str,
                "priority": int,
                "triggers": list,
                "behaviors": list,
                "led_color": str,
                "servo_settings": dict
            },
            "nested_schemas": {
                "servo_settings": {
                    "required_fields": ["speed", "range"],
                    "field_types": {
                        "speed": int,
                        "range": int
                    }
                }
            },
            "value_constraints": {
                "priority": (1, 10),
                "servo_settings.speed": (0, 100),
                "servo_settings.range": (0, 180)
            }
        },
        "behavior": {
            "required_fields": ["name", "type", "description", "duration_ms", "servos"],
            "field_types": {
                "name": str,
                "type": str,
                "description": str,
                "duration_ms": int,
                "servos": list,
                "sound_effect": (str, type(None))
            },
            "list_item_schemas": {
                "servos": {
                    "required_fields": ["id", "position", "time_ms"],
                    "field_types": {
                        "id": int,
                        "position": int,
                        "time_ms": int
                    }
                }
            },
            "value_constraints": {
                "duration_ms": (0, 10000)
            }
        },
        "animation": {
            "required_fields": ["name", "type", "description", "duration_ms", "loop", "frames"],
            "field_types": {
                "name": str,
                "type": str,
                "description": str,
                "duration_ms": int,
                "loop": bool,
                "frames": list
            },
            "list_item_schemas": {
                "frames": {
                    "required_fields": ["time_ms", "leds"],
                    "field_types": {
                        "time_ms": int,
                        "leds": list
                    }
                }
            }
        }
    }
    
    def __init__(self, assets_path: str):
        self.assets_path = Path(assets_path)
        self.errors: List[str] = []
        self.warnings: List[str] = []
        self.validated_files: List[str] = []
        
    def validate_all(self) -> bool:
        """Validate all JSON files in the assets folder recursively"""
        if not self.assets_path.exists():
            self.errors.append(f"Assets path does not exist: {self.assets_path}")
            return False
            
        json_files = list(self.assets_path.rglob("*.json"))
        
        if not json_files:
            self.warnings.append(f"No JSON files found in {self.assets_path}")
            return True
            
        print(f"Found {len(json_files)} JSON file(s) to validate...")
        
        all_valid = True
        for json_file in json_files:
            if not self.validate_file(json_file):
                all_valid = False
                
        return all_valid
        
    def validate_file(self, file_path: Path) -> bool:
        """Validate a single JSON file"""
        relative_path = file_path.relative_to(self.assets_path)
        print(f"\nValidating: {relative_path}")
        
        # Check JSON syntax
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
        except json.JSONDecodeError as e:
            self.errors.append(f"{relative_path}: Invalid JSON syntax - {e}")
            return False
        except Exception as e:
            self.errors.append(f"{relative_path}: Error reading file - {e}")
            return False
            
        # Validate structure based on type
        if not isinstance(data, dict):
            self.errors.append(f"{relative_path}: Root element must be a JSON object")
            return False
            
        # Check if 'type' field exists
        if "type" not in data:
            self.errors.append(f"{relative_path}: Missing required field 'type'")
            return False
            
        file_type = data["type"]
        if file_type not in self.SCHEMAS:
            self.warnings.append(f"{relative_path}: Unknown type '{file_type}', skipping schema validation")
            self.validated_files.append(str(relative_path))
            return True
            
        # Validate against schema
        schema = self.SCHEMAS[file_type]
        if not self.validate_schema(data, schema, str(relative_path)):
            return False
            
        # Check filename matches name field
        expected_filename = f"{data['name']}.json"
        if file_path.name != expected_filename:
            self.warnings.append(
                f"{relative_path}: Filename '{file_path.name}' doesn't match name field '{expected_filename}'"
            )
            
        self.validated_files.append(str(relative_path))
        print(f"  ✓ Valid {file_type} configuration")
        return True
        
    def validate_schema(self, data: Dict, schema: Dict, context: str) -> bool:
        """Validate data against a schema definition"""
        # Check required fields
        for field in schema.get("required_fields", []):
            if field not in data:
                self.errors.append(f"{context}: Missing required field '{field}'")
                return False
                
        # Check field types
        for field, expected_type in schema.get("field_types", {}).items():
            if field in data:
                value = data[field]
                # Handle union types (e.g., str or None)
                if isinstance(expected_type, tuple):
                    if not isinstance(value, expected_type):
                        self.errors.append(
                            f"{context}: Field '{field}' has wrong type. "
                            f"Expected one of {expected_type}, got {type(value).__name__}"
                        )
                        return False
                elif not isinstance(value, expected_type):
                    self.errors.append(
                        f"{context}: Field '{field}' has wrong type. "
                        f"Expected {expected_type.__name__}, got {type(value).__name__}"
                    )
                    return False
                    
        # Validate nested schemas
        for field, nested_schema in schema.get("nested_schemas", {}).items():
            if field in data:
                if not self.validate_schema(data[field], nested_schema, f"{context}.{field}"):
                    return False
                    
        # Validate list item schemas
        for field, item_schema in schema.get("list_item_schemas", {}).items():
            if field in data:
                for i, item in enumerate(data[field]):
                    if not self.validate_schema(item, item_schema, f"{context}.{field}[{i}]"):
                        return False
                        
        # Validate value constraints
        for field_path, (min_val, max_val) in schema.get("value_constraints", {}).items():
            value = self.get_nested_value(data, field_path)
            if value is not None:
                if not (min_val <= value <= max_val):
                    self.errors.append(
                        f"{context}: Field '{field_path}' value {value} is out of range [{min_val}, {max_val}]"
                    )
                    return False
                    
        return True
        
    def get_nested_value(self, data: Dict, path: str) -> Any:
        """Get value from nested dictionary using dot notation"""
        keys = path.split('.')
        value = data
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                return None
        return value
        
    def print_summary(self):
        """Print validation summary"""
        print("\n" + "="*60)
        print("VALIDATION SUMMARY")
        print("="*60)
        
        print(f"\nFiles validated: {len(self.validated_files)}")
        for file in self.validated_files:
            print(f"  ✓ {file}")
            
        if self.warnings:
            print(f"\nWarnings ({len(self.warnings)}):")
            for warning in self.warnings:
                print(f"  ⚠ {warning}")
                
        if self.errors:
            print(f"\nErrors ({len(self.errors)}):")
            for error in self.errors:
                print(f"  ✗ {error}")
        else:
            print("\n✓ All JSON files are valid and consistent!")
            
        print("="*60)


def main():
    """Main entry point"""
    # Determine assets path
    script_dir = Path(__file__).parent
    assets_path = script_dir / "assets"
    
    # Allow custom path via command line
    if len(sys.argv) > 1:
        assets_path = Path(sys.argv[1])
        
    print(f"P32 Animatronic Bot - JSON Validator")
    print(f"Assets path: {assets_path.absolute()}\n")
    
    validator = JSONValidator(str(assets_path))
    success = validator.validate_all()
    validator.print_summary()
    
    # Exit with appropriate code
    sys.exit(0 if success and not validator.errors else 1)


if __name__ == "__main__":
    main()
