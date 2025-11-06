"""P32 dispatch table generator.

Walks bot JSON configuration trees and emits subsystem-specific component
aggregation units, dispatch tables, and supporting build metadata.
"""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional, Set, Tuple

PROJECT_ROOT = Path(__file__).resolve().parents[1]
CONFIG_ROOT = PROJECT_ROOT / "config"
SRC_ROOT = PROJECT_ROOT / "src"
INCLUDE_ROOT = PROJECT_ROOT / "include"

SUPPORTED_CONTROLLER_BOARDS: Dict[str, str] = {
    "ESP32_S3_DEVKITC_1": "esp32-s3-devkitc-1",
    "ESP32_S3_DEVKIT": "esp32-s3-devkitc-1",
    "ESP32_S3_R8N16": "esp32-s3-devkitc-1",
    "ESP32-S3": "esp32-s3-devkitc-1",
    "ESP32_C3_MINI": "esp32-c3-devkitm-1",
    "ESP32_C3_DEVKITM_1": "esp32-c3-devkitm-1",
    "ESP32-C3": "esp32-c3-devkitm-1",
    "ESP32": "esp32dev",
    "ESP32_DEVKIT": "esp32dev",
}


class JsonLoader:
    """Caches JSON loads while enforcing ASCII encoding."""

    def __init__(self) -> None:
        self._cache: Dict[Path, Dict[str, Any]] = {}

    def load(self, path: Path) -> Dict[str, Any]:
        path = path.resolve()
        if path in self._cache:
            return self._cache[path]
        try:
            with path.open("r", encoding="ascii") as handle:
                data = json.load(handle)
        except UnicodeDecodeError as exc:
            raise RuntimeError(f"File {path} is not ASCII encoded") from exc
        except json.JSONDecodeError as exc:
            raise RuntimeError(f"Malformed JSON in {path}: {exc}") from exc
        self._cache[path] = data
        return data


class ComponentSourceIndex:
    """Maps component identifiers to permanent .src/.hdr files."""

    def __init__(self, components_root: Path) -> None:
        self._src_index: Dict[str, List[Path]] = {}
        self._hdr_index: Dict[str, List[Path]] = {}
        if components_root.exists():
            for src_path in components_root.rglob("*.src"):
                self._src_index.setdefault(src_path.stem, []).append(src_path)
            for hdr_path in components_root.rglob("*.hdr"):
                self._hdr_index.setdefault(hdr_path.stem, []).append(hdr_path)

    def find_source_file(
        self,
        component_name: str,
        data: Dict[str, Any],
        json_path: Optional[Path],
    ) -> Optional[Path]:
        candidates: List[Path] = []
        software = data.get("software")
        if isinstance(software, dict):
            explicit = software.get("source_file")
            if isinstance(explicit, str):
                resolved = resolve_reference_path(explicit)
                if resolved:
                    candidates.append(resolved)
        if json_path is not None:
            candidates.append(json_path.with_suffix(".src"))
        candidates.extend(self._src_index.get(component_name, []))
        for candidate in deduplicate_paths(candidates):
            if candidate and candidate.exists():
                return candidate
        return None

    def find_header_file(
        self,
        component_name: str,
        data: Dict[str, Any],
        json_path: Optional[Path],
    ) -> Optional[Path]:
        candidates: List[Path] = []
        software = data.get("software")
        if isinstance(software, dict):
            explicit = software.get("header_file")
            if isinstance(explicit, str):
                resolved = resolve_reference_path(explicit)
                if resolved:
                    candidates.append(resolved)
        if json_path is not None:
            candidates.append(json_path.with_suffix(".hdr"))
        candidates.extend(self._hdr_index.get(component_name, []))
        for candidate in deduplicate_paths(candidates):
            if candidate and candidate.exists():
                return candidate
        return None


@dataclass
class ComponentVisit:
    name: str
    init_func: str
    act_func: str
    hit_count: int
    json_path: Optional[Path]
    template_type: Optional[str] = None



@dataclass
class ComponentDefinition:
    name: str
    json_path: Optional[Path]
    data: Dict[str, Any]
    init_func: str
    act_func: str
    hit_count: int
    src_path: Optional[Path]
    hdr_path: Optional[Path]
    src_content: Optional[str] = None
    hdr_content: Optional[str] = None

    def load_artifacts(self) -> None:
        if self.src_path and self.src_path.exists():
            self.src_content = read_ascii_file(self.src_path)
        if self.hdr_path and self.hdr_path.exists():
            self.hdr_content = read_ascii_file(self.hdr_path)


@dataclass
class SubsystemContext:
    name: str
    controller: str
    json_path: Optional[Path]
    visits: List[ComponentVisit] = field(default_factory=list)
    unique_components: Dict[str, ComponentDefinition] = field(default_factory=dict)
    declared_fields: Dict[str, str] = field(default_factory=dict)  # Track {field_name: cpp_type} for first declarations

    def register_component(
        self,
        json_path: Optional[Path],
        data: Dict[str, Any],
        sources: ComponentSourceIndex,
        template_type: Optional[str] = None,
    ) -> None:
        component_name = data.get("name")
        if not component_name:
            return
        init_func, act_func = resolve_function_names(component_name, data)
        hit_count = resolve_hit_count(data)

        # Always record the visit for the dispatch table, which needs duplicates.
        self.visits.append(
            ComponentVisit(
                name=component_name,
                init_func=init_func,
                act_func=act_func,
                hit_count=hit_count,
                json_path=json_path,
                template_type=template_type,
            )
        )

        src_path = sources.find_source_file(component_name, data, json_path)
        hdr_path = sources.find_header_file(component_name, data, json_path)

        # If we've already aggregated this component, only attach missing artifacts.
        existing = self.unique_components.get(component_name)
        if existing is not None:
            if existing.src_path is None and src_path is not None:
                existing.src_path = src_path
                existing.src_content = read_ascii_file(src_path)
            if existing.hdr_path is None and hdr_path is not None:
                existing.hdr_path = hdr_path
                existing.hdr_content = read_ascii_file(hdr_path)
            return

        cloned_data = json.loads(json.dumps(data, ensure_ascii=True))
        definition = ComponentDefinition(
            name=component_name,
            json_path=json_path,
            data=cloned_data,
            init_func=init_func,
            act_func=act_func,
            hit_count=hit_count,
            src_path=src_path,
            hdr_path=hdr_path,
        )
        definition.load_artifacts()
        self.unique_components[component_name] = definition

    @property
    def identifier(self) -> str:
        return sanitize_identifier(self.name)


def sanitize_identifier(value: str) -> str:
    # List of C++ reserved keywords
    keywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit",
        "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case",
        "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl",
        "concept", "const", "consteval", "constexpr", "constinit", "const_cast",
        "continue", "co_await", "co_return", "co_yield", "decltype", "default",
        "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
        "export", "extern", "false", "float", "for", "friend", "goto", "if",
        "inline", "int", "long", "mutable", "namespace", "new", "noexcept",
        "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
        "protected", "public", "reflexpr", "register", "reinterpret_cast",
        "requires", "return", "short", "signed", "sizeof", "static",
        "static_assert", "static_cast", "struct", "switch", "synchronized",
        "template", "this", "thread_local", "throw", "true", "try", "typedef",
        "typeid", "typename", "union", "unsigned", "using", "virtual", "void",
        "volatile", "wchar_t", "while", "xor", "xor_eq"
    }
    
    sanitized = "".join(ch if ch.isalnum() or ch == "_" else "_" for ch in value)
    if not sanitized or sanitized[0].isdigit():
        sanitized = "_" + sanitized
    
    if sanitized in keywords:
        return sanitized + "_"
    return sanitized


def deduplicate_paths(paths: Iterable[Path]) -> List[Path]:
    seen: Set[Path] = set()
    deduped: List[Path] = []
    for path in paths:
        if path is None:
            continue
        resolved = path.resolve()
        if resolved in seen:
            continue
        seen.add(resolved)
        deduped.append(resolved)
    return deduped


def resolve_reference_path(reference: str) -> Optional[Path]:
    candidate = Path(reference)
    if not candidate.is_absolute():
        if reference.startswith("config/"):
            candidate = PROJECT_ROOT / reference
        else:
            candidate = CONFIG_ROOT / reference
    return candidate.resolve()


def read_ascii_file(path: Path) -> str:
    try:
        return path.read_text(encoding="ascii")
    except UnicodeDecodeError as exc:
        raise RuntimeError(f"File {path} must be ASCII encoded") from exc


def resolve_function_names(component_name: str, data: Dict[str, Any]) -> Tuple[str, str]:
    software = data.get("software")
    init_func: Optional[str] = None
    act_func: Optional[str] = None
    if isinstance(software, dict):
        init_candidate = software.get("init_function")
        act_candidate = software.get("act_function")
        if isinstance(init_candidate, str):
            init_func = init_candidate
        if isinstance(act_candidate, str):
            act_func = act_candidate
    if not init_func:
        init_func = f"{component_name}_init"
    if not act_func:
        act_func = f"{component_name}_act"
    return init_func, act_func


def resolve_hit_count(data: Dict[str, Any]) -> int:
    timing = data.get("timing")
    if isinstance(timing, dict):
        hit = timing.get("hitCount")
        if isinstance(hit, int):
            return max(1, hit)
        if isinstance(hit, float):
            return max(1, int(hit))
        if isinstance(hit, str):
            try:
                value = int(float(hit))
                return max(1, value)
            except ValueError:
                return 1
    return 1


def resolve_json_reference(base_path: Path, reference: str) -> Tuple[Path, Optional[str]]:
    """Resolves a JSON reference, parsing out a template type if present.
    
    Supports both old style:
      - "config/components/goblin_left_eye.json"
      - "config/components/goblin_left_eye.json<GC9A01>"
    
    And new style (by component name):
      - "goblin_left_eye"
      - "goblin_left_eye<GC9A01,OV2640>"
    """
    template_type: Optional[str] = None
    path_part = reference
    if "<" in reference and reference.endswith(">"):
        path_part, template_part = reference.rsplit("<", 1)
        template_type = template_part[:-1].strip()

    candidate = Path(path_part)
    if not candidate.is_absolute():
        if path_part.startswith("config/"):
            candidate = PROJECT_ROOT / path_part
        else:
            # Try as a file path first
            candidate = base_path.parent / path_part
            if not candidate.exists():
                # Try as a component name by searching in config/components
                from pathlib import Path as PathlibPath
                matches = list((PROJECT_ROOT / "config" / "components").rglob(f"{path_part}.json"))
                if matches:
                    candidate = matches[0]  # Use first match
    
    candidate = candidate.resolve()
    if not candidate.exists():
        raise FileNotFoundError(f"Referenced JSON file not found: {path_part}")
    
    return candidate, template_type



def is_primitive(value: Any) -> bool:
    return isinstance(value, (str, bool, int, float)) or value is None


def json_to_cpp_type(value: Any) -> str:
    if isinstance(value, str):
        return "const char*"
    if isinstance(value, bool):
        return "bool"
    if isinstance(value, int):
        return "int"
    if isinstance(value, float):
        return "float"
    return "const char*"


def to_cpp_literal(value: Any) -> str:
    if isinstance(value, str):
        escaped = value.replace("\\", "\\\\").replace('"', '\\"')
        return f'"{escaped}"'
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, int):
        return str(value)
    if isinstance(value, float):
        return repr(value)
    if value is None:
        return "nullptr"
    json_text = json.dumps(value, ensure_ascii=True)
    escaped = json_text.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


def generate_inherited_fields(data: Dict[str, Any], context: SubsystemContext) -> List[str]:
    """Generate global declarations/assignments for inherited keyword fields from use_fields.
    
    Two types of entries:
    
    1. NORMAL FIELDS: generates variable declaration/assignment
       "color_schema": "RGB565"
       First: static char* color_schema = "RGB565";
       Later: color_schema = "RGB565";
    
    2. ADD_ONCE DIRECTIVES: outputs the value as literal code, deduplicated by value
       Keys with ### markers (e.g., ###1###, ###2###) indicate unique add-once lines
       "###1###": "#include display_classes.hdr"
       "###2###": "#include mood_system.hdr"
       First: #include display_classes.hdr
       Later: (skipped if same value seen before)
    
    Type mismatches on normal fields will cause compile errors (intentional validation).
    """
    lines: List[str] = []
    use_fields = data.get("use_fields")
    if not isinstance(use_fields, dict):
        return lines
    
    for key, value in use_fields.items():
        # Check if this is a directive (contains ### markers)
        if "###" in key:
            # Output the value as literal code if not already output
            if value not in context.declared_fields:
                lines.append(str(value))  # Output directive as-is
                context.declared_fields[value] = "add_once"  # Mark as declared
        else:
            # Normal field: generate C++ variable declaration/assignment
            sanitized_key = sanitize_identifier(key)
            cpp_type = json_to_cpp_type(value)
            literal = to_cpp_literal(value)
            
            # Check if this field was already declared
            if sanitized_key in context.declared_fields:
                # Already declared - just generate assignment
                lines.append(f"{sanitized_key} = {literal};")
            else:
                # First declaration - generate declaration + assignment
                lines.append(f"static {cpp_type} {sanitized_key} = {literal};")
                context.declared_fields[sanitized_key] = cpp_type
    
    return lines


def render_component_struct(component: ComponentDefinition, context: SubsystemContext) -> str:
    """Generate inherited field declarations/assignments for this component.
    
    Only generates declarations for use_fields (inherited keywords).
    Arbitrary component data is no longer generated as structs.
    """
    lines: List[str] = []
    inherited = generate_inherited_fields(component.data, context)
    
    if inherited:
        lines.append(f"// Inherited fields for {component.name}")
        lines.extend(inherited)
    
    # Return empty string if no inherited fields (skip section entirely)
    if not lines:
        return ""
    
    return "\n".join(lines)


def render_component_header(context: SubsystemContext) -> str:
    guard = f"{context.identifier.upper()}_COMPONENT_FUNCTIONS_HPP"
    header_lines: List[str] = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <cstddef>",
        "#include <cstdint>",
        '#include "esp_err.h"',
        "",
        "// Auto-generated by tools/generate_tables.py",
        f"// Subsystem: {context.name}",
        f"// Controller: {context.controller}",
        "",
    ]
    structs: List[str] = []
    prototypes: List[str] = []
    additional: List[str] = []
    included_hdrs: Set[Path] = set()

    for definition in sorted(context.unique_components.values(), key=lambda item: item.name):
        if definition.json_path:
            try:
                source_desc = definition.json_path.relative_to(PROJECT_ROOT)
            except ValueError:
                source_desc = definition.json_path
        else:
            source_desc = "inline component"
        
        # Generate inherited fields (use_fields) if present
        inherited_fields = render_component_struct(definition, context)
        if inherited_fields:
            structs.append(f"// Component: {definition.name} (sourced from {source_desc})")
            structs.append(inherited_fields)
            structs.append("")
        
        prototypes.append(f"esp_err_t {definition.init_func}(void);")
        prototypes.append(f"void {definition.act_func}(void);")
        if definition.hdr_content and definition.hdr_path:
            resolved_hdr_path = definition.hdr_path.resolve()
            if resolved_hdr_path not in included_hdrs:
                try:
                    rel = definition.hdr_path.relative_to(PROJECT_ROOT)
                except ValueError:
                    rel = definition.hdr_path
                additional.append(f"// Declarations from {rel}")
                additional.append(definition.hdr_content.strip())
                additional.append("")
                included_hdrs.add(resolved_hdr_path)
    header_lines.extend(structs)
    header_lines.append("// ---------------------------------------------------------------------------")
    header_lines.append("// Function prototypes")
    header_lines.append("// ---------------------------------------------------------------------------")
    header_lines.extend(prototypes)
    header_lines.append("")
    header_lines.extend(additional)
    header_lines.append(f"#endif // {guard}")
    return "\n".join(header_lines) + "\n"


def collect_interface_includes(context: SubsystemContext) -> Set[str]:
    """Determine which interface headers need to be included based on component usage."""
    includes: Set[str] = set()

    # Check for SPI display bus usage
    spi_display_components = {
        "gc9a01", "generic_spi_display", "spi_display_bus",
        "goblin_left_eye", "goblin_right_eye", "goblin_eye"
    }

    for definition in context.unique_components.values():
        if definition.name in spi_display_components:
            includes.add("components/interfaces/spi_display_bus.hdr")
            break

    # Check for SPI data bus usage
    spi_data_components = {"spi_data_bus", "generic_spi_data_driver"}

    for definition in context.unique_components.values():
        if definition.name in spi_data_components:
            includes.add("components/interfaces/spi_data_bus.hdr")
            break

    return includes


def render_component_source(context: SubsystemContext) -> str:
    lines: List[str] = [
        f'#include "subsystems/{context.name}/{context.name}_component_functions.hpp"',
        "",
        "// Auto-generated component aggregation file",
        "",
    ]

    # Include interface headers for global variables used by components
    interface_includes = collect_interface_includes(context)
    for include in sorted(interface_includes):
        lines.insert(1, f'#include "{include}"')

    included_srcs: Set[Path] = set()
    for definition in sorted(context.unique_components.values(), key=lambda item: item.name):
        if definition.src_content and definition.src_path:
            resolved_src_path = definition.src_path.resolve()
            if resolved_src_path in included_srcs:
                continue
            try:
                rel = definition.src_path.relative_to(PROJECT_ROOT)
            except ValueError:
                rel = definition.src_path
            lines.append(f"// --- Begin: {rel} ---")
            lines.append(definition.src_content.rstrip())
            lines.append(f"// --- End: {rel} ---")
            included_srcs.add(resolved_src_path)
        else:
            lines.append(f"// NOTE: Source for component '{definition.name}' not found.")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def render_dispatch_header(context: SubsystemContext) -> str:
    guard = f"{context.identifier.upper()}_DISPATCH_TABLES_HPP"
    lines = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <cstddef>",
        "#include <cstdint>",
        "#include \"esp_err.h\"",
        "",
        f"// Auto-generated dispatch table header for subsystem {context.name}",
        "",
        "using init_function_t = esp_err_t (*)(void);",
        "using act_function_t = void (*)(void);",
        "",
        f"extern const init_function_t {context.identifier}_init_table[];",
        f"extern const act_function_t {context.identifier}_act_table[];",
        f"extern const uint32_t {context.identifier}_hitcount_table[];",
        f"extern const std::size_t {context.identifier}_init_table_size;",
        f"extern const std::size_t {context.identifier}_act_table_size;",
        "",
        f"#endif // {guard}",
    ]
    return "\n".join(lines) + "\n"


def render_dispatch_source(context: SubsystemContext) -> str:
    lines: List[str] = [
        f'#include "subsystems/{context.name}/{context.name}_dispatch_tables.hpp"',
        f'#include "subsystems/{context.name}/{context.name}_component_functions.hpp"',
        "",
        f"// Auto-generated dispatch table implementation for subsystem {context.name}",
        "",
    ]

    init_entries = []
    act_entries = []
    for visit in context.visits:
        if visit.template_type:
            init_entries.append(f"&{visit.init_func.replace('<T>', f'<{visit.template_type}>')}")
            act_entries.append(f"&{visit.act_func.replace('<T>', f'<{visit.template_type}>')}")
        else:
            init_entries.append(f"&{visit.init_func}")
            act_entries.append(f"&{visit.act_func}")

    hit_entries = [visit.hit_count for visit in context.visits]
    init_body = ",\n    ".join(init_entries)
    act_body = ",\n    ".join(act_entries)
    hit_body = ",\n    ".join(str(entry) for entry in hit_entries)
    lines.append(f"const init_function_t {context.identifier}_init_table[] = {{")
    if init_body:
        lines.append(f"    {init_body}")
    lines.append("};")
    lines.append("")
    lines.append(f"const act_function_t {context.identifier}_act_table[] = {{")
    if act_body:
        lines.append(f"    {act_body}")
    lines.append("};")
    lines.append("")
    lines.append(f"const uint32_t {context.identifier}_hitcount_table[] = {{")
    if hit_body:
        lines.append(f"    {hit_body}")
    lines.append("};")
    lines.append("")
    lines.append(
        f"const std::size_t {context.identifier}_init_table_size = sizeof({context.identifier}_init_table) / sizeof(init_function_t);"
    )
    lines.append(
        f"const std::size_t {context.identifier}_act_table_size = sizeof({context.identifier}_act_table) / sizeof(act_function_t);"
    )
    lines.append("")
    return "\n".join(lines) + "\n"


def render_main_header(context: SubsystemContext) -> str:
    guard = f"{context.identifier.upper()}_MAIN_HPP"
    lines = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#ifdef __cplusplus",
        "extern \"C\" {",
        "#endif",
        "",
        "void app_main(void);",
        "",
        "#ifdef __cplusplus",
        "}",
        "#endif",
        "",
        f"#endif // {guard}",
    ]
    return "\n".join(lines) + "\n"


def render_main_source(context: SubsystemContext) -> str:
    lines = [
        f'#include "subsystems/{context.name}/{context.name}_main.hpp"',
        f'#include "subsystems/{context.name}/{context.name}_dispatch_tables.hpp"',
        "",
        "#include <cstddef>",
        "#include <cstdint>",
        "",
        f"// Auto-generated subsystem main loop for {context.name}",
        "",
        "uint32_t g_loopCount = 0;",
        "",
        "extern \"C\" void app_main(void) {",
        f"    for (std::size_t i = 0; i < {context.identifier}_init_table_size; ++i) {{",
        f"        if ({context.identifier}_init_table[i]) {{",
        f"            {context.identifier}_init_table[i]();",
        "        }",
        "    }",
        "",
        "    while (true) {",
        f"        for (std::size_t i = 0; i < {context.identifier}_act_table_size; ++i) {{",
        f"            const auto func = {context.identifier}_act_table[i];",
        f"            const auto hit = {context.identifier}_hitcount_table[i];",
        "            if (func && hit > 0U && (g_loopCount % hit) == 0U) {",
        "                func();",
        "            }",
        "        }",
        "        ++g_loopCount;",
        "    }",
        "}",
        "",
    ]
    return "\n".join(lines)


def write_text_file(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="ascii")


def generate_platformio_file(subsystems: List[SubsystemContext]) -> None:
    if not subsystems:
        return
    default_envs = " ".join(sanitize_identifier(ctx.name).lower() for ctx in subsystems)
    lines: List[str] = [
        "; Auto-generated by tools/generate_tables.py",
        "; Generated PlatformIO environments per subsystem controller",
        "",
        "[platformio]",
        f"default_envs = {default_envs}",
        "",
    ]
    for ctx in subsystems:
        board = resolve_board_from_controller(ctx.controller)
        env_name = sanitize_identifier(ctx.name).lower()
        lines.extend(
            [
                f"[env:{env_name}]",
                "platform = espressif32",
                "framework = espidf",
                f"board = {board}",
                "monitor_speed = 115200",
                "build_src_filter =",
                "    -<*>",
                f"    +<subsystems/{ctx.name}/>",
                "    +<SharedMemory.cpp>",
                "",
            ]
        )
    write_text_file(PROJECT_ROOT / "platformio.generated.ini", "\n".join(lines))


def resolve_board_from_controller(controller: str) -> str:
    normalized = controller.strip()
    key_variants = {
        normalized,
        normalized.upper(),
        normalized.replace("-", "_").upper(),
    }
    for key in key_variants:
        if key in SUPPORTED_CONTROLLER_BOARDS:
            return SUPPORTED_CONTROLLER_BOARDS[key]
    raise RuntimeError(f"Unsupported controller '{controller}' - update controller mapping")


def generate_cmake_metadata(subsystems: List[SubsystemContext]) -> None:
    if not subsystems:
        return
    cmake_lines: List[str] = [
        "# Auto-generated by tools/generate_tables.py",
        "set(P32_GENERATED_SOURCES",
    ]
    for ctx in subsystems:
        cmake_lines.append(f"    src/subsystems/{ctx.name}/{ctx.name}_component_functions.cpp")
        cmake_lines.append(f"    src/subsystems/{ctx.name}/{ctx.name}_dispatch_tables.cpp")
        cmake_lines.append(f"    src/subsystems/{ctx.name}/{ctx.name}_main.cpp")
    cmake_lines.append(")")
    cmake_lines.append("")
    cmake_lines.append("set(P32_GENERATED_HEADERS")
    for ctx in subsystems:
        cmake_lines.append(f"    include/subsystems/{ctx.name}/{ctx.name}_component_functions.hpp")
        cmake_lines.append(f"    include/subsystems/{ctx.name}/{ctx.name}_dispatch_tables.hpp")
        cmake_lines.append(f"    include/subsystems/{ctx.name}/{ctx.name}_main.hpp")
    cmake_lines.append(")")
    cmake_lines.append("")
    write_text_file(PROJECT_ROOT / "p32_generated_sources.cmake", "\n".join(cmake_lines))
    cmakelists_path = PROJECT_ROOT / "CMakeLists.txt"
    existing = cmakelists_path.read_text(encoding="ascii")
    include_line = "include(p32_generated_sources.cmake OPTIONAL)"
    if include_line not in existing:
        updated = existing.rstrip() + "\n" + include_line + "\n"
        write_text_file(cmakelists_path, updated)


class TableGenerator:
    def __init__(self, root_config: Path, output_src: Path) -> None:
        self.root_config = root_config
        self.output_src = output_src
        self.loader = JsonLoader()
        self.sources = ComponentSourceIndex(CONFIG_ROOT / "components")
        self.subsystems: Dict[str, SubsystemContext] = {}
        self.subsystem_order: List[SubsystemContext] = []

    def run(self) -> None:
        self._process_json(self.root_config, [], None)
        if not self.subsystem_order:
            raise RuntimeError("No subsystem controllers discovered in configuration")
        for ctx in self.subsystem_order:
            self._emit_subsystem_outputs(ctx)
        generate_platformio_file(self.subsystem_order)
        generate_cmake_metadata(self.subsystem_order)

    def _process_json(self, json_path: Path, stack: List[SubsystemContext], template_type: Optional[str]) -> None:
        data = self.loader.load(json_path)
        component_name = data.get("name")
        controller_value = data.get("controller")
        is_controller = bool(controller_value and component_name)
        new_context: Optional[SubsystemContext] = None
        if is_controller:
            controller_str = str(controller_value)
            context = self.subsystems.get(component_name)
            if context is None:
                context = SubsystemContext(
                    name=component_name,
                    controller=controller_str,
                    json_path=json_path,
                )
                self.subsystems[component_name] = context
                self.subsystem_order.append(context)
            else:
                context.controller = controller_str
                if context.json_path is None:
                    context.json_path = json_path
            stack.append(context)
            new_context = context
        active_context = stack[-1] if stack else None
        if active_context is not None and component_name and not is_controller:
            active_context.register_component(json_path, data, self.sources, template_type)
        components = data.get("components")
        if isinstance(components, list):
            for entry in components:
                if isinstance(entry, str):
                    child_path, child_template_type = resolve_json_reference(json_path, entry)
                    self._process_json(child_path, stack, child_template_type)
                elif isinstance(entry, dict):
                    self._process_inline(entry, json_path, stack)
        if new_context is not None:
            stack.pop()

    def _process_inline(
        self,
        data: Dict[str, Any],
        base_path: Path,
        stack: List[SubsystemContext],
    ) -> None:
        component_name = data.get("name")
        controller_value = data.get("controller")
        is_controller = bool(controller_value and component_name)
        new_context: Optional[SubsystemContext] = None
        if is_controller:
            controller_str = str(controller_value)
            context = self.subsystems.get(component_name)
            if context is None:
                context = SubsystemContext(
                    name=component_name,
                    controller=controller_str,
                    json_path=None,
                )
                self.subsystems[component_name] = context
                self.subsystem_order.append(context)
            else:
                context.controller = controller_str
            stack.append(context)
            new_context = context
        active_context = stack[-1] if stack else None
        if active_context is not None and component_name and not is_controller:
            active_context.register_component(None, data, self.sources)
        nested = data.get("components")
        if isinstance(nested, list):
            for entry in nested:
                if isinstance(entry, str):
                    child_path, child_template_type = resolve_json_reference(base_path, entry)
                    self._process_json(child_path, stack, child_template_type)
                elif isinstance(entry, dict):
                    self._process_inline(entry, base_path, stack)
        if new_context is not None:
            stack.pop()

    def _emit_subsystem_outputs(self, context: SubsystemContext) -> None:
        subsystem_src_dir = self.output_src / "subsystems" / context.name
        subsystem_inc_dir = INCLUDE_ROOT / "subsystems" / context.name
        write_text_file(
            subsystem_inc_dir / f"{context.name}_component_functions.hpp",
            render_component_header(context),
        )
        write_text_file(
            subsystem_src_dir / f"{context.name}_component_functions.cpp",
            render_component_source(context),
        )
        write_text_file(
            subsystem_inc_dir / f"{context.name}_dispatch_tables.hpp",
            render_dispatch_header(context),
        )
        write_text_file(
            subsystem_src_dir / f"{context.name}_dispatch_tables.cpp",
            render_dispatch_source(context),
        )
        write_text_file(
            subsystem_inc_dir / f"{context.name}_main.hpp",
            render_main_header(context),
        )
        write_text_file(
            subsystem_src_dir / f"{context.name}_main.cpp",
            render_main_source(context),
        )


def resolve_root_config(argument: str) -> Path:
    candidate = Path(argument)
    if not candidate.is_absolute():
        direct = PROJECT_ROOT / argument
        if direct.exists():
            return direct.resolve()
    if candidate.exists():
        return candidate.resolve()
    matches = list(CONFIG_ROOT.rglob("*.json"))
    filtered = [path for path in matches if path.stem == argument]
    if len(filtered) == 1:
        return filtered[0].resolve()
    if not filtered:
        raise FileNotFoundError(f"Unable to locate configuration '{argument}'")
    raise RuntimeError(f"Ambiguous configuration name '{argument}' - provide full path")


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate subsystem dispatch tables and component aggregation files",
    )
    parser.add_argument(
        "config",
        help="Root JSON configuration file or name (e.g. goblin_full)",
    )
    parser.add_argument(
        "output",
        help="Output directory for generated source files (relative to project root)",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_arguments()
    try:
        root_config = resolve_root_config(args.config)
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)
    output_dir = Path(args.output)
    if not output_dir.is_absolute():
        output_dir = PROJECT_ROOT / output_dir
    output_dir.mkdir(parents=True, exist_ok=True)
    generator = TableGenerator(root_config, output_dir)
    try:
        generator.run()
    except Exception as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)
    print("Generation complete:")
    for ctx in generator.subsystem_order:
        print(f"  - Subsystem '{ctx.name}' ({ctx.controller}) -> {len(ctx.visits)} entries")


if __name__ == "__main__":
    main()
