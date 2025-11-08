#!/usr/bin/env python3
"""
callByTemplate.py - Generate template dispatch function from structs in a file

Usage:
    python callByTemplate.py <input_file> <function_name> <variable_name>

Example:
    python callByTemplate.py config/shared_headers/color_schema.hpp adjustMood color_schema

This script:
1. Reads the input file
2. Finds all struct definitions
3. Generates a template function: func_name_t<T>()
4. Generates a dispatcher function: func_name(std::string var)
   that calls the template with the correct struct based on var
"""

import sys
import re
from pathlib import Path


def find_structs(file_content):
    """
    Find all struct definitions in the file.
    Returns list of struct names.
    
    Patterns matched:
    - struct Name { ... };
    - struct Name;
    """
    struct_pattern = r'\bstruct\s+(\w+)\s*[{;]'
    matches = re.findall(struct_pattern, file_content)
    return list(dict.fromkeys(matches))  # Remove duplicates, preserve order


def generate_template_function(function_name):
    """Generate the template function stub."""
    template_func_name = f"{function_name}_t"
    code = f"""
template<class T>
void {template_func_name}()
{{
    // TODO: Implement template specialization logic for type T
    // This will be called with T = Pixel_RGB565, Pixel_RGB888, etc.
}}
"""
    return code.strip()


def generate_dispatcher_function(function_name, variable_name, struct_names):
    """Generate the dispatcher function that calls the template."""
    template_func_name = f"{function_name}_t"
    
    code = f"void {function_name}(const std::string& {variable_name})\n{{\n"
    
    # Generate if/else chain
    for i, struct_name in enumerate(struct_names):
        if i == 0:
            code += f'    if ({variable_name} == "{struct_name}")\n'
        else:
            code += f'    else if ({variable_name} == "{struct_name}")\n'
        code += f'    {{\n'
        code += f'        {template_func_name}<{struct_name}>();\n'
        code += f'    }}\n'
    
    # Error case
    code += f'    else\n'
    code += f'    {{\n'
    code += f'        // Unknown struct name\n'
    code += f'        throw std::runtime_error("Unknown {variable_name}: " + {variable_name});\n'
    code += f'    }}\n'
    code += f'}}\n'
    
    return code


def main():
    if len(sys.argv) != 4:
        print("Usage: python callByTemplate.py <input_file> <function_name> <variable_name>")
        print("Example: python callByTemplate.py config/shared_headers/color_schema.hpp adjustMood color_schema")
        sys.exit(1)
    
    input_file = sys.argv[1]
    function_name = sys.argv[2]
    variable_name = sys.argv[3]
    
    # Read the input file
    try:
        with open(input_file, 'r', encoding='ascii') as f:
            file_content = f.read()
    except FileNotFoundError:
        print(f"ERROR: File not found: {input_file}")
        sys.exit(1)
    except UnicodeDecodeError:
        print(f"ERROR: File is not ASCII encoded: {input_file}")
        sys.exit(1)
    
    # Find all structs
    struct_names = find_structs(file_content)
    
    if not struct_names:
        print(f"WARNING: No structs found in {input_file}")
        struct_names = []
    
    print(f"Found {len(struct_names)} struct(s): {', '.join(struct_names)}")
    print()
    
    # Generate template function
    template_func = generate_template_function(function_name)
    print("=== TEMPLATE FUNCTION ===")
    print(template_func)
    print()
    
    # Generate dispatcher function
    dispatcher_func = generate_dispatcher_function(function_name, variable_name, struct_names)
    print("=== DISPATCHER FUNCTION ===")
    print(dispatcher_func)
    print()
    
    # Combine and show output
    print("=== COMBINED OUTPUT (ready to paste in .src file) ===")
    print(template_func)
    print()
    print(dispatcher_func)
    
    # Optional: write to output file
    output_file = f"{function_name}_generated.cpp"
    with open(output_file, 'w', encoding='ascii') as f:
        f.write(template_func)
        f.write('\n\n')
        f.write(dispatcher_func)
    
    print(f"\nGenerated code also written to: {output_file}")


if __name__ == "__main__":
    main()
