# Simple Family Template Generator Test
Write-Host "=== P32 Family Template Generator ===" -ForegroundColor Cyan

# Test creating one family template
$Date = Get-Date -Format "yyyy-MM-dd"

$CatFamilyTemplate = @"
{
  "relative_filename": "config/bots/bot_families/animals/cats_family.json",
  "version": "1.0.0", 
  "author": "config/author.json",
  "family_type": "CAT",
  "family_category": "animals",
  "description": "Curious, independent cat family with almond eyes and triangular nose",
  "created": "$Date",
  "template_type": "family_template",
  
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center",
  "baseline_dimensions": {
    "eye_spacing": "2.0 INCH",
    "compression_factor": 0.9,
    "skull_scale": 0.8,
    "whisker_span": "3.5 INCH"
  },
  
  "default_components": [
    "config/components/positioned/cat_eye_left.json",
    "config/components/positioned/cat_eye_right.json", 
    "config/components/positioned/cat_nose.json",
    "config/components/positioned/cat_mouth.json",
    "config/components/positioned/cat_speaker.json"
  ],
  
  "family_mood_profile": {
    "FEAR": 15,
    "ANGER": 8,
    "IRRITATION": 12,
    "HAPPINESS": 20,
    "CONTENTMENT": 25,
    "HUNGER": 15,
    "CURIOSITY": 30,
    "AFFECTION": 20
  },
  
  "family_behavior_patterns": {
    "threat_response_enabled": true,
    "auto_sleep_timeout_seconds": 400,
    "interaction_range_cm": 120,
    "aggressive_threshold": 60,
    "calm_threshold": 40,
    "personality_traits": [
      "independent",
      "curious",
      "cautious", 
      "playful",
      "selective_affection"
    ]
  },
  
  "hardware_preferences": {
    "coordinate_system": "skull_3d"
  },
  
  "aesthetic_profile": {
    "surface_texture": "smooth_fur",
    "color_scheme": "orange_tabby",
    "eye_style": "almond_socket",
    "nose_style": "feline_triangle", 
    "mouth_style": "whisker_frame",
    "feature_asymmetry": false,
    "weathering_effects": false
  },
  
  "sound_characteristics": {
    "voice_pitch": "medium_melodic",
    "breathing_pattern": "quiet_purr",
    "comfort_sounds": ["purr", "trill", "chirp"],
    "alert_sounds": ["meow", "chirrup", "hiss"],
    "ambient_volume": 0.3,
    "reaction_volume": 0.7
  },
  
  "animation_style": {
    "movement_speed": "quick_fluid",
    "eye_tracking": "sharp_precise",
    "idle_behaviors": ["lazy_blink", "curious_tilt"],
    "threat_responses": ["alert_crouch", "defensive_arch"], 
    "expression_intensity": "high"
  },
  
  "subfamily_variants": {
    "siamese_cat": {
      "color_scheme": "cream_dark_points",
      "personality_modifiers": {
        "CURIOSITY": "+10",
        "IRRITATION": "+5"
      }
    },
    "maine_coon": {
      "color_scheme": "brown_tabby_large",
      "size_modifier": 1.3,
      "personality_modifiers": {
        "AFFECTION": "+8",
        "CONTENTMENT": "+5"
      }
    }
  },
  
  "family_compatibility": {
    "animals_standard": "EXCELLENT",
    "cross_category": "GOOD", 
    "mechanical_hybrid": "FAIR"
  },
  
  "prototype_status": "design_complete",
  "tested": false,
  "notes": [
    "Cat family template with feline-specific behaviors",
    "Optimized for curious and independent personalities",
    "Includes purring and whisker animations"
  ]
}
"@

# Write the template
$CatsDir = "config\bots\bot_families\animals"
if (-not (Test-Path $CatsDir)) {
    New-Item -ItemType Directory -Path $CatsDir -Force | Out-Null
}

$FilePath = "$CatsDir\cats_family.json"
$CatFamilyTemplate | Out-File -FilePath $FilePath -Encoding UTF8

Write-Host "Created: $FilePath" -ForegroundColor Green
Write-Host "Family template structure complete!" -ForegroundColor Cyan