?#!/usr/bin/env python3
"""Quick test of inheritance processor"""

import sys
sys.path.append('tools')
from inheritance_processor import P32InheritanceProcessor

print("Testing P32 Inheritance Processor")
print("-" * 40)

# Test loading templates
proc = P32InheritanceProcessor('config')

# Test 1: Load family template
try:
    family_template = proc.load_template('templates/families/goblin_family.json')
    print(" Successfully loaded family template")
    print(f"  Family type: {family_template.get('family_type', 'unknown')}")
except Exception as e:
    print(f" Error loading family template: {e}")

# Test 2: Load base template  
try:
    base_template = proc.load_template('templates/base/humanoid_head_base.json')
    print(" Successfully loaded base template")
    print(f"  Template type: {base_template.get('template_type', 'unknown')}")
except Exception as e:
    print(f" Error loading base template: {e}")

# Test 3: Load creature
try:
    creature = proc.load_template('creatures/goblin_warrior_head.json')
    print(" Successfully loaded creature")
    print(f"  Bot ID: {creature.get('bot_id', 'unknown')}")
except Exception as e:
    print(f" Error loading creature: {e}")

# Test 4: Full inheritance resolution
try:
    print("\nTesting full inheritance resolution...")
    creature = proc.load_template('creatures/goblin_warrior_head.json')
    resolved = proc.resolve_inheritance_chain(creature)
    print(" Successfully resolved inheritance chain!")
    
    print(f"  Final bot ID: {resolved.get('bot_id', 'unknown')}")
    print(f"  Components count: {len(resolved.get('positioned_components', []))}")
    print(f"  Has mood profiles: {bool('base_mood_profile' in resolved)}")
    print(f"  Inheritance chain length: {len(resolved.get('inheritance_chain', []))}")
    
    # Save resolved configuration
    import json
    with open('config/resolved/goblin_warrior_head_resolved.json', 'w') as f:
        json.dump(resolved, f, indent=2)
    print("   Saved resolved configuration to config/resolved/")
    
except Exception as e:
    print(f" Error in inheritance resolution: {e}")

print("\nTest complete!")