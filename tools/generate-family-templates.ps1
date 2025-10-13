# Family Template Generator for P32 Animatronic Bot
# Creates standardized family.json files for all bot families

param(
    [string]$FamilyName = "",
    [string]$Category = "",
    [switch]$GenerateAll = $false,
    [switch]$ValidateExisting = $false
)

Write-Host "=== P32 Family Template Generator ===" -ForegroundColor Cyan

# Define family categories and their characteristics
$FamilyCategories = @{
    "animals" = @{
        "cats" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "2.0 INCH"
            "compression_factor" = 0.9
            "skull_scale" = 0.8
            "surface_texture" = "smooth_fur"
            "voice_pitch" = "medium_melodic"
            "movement_speed" = "quick_fluid"
        }
        "bears" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "4.0 INCH"
            "compression_factor" = 0.8
            "skull_scale" = 1.4
            "surface_texture" = "thick_fur"
            "voice_pitch" = "low_rumbling"
            "movement_speed" = "slow_deliberate"
        }
        "wolves" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "3.2 INCH"
            "compression_factor" = 0.75
            "skull_scale" = 1.1
            "surface_texture" = "coarse_fur"
            "voice_pitch" = "medium_howling"
            "movement_speed" = "pack_coordinated"
        }
    }
    "fantasy" = @{
        "goblins" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "3.0 INCH"
            "compression_factor" = 0.7
            "skull_scale" = 1.0
            "surface_texture" = "rough_warty"
            "voice_pitch" = "raspy_harsh"
            "movement_speed" = "quick_jerky"
        }
        "dragons" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "3.5 INCH"
            "compression_factor" = 0.6
            "skull_scale" = 1.3
            "surface_texture" = "scaled_reptilian"
            "voice_pitch" = "deep_resonant"
            "movement_speed" = "serpentine_smooth"
        }
        "elves" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "2.8 INCH"
            "compression_factor" = 1.0
            "skull_scale" = 0.95
            "surface_texture" = "smooth_ethereal"
            "voice_pitch" = "high_melodic"
            "movement_speed" = "graceful_fluid"
        }
    }
    "mechanical" = @{
        "robots" = @{
            "coordinate_system" = "planar_2d"
            "face_width" = "6.0 INCH"
            "face_height" = "8.0 INCH"
            "component_spacing" = "1.5 INCH"
            "surface_texture" = "metallic_brushed"
            "voice_pitch" = "synthesized_digital"
            "movement_speed" = "mechanical_precise"
        }
        "androids" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "2.6 INCH"
            "compression_factor" = 1.0
            "skull_scale" = 1.0
            "surface_texture" = "synthetic_skin"
            "voice_pitch" = "human_like"
            "movement_speed" = "human_natural"
        }
        "steampunk" = @{
            "coordinate_system" = "planar_2d"
            "face_width" = "5.5 INCH"
            "face_height" = "7.5 INCH"
            "component_spacing" = "1.8 INCH"
            "surface_texture" = "brass_copper"
            "voice_pitch" = "mechanical_steam"
            "movement_speed" = "clockwork_rhythmic"
        }
    }
    "horror" = @{
        "zombies" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "2.8 INCH"
            "compression_factor" = 0.9
            "skull_scale" = 1.05
            "surface_texture" = "decaying_flesh"
            "voice_pitch" = "guttural_moan"
            "movement_speed" = "shambling_jerky"
        }
        "skeletons" = @{
            "coordinate_system" = "skull_3d"
            "eye_spacing" = "3.0 INCH"
            "compression_factor" = 1.0
            "skull_scale" = 1.0
            "surface_texture" = "bone_weathered"
            "voice_pitch" = "hollow_rattling"
            "movement_speed" = "bone_clicking"
        }
    }
}

# Mood profiles by category
$MoodProfiles = @{
    "animals" = @{
        "FEAR" = 15; "ANGER" = 8; "IRRITATION" = 12; "HAPPINESS" = 20; 
        "CONTENTMENT" = 25; "HUNGER" = 15; "CURIOSITY" = 30; "AFFECTION" = 20
    }
    "fantasy" = @{
        "FEAR" = 10; "ANGER" = 20; "IRRITATION" = 18; "HAPPINESS" = 15; 
        "CONTENTMENT" = 12; "HUNGER" = 20; "CURIOSITY" = 25; "AFFECTION" = 10
    }
    "mechanical" = @{
        "FEAR" = 5; "ANGER" = 10; "IRRITATION" = 15; "HAPPINESS" = 25; 
        "CONTENTMENT" = 20; "HUNGER" = 0; "CURIOSITY" = 35; "AFFECTION" = 5
    }
    "horror" = @{
        "FEAR" = 5; "ANGER" = 35; "IRRITATION" = 30; "HAPPINESS" = 5; 
        "CONTENTMENT" = 8; "HUNGER" = 40; "CURIOSITY" = 15; "AFFECTION" = 2
    }
}

function New-FamilyTemplate {
    param(
        [string]$FamilyName,
        [string]$Category,
        [hashtable]$FamilySpec
    )
    
    $Date = Get-Date -Format "yyyy-MM-dd"
    $FamilyUpper = $FamilyName.ToUpper()
    
    # Get mood profile for category
    $MoodProfile = $MoodProfiles[$Category]
    
    # Build coordinate system configuration
    $CoordinateConfig = ""
    if ($FamilySpec.coordinate_system -eq "skull_3d") {
        $CoordinateConfig = @"
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center",
  "baseline_dimensions": {
    "eye_spacing": "$($FamilySpec.eye_spacing)",
    "compression_factor": $($FamilySpec.compression_factor),
    "skull_scale": $($FamilySpec.skull_scale),
    "mechanical_precision": "moderate_tolerance"
  },"@
    } else {
        $CoordinateConfig = @"
  "coordinate_system": "planar_2d",
  "reference_point": "nose_center", 
  "baseline_dimensions": {
    "face_width": "$($FamilySpec.face_width)",
    "face_height": "$($FamilySpec.face_height)",
    "component_spacing": "$($FamilySpec.component_spacing)"
  },"@
    }
    
    # Generate family template
    $FamilyTemplate = @"
{
  "relative_filename": "config/bots/bot_families/$Category/${FamilyName}_family.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "family_type": "$FamilyUpper",
  "family_category": "$Category",
  "description": "$FamilyName family with distinctive characteristics and behaviors",
  "created": "$Date",
  "template_type": "family_template",
  
$CoordinateConfig
  
  "default_components": [
    "config/components/positioned/${FamilyName}_eye_left.json",
    "config/components/positioned/${FamilyName}_eye_right.json",
    "config/components/positioned/${FamilyName}_nose.json", 
    "config/components/positioned/${FamilyName}_mouth.json",
    "config/components/positioned/${FamilyName}_speaker.json"
  ],
  
  "family_mood_profile": {
    "FEAR": $($MoodProfile.FEAR),
    "ANGER": $($MoodProfile.ANGER),
    "IRRITATION": $($MoodProfile.IRRITATION),
    "HAPPINESS": $($MoodProfile.HAPPINESS),
    "CONTENTMENT": $($MoodProfile.CONTENTMENT),
    "HUNGER": $($MoodProfile.HUNGER),
    "CURIOSITY": $($MoodProfile.CURIOSITY),
    "AFFECTION": $($MoodProfile.AFFECTION)
  },
  
  "family_behavior_patterns": {
    "threat_response_enabled": true,
    "auto_sleep_timeout_seconds": 300,
    "interaction_range_cm": 150,
    "aggressive_threshold": 50,
    "calm_threshold": 30,
    "personality_traits": [
      "family_trait_1",
      "family_trait_2", 
      "family_trait_3",
      "family_trait_4",
      "family_trait_5"
    ]
  },
  
  "hardware_preferences": {
    "coordinate_system": "$($FamilySpec.coordinate_system)",
    "spi_device_assignments": {
      "left_eye": "SPI_DEVICE_1",
      "right_eye": "SPI_DEVICE_2",
      "mouth_display": "SPI_DEVICE_3"
    },
    "i2s_device_assignments": {
      "primary_speaker": "I2S_DEVICE_1"
    }
  },
  
  "aesthetic_profile": {
    "surface_texture": "$($FamilySpec.surface_texture)",
    "color_scheme": "family_default_colors",
    "eye_style": "family_eye_design",
    "nose_style": "family_nose_design", 
    "mouth_style": "family_mouth_design",
    "feature_asymmetry": false,
    "weathering_effects": false
  },
  
  "sound_characteristics": {
    "voice_pitch": "$($FamilySpec.voice_pitch)",
    "breathing_pattern": "family_breathing",
    "comfort_sounds": ["sound1", "sound2", "sound3"],
    "alert_sounds": ["alert1", "alert2", "alert3"],
    "ambient_volume": 0.3,
    "reaction_volume": 0.7
  },
  
  "animation_style": {
    "movement_speed": "$($FamilySpec.movement_speed)",
    "eye_tracking": "family_tracking_style",
    "idle_behaviors": ["idle1", "idle2", "idle3"],
    "threat_responses": ["threat1", "threat2", "threat3"],
    "expression_intensity": "moderate"
  },
  
  "subfamily_variants": {
    "standard_${FamilyName}": {
      "color_scheme": "standard_colors",
      "personality_modifiers": {},
      "size_modifier": 1.0,
      "special_features": []
    }
  },
  
  "family_compatibility": {
    "${Category}_standard": "EXCELLENT",
    "cross_category": "GOOD",
    "mechanical_hybrid": "FAIR"
  },
  
  "prototype_status": "template_generated",
  "tested": false,
  "notes": [
    "Generated family template for $FamilyName",
    "Customize personality_traits, sounds, and behaviors",
    "Add subfamily variants as needed",
    "Update component references to match actual positioned components"
  ]
}
"@

    return $FamilyTemplate
}

# Main execution
if ($GenerateAll) {
    Write-Host "Generating family templates for all categories..." -ForegroundColor Yellow
    
    foreach ($Category in $FamilyCategories.Keys) {
        foreach ($FamilyName in $FamilyCategories[$Category].Keys) {
            $FamilySpec = $FamilyCategories[$Category][$FamilyName]
            
            # Create category directory if it doesn't exist
            $CategoryDir = "config\bots\bot_families\$Category"
            if (-not (Test-Path $CategoryDir)) {
                New-Item -ItemType Directory -Path $CategoryDir -Force | Out-Null
            }
            
            # Generate family template
            $Template = New-FamilyTemplate -FamilyName $FamilyName -Category $Category -FamilySpec $FamilySpec
            
            # Write to file
            $FilePath = "$CategoryDir\${FamilyName}_family.json"
            $Template | Out-File -FilePath $FilePath -Encoding UTF8
            
            Write-Host "Created: $FilePath" -ForegroundColor Green
        }
    }
} elseif ($FamilyName -and $Category) {
    # Generate single family template
    if ($FamilyCategories.ContainsKey($Category) -and $FamilyCategories[$Category].ContainsKey($FamilyName)) {
        $FamilySpec = $FamilyCategories[$Category][$FamilyName]
        
        $CategoryDir = "config\bots\bot_families\$Category"
        if (-not (Test-Path $CategoryDir)) {
            New-Item -ItemType Directory -Path $CategoryDir -Force | Out-Null
        }
        
        $Template = New-FamilyTemplate -FamilyName $FamilyName -Category $Category -FamilySpec $FamilySpec
        $FilePath = "$CategoryDir\${FamilyName}_family.json"
        $Template | Out-File -FilePath $FilePath -Encoding UTF8
        
        Write-Host "Created: $FilePath" -ForegroundColor Green
    } else {
        Write-Host "Error: Family '$FamilyName' not found in category '$Category'" -ForegroundColor Red
    }
} else {
    # Show available families
    Write-Host "Available families by category:" -ForegroundColor Yellow
    foreach ($Category in $FamilyCategories.Keys) {
        Write-Host "  ${Category}:" -ForegroundColor Cyan
        foreach ($FamilyName in $FamilyCategories[$Category].Keys) {
            Write-Host "    - $FamilyName" -ForegroundColor White
        }
    }
    Write-Host "`nUsage:" -ForegroundColor Yellow
    Write-Host "  .\tools\generate-family-templates.ps1 -GenerateAll" -ForegroundColor Green
    Write-Host "  .\tools\generate-family-templates.ps1 -FamilyName cats -Category animals" -ForegroundColor Green
}