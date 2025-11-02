#!/usr/bin/env python3
"""
P32 Component File Auditor and Generator
Ensures every component has proper .hpp and .cpp files with correct function signatures
CRITICAL: Preserves all existing code - no code loss
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple

class P32ComponentFileAuditor:
    def __init__(self, config_dir: str, src_dir: str, include_dir: str):
        self.config_dir = Path(config_dir)
        self.src_dir = Path(src_dir)
        self.include_dir = Path(include_dir)
        self.components_src = self.src_dir / "components"
        self.components_include = self.include_dir / "components"
        
        # Required components from generated tables
        self.required_components = []
        
        # Existing files
        self.existing_hpp_files = set()
        self.existing_cpp_files = set()
        
        # Analysis results
        self.missing_hpp = []
        self.missing_cpp = []
        self.mismatched_names = []
        self.existing_implementations = {}
        
    def scan_existing_files(self):
        """Scan existing .hpp and .cpp files"""
        print("Scanning existing component files...")
        
        # Scan include/components/
        if self.components_include.exists():
            for hpp_file in self.components_include.glob("*.hpp"):
                component_name = hpp_file.stem
                self.existing_hpp_files.add(component_name)
                print(f"  Found header: {component_name}.hpp")
        
        # Scan src/components/
        if self.components_src.exists():
            for cpp_file in self.components_src.glob("*.cpp"):
                component_name = cpp_file.stem
                self.existing_cpp_files.add(component_name)
                print(f"  Found implementation: {component_name}.cpp")
                
            # Also scan .c files (should be converted to .cpp)
            for c_file in self.components_src.glob("*.c"):
                component_name = c_file.stem
                print(f"  Found C file (needs conversion): {component_name}.c")
    
    def load_required_components_from_table(self):
        """Load required components from generated component tables"""
        table_header = self.include_dir / "p32_component_tables.h"
        
        if not table_header.exists():
            print("ERROR: p32_component_tables.h not found. Run generate_tables.py first.")
            return
        
        print("Extracting component names from p32_component_tables.h...")
        
        with open(table_header, 'r') as f:
            content = f.read()
        
        # Extract function names and derive component names
        import re
        
        # Find all init function declarations
        init_pattern = r'esp_err_t\s+(\w+)_init\(void\);'
        init_matches = re.findall(init_pattern, content)
        
        # Find all act function declarations  
        act_pattern = r'void\s+(\w+)_act\(void\);'
        act_matches = re.findall(act_pattern, content)
        
        # Extract component names (remove p32_comp_ prefix if present)
        component_names = set()
        
        for init_func in init_matches:
            if init_func.startswith('p32_comp_'):
                comp_name = init_func[9:]  # Remove 'p32_comp_' prefix
            else:
                comp_name = init_func
            component_names.add(comp_name)
            
        for act_func in act_matches:
            if act_func.startswith('p32_comp_'):
                comp_name = act_func[9:]  # Remove 'p32_comp_' prefix  
            else:
                comp_name = act_func
            component_names.add(comp_name)
        
        self.required_components = sorted(list(component_names))
        print(f"Found {len(self.required_components)} required components:")
        for comp in self.required_components:
            print(f"  - {comp}")
    
    def analyze_gaps(self):
        """Analyze what files are missing"""
        print("\\nAnalyzing missing files...")
        
        for comp_name in self.required_components:
            # Check for missing .hpp files
            if comp_name not in self.existing_hpp_files:
                self.missing_hpp.append(comp_name)
                print(f"  MISSING HEADER: {comp_name}.hpp")
            
            # Check for missing .cpp files
            if comp_name not in self.existing_cpp_files:
                self.missing_cpp.append(comp_name)
                print(f"  MISSING IMPLEMENTATION: {comp_name}.cpp")
        
        print(f"\\nSummary:")
        print(f"  Required components: {len(self.required_components)}")
        print(f"  Missing headers: {len(self.missing_hpp)}")
        print(f"  Missing implementations: {len(self.missing_cpp)}")
    
    def check_existing_implementations(self):
        """Check existing implementations for function signatures"""
        print("\\nChecking existing implementations...")
        
        for comp_name in self.required_components:
            if comp_name in self.existing_cpp_files:
                cpp_file = self.components_src / f"{comp_name}.cpp"
                self.analyze_cpp_file(comp_name, cpp_file)
            else:
                # Check if there's a .c file
                c_file = self.components_src / f"{comp_name}.c" 
                if c_file.exists():
                    self.analyze_cpp_file(comp_name, c_file)
    
    def analyze_cpp_file(self, comp_name: str, file_path: Path):
        """Analyze a specific .cpp/.c file for function implementations"""
        try:
            with open(file_path, 'r') as f:
                content = f.read()
            
            # Look for init and act functions
            import re
            
            init_patterns = [
                rf'{comp_name}_init\s*\(',
                rf'p32_comp_{comp_name}_init\s*\(',
            ]
            
            act_patterns = [
                rf'{comp_name}_act\s*\(',
                rf'p32_comp_{comp_name}_act\s*\(',
            ]
            
            has_init = any(re.search(pattern, content) for pattern in init_patterns)
            has_act = any(re.search(pattern, content) for pattern in act_patterns)
            
            self.existing_implementations[comp_name] = {
                'file': str(file_path),
                'has_init': has_init,
                'has_act': has_act,
                'size': len(content)
            }
            
            status = []
            if has_init:
                status.append("? init")
            else:
                status.append("? init")
                
            if has_act:
                status.append("? act")
            else:
                status.append("? act")
            
            print(f"  {comp_name}: {' '.join(status)} ({len(content)} chars)")
            
        except Exception as e:
            print(f"  ERROR reading {file_path}: {e}")
    
    def generate_missing_files(self, preview_only=True):
        """Generate missing .hpp and .cpp files"""
        
        if preview_only:
            print("\\n=== PREVIEW MODE - No files will be created ===")
        else:
            print("\\n=== GENERATING MISSING FILES ===")
        
        # Create directories if they don't exist
        if not preview_only:
            self.components_include.mkdir(parents=True, exist_ok=True)
            self.components_src.mkdir(parents=True, exist_ok=True)
        
        # Generate missing headers
        for comp_name in self.missing_hpp:
            self.generate_header_file(comp_name, preview_only)
        
        # Generate missing implementations
        for comp_name in self.missing_cpp:
            self.generate_implementation_file(comp_name, preview_only)
    
    def generate_header_file(self, comp_name: str, preview_only: bool):
        """Generate .hpp file for component"""
        guard_name = f"P32_{comp_name.upper()}_HPP"
        
        content = f"""#ifndef {guard_name}
#define {guard_name}

#include "esp_err.h"

// Component: {comp_name}
// Auto-generated header file

// Component initialization function - NO ARGUMENTS
esp_err_t {comp_name}_init(void);

// Component action function - NO ARGUMENTS  
void {comp_name}_act(void);

#endif // {guard_name}
"""
        
        header_path = self.components_include / f"{comp_name}.hpp"
        
        if preview_only:
            print(f"  WOULD CREATE: {header_path}")
        else:
            with open(header_path, 'w', encoding='ascii') as f:
                f.write(content)
            print(f"  CREATED: {header_path}")
    
    def generate_implementation_file(self, comp_name: str, preview_only: bool):
        """Generate .cpp file for component"""
        
        content = f"""// P32 Component: {comp_name}
// Auto-generated implementation file
// PLACEHOLDER IMPLEMENTATION - prints function names to serial

#include "components/{comp_name}.hpp"
#include "esp_log.h"
#include "p32_shared_state.h"

static const char *TAG = "{comp_name.upper()}";

// Component initialization function - NO ARGUMENTS
esp_err_t {comp_name}_init(void)
{{
    printf("INIT: {comp_name}_init() called\\n");
    ESP_LOGI(TAG, "Component {comp_name} initialized (placeholder)");
    return ESP_OK;
}}

// Component action function - NO ARGUMENTS
void {comp_name}_act(void)
{{
    // Print function name periodically to avoid spam
    if (g_loopCount % 10000 == 0)
    {{
        printf("ACT: {comp_name}_act() called (loop %llu)\\n", g_loopCount);
        ESP_LOGD(TAG, "Component {comp_name} action (placeholder)");
    }}
}}
"""
        
        impl_path = self.components_src / f"{comp_name}.cpp"
        
        if preview_only:
            print(f"  WOULD CREATE: {impl_path}")
        else:
            with open(impl_path, 'w', encoding='ascii') as f:
                f.write(content)
            print(f"  CREATED: {impl_path}")
    
    def run_audit(self, generate_files=False):
        """Run complete audit process"""
        print("P32 Component File Auditor")
        print("=" * 50)
        
        self.scan_existing_files()
        self.load_required_components_from_table()
        self.analyze_gaps()
        self.check_existing_implementations()
        
        if generate_files:
            self.generate_missing_files(preview_only=False)
        else:
            self.generate_missing_files(preview_only=True)
            print("\\n*** Run with --generate to create missing files ***")

def main():
    if len(sys.argv) < 2:
        print("Usage: python audit_components.py [--generate]")
        print("  --generate: Actually create missing files (default is preview)")
        sys.exit(1)
    
    generate_files = "--generate" in sys.argv
    
    auditor = P32ComponentFileAuditor("config", "src", "include")
    auditor.run_audit(generate_files)

if __name__ == "__main__":
    main()