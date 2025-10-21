#!/usr/bin/env python3
"""
Test suite for P32 JSON Inheritance Processor
Tests inheritance resolution with existing bot family configurations
"""

import unittest
import json
import tempfile
import shutil
from pathlib import Path
import sys
import os

# Add tools directory to path for importing inheritance_processor
sys.path.append(str(Path(__file__).parent))
from inheritance_processor import P32InheritanceProcessor

class TestInheritanceProcessor(unittest.TestCase):
    
    def setUp(self):
        """Create temporary test configuration structure"""
        self.test_dir = Path(tempfile.mkdtemp())
        self.config_dir = self.test_dir / "config"
        self.config_dir.mkdir()
        
        # Create test template structure
        self._create_test_templates()
        
        self.processor = P32InheritanceProcessor(str(self.config_dir))
        
    def tearDown(self):
        """Clean up temporary test files"""
        shutil.rmtree(self.test_dir)
        
    def _create_test_templates(self):
        """Create minimal test template structure"""
        
        # Base humanoid template
        base_template = {
            "relative_filename": "config/templates/humanoid_base.json",
            "version": "1.0.0",
            "template_type": "base_template",
            "coordinate_system": "skull_3d",
            "reference_point": "nose_center",
            "default_components": [
                "config/components/positioned/base_eye_left.json",
                "config/components/positioned/base_eye_right.json"
            ],
            "base_mood_profile": {
                "FEAR": 10,
                "ANGER": 10,
                "HAPPINESS": 10,
                "CONTENTMENT": 10
            }
        }
        
        # Goblin family template
        family_template = {
            "relative_filename": "config/templates/goblin_family.json",
            "version": "1.0.0",
            "template_type": "family_template",
            "family_type": "GOBLIN",
            "coordinate_system": "skull_3d", 
            "baseline_dimensions": {
                "eye_spacing": "3.0 INCH",
                "compression_factor": 0.7,
                "skull_scale": 0.8
            },
            "positioned_components": [
                "config/components/positioned/goblin_nose.json",
                "config/components/positioned/goblin_mouth.json"
            ],
            "family_mood_profile": {
                "ANGER": 15,
                "IRRITATION": 15,
                "CURIOSITY": 8
            }
        }
        
        # Individual goblin creature
        creature_config = {
            "relative_filename": "config/bots/test_goblin.json",
            "version": "1.0.0",
            "bot_id": "test_goblin_001",
            "bot_type": "GOBLIN_WARRIOR",
            "base_template": "config/templates/humanoid_base.json",
            "family_template": "config/templates/goblin_family.json",
            "description": "Test goblin for inheritance validation",
            "positioned_components": [
                "config/components/positioned/goblin_ear_left.json",
                "config/components/positioned/goblin_ear_right.json"
            ],
            "individual_mood_overrides": {
                "ANGER": 20,
                "HUNGER": 12
            }
        }
        
        # Create template directories
        templates_dir = self.config_dir / "templates"
        templates_dir.mkdir()
        bots_dir = self.config_dir / "bots"
        bots_dir.mkdir()
        
        # Write template files
        with open(templates_dir / "humanoid_base.json", 'w') as f:
            json.dump(base_template, f, indent=2)
            
        with open(templates_dir / "goblin_family.json", 'w') as f:
            json.dump(family_template, f, indent=2)
            
        with open(bots_dir / "test_goblin.json", 'w') as f:
            json.dump(creature_config, f, indent=2)
            
    def test_template_loading(self):
        """Test that templates load correctly"""
        base_template = self.processor.load_template("templates/humanoid_base.json")
        self.assertEqual(base_template["template_type"], "base_template")
        
        family_template = self.processor.load_template("templates/goblin_family.json")
        self.assertEqual(family_template["family_type"], "GOBLIN")
        
    def test_inheritance_resolution(self):
        """Test complete inheritance chain resolution"""
        creature_config = self.processor.load_template("bots/test_goblin.json")
        resolved = self.processor.resolve_inheritance_chain(creature_config)
        
        # Check that base template fields were inherited
        self.assertIn("base_mood_profile", resolved)
        self.assertEqual(resolved["coordinate_system"], "skull_3d")
        self.assertEqual(resolved["reference_point"], "nose_center")
        
        # Check that family template fields were inherited
        self.assertIn("baseline_dimensions", resolved)
        self.assertEqual(resolved["baseline_dimensions"]["eye_spacing"], "3.0 INCH")
        
        # Check that components were merged correctly
        self.assertIn("positioned_components", resolved)
        components = resolved["positioned_components"]
        
        # Should have base components + family components + individual components
        expected_components = [
            "config/components/positioned/base_eye_left.json",
            "config/components/positioned/base_eye_right.json",
            "config/components/positioned/goblin_nose.json", 
            "config/components/positioned/goblin_mouth.json",
            "config/components/positioned/goblin_ear_left.json",
            "config/components/positioned/goblin_ear_right.json"
        ]
        
        for expected in expected_components:
            self.assertIn(expected, components)
            
        # Check mood profile merging
        self.assertIn("base_mood_profile", resolved)
        self.assertIn("family_mood_profile", resolved)
        self.assertIn("individual_mood_overrides", resolved)
        
        # Check inheritance metadata cleanup
        self.assertNotIn("base_template", resolved)
        self.assertNotIn("family_template", resolved)
        self.assertTrue(resolved["resolved_from_inheritance"])
        self.assertIn("inheritance_chain", resolved)
        
    def test_mood_profile_merging(self):
        """Test that mood profiles merge correctly with overrides"""
        creature_config = self.processor.load_template("bots/test_goblin.json")
        resolved = self.processor.resolve_inheritance_chain(creature_config)
        
        # Base mood values should be present
        self.assertEqual(resolved["base_mood_profile"]["FEAR"], 10)
        self.assertEqual(resolved["base_mood_profile"]["HAPPINESS"], 10)
        
        # Family overrides should be present
        self.assertEqual(resolved["family_mood_profile"]["ANGER"], 15)
        self.assertEqual(resolved["family_mood_profile"]["CURIOSITY"], 8)
        
        # Individual overrides should be present
        self.assertEqual(resolved["individual_mood_overrides"]["ANGER"], 20)
        self.assertEqual(resolved["individual_mood_overrides"]["HUNGER"], 12)
        
    def test_component_deduplication(self):
        """Test that duplicate components are removed"""
        # Create config with duplicate components
        duplicate_config = {
            "bot_id": "test_duplicate",
            "positioned_components": [
                "config/components/positioned/eye_left.json",
                "config/components/positioned/eye_right.json",
                "config/components/positioned/eye_left.json"  # Duplicate
            ]
        }
        
        # Mock base template with overlapping components
        base_template = {
            "positioned_components": [
                "config/components/positioned/eye_left.json",  # Overlaps
                "config/components/positioned/base_speaker.json"
            ]
        }
        
        merged = self.processor._merge_templates(base_template, duplicate_config)
        components = merged["positioned_components"]
        
        # Should have no duplicates
        self.assertEqual(len(components), len(set(components)))
        
        # Should contain all unique components
        expected = [
            "config/components/positioned/eye_left.json",
            "config/components/positioned/base_speaker.json",
            "config/components/positioned/eye_right.json"
        ]
        
        for component in expected:
            self.assertIn(component, components)
            
    def test_validation(self):
        """Test configuration validation"""
        creature_config = self.processor.load_template("bots/test_goblin.json")
        resolved = self.processor.resolve_inheritance_chain(creature_config)
        
        errors = self.processor.validate_resolution(resolved)
        
        # Should be valid (empty error list)
        self.assertEqual(len(errors), 0)
        
        # Test invalid configuration
        invalid_config = {
            "bot_id": "invalid_bot"
            # Missing required fields
        }
        
        errors = self.processor.validate_resolution(invalid_config)
        self.assertGreater(len(errors), 0)
        self.assertTrue(any("bot_type" in error for error in errors))
        
    def test_real_bear_family_integration(self):
        """Test with actual bear family configuration if available"""
        # This test will work with the real config directory
        real_config_dir = Path(__file__).parent.parent / "config"
        
        if not real_config_dir.exists():
            self.skipTest("Real config directory not available")
            
        real_processor = P32InheritanceProcessor(str(real_config_dir))
        
        # Try to load bear family template
        bear_family_path = "bots/bot_families/animals/bear_family.json"
        
        try:
            bear_family = real_processor.load_template(bear_family_path)
            self.assertEqual(bear_family["family_type"], "BEAR")
            
            print(f"Successfully loaded real bear family configuration")
            print(f"Default components: {len(bear_family.get('default_components', []))}")
            
        except FileNotFoundError:
            self.skipTest("Bear family template not found")


def run_integration_test():
    """Run integration test with real configuration"""
    config_dir = Path(__file__).parent.parent / "config"
    
    if not config_dir.exists():
        print("Real config directory not found - skipping integration test")
        return
        
    processor = P32InheritanceProcessor(str(config_dir))
    
    print("P32 Inheritance Processor Integration Test")
    print("=" * 50)
    
    # Test loading existing templates
    templates_to_test = [
        "bots/bot_families/animals/bear_family.json",
        "bots/bot_families/animals/cat_family.json",
        "bots/goblin_full_torso.json"
    ]
    
    for template_path in templates_to_test:
        try:
            config = processor.load_template(template_path)
            print(f"OK Loaded: {template_path}")
            
            # Check if this has inheritance fields
            has_inheritance = any(field in config for field in ["base_template", "family_template", "inherits_from"])
            
            if has_inheritance:
                print(f"  -> Has inheritance - testing resolution...")
                resolved = processor.resolve_inheritance_chain(config)
                errors = processor.validate_resolution(resolved)
                
                if errors:
                    print(f"  ERROR Validation errors: {errors}")
                else:
                    print(f"  OK Resolved successfully")
            else:
                print(f"  -> No inheritance fields")
                
        except Exception as e:
            print(f"ERROR Error loading {template_path}: {e}")
            
    print("\nIntegration test complete!")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "integration":
        run_integration_test()
    else:
        unittest.main()