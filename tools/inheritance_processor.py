#!/usr/bin/env python3
"""
P32 JSON Inheritance Processor
Resolves family template inheritance chains into final creature configurations.

Inheritance Pattern:
Base Templates -> Family Templates -> Creature Overrides -> Final Configuration

Example:
humanoid_base.json -> goblin_family.json -> goblin_warrior.json -> final_goblin_warrior.json
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional
import copy

class P32InheritanceProcessor:
    def __init__(self, config_dir: str, output_dir: str = None):
        self.config_dir = Path(config_dir)
        self.output_dir = Path(output_dir) if output_dir else self.config_dir / "resolved"
        self.output_dir.mkdir(exist_ok=True)
        
        # Cache for loaded templates to avoid redundant file reads
        self.template_cache = {}
        
        # Inheritance validation tracking
        self.resolution_log = []
        
    def load_template(self, template_path: str) -> Dict[str, Any]:
        """Load a template file with caching"""
        # Normalize path - remove 'config/' prefix if present
        clean_path = template_path.replace("config/", "") if template_path.startswith("config/") else template_path
        
        if clean_path in self.template_cache:
            return self.template_cache[clean_path]
            
        full_path = self.config_dir / clean_path
        
        if not full_path.exists():
            raise FileNotFoundError(f"Template not found: {full_path}")
            
        with open(full_path, 'r') as f:
            template_data = json.load(f)
            self.template_cache[clean_path] = template_data
            return template_data
            
    def resolve_inheritance_chain(self, creature_config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Resolve complete inheritance chain for a creature configuration.
        
        Expected inheritance fields:
        - inherits_from: Direct parent template path
        - family_template: Family-level characteristics 
        - base_template: Base subsystem template
        """
        
        self.resolution_log.clear()
        resolved_config = copy.deepcopy(creature_config)
        
        # Track what we're resolving
        creature_name = resolved_config.get("bot_id", "unknown_creature")
        self.resolution_log.append(f"Resolving: {creature_name}")
        
        # 1. Apply base template if specified
        if "base_template" in resolved_config:
            base_template = self.load_template(resolved_config["base_template"])
            resolved_config = self._merge_templates(base_template, resolved_config)
            self.resolution_log.append(f"  Applied base: {resolved_config['base_template']}")
            
        # 2. Apply family template if specified  
        if "family_template" in resolved_config:
            family_template = self.load_template(resolved_config["family_template"])
            resolved_config = self._merge_templates(family_template, resolved_config)
            self.resolution_log.append(f"  Applied family: {resolved_config['family_template']}")
            
        # 3. Apply direct inheritance if specified
        if "inherits_from" in resolved_config:
            parent_template = self.load_template(resolved_config["inherits_from"])
            resolved_config = self._merge_templates(parent_template, resolved_config)
            self.resolution_log.append(f"  Applied parent: {resolved_config['inherits_from']}")
        
        # 4. Resolve positioned components inheritance
        if "positioned_components" in resolved_config:
            resolved_config["positioned_components"] = self._resolve_component_inheritance(
                resolved_config["positioned_components"]
            )
            
        # 5. Clean up inheritance metadata from final config
        inheritance_fields = ["base_template", "family_template", "inherits_from"]
        for field in inheritance_fields:
            resolved_config.pop(field, None)
            
        # 6. Update metadata
        resolved_config["resolved_from_inheritance"] = True
        resolved_config["inheritance_chain"] = self.resolution_log.copy()
        
        return resolved_config
        
    def _merge_templates(self, base_template: Dict[str, Any], override_config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Merge two templates with intelligent override rules.
        
        Merge Rules:
        - Simple values: override wins
        - Arrays: merge and deduplicate
        - Objects: recursive merge
        - positioned_components: append from base + override
        """
        
        merged = copy.deepcopy(base_template)
        
        for key, value in override_config.items():
            if key not in merged:
                # New field - add directly
                merged[key] = copy.deepcopy(value)
            elif isinstance(value, dict) and isinstance(merged[key], dict):
                # Recursive merge for objects
                merged[key] = self._merge_templates(merged[key], value)
            elif isinstance(value, list) and isinstance(merged[key], list):
                # Smart list merging
                if key == "positioned_components":
                    # For components, append and deduplicate
                    combined = merged[key] + value
                    merged[key] = list(dict.fromkeys(combined))  # Preserves order, removes dupes
                else:
                    # For other arrays, override completely
                    merged[key] = copy.deepcopy(value)
            else:
                # Simple override
                merged[key] = copy.deepcopy(value)
                
        return merged
        
    def _resolve_component_inheritance(self, component_list: List[str]) -> List[str]:
        """
        Resolve inheritance for positioned components.
        Some components may reference template components that need resolution.
        """
        
        resolved_components = []
        
        for component_ref in component_list:
            # Check if this component needs inheritance resolution
            try:
                component_config = self.load_template(component_ref)
                
                if any(field in component_config for field in ["inherits_from", "component_template"]):
                    # This component has inheritance - resolve it
                    resolved_component = self.resolve_inheritance_chain(component_config)
                    
                    # Generate resolved component file
                    component_name = Path(component_ref).stem + "_resolved.json"
                    component_output_path = self.output_dir / "components" / component_name
                    component_output_path.parent.mkdir(parents=True, exist_ok=True)
                    
                    with open(component_output_path, 'w', encoding='ascii') as f:
                        json.dump(resolved_component, f, indent=2)
                        
                    # Update reference to point to resolved component
                    resolved_ref = f"config/resolved/components/{component_name}"
                    resolved_components.append(resolved_ref)
                    self.resolution_log.append(f"    Resolved component: {component_ref} -> {resolved_ref}")
                else:
                    # No inheritance needed
                    resolved_components.append(component_ref)
                    
            except FileNotFoundError:
                # Component file doesn't exist - keep original reference
                resolved_components.append(component_ref)
                self.resolution_log.append(f"    Warning: Component not found: {component_ref}")
                
        return resolved_components
        
    def resolve_creature(self, creature_file: str, output_name: str = None) -> str:
        """
        Resolve a single creature configuration file.
        Returns path to resolved configuration.
        """
        
        # Load creature configuration
        creature_config = self.load_template(creature_file)
        
        # Resolve inheritance
        resolved_config = self.resolve_inheritance_chain(creature_config)
        
        # Generate output filename
        if not output_name:
            creature_name = Path(creature_file).stem
            output_name = f"{creature_name}_resolved.json"
            
        output_path = self.output_dir / output_name
        
        # Write resolved configuration
        with open(output_path, 'w', encoding='ascii') as f:
            json.dump(resolved_config, f, indent=2)
            
        print(f"Resolved creature: {creature_file}")
        print(f"Output: {output_path}")
        print("Inheritance chain:")
        for log_entry in self.resolution_log:
            print(f"  {log_entry}")
        print()
        
        return str(output_path)
        
    def resolve_family(self, family_dir: str) -> List[str]:
        """
        Resolve all creatures in a family directory.
        Returns list of resolved configuration paths.
        """
        
        family_path = Path(family_dir)
        resolved_files = []
        
        print(f"Resolving family: {family_path.name}")
        print("=" * 50)
        
        # Find all creature JSON files in family
        for json_file in family_path.glob("*.json"):
            if json_file.name != "family.json":  # Skip family template itself
                try:
                    resolved_path = self.resolve_creature(str(json_file))
                    resolved_files.append(resolved_path)
                except Exception as e:
                    print(f"Error resolving {json_file}: {e}")
                    
        print(f"Resolved {len(resolved_files)} creatures from {family_path.name} family")
        print()
        
        return resolved_files
        
    def resolve_all_families(self) -> Dict[str, List[str]]:
        """
        Resolve all bot families in the configuration.
        Returns mapping of family names to resolved creature files.
        """
        
        families_dir = self.config_dir / "bots" / "bot_families"
        
        if not families_dir.exists():
            print(f"No bot families directory found: {families_dir}")
            return {}
            
        resolved_families = {}
        
        print("P32 JSON Inheritance Processor")
        print("=" * 50)
        print(f"Processing families from: {families_dir}")
        print()
        
        # Process each family category
        for category_dir in families_dir.iterdir():
            if category_dir.is_dir():
                print(f"Category: {category_dir.name}")
                print("-" * 30)
                
                # Process each family in category
                for family_dir in category_dir.iterdir():
                    if family_dir.is_dir():
                        family_name = f"{category_dir.name}/{family_dir.name}"
                        resolved_files = self.resolve_family(family_dir)
                        resolved_families[family_name] = resolved_files
                        
        return resolved_families
        
    def validate_resolution(self, resolved_config: Dict[str, Any]) -> List[str]:
        """
        Validate that resolved configuration is complete and valid.
        Returns list of validation errors.
        """
        
        errors = []
        
        # Check required fields
        required_fields = ["bot_id", "bot_type", "positioned_components"]
        for field in required_fields:
            if field not in resolved_config:
                errors.append(f"Missing required field: {field}")
                
        # Check positioned components exist
        if "positioned_components" in resolved_config:
            for component_ref in resolved_config["positioned_components"]:
                try:
                    self.load_template(component_ref)
                except FileNotFoundError:
                    errors.append(f"Component not found: {component_ref}")
                    
        # Check coordinate system consistency
        if "coordinate_system" in resolved_config:
            coord_system = resolved_config["coordinate_system"]
            if coord_system not in ["planar_2d", "skull_3d", "humanoid_3d"]:
                errors.append(f"Invalid coordinate system: {coord_system}")
                
        return errors


def main():
    """Command-line interface for inheritance processor"""
    
    if len(sys.argv) < 2:
        print("Usage:")
        print("  python inheritance_processor.py <config_dir> [output_dir]")
        print("  python inheritance_processor.py resolve <creature_file>")
        print("  python inheritance_processor.py resolve-family <family_dir>") 
        print("  python inheritance_processor.py resolve-all")
        sys.exit(1)
        
    if sys.argv[1] == "resolve-all":
        # Resolve all families
        config_dir = "config" if len(sys.argv) < 3 else sys.argv[2]
        processor = P32InheritanceProcessor(config_dir)
        resolved_families = processor.resolve_all_families()
        
        print("Resolution Summary:")
        print("=" * 50)
        for family_name, files in resolved_families.items():
            print(f"{family_name}: {len(files)} creatures resolved")
            
    elif sys.argv[1] == "resolve":
        # Resolve single creature
        config_dir = Path(sys.argv[2]).parent.parent.parent  # Assume config/bots/... structure
        processor = P32InheritanceProcessor(config_dir)
        resolved_path = processor.resolve_creature(sys.argv[2])
        print(f"Resolved to: {resolved_path}")
        
    elif sys.argv[1] == "resolve-family":
        # Resolve family
        config_dir = Path(sys.argv[2]).parent.parent.parent
        processor = P32InheritanceProcessor(config_dir)
        resolved_files = processor.resolve_family(sys.argv[2])
        print(f"Resolved {len(resolved_files)} creatures")
        
    else:
        # Standard usage
        config_dir = sys.argv[1]
        output_dir = sys.argv[2] if len(sys.argv) > 2 else None
        
        processor = P32InheritanceProcessor(config_dir, output_dir)
        resolved_families = processor.resolve_all_families()
        
        print("Resolution complete!")
        print(f"Resolved configurations saved to: {processor.output_dir}")


if __name__ == "__main__":
    main()