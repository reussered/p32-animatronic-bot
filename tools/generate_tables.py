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

    def register_component(
        self,
        json_path: Optional[Path],
        data: Dict[str, Any],
        sources: ComponentSourceIndex,
    ) -> None:
        component_name = data.get("name")
        if not component_name:
            return
        init_func, act_func = resolve_function_names(component_name, data)
        hit_count = resolve_hit_count(data)
        self.visits.append(
            ComponentVisit(
                name=component_name,
                init_func=init_func,
                act_func=act_func,
                hit_count=hit_count,
                json_path=json_path,
            )
        )
        unique_key = str(json_path.resolve()) if json_path is not None else f"inline::{component_name}"
        if unique_key in self.unique_components:
            return
        cloned_data = json.loads(json.dumps(data, ensure_ascii=True))
        definition = ComponentDefinition(
            name=component_name,
            json_path=json_path,
            data=cloned_data,
            init_func=init_func,
            act_func=act_func,
            hit_count=hit_count,
            src_path=sources.find_source_file(component_name, data, json_path),
            hdr_path=sources.find_header_file(component_name, data, json_path),
        )
        definition.load_artifacts()
        self.unique_components[unique_key] = definition

    @property
    def identifier(self) -> str:
        return sanitize_identifier(self.name)


def sanitize_identifier(value: str) -> str:
    sanitized = [ch if ch.isalnum() or ch == "_" else "_" for ch in value]
    result = "".join(sanitized) or "identifier"
    if result[0].isdigit():
        result = "_" + result
    return result


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


def resolve_json_reference(base_path: Path, reference: str) -> Path:
    candidate = Path(reference)
    if not candidate.is_absolute():
        if reference.startswith("config/"):
            candidate = PROJECT_ROOT / reference
        else:
            candidate = base_path.parent / reference
    candidate = candidate.resolve()
    if not candidate.exists():
        raise FileNotFoundError(f"Referenced JSON file not found: {reference}")
    return candidate


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


def generate_struct_members(data: Dict[str, Any], indent: int = 4) -> List[str]:
    lines: List[str] = []
    for key, value in data.items():
        if key == "components":
            continue
        sanitized_key = sanitize_identifier(key)
        comment_needed = sanitized_key != key
        indent_spaces = " " * indent
        if isinstance(value, dict):
            struct_type = f"{sanitized_key}_t"
            if comment_needed:
                lines.append(f"{indent_spaces}// {key}")
            lines.append(f"{indent_spaces}struct {struct_type} {{")
            lines.extend(generate_struct_members(value, indent + 4))
            lines.append(f"{indent_spaces}}} {sanitized_key};")
        elif isinstance(value, list):
            if comment_needed:
                lines.append(f"{indent_spaces}// {key}")
            if not value:
                lines.append(f"{indent_spaces}// Empty array: {sanitized_key}")
                continue
            if all(is_primitive(item) for item in value):
                element_type = json_to_cpp_type(value[0])
                homogeneous = all(isinstance(item, type(value[0])) for item in value)
                if not homogeneous:
                    element_type = "const char*"
                literals = ", ".join(to_cpp_literal(item) for item in value)
                lines.append(
                    f"{indent_spaces}{element_type} {sanitized_key}[{len(value)}] = {{ {literals} }};"
                )
            else:
                json_text = json.dumps(value, ensure_ascii=True)
                literal = to_cpp_literal(json_text)
                lines.append(f"{indent_spaces}const char* {sanitized_key} = {literal};")
        else:
            if comment_needed:
                lines.append(f"{indent_spaces}// {key}")
            cpp_type = json_to_cpp_type(value)
            literal = to_cpp_literal(value)
            lines.append(f"{indent_spaces}{cpp_type} {sanitized_key} = {literal};")
    if not lines:
        lines.append(" " * indent + "// (empty)")
    return lines


def render_component_struct(component: ComponentDefinition) -> str:
    lines: List[str] = []
    lines.append(f"struct {component.name}_config {{")
    lines.extend(generate_struct_members(component.data))
    lines.append("};")
    return "\n".join(lines)


def render_component_header(context: SubsystemContext) -> str:
    guard = f"{context.identifier.upper()}_COMPONENT_FUNCTIONS_HPP"
    header_lines: List[str] = [
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <cstddef>",
        "#include <cstdint>",
        "#include \"esp_err.h\"",
        "",
        "// Auto-generated by tools/generate_tables.py",
        f"// Subsystem: {context.name}",
        f"// Controller: {context.controller}",
        "",
    ]
    structs: List[str] = []
    prototypes: List[str] = []
    additional: List[str] = []
    for definition in sorted(context.unique_components.values(), key=lambda item: item.name):
        if definition.json_path:
            try:
                source_desc = definition.json_path.relative_to(PROJECT_ROOT)
            except ValueError:
                source_desc = definition.json_path
        else:
            source_desc = "inline component"
        structs.append(f"// Component definition sourced from {source_desc}")
        structs.append(render_component_struct(definition))
        structs.append("")
        prototypes.append(f"esp_err_t {definition.init_func}(void);")
        prototypes.append(f"void {definition.act_func}(void);")
        if definition.hdr_content:
            if definition.hdr_path:
                try:
                    rel = definition.hdr_path.relative_to(PROJECT_ROOT)
                except ValueError:
                    rel = definition.hdr_path
            else:
                rel = ""
            additional.append(f"// Declarations from {rel}")
            additional.append(definition.hdr_content.strip())
            additional.append("")
    header_lines.extend(structs)
    header_lines.append("// ---------------------------------------------------------------------------")
    header_lines.append("// Function prototypes")
    header_lines.append("// ---------------------------------------------------------------------------")
    header_lines.extend(prototypes)
    header_lines.append("")
    header_lines.extend(additional)
    header_lines.append(f"#endif // {guard}")
    return "\n".join(header_lines) + "\n"


def render_component_source(context: SubsystemContext) -> str:
    lines: List[str] = [
        f'#include "{context.name}_component_functions.hpp"',
        "",
        "// Auto-generated component aggregation file",
        "",
    ]
    for definition in sorted(context.unique_components.values(), key=lambda item: item.name):
        if definition.src_content and definition.src_path:
            try:
                rel = definition.src_path.relative_to(PROJECT_ROOT)
            except ValueError:
                rel = definition.src_path
            lines.append(f"// --- Begin: {rel} ---")
            lines.append(definition.src_content.rstrip())
            lines.append(f"// --- End: {rel} ---")
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
        f'#include "{context.name}_dispatch_tables.hpp"',
        f'#include "{context.name}_component_functions.hpp"',
        "",
        f"// Auto-generated dispatch table implementation for subsystem {context.name}",
        "",
    ]
    init_entries = [visit.init_func for visit in context.visits]
    act_entries = [visit.act_func for visit in context.visits]
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
        f'#include "{context.name}_main.hpp"',
        f'#include "{context.name}_dispatch_tables.hpp"',
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
        self._process_json(self.root_config, stack=[])
        if not self.subsystem_order:
            raise RuntimeError("No subsystem controllers discovered in configuration")
        for ctx in self.subsystem_order:
            self._emit_subsystem_outputs(ctx)
        generate_platformio_file(self.subsystem_order)
        generate_cmake_metadata(self.subsystem_order)

    def _process_json(self, json_path: Path, stack: List[SubsystemContext]) -> None:
        data = self.loader.load(json_path)
        component_name = data.get("name")
        controller_value = data.get("controller")
        new_context: Optional[SubsystemContext] = None
        if controller_value and component_name:
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
        if active_context is not None and component_name:
            active_context.register_component(json_path, data, self.sources)
        components = data.get("components")
        if isinstance(components, list):
            for entry in components:
                if isinstance(entry, str):
                    child_path = resolve_json_reference(json_path, entry)
                    self._process_json(child_path, stack)
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
        new_context: Optional[SubsystemContext] = None
        if controller_value and component_name:
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
        if active_context is not None and component_name:
            active_context.register_component(None, data, self.sources)
        nested = data.get("components")
        if isinstance(nested, list):
            for entry in nested:
                if isinstance(entry, str):
                    child_path = resolve_json_reference(base_path, entry)
                    self._process_json(child_path, stack)
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
