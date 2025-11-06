#!/usr/bin/env python3
"""
Deep Subsystem Component Chain Validator
Analyzes each subsystem to verify complete component chains from positioned  hardware  interface  driver
"""

import json
from pathlib import Path
from collections import defaultdict

PROJECT_ROOT = Path("f:/GitHub/p32-animatronic-bot")

class ComponentChainValidator:
    def __init__(self):
        self.all_components = {}  # Cache all loaded components
        self.issues = defaultdict(list)
        
    def load_component(self, path):
        """Load a component JSON file"""
        if path in self.all_components:
            return self.all_components[path]
        
        full_path = PROJECT_ROOT / path
        if not full_path.exists():
            return None
        
        try:
            data = json.loads(full_path.read_text(encoding='utf-8'))
            self.all_components[path] = data
            return data
        except:
            return None
    
    def get_component_type(self, comp_data):
        """Determine component type from metadata"""
        if not comp_data:
            return "UNKNOWN"
        
        comp_type = comp_data.get('component_type', '')
        hardware_type = comp_data.get('hardware_type', '')
        subsystem = comp_data.get('subsystem', '')
        
        if 'POSITIONED' in comp_type:
            return 'POSITIONED'
        elif 'HARDWARE' in comp_type or 'SENSOR' in hardware_type or 'DISPLAY' in hardware_type:
            return 'HARDWARE'
        elif 'INTERFACE' in comp_type or 'BUS_INTERFACE' in hardware_type:
            return 'INTERFACE'
        elif 'DRIVER' in comp_type or 'BUS_DRIVER' in hardware_type:
            return 'DRIVER'
        elif subsystem in ['ACTUATOR', 'SENSOR', 'AUDIO', 'VISION', 'INDICATOR']:
            return 'POSITIONED'
        
        return 'OTHER'
    
    def trace_chain(self, component_path, depth=0, visited=None):
        """Recursively trace component chain"""
        if visited is None:
            visited = set()
        
        if component_path in visited:
            return {'path': component_path, 'error': 'CIRCULAR_REFERENCE', 'depth': depth}
        
        visited.add(component_path)
        
        comp_data = self.load_component(component_path)
        if not comp_data:
            return {'path': component_path, 'error': 'FILE_NOT_FOUND', 'depth': depth}
        
        comp_type = self.get_component_type(comp_data)
        name = comp_data.get('name', 'unknown')
        
        result = {
            'path': component_path,
            'name': name,
            'type': comp_type,
            'depth': depth,
            'children': []
        }
        
        # Get components array
        components = comp_data.get('components', [])
        
        # Also check in software.components for hardware components
        if 'software' in comp_data and 'components' in comp_data['software']:
            components.extend(comp_data['software']['components'])
        
        if not components and comp_type in ['HARDWARE', 'INTERFACE']:
            result['warning'] = 'NO_CHILDREN'
        
        for child_path in components:
            child_result = self.trace_chain(child_path, depth + 1, visited.copy())
            result['children'].append(child_result)
        
        return result
    
    def validate_chain_completeness(self, chain, component_path):
        """Check if chain terminates properly"""
        issues = []
        
        # Expected chain: POSITIONED  HARDWARE  INTERFACE  DRIVER
        comp_type = chain['type']
        
        if comp_type == 'POSITIONED':
            if not chain['children']:
                issues.append(f"Positioned component has no hardware reference")
            else:
                for child in chain['children']:
                    if child['type'] not in ['HARDWARE', 'INTERFACE']:
                        issues.append(f"Positioned component references {child['type']} instead of HARDWARE")
        
        elif comp_type == 'HARDWARE':
            if not chain['children']:
                issues.append(f"Hardware component has no interface/driver reference")
            else:
                for child in chain['children']:
                    if child['type'] not in ['INTERFACE', 'DRIVER']:
                        issues.append(f"Hardware component references {child['type']} instead of INTERFACE/DRIVER")
        
        elif comp_type == 'INTERFACE':
            if not chain['children']:
                issues.append(f"Interface component has no driver reference")
            else:
                for child in chain['children']:
                    if child['type'] not in ['DRIVER']:
                        issues.append(f"Interface component references {child['type']} instead of DRIVER")
        
        # Check for errors in chain
        if 'error' in chain:
            issues.append(f"Chain error: {chain['error']}")
        
        if 'warning' in chain:
            issues.append(f"Chain warning: {chain['warning']}")
        
        # Recursively check children
        for child in chain.get('children', []):
            child_issues = self.validate_chain_completeness(child, child['path'])
            issues.extend(child_issues)
        
        return issues
    
    def analyze_subsystem(self, subsystem_path):
        """Analyze a complete subsystem"""
        subsystem_data = self.load_component(subsystem_path)
        if not subsystem_data:
            return None
        
        subsystem_name = subsystem_data.get('name', subsystem_path)
        components = subsystem_data.get('components', [])
        
        print(f"\n{'='*70}")
        print(f"SUBSYSTEM: {subsystem_name}")
        print(f"{'='*70}")
        print(f"Components: {len(components)}")
        
        all_issues = []
        
        for comp_path in components:
            chain = self.trace_chain(comp_path)
            issues = self.validate_chain_completeness(chain, comp_path)
            
            if issues:
                all_issues.append({
                    'component': comp_path,
                    'issues': issues
                })
        
        return {
            'subsystem': subsystem_name,
            'path': subsystem_path,
            'component_count': len(components),
            'issues': all_issues
        }
    
    def print_chain(self, chain, indent=0):
        """Pretty print a component chain"""
        prefix = "  " * indent
        name = chain.get('name', 'unknown')
        comp_type = chain.get('type', 'UNKNOWN')
        path = chain.get('path', '')
        
        status = ""
        if 'error' in chain:
            status = f"  {chain['error']}"
        elif 'warning' in chain:
            status = f"   {chain['warning']}"
        
        print(f"{prefix} [{comp_type}] {name}{status}")
        
        for child in chain.get('children', []):
            self.print_chain(child, indent + 1)

def main():
    validator = ComponentChainValidator()
    
    # Find all subsystems
    subsystems_dir = PROJECT_ROOT / "config" / "subsystems"
    subsystem_files = list(subsystems_dir.glob("*.json"))
    
    print(f"\n{'='*70}")
    print("DEEP SUBSYSTEM COMPONENT CHAIN VALIDATION")
    print(f"{'='*70}")
    print(f"\nFound {len(subsystem_files)} subsystems to analyze\n")
    
    all_subsystem_results = []
    
    for subsystem_file in subsystem_files:
        rel_path = str(subsystem_file.relative_to(PROJECT_ROOT)).replace('\\', '/')
        result = validator.analyze_subsystem(rel_path)
        
        if result:
            all_subsystem_results.append(result)
            
            if result['issues']:
                print(f"\n  Found {len(result['issues'])} components with issues:")
                for issue_data in result['issues']:
                    print(f"\n  Component: {issue_data['component']}")
                    for issue in issue_data['issues']:
                        print(f"    - {issue}")
            else:
                print(f"\n All chains complete!")
    
    # Summary
    print(f"\n{'='*70}")
    print("SUMMARY")
    print(f"{'='*70}\n")
    
    total_issues = sum(len(r['issues']) for r in all_subsystem_results)
    subsystems_with_issues = sum(1 for r in all_subsystem_results if r['issues'])
    
    print(f"Subsystems analyzed: {len(all_subsystem_results)}")
    print(f"Subsystems with issues: {subsystems_with_issues}")
    print(f"Total components with issues: {total_issues}")
    
    if total_issues == 0:
        print(f"\n All subsystem component chains are complete!")
    else:
        print(f"\n  Component chains need attention")
    
    print(f"\n{'='*70}\n")

if __name__ == "__main__":
    main()
