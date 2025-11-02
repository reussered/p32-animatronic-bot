#!/usr/bin/env python3
"""
Trace the complete component chain for goblin eyes to identify what's broken
"""

import json
from pathlib import Path

PROJECT_ROOT = Path("f:/GitHub/p32-animatronic-bot")

def load_component(path):
    full_path = PROJECT_ROOT / path
    if not full_path.exists():
        print(f"  ❌ FILE NOT FOUND: {path}")
        return None
    try:
        return json.loads(full_path.read_text(encoding='utf-8'))
    except Exception as e:
        print(f"  ❌ PARSE ERROR: {path} - {e}")
        return None

def trace_chain(path, indent=0):
    prefix = "  " * indent
    comp = load_component(path)
    
    if not comp:
        return
    
    name = comp.get('name', 'unknown')
    comp_type = comp.get('component_type', '')
    hw_type = comp.get('hardware_type', '')
    
    print(f"{prefix}├─ {name}")
    print(f"{prefix}│  Type: {comp_type}")
    print(f"{prefix}│  HW Type: {hw_type}")
    print(f"{prefix}│  Path: {path}")
    
    # Get components array
    components = comp.get('components', [])
    
    # Check software.components too
    if 'software' in comp and 'components' in comp['software']:
        sw_comps = comp['software']['components']
        if sw_comps:
            print(f"{prefix}│  Software components: {sw_comps}")
            components.extend(sw_comps)
    
    if not components:
        print(f"{prefix}│  ⚠️  NO CHILDREN - CHAIN ENDS HERE")
    else:
        print(f"{prefix}│  Children: {len(components)}")
    
    print(f"{prefix}│")
    
    for child_path in components:
        trace_chain(child_path, indent + 1)

def main():
    print("\n" + "="*70)
    print("GOBLIN EYE COMPONENT CHAIN ANALYSIS")
    print("="*70 + "\n")
    
    # Trace left eye
    print("LEFT EYE CHAIN:")
    print("-" * 70)
    trace_chain("config/components/positioned/goblin_left_eye.json")
    
    print("\n" + "="*70)
    print("\nRIGHT EYE CHAIN:")
    print("-" * 70)
    trace_chain("config/components/positioned/goblin_right_eye.json")
    
    print("\n" + "="*70)
    print("\nKEY QUESTIONS TO ANSWER:")
    print("="*70)
    print("1. Does goblin_left_eye → reference a display hardware component?")
    print("2. Does the display hardware (GC9A01) → reference an SPI interface?")
    print("3. Does the SPI interface → reference an SPI driver?")
    print("4. Are the .src/.hdr files present for each component?")
    print("5. Can we compile and run this chain on ESP32-S3?")
    print("\n" + "="*70 + "\n")

if __name__ == "__main__":
    main()
