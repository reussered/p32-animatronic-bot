#!/usr/bin/env python3
"""
P32 Auto-Build Script
Automatically selects PlatformIO environments based on controller specifications in subsystem JSON files.
"""

import json
import os
import sys
import subprocess
from pathlib import Path

# Controller to PlatformIO environment mapping
CONTROLLER_MAPPING = {
    "ESP32_S3_DEVKITC_1": "goblin_head",      # ESP32-S3-DevKitC-1
    "ESP32_S3_DEVKIT": "goblin_head",         # ESP32-S3-DevKitC-1
    "ESP32_S3_R8N16": "goblin_head",          # ESP32-S3 with PSRAM
    "ESP32_C3_MINI": "left_arm",              # ESP32-C3-DevKitM-1
    "ESP32_C3_DEVKITM_1": "left_arm",         # ESP32-C3-DevKitM-1
    "ESP32": "left_arm_advanced",             # Standard ESP32
    "ESP32-S3": "goblin_head",                # Generic ESP32-S3
}

def load_json_file(filepath):
    """Load and return JSON data from file."""
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"Error loading {filepath}: {e}")
        return None

def find_subsystems_in_bot(bot_config):
    """Extract subsystem file paths from bot configuration."""
    subsystems = []

    # Check for direct subsystem references
    if "components" in bot_config:
        for component in bot_config["components"]:
            if component.startswith("config/subsystems/"):
                subsystems.append(component)

    # Check for subsystem assemblies
    if "subsystem_assemblies" in bot_config:
        for assembly in bot_config["subsystem_assemblies"]:
            if "subsystem" in assembly:
                subsystems.append(assembly["subsystem"])

    return subsystems

def get_controller_from_subsystem(subsystem_path):
    """Extract controller type from subsystem JSON."""
    full_path = Path(__file__).parent / subsystem_path
    subsystem_data = load_json_file(full_path)

    if subsystem_data and "controller" in subsystem_data:
        controller = subsystem_data["controller"]
        # Handle both string and object controller specifications
        if isinstance(controller, str):
            return controller
        elif isinstance(controller, dict) and "type" in controller:
            return controller["type"]

    return None

def get_platformio_env_for_controller(controller):
    """Map controller type to PlatformIO environment."""
    return CONTROLLER_MAPPING.get(controller)

def build_subsystem(subsystem_path, pio_env):
    """Build a subsystem using the specified PlatformIO environment."""
    print(f"Building subsystem: {subsystem_path}")
    print(f"Using PlatformIO environment: {pio_env}")

    # Generate tables for the subsystem
    cmd_generate = ["python", "tools/generate_tables.py", subsystem_path.replace("config/subsystems/", "").replace(".json", ""), "src"]
    print(f"Running: {' '.join(cmd_generate)}")

    result = subprocess.run(cmd_generate, cwd=Path(__file__).parent)
    if result.returncode != 0:
        print(f"Failed to generate tables for {subsystem_path}")
        return False

    # Build with PlatformIO
    cmd_build = ["pio", "run", "-c", "platformio_multi_variant.ini", "-e", pio_env]
    print(f"Running: {' '.join(cmd_build)}")

    result = subprocess.run(cmd_build, cwd=Path(__file__).parent)
    if result.returncode != 0:
        print(f"Failed to build {subsystem_path} with environment {pio_env}")
        return False

    return True

def main():
    if len(sys.argv) != 2:
        print("Usage: python auto_build.py <bot_config>")
        print("Example: python auto_build.py test_bot")
        sys.exit(1)

    bot_name = sys.argv[1]
    bot_config_path = f"config/bots/bot_families/tests/{bot_name}.json"

    # Load bot configuration
    bot_config = load_json_file(bot_config_path)
    if not bot_config:
        print(f"Could not load bot configuration: {bot_config_path}")
        sys.exit(1)

    print(f"Building bot: {bot_name}")
    print(f"Configuration: {bot_config_path}")

    # Find subsystems in bot config
    subsystems = find_subsystems_in_bot(bot_config)
    print(f"Found subsystems: {subsystems}")

    if not subsystems:
        print("No subsystems found in bot configuration")
        sys.exit(1)

    # Build each subsystem
    success_count = 0
    for subsystem_path in subsystems:
        controller = get_controller_from_subsystem(subsystem_path)
        if not controller:
            print(f"Could not determine controller for {subsystem_path}")
            continue

        pio_env = get_platformio_env_for_controller(controller)
        if not pio_env:
            print(f"No PlatformIO environment mapping for controller: {controller}")
            continue

        print(f"\n=== Building {subsystem_path} ===")
        print(f"Controller: {controller} -> Environment: {pio_env}")

        if build_subsystem(subsystem_path, pio_env):
            success_count += 1
            print(f" Successfully built {subsystem_path}")
        else:
            print(f" Failed to build {subsystem_path}")

    print(f"\nBuild Summary: {success_count}/{len(subsystems)} subsystems built successfully")

    if success_count == len(subsystems):
        print(" All subsystems built successfully!")
        return 0
    else:
        print("  Some subsystems failed to build")
        return 1

if __name__ == "__main__":
    sys.exit(main())