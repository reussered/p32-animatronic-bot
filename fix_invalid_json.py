#!/usr/bin/env python3
"""
Fix all invalid JSON files in the project
"""

import os
import json
from pathlib import Path

def fix_empty_json_files():
    """Fix empty JSON files by giving them minimal valid structure"""
    empty_files = [
        "config/bots/bot_families/animals/bears_family.json",
        "config/bots/bot_families/animals/cats_family.json", 
        "config/bots/bot_families/animals/wolves_family.json",
        "config/bots/bot_families/fantasy/dragons_family.json",
        "config/bots/bot_families/fantasy/elves_family.json",
        "config/bots/bot_families/fantasy/goblins_family.json",
        "config/bots/bot_families/fantasy/orcs_family.json",
        "config/bots/bot_families/horror/ghosts_family.json",
        "config/bots/bot_families/horror/skeletons_family.json",
        "config/bots/bot_families/horror/zombies_family.json",
        "config/bots/bot_families/mechanical/androids_family.json",
        "config/bots/bot_families/mechanical/robots_family.json"
    ]
    
    root = Path("f:/GitHub/p32-animatronic-bot")
    
    for file_path in empty_files:
        full_path = root / file_path
        if full_path.exists():
            # Get family name from filename
            family_name = full_path.stem.replace('_family', '').replace('_', ' ').title()
            category = full_path.parent.name
            
            # Create minimal valid JSON structure
            minimal_structure = {
                "relative_filename": file_path,
                "version": "1.0",
                "author": "config/author.json",
                "family_name": family_name,
                "family_type": family_name.upper().replace(' ', '_'),
                "family_category": category,
                "description": f"{family_name} family - placeholder configuration",
                "created": "2025-10-21",
                "template_type": "family_template",
                "coordinate_system": "skull_3d",
                "reference_point": "nose_center",
                "baseline_dimensions": {
                    "eye_spacing": "3.0 INCH",
                    "skull_height": "8.0 INCH",
                    "skull_depth": "6.0 INCH"
                },
                "default_components": [],
                "family_mood_profile": {
                    "FEAR": 10,
                    "ANGER": 15,
                    "IRRITATION": 20,
                    "HAPPINESS": 25,
                    "CONTENTMENT": 15,
                    "HUNGER": 10,
                    "CURIOSITY": 25,
                    "AFFECTION": 20
                },
                "family_behavior_patterns": {
                    "threat_response_enabled": True,
                    "auto_sleep_timeout_seconds": 1800,
                    "interaction_range_cm": 150,
                    "aggressive_threshold": 70,
                    "calm_threshold": 30,
                    "personality_traits": [
                        "curious",
                        "friendly", 
                        "responsive"
                    ]
                }
            }
            
            with open(full_path, 'w', encoding='utf-8') as f:
                json.dump(minimal_structure, f, indent=2, ensure_ascii=False)
            print(f" Fixed empty file: {file_path}")

def fix_robot_family_json():
    """Fix the malformed robot_family.json"""
    file_path = Path("f:/GitHub/p32-animatronic-bot/config/bots/bot_families/mechanical/robot_family.json")
    
    if file_path.exists():
        # Create corrected structure
        corrected_structure = {
            "relative_filename": "config/bots/bot_families/mechanical/robot_family.json",
            "version": "1.0",
            "author": "config/author.json",
            "family_name": "Robot",
            "family_type": "ROBOT",
            "family_category": "mechanical", 
            "description": "Advanced robots with LED displays, servo mechanisms, and AI-inspired behaviors",
            "created": "2025-10-12",
            "template_type": "family_template",
            "coordinate_system": "planar_2d",
            "reference_point": "nose_center",
            "baseline_dimensions": {
                "face_width": "6.0 INCH",
                "face_height": "8.0 INCH",
                "component_spacing": "1.5 INCH"
            },
            "default_components": [
                "config/components/positioned/robot_chest_matrix.json",
                "config/components/positioned/robot_status_oled.json",
                "config/components/positioned/robot_camera_eyes.json",
                "config/components/positioned/robot_speaker_array.json",
                "config/components/positioned/robot_led_strips.json"
            ],
            "family_mood_profile": {
                "FEAR": 5,
                "ANGER": 10,
                "IRRITATION": 15,
                "HAPPINESS": 25,
                "CONTENTMENT": 20,
                "HUNGER": 0,
                "CURIOSITY": 25,
                "AFFECTION": 5
            },
            "family_behavior_patterns": {
                "threat_response_enabled": True,
                "auto_sleep_timeout_seconds": 1800,
                "interaction_range_cm": 200,
                "aggressive_threshold": 50,
                "calm_threshold": 30,
                "personality_traits": [
                    "logical",
                    "methodical",
                    "helpful",
                    "precise", 
                    "analytical"
                ]
            }
        }
        
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(corrected_structure, f, indent=2, ensure_ascii=False)
        print(f" Fixed malformed file: {file_path}")

def fix_robot_family_corrupted():
    """Fix the corrupted robot_family_corrupted.json"""
    file_path = Path("f:/GitHub/p32-animatronic-bot/config/bots/bot_families/mechanical/robot_family_corrupted.json")
    
    if file_path.exists():
        # Replace with minimal valid structure
        corrected_structure = {
            "relative_filename": "config/bots/bot_families/mechanical/robot_family_corrupted.json",
            "version": "1.0",
            "author": "config/author.json",
            "family_name": "Robot Corrupted",
            "family_type": "ROBOT_CORRUPTED",
            "family_category": "mechanical",
            "description": "Corrupted robot family - previously damaged data restored",
            "created": "2025-10-21",
            "template_type": "family_template",
            "coordinate_system": "planar_2d",
            "reference_point": "nose_center",
            "baseline_dimensions": {
                "face_width": "6.0 INCH",
                "face_height": "8.0 INCH",
                "component_spacing": "1.5 INCH"
            },
            "default_components": [],
            "family_mood_profile": {
                "FEAR": 30,
                "ANGER": 25,
                "IRRITATION": 35,
                "HAPPINESS": 5,
                "CONTENTMENT": 5,
                "HUNGER": 0,
                "CURIOSITY": 10,
                "AFFECTION": 0
            },
            "family_behavior_patterns": {
                "threat_response_enabled": True,
                "auto_sleep_timeout_seconds": 900,
                "interaction_range_cm": 100,
                "aggressive_threshold": 80,
                "calm_threshold": 10,
                "personality_traits": [
                    "unpredictable",
                    "glitchy",
                    "aggressive"
                ]
            }
        }
        
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(corrected_structure, f, indent=2, ensure_ascii=False)
        print(f" Fixed corrupted file: {file_path}")

def fix_vscode_files():
    """Fix VS Code configuration files"""
    vscode_files = [
        ".vscode/c_cpp_properties.json",
        ".vscode/extensions.json", 
        ".vscode/launch.json"
    ]
    
    root = Path("f:/GitHub/p32-animatronic-bot")
    
    for file_path in vscode_files:
        full_path = root / file_path
        if full_path.exists():
            if "c_cpp_properties" in file_path:
                structure = {
                    "configurations": [
                        {
                            "name": "ESP32-S3",
                            "includePath": [
                                "${workspaceFolder}/**",
                                "${workspaceFolder}/include/**"
                            ],
                            "defines": [],
                            "compilerPath": "",
                            "cStandard": "c17",
                            "cppStandard": "c++17",
                            "intelliSenseMode": "gcc-x64"
                        }
                    ],
                    "version": 4
                }
            elif "extensions" in file_path:
                structure = {
                    "recommendations": [
                        "platformio.platformio-ide",
                        "ms-vscode.cpptools",
                        "ms-python.python"
                    ]
                }
            elif "launch" in file_path:
                structure = {
                    "version": "0.2.0",
                    "configurations": []
                }
            
            with open(full_path, 'w', encoding='utf-8') as f:
                json.dump(structure, f, indent=2, ensure_ascii=False)
            print(f" Fixed VS Code file: {file_path}")

if __name__ == "__main__":
    print(" Fixing all invalid JSON files...")
    fix_empty_json_files()
    fix_robot_family_json()
    fix_robot_family_corrupted()
    fix_vscode_files()
    print(" All JSON files fixed!")