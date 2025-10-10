#!/usr/bin/env python3
"""
Test suite for JSON validator

This script tests the validation functionality with both valid and invalid JSON files.
"""

import json
import os
import sys
import tempfile
import shutil
from pathlib import Path

# Add parent directory to path to import validator
sys.path.insert(0, str(Path(__file__).parent))
from validate_json import JSONValidator


def create_test_file(test_dir: Path, filename: str, content: dict) -> Path:
    """Create a test JSON file"""
    file_path = test_dir / filename
    file_path.parent.mkdir(parents=True, exist_ok=True)
    with open(file_path, 'w') as f:
        json.dump(content, f, indent=2)
    return file_path


def create_invalid_json_file(test_dir: Path, filename: str, content: str) -> Path:
    """Create a test file with invalid JSON syntax"""
    file_path = test_dir / filename
    file_path.parent.mkdir(parents=True, exist_ok=True)
    with open(file_path, 'w') as f:
        f.write(content)
    return file_path


def test_valid_mood():
    """Test validation of valid mood file"""
    print("\n[TEST] Valid mood file...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        mood_data = {
            "name": "test_mood",
            "type": "mood",
            "description": "Test mood",
            "priority": 5,
            "triggers": ["test_trigger"],
            "behaviors": ["test_behavior"],
            "led_color": "#FF0000",
            "servo_settings": {
                "speed": 50,
                "range": 90
            }
        }
        
        create_test_file(test_dir, "test_mood.json", mood_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == True, "Valid mood should pass validation"
        assert len(validator.errors) == 0, "Should have no errors"
        print("  ✓ PASS")
        return True


def test_missing_required_field():
    """Test detection of missing required field"""
    print("\n[TEST] Missing required field...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        mood_data = {
            "name": "incomplete_mood",
            "type": "mood",
            "description": "Incomplete mood",
            # Missing priority field
            "triggers": ["test"],
            "behaviors": ["test"],
            "led_color": "#FF0000",
            "servo_settings": {
                "speed": 50,
                "range": 90
            }
        }
        
        create_test_file(test_dir, "incomplete_mood.json", mood_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == False, "Invalid mood should fail validation"
        assert len(validator.errors) > 0, "Should have errors"
        assert any("priority" in error for error in validator.errors), "Should detect missing priority field"
        print("  ✓ PASS")
        return True


def test_wrong_field_type():
    """Test detection of wrong field type"""
    print("\n[TEST] Wrong field type...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        mood_data = {
            "name": "bad_type_mood",
            "type": "mood",
            "description": "Bad type mood",
            "priority": "five",  # Should be int, not string
            "triggers": ["test"],
            "behaviors": ["test"],
            "led_color": "#FF0000",
            "servo_settings": {
                "speed": 50,
                "range": 90
            }
        }
        
        create_test_file(test_dir, "bad_type_mood.json", mood_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == False, "Invalid type should fail validation"
        assert len(validator.errors) > 0, "Should have errors"
        assert any("priority" in error and "type" in error.lower() for error in validator.errors), \
            "Should detect wrong type for priority field"
        print("  ✓ PASS")
        return True


def test_out_of_range_value():
    """Test detection of out-of-range values"""
    print("\n[TEST] Out of range value...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        mood_data = {
            "name": "out_of_range_mood",
            "type": "mood",
            "description": "Out of range mood",
            "priority": 15,  # Should be 1-10
            "triggers": ["test"],
            "behaviors": ["test"],
            "led_color": "#FF0000",
            "servo_settings": {
                "speed": 50,
                "range": 90
            }
        }
        
        create_test_file(test_dir, "out_of_range_mood.json", mood_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == False, "Out of range value should fail validation"
        assert len(validator.errors) > 0, "Should have errors"
        assert any("priority" in error and "range" in error.lower() for error in validator.errors), \
            "Should detect out of range priority value"
        print("  ✓ PASS")
        return True


def test_invalid_json_syntax():
    """Test detection of invalid JSON syntax"""
    print("\n[TEST] Invalid JSON syntax...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        # Invalid JSON with missing closing brace
        invalid_json = '{"name": "broken", "type": "mood"'
        
        create_invalid_json_file(test_dir, "broken.json", invalid_json)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == False, "Invalid JSON syntax should fail validation"
        assert len(validator.errors) > 0, "Should have errors"
        assert any("syntax" in error.lower() for error in validator.errors), \
            "Should detect JSON syntax error"
        print("  ✓ PASS")
        return True


def test_nested_validation():
    """Test validation of nested objects"""
    print("\n[TEST] Nested object validation...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        mood_data = {
            "name": "nested_invalid",
            "type": "mood",
            "description": "Nested invalid",
            "priority": 5,
            "triggers": ["test"],
            "behaviors": ["test"],
            "led_color": "#FF0000",
            "servo_settings": {
                "speed": 150,  # Out of range (should be 0-100)
                "range": 90
            }
        }
        
        create_test_file(test_dir, "nested_invalid.json", mood_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == False, "Invalid nested value should fail validation"
        assert len(validator.errors) > 0, "Should have errors"
        assert any("speed" in error for error in validator.errors), \
            "Should detect out of range speed in nested servo_settings"
        print("  ✓ PASS")
        return True


def test_behavior_validation():
    """Test validation of behavior files"""
    print("\n[TEST] Valid behavior file...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        behavior_data = {
            "name": "test_behavior",
            "type": "behavior",
            "description": "Test behavior",
            "duration_ms": 1000,
            "servos": [
                {"id": 0, "position": 90, "time_ms": 500}
            ],
            "sound_effect": None
        }
        
        create_test_file(test_dir, "test_behavior.json", behavior_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == True, "Valid behavior should pass validation"
        assert len(validator.errors) == 0, "Should have no errors"
        print("  ✓ PASS")
        return True


def test_recursive_validation():
    """Test recursive validation of subdirectories"""
    print("\n[TEST] Recursive subdirectory validation...")
    with tempfile.TemporaryDirectory() as tmpdir:
        test_dir = Path(tmpdir)
        
        # Create files in nested directories
        mood_data = {
            "name": "nested_mood",
            "type": "mood",
            "description": "Nested mood",
            "priority": 3,
            "triggers": ["test"],
            "behaviors": ["test"],
            "led_color": "#00FF00",
            "servo_settings": {"speed": 60, "range": 120}
        }
        
        behavior_data = {
            "name": "nested_behavior",
            "type": "behavior",
            "description": "Nested behavior",
            "duration_ms": 2000,
            "servos": [{"id": 1, "position": 45, "time_ms": 1000}],
            "sound_effect": None
        }
        
        create_test_file(test_dir / "moods", "nested_mood.json", mood_data)
        create_test_file(test_dir / "behaviors", "nested_behavior.json", behavior_data)
        
        validator = JSONValidator(str(test_dir))
        result = validator.validate_all()
        
        assert result == True, "All nested files should pass validation"
        assert len(validator.validated_files) == 2, "Should validate both nested files"
        print("  ✓ PASS")
        return True


def run_all_tests():
    """Run all test cases"""
    print("="*60)
    print("JSON VALIDATOR TEST SUITE")
    print("="*60)
    
    tests = [
        test_valid_mood,
        test_missing_required_field,
        test_wrong_field_type,
        test_out_of_range_value,
        test_invalid_json_syntax,
        test_nested_validation,
        test_behavior_validation,
        test_recursive_validation
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"  ✗ FAIL")
        except Exception as e:
            failed += 1
            print(f"  ✗ FAIL - {e}")
    
    print("\n" + "="*60)
    print(f"RESULTS: {passed} passed, {failed} failed")
    print("="*60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
