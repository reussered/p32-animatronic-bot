"""Multi-hardware template expansion for generate_tables.py.

This module extends the table generator to parse and expand component templates
from the new multi-hardware format like: component<TEMPLATE1,TEMPLATE2>
"""

from __future__ import annotations

import re
from pathlib import Path
from typing import List, Optional, Tuple

from component_template_validator import ComponentTemplate, TemplateValidator


class TemplateExpander:
    """Expands component template syntax into resolved hardware configs."""

    def __init__(self, validator: TemplateValidator):
        self.validator = validator
        self.component_cache: dict[str, ComponentTemplate] = {}

    def parse_component_reference(self, reference: str) -> Tuple[str, List[str]]:
        """Parse 'component_name<TEMPLATE1,TEMPLATE2>' syntax.
        
        Returns:
            Tuple of (component_name, [template_args])
        """
        # Match pattern: name<arg1,arg2,arg3>
        pattern = r"^(\w+)\s*(?:<\s*([^>]+)\s*>)?$"
        match = re.match(pattern, reference.strip())

        if not match:
            raise ValueError(f"Invalid component reference syntax: {reference}")

        component_name = match.group(1)
        templates_str = match.group(2)

        template_args = []
        if templates_str:
            # Split by comma and strip whitespace
            template_args = [arg.strip() for arg in templates_str.split(",")]

        return component_name, template_args

    def resolve_component_json_path(self, component_name: str) -> Optional[Path]:
        """Find component JSON file by name."""
        if component_name in self.component_cache:
            return self.component_cache[component_name].json_path

        # Search config/components for matching JSON
        config_root = Path(__file__).resolve().parents[1] / "config" / "components"
        if not config_root.exists():
            return None

        candidates = list(config_root.rglob(f"{component_name}.json"))
        if len(candidates) == 1:
            return candidates[0]
        if len(candidates) > 1:
            raise ValueError(
                f"Ambiguous component name '{component_name}' - "
                f"found {len(candidates)} matches"
            )

        return None

    def get_component_template(self, component_name: str) -> ComponentTemplate:
        """Load and cache component template definition."""
        if component_name in self.component_cache:
            return self.component_cache[component_name]

        json_path = self.resolve_component_json_path(component_name)
        if not json_path:
            raise ValueError(f"Component '{component_name}' not found")

        component = self.validator.validate_component_file(json_path)
        if not component:
            raise ValueError(f"Component '{component_name}' failed validation")

        self.component_cache[component_name] = component
        return component

    def expand_reference(self, reference: str) -> dict:
        """Expand a component reference into a full hardware config.
        
        Returns:
            Dict with keys:
                - component_name: str
                - template_args: List[str]
                - config_struct_name: str
                - config_struct_def: str
                - init_func: str
                - act_func: str
        """
        component_name, template_args = self.parse_component_reference(reference)

        component = self.get_component_template(component_name)

        # If no templates specified but component has them, that's an error
        if not template_args and component.templates:
            required_roles = [
                role for role, impls in component.templates.items()
                if any(t.required for t in impls.values())
            ]
            if required_roles:
                raise ValueError(
                    f"Component '{component_name}' requires templates for roles: "
                    f"{', '.join(required_roles)}"
                )

        # Generate config struct
        config_struct_def = None
        config_struct_name = None
        if template_args:
            try:
                config_struct_def = component.generate_config_struct(template_args)
                # Extract struct name from definition
                config_struct_name = (
                    component._make_config_struct_name(template_args)
                )
            except ValueError as e:
                raise ValueError(
                    f"Failed to expand templates for '{component_name}': {e}"
                )

        return {
            "component_name": component_name,
            "template_args": template_args,
            "config_struct_name": config_struct_name,
            "config_struct_def": config_struct_def,
            "init_func": component.init_func,
            "act_func": component.act_func,
            "component": component,
        }

    def validate_and_expand_all(
        self, references: List[str]
    ) -> List[dict]:
        """Validate and expand a list of component references.
        
        Args:
            references: List of component references like ['component<TEMPLATE1,TEMPLATE2>']
        
        Returns:
            List of expanded component configs
        """
        expanded = []
        for ref in references:
            try:
                config = self.expand_reference(ref)
                expanded.append(config)
            except ValueError as e:
                raise ValueError(f"Failed to expand '{ref}': {e}")

        return expanded


def main_integration_example():
    """Example of how to integrate with generate_tables.py"""
    # This would be called during component processing in generate_tables.py
    # when a component with template syntax is encountered

    validator = TemplateValidator()
    expander = TemplateExpander(validator)

    # Example: Parse and expand a component reference
    test_refs = [
        "goblin_left_eye<GC9A01,OV2640>",
        "goblin_jaw<NEMA17_STEPPER>",
        "sdi_display_bus",
    ]

    try:
        for ref in test_refs:
            result = expander.expand_reference(ref)
            print(f"\n✅ Expanded: {ref}")
            print(f"   Component: {result['component_name']}")
            print(f"   Templates: {result['template_args']}")
            if result['config_struct_name']:
                print(f"   Config struct: {result['config_struct_name']}")
    except ValueError as e:
        print(f"\n❌ Error: {e}")


if __name__ == "__main__":
    main_integration_example()
