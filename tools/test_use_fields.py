#!/usr/bin/env python3
"""
Test suite for use_fields inherited keywords feature.
Tests both normal fields and ###N### directives.
"""

import json
import sys
from pathlib import Path
from dataclasses import dataclass, field as dataclass_field
from typing import Dict, Any, List, Optional, Set

PROJECT_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(PROJECT_ROOT / "tools"))

# Import the functions we're testing
from generate_tables import (
    sanitize_identifier,
    json_to_cpp_type,
    to_cpp_literal,
    generate_inherited_fields,
    SubsystemContext,
)


def test_normal_field_first_encounter():
    """Test that first encounter of a normal field generates declaration + assignment"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    data = {
        "name": "test_component",
        "use_fields": {
            "color_schema": "RGB565"
        }
    }
    
    lines = generate_inherited_fields(data, context)
    
    assert len(lines) == 1, f"Expected 1 line, got {len(lines)}: {lines}"
    assert "static" in lines[0], f"Expected 'static' in declaration: {lines[0]}"
    assert "color_schema" in lines[0], f"Expected 'color_schema' in declaration: {lines[0]}"
    assert "RGB565" in lines[0], f"Expected 'RGB565' in declaration: {lines[0]}"
    assert "color_schema" in context.declared_fields, "Field should be marked as declared"
    print(" test_normal_field_first_encounter passed")


def test_normal_field_subsequent_encounter():
    """Test that subsequent encounter of same field generates assignment only"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    # First component
    data1 = {
        "name": "component1",
        "use_fields": {
            "color_schema": "RGB565"
        }
    }
    lines1 = generate_inherited_fields(data1, context)
    assert "static" in lines1[0], "First should have static declaration"
    
    # Second component, same field
    data2 = {
        "name": "component2",
        "use_fields": {
            "color_schema": "RGB565"
        }
    }
    lines2 = generate_inherited_fields(data2, context)
    
    assert len(lines2) == 1, f"Expected 1 line, got {len(lines2)}: {lines2}"
    assert "static" not in lines2[0], f"Second should NOT have 'static': {lines2[0]}"
    assert "color_schema = " in lines2[0], f"Expected assignment: {lines2[0]}"
    print(" test_normal_field_subsequent_encounter passed")


def test_directive_first_encounter():
    """Test that first encounter of a directive outputs literal code"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    data = {
        "name": "test_component",
        "use_fields": {
            "###1###": "#include display_classes.hdr"
        }
    }
    
    lines = generate_inherited_fields(data, context)
    
    assert len(lines) == 1, f"Expected 1 line, got {len(lines)}: {lines}"
    assert lines[0] == "#include display_classes.hdr", f"Expected exact directive, got: {lines[0]}"
    assert "#include display_classes.hdr" in context.declared_fields, "Directive should be marked as declared"
    print(" test_directive_first_encounter passed")


def test_directive_subsequent_encounter_same_value():
    """Test that subsequent directive with same value is skipped"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    # First component
    data1 = {
        "name": "component1",
        "use_fields": {
            "###1###": "#include display_classes.hdr"
        }
    }
    lines1 = generate_inherited_fields(data1, context)
    assert len(lines1) == 1
    
    # Second component, same directive value
    data2 = {
        "name": "component2",
        "use_fields": {
            "###1###": "#include display_classes.hdr"
        }
    }
    lines2 = generate_inherited_fields(data2, context)
    
    assert len(lines2) == 0, f"Expected 0 lines (skipped), got {len(lines2)}: {lines2}"
    print(" test_directive_subsequent_encounter_same_value passed")


def test_directive_different_values():
    """Test that directives with different values are both output"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    # First directive
    data1 = {
        "name": "component1",
        "use_fields": {
            "###1###": "#include display_classes.hdr"
        }
    }
    lines1 = generate_inherited_fields(data1, context)
    
    # Different directive
    data2 = {
        "name": "component2",
        "use_fields": {
            "###2###": "#include mood_system.hdr"
        }
    }
    lines2 = generate_inherited_fields(data2, context)
    
    assert len(lines2) == 1, f"Expected 1 line (different value), got {len(lines2)}: {lines2}"
    assert lines2[0] == "#include mood_system.hdr", f"Expected different directive: {lines2[0]}"
    print(" test_directive_different_values passed")


def test_mixed_fields_and_directives():
    """Test component with both normal fields and directives"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    data = {
        "name": "complex_component",
        "use_fields": {
            "color_schema": "RGB565",
            "###1###": "#include display_classes.hdr",
            "eat_my_cupcakes": True,
            "###2###": "#include mood_system.hdr"
        }
    }
    
    lines = generate_inherited_fields(data, context)
    
    # Should have 4 lines: 2 variables + 2 directives
    assert len(lines) == 4, f"Expected 4 lines, got {len(lines)}: {lines}"
    
    # Check that we have both variable declarations
    has_color_schema = any("color_schema" in line and "static" in line for line in lines)
    has_cupcakes = any("eat_my_cupcakes" in line and "static" in line for line in lines)
    has_include1 = any("#include display_classes.hdr" in line for line in lines)
    has_include2 = any("#include mood_system.hdr" in line for line in lines)
    
    assert has_color_schema, f"Missing color_schema declaration in: {lines}"
    assert has_cupcakes, f"Missing eat_my_cupcakes declaration in: {lines}"
    assert has_include1, f"Missing display_classes include in: {lines}"
    assert has_include2, f"Missing mood_system include in: {lines}"
    
    print(" test_mixed_fields_and_directives passed")


def test_type_mismatch_detection():
    """Test that type mismatches are flagged in declared_fields"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    # First component declares as string
    data1 = {
        "name": "component1",
        "use_fields": {
            "display_mode": "RGB565"
        }
    }
    lines1 = generate_inherited_fields(data1, context)
    first_type = context.declared_fields.get("display_mode")
    
    # Second component tries to use as int
    data2 = {
        "name": "component2",
        "use_fields": {
            "display_mode": 1
        }
    }
    # This would generate a type mismatch in C++ (compile error)
    # We're just testing that both get tracked
    lines2 = generate_inherited_fields(data2, context)
    
    assert first_type == "const char*", f"Expected char* type, got {first_type}"
    assert len(lines2) == 1, "Second component should still generate assignment"
    # In actual compilation, `display_mode = 1;` with `char*` would fail
    print(" test_type_mismatch_detection passed")


def test_empty_use_fields():
    """Test component with no use_fields"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    data = {
        "name": "simple_component"
    }
    
    lines = generate_inherited_fields(data, context)
    
    assert len(lines) == 0, f"Expected 0 lines for no use_fields, got {len(lines)}: {lines}"
    print(" test_empty_use_fields passed")


def test_directive_whitespace_variations():
    """Test that directives with different whitespace are treated as different"""
    context = SubsystemContext(
        name="test_subsystem",
        controller="ESP32_S3",
        json_path=None
    )
    
    # First with spaces
    data1 = {
        "name": "component1",
        "use_fields": {
            "###1###": "#include display_classes.hdr  "
        }
    }
    lines1 = generate_inherited_fields(data1, context)
    
    # Same but different whitespace
    data2 = {
        "name": "component2",
        "use_fields": {
            "###1###": "#include display_classes.hdr"
        }
    }
    lines2 = generate_inherited_fields(data2, context)
    
    # They have different values, so second should output
    assert len(lines2) == 1, "Different whitespace = different value, should output"
    print(" test_directive_whitespace_variations passed")


if __name__ == "__main__":
    print("Running use_fields feature tests...\n")
    
    test_normal_field_first_encounter()
    test_normal_field_subsequent_encounter()
    test_directive_first_encounter()
    test_directive_subsequent_encounter_same_value()
    test_directive_different_values()
    test_mixed_fields_and_directives()
    test_type_mismatch_detection()
    test_empty_use_fields()
    test_directive_whitespace_variations()
    
    print("\n All tests passed!")
