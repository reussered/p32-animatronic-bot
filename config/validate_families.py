# Bot Family Inheritance Validator
# Validates family template inheritance and resolves final bot configurations

import json
import os
from pathlib import Path

class BotFamilyResolver:
    def __init__(self, config_root="config"):
        self.config_root = Path(config_root)
        self.families_cache = {}
        
    def load_family_template(self, family_path):
        """Load and cache family template"""
        if family_path in self.families_cache:
            return self.families_cache[family_path]
            
        full_path = self.config_root.parent / family_path
        with open(full_path, 'r') as f:
            family_config = json.load(f)
            
        self.families_cache[family_path] = family_config
        return family_config
        
    def resolve_bot_config(self, bot_path):
        """Resolve bot configuration with family inheritance"""
        with open(bot_path, 'r') as f:
            bot_config = json.load(f)
            
        # If no family template, return bot as-is
        if "family_template" not in bot_config:
            return bot_config
            
        # Load family template
        family_config = self.load_family_template(bot_config["family_template"])
        
        # Start with family as base
        resolved_config = self.deep_copy_dict(family_config)
        
        # Apply bot-specific properties
        for key, value in bot_config.items():
            if key == "overrides":
                # Handle overrides specially
                self.apply_overrides(resolved_config, value)
            elif key == "additional_components":
                # Merge additional components
                if "default_components" in resolved_config:
                    resolved_config["default_components"].extend(value)
                else:
                    resolved_config["components"] = value
            elif key == "subfamily_selection":
                # Apply subfamily variant
                self.apply_subfamily_variant(resolved_config, value)
            elif key not in ["family_template", "template_inheritance"]:
                # Direct property override
                resolved_config[key] = value
                
        return resolved_config
        
    def apply_overrides(self, config, overrides):
        """Apply override values to configuration"""
        for key, value in overrides.items():
            if isinstance(value, dict) and key in config and isinstance(config[key], dict):
                # Merge dictionaries
                config[key].update(value)
            else:
                # Direct replacement
                config[key] = value
                
    def apply_subfamily_variant(self, config, subfamily_name):
        """Apply subfamily variant modifications"""
        if "subfamily_variants" not in config:
            return
            
        if subfamily_name not in config["subfamily_variants"]:
            print(f"Warning: Subfamily '{subfamily_name}' not found")
            return
            
        variant = config["subfamily_variants"][subfamily_name]
        
        # Apply variant modifications
        for key, value in variant.items():
            if key == "personality_modifiers":
                self.apply_personality_modifiers(config, value)
            elif key == "aesthetic_modifiers":
                if "aesthetic_profile" in config:
                    config["aesthetic_profile"].update(value)
            else:
                config[key] = value
                
    def apply_personality_modifiers(self, config, modifiers):
        """Apply personality modifier strings like '+10' or '-5'"""
        if "family_mood_profile" not in config:
            return
            
        for mood, modifier in modifiers.items():
            if mood in config["family_mood_profile"]:
                current_value = config["family_mood_profile"][mood]
                if isinstance(modifier, str):
                    if modifier.startswith('+'):
                        config["family_mood_profile"][mood] = current_value + int(modifier[1:])
                    elif modifier.startswith('-'):
                        config["family_mood_profile"][mood] = current_value - int(modifier[1:])
                else:
                    config["family_mood_profile"][mood] = modifier
                    
    def deep_copy_dict(self, d):
        """Deep copy dictionary"""
        import copy
        return copy.deepcopy(d)
        
    def validate_bot_config(self, bot_path):
        """Validate bot configuration and family inheritance"""
        try:
            resolved_config = self.resolve_bot_config(bot_path)
            
            # Validation checks
            errors = []
            warnings = []
            
            # Check required fields
            required_fields = ["bot_type", "bot_id", "coordinate_system"]
            for field in required_fields:
                if field not in resolved_config:
                    errors.append(f"Missing required field: {field}")
                    
            # Check mood profile sums
            if "family_mood_profile" in resolved_config:
                mood_sum = sum(resolved_config["family_mood_profile"].values())
                if mood_sum > 100:
                    warnings.append(f"Mood profile sum exceeds 100: {mood_sum}")
                    
            # Check component references
            if "default_components" in resolved_config:
                for component_path in resolved_config["default_components"]:
                    full_component_path = self.config_root.parent / component_path
                    if not full_component_path.exists():
                        errors.append(f"Component not found: {component_path}")
                        
            return {
                "valid": len(errors) == 0,
                "errors": errors,
                "warnings": warnings,
                "resolved_config": resolved_config
            }
            
        except Exception as e:
            return {
                "valid": False,
                "errors": [f"Exception during validation: {str(e)}"],
                "warnings": [],
                "resolved_config": None
            }

def main():
    resolver = BotFamilyResolver()
    
    # Test with goblin_warrior
    test_bot = "config/bots/goblin_warrior.json"
    if Path(test_bot).exists():
        result = resolver.validate_bot_config(test_bot)
        
        print(f"Bot Validation: {test_bot}")
        print(f"Valid: {result['valid']}")
        
        if result['errors']:
            print("Errors:")
            for error in result['errors']:
                print(f"  - {error}")
                
        if result['warnings']:
            print("Warnings:")
            for warning in result['warnings']:
                print(f"  - {warning}")
                
        if result['resolved_config']:
            print(f"Resolved mood profile: {result['resolved_config'].get('family_mood_profile', 'Not found')}")
            
    else:
        print(f"Test bot not found: {test_bot}")

if __name__ == "__main__":
    main()