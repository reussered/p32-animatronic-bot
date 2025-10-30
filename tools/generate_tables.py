#!/usr/bin/env python3
"""
P32 Component Table Generator
Reads JSON bot configuration and generates initTable.h, actTable.h and component files
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Any

class P32ComponentGenerator:
    def __init__(self, config_dir: str, output_dir: str):
        self.config_dir = Path(config_dir)
        self.output_dir = Path(output_dir)
        self.include_dir = Path(output_dir).parent / "include"
        self.components = []
        self.init_functions = []
        self.act_functions = []
        self.seen_components = set()  # Track components we've seen for deduplication
        self.component_configs = {}   # Store component configs for struct generation
        self.current_subsystem = "unknown"  # Current subsystem context
        self.current_component = None  # Current component being processed
        self.is_bot_level = True  # Flag to track if we're processing the root bot file
        self.family_components_added = False  # Flag to track if family_components have been added to first subsystem
        self.family_components = []  # Store family_components from bot config
        self.controller_chip = None  # ESP32 chip specification
        self.used_pins = set()  # Track pins that have been assigned
        self.bus_assignments = {}  # Track pin assignments for buses
        
    def load_bot_config(self, bot_name: str) -> Dict[str, Any]:
        """Load bot configuration from JSON file"""
        # Try multiple locations: bots/, subsystems/, families, or direct path
        possible_paths = [
            self.config_dir / "bots" / f"{bot_name}.json",
            self.config_dir / "subsystems" / f"{bot_name}.json",
            self.config_dir / "bots" / "bot_families" / "goblins" / f"{bot_name}.json",
            self.config_dir / "bots" / "bot_families" / "cats" / f"{bot_name}.json",
            self.config_dir / "bots" / "bot_families" / "bears" / f"{bot_name}.json",
            Path(bot_name)  # Direct path
        ]
        
        bot_file = None
        for path in possible_paths:
            if path.exists():
                bot_file = path
                break
        
        if bot_file is None:
            raise FileNotFoundError(f"Bot config not found: {bot_name} (tried bots/, subsystems/, families, and direct path)")
        
        print(f"DEBUG: Loading bot config from: {bot_file}")
        with open(bot_file, 'r') as f:
            return json.load(f)
    
    def load_positioned_components(self, bot_config: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Load all positioned components referenced in bot config - handles recursive composition"""
        components = []
        
        # Store family_components for addition to first subsystem
        if "family_components" in bot_config:
            self.family_components = bot_config["family_components"]
            print(f"DEBUG: Stored {len(self.family_components)} family_components for first subsystem")
        
        # Handle direct positioned_components (backward compatibility)
        if "positioned_components" in bot_config:
            for component_ref in bot_config["positioned_components"]:
                components.extend(self._load_component_recursive(component_ref))
        
        # Handle new subsystem_assemblies structure (recursive composition)
        if "subsystem_assemblies" in bot_config or "subsystems" in bot_config:
            subsystem_key = "subsystems" if "subsystems" in bot_config else "subsystem_assemblies"
            for subsystem_ref in bot_config[subsystem_key]:
                components.extend(self._load_subsystem_recursive(subsystem_ref))
        
        # Handle Components array at bot level (standardized plural form)
        if "Components" in bot_config:
            for component_ref in bot_config["Components"]:
                components.extend(self._load_component_recursive(component_ref))
        
        return components
    
    def _load_subsystem_for_traversal(self, subsystem_ref: str) -> None:
        """Load a subsystem for traversal - add family_components to first subsystem"""
        # Remove 'config/' prefix if present since config_dir already points to config/
        clean_ref = subsystem_ref.replace("config/", "") if subsystem_ref.startswith("config/") else subsystem_ref
        subsystem_file = self.config_dir / clean_ref
        
        if subsystem_file.exists():
            print(f"DEBUG: Loading subsystem for traversal: {subsystem_file}")
            with open(subsystem_file, 'r', encoding='ascii') as f:
                subsystem_data = json.load(f)
                subsystem_data['config_file'] = subsystem_ref
                
                # Add family_components to the first subsystem encountered
                if not self.family_components_added and self.family_components:
                    print(f"DEBUG: Adding {len(self.family_components)} family_components to first subsystem: {subsystem_ref}")
                    # Add family components as the first components
                    for family_component_ref in self.family_components:
                        self._process_single_component(family_component_ref)
                    self.family_components_added = True
                
                # Process the subsystem itself
                subsystem_name = subsystem_data.get("component_name", subsystem_data.get("name", subsystem_ref.split('/')[-1].replace('.json', '')))
                hit_count = subsystem_data.get("timing", {}).get("hitCount", 1)
                software = subsystem_data.get("software", {})
                init_func = software.get("init_function") or self.generate_function_name(subsystem_name, "init")
                act_func = software.get("act_function") or self.generate_function_name(subsystem_name, "act")
                
                self.components.append({
                    "name": subsystem_name,
                    "init_func": init_func,
                    "act_func": act_func,
                    "hitCount": hit_count,
                    "description": subsystem_data.get("description", f"{subsystem_name} subsystem"),
                    "type": "traversed",
                    "config": subsystem_data,
                    "subsystem": self.current_subsystem
                })
                
                # Generate configuration struct only for first encounter
                if subsystem_name not in self.seen_components:
                    self.seen_components.add(subsystem_name)
                    self._generate_component_config_struct(subsystem_name, subsystem_data)
                    print(f"DEBUG: First encounter of subsystem: {subsystem_name}")
                
                # Continue traversing the subsystem's JSON structure
                self.traverse_json_for_components(subsystem_data)
    
    def _load_component_recursive(self, component_ref: str) -> List[Dict[str, Any]]:
        """Recursively load a single component and its nested components"""
        components = []
        
        # Remove 'config/' prefix if present since config_dir already points to config/
        clean_ref = component_ref.replace("config/", "") if component_ref.startswith("config/") else component_ref
        component_file = self.config_dir / clean_ref
        
        if component_file.exists():
            print(f"DEBUG: Loading component: {component_file}")
            try:
                with open(component_file, 'r', encoding='ascii') as f:
                    component_data = json.load(f)
                    component_data['config_file'] = component_ref
                    components.append(component_data)
                    
                    # Process Components array (standardized plural form)
                    if "Components" in component_data:
                        for component_ref in component_data["Components"]:
                            components.extend(self._load_component_recursive(component_ref))
                    
                    # Process single Component reference (standardized singular form)
                    if "Component" in component_data:
                        components.extend(self._load_component_recursive(component_data["Component"]))
                    
                    # Also check for legacy positioned_components (backward compatibility)
                    if "positioned_components" in component_data:
                        for nested_ref in component_data["positioned_components"]:
                            components.extend(self._load_component_recursive(nested_ref))
                            
                    # Load hardware_reference if present (positioned components reference hardware)
                    if "hardware_reference" in component_data:
                        hardware_ref = component_data["hardware_reference"]
                        components.extend(self._load_component_recursive(hardware_ref))
            except json.JSONDecodeError as e:
                print(f"ERROR: Invalid JSON in {component_file}: {e}")
            except Exception as e:
                print(f"ERROR: Failed to load {component_file}: {e}")
        else:
            print(f"Warning: Component file not found: {component_file}")
        
        return components
    
    def _load_subsystem_recursive(self, subsystem_ref: str) -> List[Dict[str, Any]]:
        """Recursively load a subsystem and all its components"""
        components = []
        
        # Remove 'config/' prefix if present since config_dir already points to config/
        clean_ref = subsystem_ref.replace("config/", "") if subsystem_ref.startswith("config/") else subsystem_ref
        subsystem_file = self.config_dir / clean_ref
        
        if subsystem_file.exists():
            print(f"DEBUG: Loading subsystem: {subsystem_file}")
            with open(subsystem_file, 'r') as f:
                subsystem_data = json.load(f)
                subsystem_data['config_file'] = subsystem_ref
                
                # Add the subsystem itself as a component
                components.append(subsystem_data)
                
                # Add family_components to the first subsystem encountered
                if not self.family_components_added and self.family_components:
                    print(f"DEBUG: Adding {len(self.family_components)} family_components to first subsystem: {subsystem_ref}")
                    for family_component_ref in self.family_components:
                        components.extend(self._load_component_recursive(family_component_ref))
                    self.family_components_added = True
                        
                # Also check for legacy positioned_components (backward compatibility)
                if "positioned_components" in subsystem_data:
                    for component_ref in subsystem_data["positioned_components"]:
                        components.extend(self._load_component_recursive(component_ref))
        else:
            print(f"Warning: Subsystem file not found: {subsystem_file}")
        
        return components

    def generate_function_name(self, component_name: str, func_type: str) -> str:
        """Generate standardized function names from component names"""
        # Convert component name to function name format
        # RULE 5: Functions should be {name}_init() and {name}_act() - NO p32_comp_ prefix
        name = component_name.lower().replace(' ', '_').replace('-', '_')
        return f"{name}_{func_type}"

    def traverse_json_for_components(self, json_data: Any) -> None:
        """Streaming JSON parser following component references only (no object hierarchy traversal)"""
        if isinstance(json_data, dict):
            for key, value in json_data.items():
                print(f"DEBUG: Traversing key: {key} (bot_level: {self._is_bot_level()})")
                if key == 'controller':
                    # Set subsystem context to ESP32 chip type
                    self.current_subsystem = str(value)
                    print(f"DEBUG: Set subsystem context to: {self.current_subsystem}")
                elif key in ['components', 'family_components']:
                    # Process component reference(s) - recurse into component files
                    print(f"DEBUG: Processing {key} with {len(value) if isinstance(value, list) else 1} items")
                    self._process_component_references(value)
                elif key in ['subsystem_assemblies', 'subsystems'] and self._is_bot_level():
                    # Process subsystems - load subsystem files and their components
                    print(f"DEBUG: Processing {key} with {len(value) if isinstance(value, list) else 1} subsystems (bot_level: {self._is_bot_level()})")
                    if isinstance(value, list):
                        for subsystem_ref in value:
                            if isinstance(subsystem_ref, str):
                                self._load_subsystem_for_traversal(subsystem_ref)
                elif key in ['software', 'hardware', 'interfaces'] and isinstance(value, dict):
                    # Continue processing nested sections that may contain components
                    self.traverse_json_for_components(value)
                else:
                    # Process other key-value pairs (extract configuration data)
                    self._process_key_value_pair(key, value)
        elif isinstance(json_data, list):
            # Handle array values (like components arrays)
            for item in json_data:
                if isinstance(item, str):
                    # String in array might be component reference
                    self._process_potential_component_reference(item)
                else:
                    # For non-string items in arrays, check if they're objects with components
                    self.traverse_json_for_components(item)
    
    def _process_key_value_pair(self, key: str, value: Any) -> None:
        """Process a key-value pair for configuration data extraction"""
        # For now, just log - in full implementation this would extract
        # configuration data for the current component
        if hasattr(self, 'current_component') and self.current_component:
            print(f"DEBUG: Processing config key '{key}' for component '{self.current_component}'")
    
    def _is_bot_level(self) -> bool:
        """Check if we're currently processing the root bot level JSON"""
        return self.is_bot_level
    
    def _process_component_references(self, component_refs: Any) -> None:
        """Process component reference(s) - can be string or array"""
        if isinstance(component_refs, str):
            # Single component reference
            self._process_single_component(component_refs)
        elif isinstance(component_refs, list):
            # Array of component references
            for ref in component_refs:
                if isinstance(ref, str):
                    self._process_single_component(ref)
    
    def _process_single_component(self, component_ref: str) -> None:
        """Process a single component reference"""
        if not component_ref or not isinstance(component_ref, str):
            return
            
        # Remove 'config/' prefix if present
        clean_ref = component_ref.replace("config/", "") if component_ref.startswith("config/") else component_ref
        component_file = self.config_dir / clean_ref
        
        if component_file.exists():
            print(f"DEBUG: Processing component: {component_file}")
            # When processing component files, we're no longer at bot level
            was_bot_level = self.is_bot_level
            self.is_bot_level = False
            try:
                with open(component_file, 'r', encoding='ascii') as f:
                    component_data = json.load(f)
                
                # component_name MUST be provided by AI - no default allowed
                if 'component_name' not in component_data:
                    print(f"ERROR: Missing mandatory field 'component_name' in {component_file}")
                    print(f"AI agents must always specify component_name when creating components")
                    return
                    
                comp_name = component_data["component_name"]
                
                # Validate component_name doesn't contain "unknown" unless user-specified
                if "unknown" in comp_name.lower() and comp_name.lower() != "unknown":
                    print(f"ERROR: Component name '{comp_name}' contains 'unknown' - this is illegal unless explicitly specified by user")
                    return
                
                # Set defaults for all other fields so AI agents have complete templates
                if 'version' not in component_data:
                    component_data['version'] = '1.0.0.0'
                    
                if 'description' not in component_data:
                    component_data['description'] = '<describe the components purpose and functionality here>'
                    
                if 'author' not in component_data:
                    component_data['author'] = 'config/author.json'
                    
                if 'relative_filename' not in component_data:
                    # Generate relative_filename from component_name if missing
                    component_data['relative_filename'] = f'config/components/{comp_name}.json'
                
                hit_count = component_data.get("timing", {}).get("hitCount", 1)
                
                # Set current component context for configuration processing
                self.current_component = comp_name
                
                # Check if component has already been added to dispatch table (prevent duplicates)
                component_already_added = any(comp["name"] == comp_name for comp in self.components)
                
                if not component_already_added:
                    # Add to dispatch tables when component is first encountered
                    # Check for explicitly defined function names in software section
                    software = component_data.get("software", {})
                    init_func = software.get("init_function") or self.generate_function_name(comp_name, "init")
                    act_func = software.get("act_function") or self.generate_function_name(comp_name, "act")
                    
                    self.components.append({
                        "name": comp_name,
                        "init_func": init_func,
                        "act_func": act_func,
                        "hitCount": hit_count,
                        "description": component_data.get("description", f"{comp_name} component"),
                        "type": "traversed",
                        "config": component_data,
                        "subsystem": self.current_subsystem
                    })
                    print(f"DEBUG: Added component '{comp_name}' to dispatch table")
                else:
                    print(f"DEBUG: Skipping duplicate component '{comp_name}' - already in dispatch table")
                
                # Generate configuration struct only for first encounter
                if comp_name not in self.seen_components:
                    self.seen_components.add(comp_name)
                    self._generate_component_config_struct(comp_name, component_data)
                    print(f"DEBUG: First encounter of component: {comp_name}")
                
                # Recursively process this component's "components" key
                self.traverse_json_for_components(component_data)
                    
            except json.JSONDecodeError as e:
                print(f"ERROR: Invalid JSON in {component_file}: {e}")
            except Exception as e:
                print(f"ERROR: Failed to process {component_file}: {e}")
            finally:
                # Restore bot level state
                self.is_bot_level = was_bot_level
        else:
            print(f"Warning: Component file not found: {component_file}")
    
    def _generate_component_config_struct(self, comp_name: str, component_data: Dict[str, Any]) -> None:
        """Generate C++ struct for component configuration mirroring JSON hierarchy"""
        # This is a placeholder - full implementation would recursively generate
        # nested structs based on the JSON structure
        # For now, we'll store the data for later use
        pass
    
    def generate_unified_header(self) -> str:
        """Generate unified p32_component_registry.hpp header file content"""
        content = [
            "#ifndef P32_COMPONENT_REGISTRY_HPP",
            "#define P32_COMPONENT_REGISTRY_HPP",
            "",
            '#include "esp_err.h"',
            '#include <stdint.h>',
            "",
            "// ============================================================================",
            "// P32 Component Registry - Function Declarations",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
            "",
            "// Forward Declarations - Init Functions (from individual component files)",
            ""
        ]

        for comp in self.components:
            content.append(f"esp_err_t {comp['init_func']}(void);")

        content.extend([
            "",
            "// Forward Declarations - Act Functions (from individual component files)",
            ""
        ])

        for comp in self.components:
            content.append(f"void {comp['act_func']}(void);")

        content.extend([
            "",
            "// Table size constant - ALL TABLES MUST HAVE SAME SIZE",
            f"#define TABLE_SIZE {len(self.components)}",
            "",
            "#endif // P32_COMPONENT_REGISTRY_HPP"
        ])

        return "\n".join(content)
    
    def generate_unified_implementation(self) -> str:
        """Generate unified component_tables.cpp implementation file content"""
        content = [
            "// ============================================================================",
            "// P32 Component Dispatch Tables - Implementation",
            "// Auto-generated from JSON bot configuration",
            "// ============================================================================",
            "",
            '#include "p32_component_registry.hpp"',
            ""
        ]

        # Include individual component headers (RULE 5 compliance)
        includes = set()
        for comp in self.components:
            # Use base component type for includes (remove _init/_act suffix from function name)
            base_name = comp['init_func'].replace('_init', '')
            include_file = f'"components/{base_name}.hdr"'
            if include_file not in includes:
                content.append(f"#include {include_file}")
                includes.add(include_file)

        content.extend([
            "",
            "// ============================================================================",
            "// Initialization Table",
            "// ============================================================================",
            "",
            f"esp_err_t (*initTable[TABLE_SIZE])(void) = {{"
        ])

        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            content.append(f"    {comp['init_func']}{comma}")

        content.extend([
            "};",
            "",
            "// ============================================================================",
            "// Action Table", 
            "// ============================================================================",
            "",
            f"void (*actTable[TABLE_SIZE])(void) = {{"
        ])

        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            comment = f"// [{i}] {comp['description']}"
            content.append(f"    {comp['act_func']}{comma:1s}    {comment}")

        content.extend([
            "};",
            "",
            "// ============================================================================",
            "// Timing Table - Execution Frequency Control",
            "// ============================================================================",
            "",
            f"uint32_t hitCountTable[TABLE_SIZE] = {{"
        ])

        for i, comp in enumerate(self.components):
            comma = "," if i < len(self.components) - 1 else ""
            comment = f"// [{i}] {comp['name']} - every {comp['hitCount']} loops"
            content.append(f"    {comp['hitCount']}{comma:1s}    {comment}")

        content.extend([
            "};",
            ""
        ])

        return "\n".join(content)
    
    def load_controller_chip_spec(self, chip_name: str) -> Dict[str, Any]:
        """Load ESP32 chip specification for pin assignment"""
        chip_file = self.config_dir / "components" / "hardware" / f"{chip_name}_chip.json"
        if not chip_file.exists():
            print(f"Warning: Chip spec not found: {chip_file}, using default ESP32-S3")
            chip_file = self.config_dir / "components" / "hardware" / "esp32_s3_devkit_chip.json"
        
        if chip_file.exists():
            with open(chip_file, 'r') as f:
                chip_data = json.load(f)
                print(f"DEBUG: Loaded chip spec: {chip_data.get('component_name', 'unknown')}")
                return chip_data
        else:
            print(f"ERROR: No chip specification found for {chip_name}")
            return None
    
    def assign_pins_to_buses(self) -> bool:
        """Validate pin assignment by simulating the runtime component loop pin assignment process"""
        if not self.controller_chip:
            print("Warning: No controller chip loaded, skipping pin validation")
            return True
        
        chip_pins = self.controller_chip.get("exposed_pins", [])
        print(f"DEBUG: Validating pins for {len(chip_pins)} available pins on {self.controller_chip.get('component_name', 'unknown')}")
        
        # Count how many times each bus appears in the component list
        bus_encounter_counts = {}
        for comp in self.components:
            comp_data = comp.get("config", {})
            bus_type = comp_data.get("bus_type")
            if bus_type:
                bus_name = comp.get("name", "unknown")
                bus_encounter_counts[bus_name] = bus_encounter_counts.get(bus_name, 0) + 1
        
        # Simulate runtime pin assignment process
        active_pin_index = 0  # Resets every time through component loop
        bus_shared_assigned = {}  # Track which buses have had their shared pins assigned
        max_pin_index = 0
        
        for comp in self.components:
            comp_data = comp.get("config", {})
            comp_name = comp.get("name", "unknown")
            
            # Check if this is a bus component
            bus_type = comp_data.get("bus_type")
            if bus_type:
                bus_name = comp_name
                pin_reqs = comp_data.get("pin_requirements", {})
                
                # First encounter with this bus: assign shared pins
                if bus_name not in bus_shared_assigned:
                    shared_needed = pin_reqs.get("shared_pins_needed", [])
                    pins_assigned = len(shared_needed)
                    active_pin_index += pins_assigned
                    bus_shared_assigned[bus_name] = True
                    
                    print(f"DEBUG: Bus {bus_name} assigned {pins_assigned} shared pins, active_pin_index now {active_pin_index}")
                
                # Each encounter beyond the first represents a device using unique pins
                else:
                    unique_needed = pin_reqs.get("unique_pins_needed", [])
                    pins_assigned = len(unique_needed)
                    if pins_assigned == 0:
                        # If no pin requirements specified, assume 1 pin (CS) for SPI devices
                        pins_assigned = 1
                    
                    active_pin_index += pins_assigned
                    
                    print(f"DEBUG: Additional {bus_name} encounter (device) assigned {pins_assigned} unique pins, active_pin_index now {active_pin_index}")
            
            # Track the maximum pin index reached
            max_pin_index = max(max_pin_index, active_pin_index)
        
        print(f"DEBUG: Bus encounter counts: {bus_encounter_counts}")
        print(f"DEBUG: Maximum pin index reached: {max_pin_index}")
        
        # Validate against available pins
        if max_pin_index > len(chip_pins):
            print(f"ERROR: Insufficient pins! Chip has {len(chip_pins)} pins but pin assignment reached index {max_pin_index}")
            self._suggest_alternative_chips(max_pin_index)
            return False
        
        print(f"SUCCESS: Pin validation passed - max pin index {max_pin_index}, {len(chip_pins)} pins available")
        return True
    
    def _suggest_alternative_chips(self, pins_needed: int) -> None:
        """Suggest alternative ESP32 chips that have enough pins"""
        chip_options = [
            {"name": "esp32_s3_devkit", "pins": 39, "price": 45.99},
            {"name": "esp32_c3_devkit", "pins": 15, "price": 12.99},
            {"name": "esp32_wroom_32", "pins": 38, "price": 8.99},
            {"name": "esp32_wrover", "pins": 38, "price": 9.99}
        ]
        
        suitable_chips = [chip for chip in chip_options if chip["pins"] >= pins_needed]
        suitable_chips.sort(key=lambda x: x["price"])  # Sort by price ascending
        
        if suitable_chips:
            print("SUGGESTED ALTERNATIVES:")
            for chip in suitable_chips[:3]:  # Show top 3 cheapest options
                print(f"  - {chip['name']}: {chip['pins']} pins (${chip['price']})")
        else:
            print("NO SUITABLE ALTERNATIVES FOUND - Consider reducing bus devices or using pin multiplexing")
    
    def _assign_pins_to_bus(self, bus_comp: Dict[str, Any], bus_type: str) -> None:
        """Assign pins to a specific bus component"""
        comp_data = bus_comp.get("config", {})
        pin_requirements = comp_data.get("pin_requirements", {})
        
        shared_needed = pin_requirements.get("shared_pins_needed", [])
        unique_needed = pin_requirements.get("unique_pins_needed", [])
        
        bus_name = bus_comp.get("name", "unknown")
        assignment = {"shared": {}, "unique": {}}
        
        # Assign shared pins (used by all devices on this bus)
        for req in shared_needed:
            func = req.get("function")
            count = req.get("count", 1)
            assigned_pins = self._find_available_pins(func, count, allow_shared=False)
            if assigned_pins:
                assignment["shared"][func] = assigned_pins
                print(f"DEBUG: Assigned shared {func} pins {assigned_pins} to {bus_type} bus {bus_name}")
            else:
                print(f"Warning: Could not assign {count} {func} pins for {bus_type} bus {bus_name}")
        
        # Assign unique pins (one per device on this bus)
        for req in unique_needed:
            func = req.get("function")
            count = req.get("count", 1)
            assigned_pins = self._find_available_pins(func, count, allow_shared=True)
            if assigned_pins:
                assignment["unique"][func] = assigned_pins
                print(f"DEBUG: Assigned unique {func} pins {assigned_pins} to {bus_type} bus {bus_name}")
            else:
                print(f"Warning: Could not assign {count} {func} pins for {bus_type} bus {bus_name}")
        
        # Store assignment for later use
        self.bus_assignments[bus_name] = assignment
        
        # Update component configuration with pin assignments
        if "pins" not in comp_data:
            comp_data["pins"] = {}
        
        # Map function names to pin configuration keys
        func_to_key = {
            "SPI_SCLK": "clock",
            "SPI_Q": "data_input", 
            "SPI_HD": "data_output",
            "SPI_CS": "chip_select",
            "I2S_BCLK": "bclk",
            "I2S_WS": "ws",
            "I2S_DATA": "data"
        }
        
        for func, pins in assignment["shared"].items():
            key = func_to_key.get(func, func.lower())
            comp_data["pins"][key] = pins[0] if len(pins) == 1 else pins
        
        # For unique pins, store them in unique_device_pins
        if assignment["unique"]:
            if "unique_device_pins" not in comp_data:
                comp_data["unique_device_pins"] = {}
            for func, pins in assignment["unique"].items():
                key = func_to_key.get(func, func.lower())
                comp_data["unique_device_pins"][f"{key}_available"] = pins
    
    def _assign_pin_to_pwm_channel(self, pwm_comp: Dict[str, Any]) -> None:
        """Assign a pin to a PWM channel"""
        comp_data = pwm_comp.get("config", {})
        pwm_name = pwm_comp.get("name", "unknown")
        
        # PWM channels typically need one GPIO pin
        assigned_pins = self._find_available_pins("GPIO", 1, allow_shared=True)
        if assigned_pins:
            comp_data["pin"] = assigned_pins[0]
            print(f"DEBUG: Assigned GPIO pin {assigned_pins[0]} to PWM channel {pwm_name}")
        else:
            print(f"Warning: Could not assign GPIO pin for PWM channel {pwm_name}")
    
    def _find_available_pins(self, function: str, count: int, allow_shared: bool = True) -> List[int]:
        """Find available pins that support the requested function"""
        if not self.controller_chip:
            return []
        
        available_pins = []
        chip_pins = self.controller_chip.get("exposed_pins", [])
        
        for pin in chip_pins:
            pin_name = pin.get("name", "")
            pin_num = int(pin_name.replace("GPIO", "")) if pin_name.startswith("GPIO") else None
            functions = pin.get("functions", [])
            can_be_shared = pin.get("can_be_shared", True)
            
            if pin_num is not None and function in functions:
                if pin_num not in self.used_pins:
                    if not allow_shared and not can_be_shared:
                        continue
                    available_pins.append(pin_num)
                    if len(available_pins) >= count:
                        break
        
        # Mark pins as used
        for pin_num in available_pins[:count]:
            self.used_pins.add(pin_num)
        
        return available_pins[:count]
    
    def generate_component_stub(self, comp_type: str) -> str:
        """Generate component implementation stub file"""
        if comp_type == "system":
            components = [c for c in self.components if c['type'] == 'system']
            filename = "system_components.cpp"
        else:
            components = [c for c in self.components if c['type'] == 'positioned']
            filename = "bot_components.cpp"
        
        content = [
            '#include "p32_component_tables.h"',
            '#include "esp_log.h"',
            "",
            f"// {comp_type.title()} component implementations",
            f"// Generated from JSON bot configuration",
            ""
        ]
        
        # Generate TAG definitions
        tags = set()
        for comp in components:
            tag = comp['name'].upper()
            if tag not in tags:
                content.append(f'static const char *TAG_{tag} = "{tag}";')
                tags.add(tag)
        
        content.append("")
        
        # Generate function implementations
        for comp in components:
            tag = comp['name'].upper()
            
            # Init function - Allman style braces
            content.extend([
                f"esp_err_t {comp['init_func']}(void)",
                "{",
                "#ifdef SIMPLE_TEST",
                f'    printf("INIT: {comp["name"]} - {comp["description"]}\\n");',
                "    return ESP_OK;",
                "#endif",
                f'    ESP_LOGI(TAG_{tag}, "{comp["description"]} initialized");',
                "    return ESP_OK;",
                "}",
                ""
            ])
            
            # Act function - NO ARGUMENTS - accesses g_loopCount from globals - Allman style
            content.extend([
                f"void {comp['act_func']}(void)",
                "{",
                f'    // Component: {comp["name"]} - {comp["description"]}',
                f'    // Timing: Execute every {comp["hitCount"]} loops',
                "#ifdef SIMPLE_TEST",
                f'    printf("ACT: {comp["name"]} - hitCount:{comp["hitCount"]}\\n");',
                "    return;",
                "#endif",
                f'    ESP_LOGI(TAG_{tag}, "{comp["description"]}");',
                "    // TODO: Implement actual component logic",
                "}",
                ""
            ])
        
        return "\n".join(content)
    
    def generate_cmake_files(self) -> None:
        """Generate CMakeLists.txt with component dispatch table only"""
        # Create CMake content for component sources
        cmake_content = [
            "# P32 Component Dispatch Tables - Auto-generated CMake",
            "# Includes component dispatch table for build system",
            "",
            "set(P32_COMPONENT_SOURCES",
            "    component_tables.cpp",
            ")"
        ]
        
        # Write CMake file
        cmake_file = self.output_dir / "p32_components.cmake"
        with open(cmake_file, 'w') as f:
            f.write('\n'.join(cmake_content))
        print(f"Generated: {cmake_file}")
    
    def write_files(self) -> None:
        """Write all generated files to appropriate directories"""
        self.output_dir.mkdir(exist_ok=True)
        self.include_dir.mkdir(exist_ok=True)

        # Use correct filename per existing architecture
        header_file = ("p32_component_registry.hpp", self.generate_unified_header())

        # Source files - only generate the dispatch table, not individual components
        source_files = [
            ("component_tables.cpp", self.generate_unified_implementation())
        ]

        # Write unified header to include/
        filepath = self.include_dir / header_file[0]
        with open(filepath, 'w') as f:
            f.write(header_file[1])
        print(f"Generated: {filepath}")

        # Write source files to src/
        for filename, content in source_files:
            filepath = self.output_dir / filename
            with open(filepath, 'w') as f:
                f.write(content)
            print(f"Generated: {filepath}")

    def generate_from_bot(self, bot_name: str, environment: str = None) -> None:
        """Main generation process using depth-first JSON traversal algorithm"""
        print(f"Generating P32 component tables for bot: {bot_name}")
        if environment:
            print(f"Filtering for environment: {environment}")

        # Load bot configuration
        bot_config = self.load_bot_config(bot_name)
        print(f"Loaded bot config: {bot_config.get('bot_name', 'Unknown')}")

        # Initialize component tracking
        self.components = []
        self.seen_components = set()
        self.component_configs = {}
        self.current_subsystem = "unknown"
        self.current_component = None
        self.is_bot_level = True  # Start with bot level processing
        
        # Traverse JSON structure using depth-first algorithm
        self.traverse_json_for_components(bot_config)
        print(f"Found {len(self.components)} total dispatch table entries after traversal")

        # Filter components based on environment if specified
        if environment:
            components = self.filter_components_for_environment(self.components, environment)
            print(f"Filtered to {len(components)} components for {environment} environment")
            self.components = components

        # Load controller chip and validate pin requirements
        controller_name = bot_config.get("controller", "esp32_s3_devkit")
        self.controller_chip = self.load_controller_chip_spec(controller_name)
        if self.controller_chip:
            pin_validation_passed = self.assign_pins_to_buses()
            if not pin_validation_passed:
                print("ERROR: Pin validation failed - cannot generate tables")
                return
        else:
            print(f"Warning: Could not load chip spec for {controller_name}, skipping pin validation")

        # Write generated files
        self.write_files()
        self.generate_cmake_files()
        print("Generation complete!")

    def filter_components_for_environment(self, components: List[Dict[str, Any]], environment: str) -> List[Dict[str, Any]]:
        """Filter components based on the build environment"""
        if environment == "goblin_head":
            # Define which components are allowed in goblin_head environment
            allowed_components = {
                "heartbeat", "network_monitor", "goblin_head", "spi_bus_vspi", 
                "goblin_left_eye", "goblin_eye", "gc9a01", "generic_spi_display",
                "goblin_right_eye", "goblin_nose", "hc_sr04_ultrasonic_distance_sensor",
                "goblin_mouth", "goblin_speaker", "speaker", "goblin_left_ear",
                "servo_sg90_micro", "goblin_right_ear"
            }
            
            filtered = []
            for comp in components:
                comp_name = comp.get("component_name", "") or comp.get("name", "")
                if comp_name in allowed_components:
                    filtered.append(comp)
                else:
                    print(f"Filtering out component '{comp_name}' (not in {environment} environment)")
            
            return filtered
        else:
            # For other environments, include all components
            return components

def main():
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        print("Usage: python generate_tables.py <bot_name> <output_dir> [environment]")
        print("Example: python generate_tables.py goblin_full src goblin_head")
        sys.exit(1)

    bot_name = sys.argv[1]
    output_dir = sys.argv[2]
    environment = sys.argv[3] if len(sys.argv) > 3 else None
    config_dir = "config"  # Relative to current directory

    try:
        generator = P32ComponentGenerator(config_dir, output_dir)
        generator.generate_from_bot(bot_name, environment)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()