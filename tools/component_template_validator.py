"""Multi-hardware component template validation and config generation.

Validates component JSON definitions against the multi-hardware template schema
and generates type-safe C++ config structs from template specifications.
"""

from __future__ import annotations

import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

PROJECT_ROOT = Path(__file__).resolve().parents[1]
CONFIG_ROOT = PROJECT_ROOT / "config"


@dataclass
class TemplateArg:
    """Single template argument specification."""
    key: str
    value: Any
    description: str = ""

    def to_cpp_type(self) -> str:
        """Map Python value to C++ type."""
        if isinstance(self.value, str):
            return "const char*"
        if isinstance(self.value, bool):
            return "bool"
        if isinstance(self.value, int):
            return "int"
        if isinstance(self.value, float):
            return "float"
        return "const char*"

    def to_cpp_literal(self) -> str:
        """Convert Python value to C++ literal."""
        if isinstance(self.value, str):
            escaped = self.value.replace("\\", "\\\\").replace('"', '\\"')
            return f'"{escaped}"'
        if isinstance(self.value, bool):
            return "true" if self.value else "false"
        if isinstance(self.value, int):
            return str(self.value)
        if isinstance(self.value, float):
            return repr(self.value)
        return "nullptr"


@dataclass
class HardwareTemplate:
    """Specification for a single hardware template."""
    role: str  # "display", "camera", "speaker", etc.
    device: str  # "GC9A01", "OV2640", etc.
    args: Dict[str, Any]  # Template arguments
    required: bool = True

    def to_struct_fields(self, prefix: str) -> List[str]:
        """Generate C struct field definitions for this hardware role."""
        fields = []
        for key, value in self.args.items():
            field_name = f"{prefix}_{key}"
            arg = TemplateArg(field_name, value)
            fields.append(f"    {arg.to_cpp_type()} {field_name};")
        return fields

    def validate(self) -> None:
        """Validate template specification."""
        if not self.role or not isinstance(self.role, str):
            raise ValueError("Template 'role' must be a non-empty string")
        if not self.device or not isinstance(self.device, str):
            raise ValueError("Template 'device' must be a non-empty string")
        if not isinstance(self.args, dict):
            raise ValueError("Template 'args' must be a dictionary")


@dataclass
class ComponentTemplate:
    """Complete multi-hardware component template specification."""
    name: str
    templates: Dict[str, Dict[str, HardwareTemplate]]  # role -> device -> HardwareTemplate
    init_func: str
    act_func: str
    json_path: Optional[Path] = None

    @staticmethod
    def from_json(data: Dict[str, Any], json_path: Optional[Path] = None) -> ComponentTemplate:
        """Load component template from JSON data."""
        name = data.get("name")
        if not name:
            raise ValueError("Component must have a 'name' field")

        software = data.get("software", {})
        init_func = software.get("init_func", f"{name}_init")
        act_func = software.get("act_func", f"{name}_act")

        # Parse templates if using new multi-hardware format
        templates: Dict[str, Dict[str, HardwareTemplate]] = {}
        raw_templates = data.get("templates", {})

        if isinstance(raw_templates, dict):
            for role, implementations in raw_templates.items():
                if not isinstance(implementations, dict):
                    raise ValueError(f"Template role '{role}' implementations must be a dict")
                templates[role] = {}
                for device, spec in implementations.items():
                    if not isinstance(spec, dict):
                        raise ValueError(f"Template '{role}.{device}' must be a dict")
                    hw_template = HardwareTemplate(
                        role=spec.get("role", role),
                        device=spec.get("device", device),
                        args=spec.get("args", {}),
                        required=spec.get("required", True),
                    )
                    hw_template.validate()
                    templates[role][device] = hw_template

        return ComponentTemplate(
            name=name,
            templates=templates,
            init_func=init_func,
            act_func=act_func,
            json_path=json_path,
        )

    def get_templates_for_declaration(self, template_args: List[str]) -> List[HardwareTemplate]:
        """Given a list of template arguments like ['GC9A01', 'OV2640'],
        resolve them to actual HardwareTemplate objects in order."""
        resolved = []
        used_roles = set()

        for arg in template_args:
            found = False
            # Search all roles for this device name
            for role, implementations in self.templates.items():
                if arg in implementations:
                    if role in used_roles:
                        raise ValueError(
                            f"Template argument '{arg}' (role '{role}') already specified"
                        )
                    resolved.append(implementations[arg])
                    used_roles.add(role)
                    found = True
                    break
            if not found:
                raise ValueError(
                    f"Template argument '{arg}' not found in component '{self.name}' templates"
                )

        return resolved

    def validate(self) -> None:
        """Validate all template specifications."""
        if not self.templates:
            raise ValueError(f"Component '{self.name}' has no templates defined")
        for role, implementations in self.templates.items():
            for device, template in implementations.items():
                template.validate()

    def generate_config_struct(self, template_args: List[str]) -> str:
        """Generate a C++ struct definition for this component with given templates."""
        resolved = self.get_templates_for_declaration(template_args)

        struct_name = self._make_config_struct_name(template_args)
        lines = [f"typedef struct {{"]

        for template in resolved:
            prefix = template.role
            lines.extend(template.to_struct_fields(prefix))

        lines.append(f"}} {struct_name};")
        return "\n".join(lines)

    def _make_config_struct_name(self, template_args: List[str]) -> str:
        """Generate struct name from component and templates."""
        sanitized_component = "".join(
            c if c.isalnum() or c == "_" else "_" for c in self.name
        )
        sanitized_templates = "_".join(
            "".join(c if c.isalnum() or c == "_" else "_" for c in arg)
            for arg in template_args
        )
        return f"{sanitized_component}_{sanitized_templates}_config_t"


class TemplateValidator:
    """Validates component templates across the project."""

    def __init__(self, config_root: Path = CONFIG_ROOT):
        self.config_root = config_root
        self.errors: List[str] = []
        self.warnings: List[str] = []

    def validate_component_file(self, json_path: Path) -> Optional[ComponentTemplate]:
        """Validate a single component JSON file. Returns ComponentTemplate on success, None on error."""
        try:
            with json_path.open("r", encoding="ascii") as f:
                data = json.load(f)
        except json.JSONDecodeError as e:
            self.errors.append(f"{json_path}: Invalid JSON - {e}")
            return None
        except UnicodeDecodeError as e:
            self.errors.append(f"{json_path}: Not ASCII encoded - {e}")
            return None

        try:
            component = ComponentTemplate.from_json(data, json_path)
            component.validate()
            return component
        except ValueError as e:
            self.errors.append(f"{json_path}: {e}")
            return None

    def validate_all_components(self) -> List[ComponentTemplate]:
        """Validate all component JSON files in config/components."""
        components = []
        if not self.config_root.exists():
            self.warnings.append(f"Config root not found: {self.config_root}")
            return components

        for json_file in sorted(self.config_root.rglob("*.json")):
            # Skip bot definitions, only validate components
            if "bots" not in json_file.parts:
                component = self.validate_component_file(json_file)
                if component:
                    components.append(component)

        return components

    def report(self, file=None) -> int:
        """Print validation report. Returns 0 if no errors, 1 if errors found."""
        if self.errors:
            print(f"\n Validation Errors ({len(self.errors)}):", file=file)
            for error in self.errors:
                print(f"   {error}", file=file)

        if self.warnings:
            print(f"\n  Warnings ({len(self.warnings)}):", file=file)
            for warning in self.warnings:
                print(f"   {warning}", file=file)

        if not self.errors and not self.warnings:
            print("\n All components valid", file=file)

        return 1 if self.errors else 0


def main() -> int:
    """Validate all components in the project."""
    validator = TemplateValidator()
    components = validator.validate_all_components()

    print(f"\n Validated {len(components)} components", file=sys.stdout)
    for comp in components:
        if comp.templates:
            template_list = ", ".join(
                f"{role}({', '.join(implementations.keys())})"
                for role, implementations in comp.templates.items()
            )
            print(f"   {comp.name}: {template_list}", file=sys.stdout)
        else:
            print(f"   {comp.name}: (no templates)", file=sys.stdout)

    return validator.report()


if __name__ == "__main__":
    sys.exit(main())
