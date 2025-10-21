# Complete Family Template Generator
# Creates all family.json files for the P32 bot ecosystem

Write-Host "=== P32 Complete Family Template Generator ===" -ForegroundColor Cyan

$Date = Get-Date -Format "yyyy-MM-dd"

# Define all families to create
$Families = @{
    "animals" = @(
        @{name="cats"; eye_spacing="2.0"; compression=0.9; scale=0.8; texture="smooth_fur"; pitch="medium_melodic"; speed="quick_fluid"},
        @{name="bears"; eye_spacing="4.0"; compression=0.8; scale=1.4; texture="thick_fur"; pitch="low_rumbling"; speed="slow_powerful"},
        @{name="wolves"; eye_spacing="3.2"; compression=0.75; scale=1.1; texture="coarse_fur"; pitch="pack_howling"; speed="coordinated_pack"}
    )
    "fantasy" = @(
        @{name="goblins"; eye_spacing="3.0"; compression=0.7; scale=1.0; texture="rough_warty"; pitch="raspy_harsh"; speed="quick_jerky"},
        @{name="dragons"; eye_spacing="3.5"; compression=0.6; scale=1.3; texture="scaled_reptilian"; pitch="deep_resonant"; speed="serpentine_smooth"},
        @{name="elves"; eye_spacing="2.8"; compression=1.0; scale=0.95; texture="smooth_ethereal"; pitch="high_melodic"; speed="graceful_fluid"},
        @{name="orcs"; eye_spacing="3.4"; compression=0.75; scale=1.2; texture="rough_scarred"; pitch="guttural_roar"; speed="brutal_direct"}
    )
    "mechanical" = @(
        @{name="robots"; face_width="6.0"; face_height="8.0"; spacing="1.5"; texture="metallic_brushed"; pitch="synthesized_digital"; speed="mechanical_precise"},
        @{name="androids"; eye_spacing="2.6"; compression=1.0; scale=1.0; texture="synthetic_skin"; pitch="human_like"; speed="human_natural"},
        @{name="steampunk"; face_width="5.5"; face_height="7.5"; spacing="1.8"; texture="brass_copper"; pitch="mechanical_steam"; speed="clockwork_rhythmic"}
    )
    "horror" = @(
        @{name="zombies"; eye_spacing="2.8"; compression=0.9; scale=1.05; texture="decaying_flesh"; pitch="guttural_moan"; speed="shambling_jerky"},
        @{name="skeletons"; eye_spacing="3.0"; compression=1.0; scale=1.0; texture="bone_weathered"; pitch="hollow_rattling"; speed="bone_clicking"},
        @{name="ghosts"; eye_spacing="2.5"; compression=1.1; scale=0.9; texture="ethereal_mist"; pitch="whispered_echo"; speed="floating_drift"}
    )
}

# Mood profiles by category
$MoodProfiles = @{
    "animals" = @{FEAR=15; ANGER=8; IRRITATION=12; HAPPINESS=20; CONTENTMENT=25; HUNGER=15; CURIOSITY=30; AFFECTION=20}
    "fantasy" = @{FEAR=10; ANGER=20; IRRITATION=18; HAPPINESS=15; CONTENTMENT=12; HUNGER=20; CURIOSITY=25; AFFECTION=10}
    "mechanical" = @{FEAR=5; ANGER=10; IRRITATION=15; HAPPINESS=25; CONTENTMENT=20; HUNGER=0; CURIOSITY=35; AFFECTION=5}
    "horror" = @{FEAR=5; ANGER=35; IRRITATION=30; HAPPINESS=5; CONTENTMENT=8; HUNGER=40; CURIOSITY=15; AFFECTION=2}
}

function New-FamilyTemplate {
    param($FamilySpec, $Category)
    
    $FamilyName = $FamilySpec.name
    $FamilyUpper = $FamilyName.ToUpper()
    $Moods = $MoodProfiles[$Category]
    
    # Determine coordinate system and dimensions
    $CoordConfig = ""
    if ($FamilySpec.eye_spacing) {
        # 3D skull system
        $CoordConfig = @"
  "coordinate_system": "skull_3d",
  "reference_point": "nose_center",
  "baseline_dimensions": {
    "eye_spacing": "$($FamilySpec.eye_spacing) INCH",
    "compression_factor": $($FamilySpec.compression),
    "skull_scale": $($FamilySpec.scale),
    "mechanical_precision": "moderate_tolerance"
  },"@
    } else {
        # 2D planar system
        $CoordConfig = @"
  "coordinate_system": "planar_2d", 
  "reference_point": "nose_center",
  "baseline_dimensions": {
    "face_width": "$($FamilySpec.face_width) INCH",
    "face_height": "$($FamilySpec.face_height) INCH",
    "component_spacing": "$($FamilySpec.spacing) INCH"
  },"@
    }
    
    return @"
{
  "relative_filename": "config/bots/bot_families/$Category/${FamilyName}_family.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "family_type": "$FamilyUpper",
  "family_category": "$Category",
  "description": "$FamilyName family with distinctive $Category characteristics and behaviors",
  "created": "$Date",
  "template_type": "family_template",
  
$CoordConfig
  
  "default_components": [
    "config/components/positioned/${FamilyName}_eye_left.json",
    "config/components/positioned/${FamilyName}_eye_right.json",
    "config/components/positioned/${FamilyName}_nose.json",
    "config/components/positioned/${FamilyName}_mouth.json",
    "config/components/positioned/${FamilyName}_speaker.json"
  ],
  
  "family_mood_profile": {
    "FEAR": $($Moods.FEAR),
    "ANGER": $($Moods.ANGER),
    "IRRITATION": $($Moods.IRRITATION),
    "HAPPINESS": $($Moods.HAPPINESS),
    "CONTENTMENT": $($Moods.CONTENTMENT),
    "HUNGER": $($Moods.HUNGER),
    "CURIOSITY": $($Moods.CURIOSITY),
    "AFFECTION": $($Moods.AFFECTION)
  },
  
  "family_behavior_patterns": {
    "threat_response_enabled": true,
    "auto_sleep_timeout_seconds": 300,
    "interaction_range_cm": 150,
    "aggressive_threshold": 50,
    "calm_threshold": 30,
    "personality_traits": [
      "${FamilyName}_trait_1",
      "${FamilyName}_trait_2",
      "${FamilyName}_trait_3",
      "${FamilyName}_trait_4",
      "${FamilyName}_trait_5"
    ]
  },
  
  "hardware_preferences": {
    "coordinate_system": "auto_detected",
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
    "surface_texture": "$($FamilySpec.texture)",
    "color_scheme": "${FamilyName}_default_colors",
    "eye_style": "${FamilyName}_eye_design",
    "nose_style": "${FamilyName}_nose_design",
    "mouth_style": "${FamilyName}_mouth_design", 
    "feature_asymmetry": false,
    "weathering_effects": false
  },
  
  "sound_characteristics": {
    "voice_pitch": "$($FamilySpec.pitch)",
    "breathing_pattern": "${FamilyName}_breathing",
    "comfort_sounds": ["sound1", "sound2", "sound3"],
    "alert_sounds": ["alert1", "alert2", "alert3"],
    "ambient_volume": 0.3,
    "reaction_volume": 0.7
  },
  
  "animation_style": {
    "movement_speed": "$($FamilySpec.speed)",
    "eye_tracking": "${FamilyName}_tracking_style",
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
    "Generated $FamilyName family template for $Category category",
    "Customize personality_traits and sounds for family characteristics",
    "Add subfamily variants as needed",
    "Update component references to match actual positioned components"
  ]
}
"@
}

# Generate all family templates
foreach ($Category in $Families.Keys) {
    # Create category directory
    $CategoryDir = "config\bots\bot_families\$Category"
    if (-not (Test-Path $CategoryDir)) {
        New-Item -ItemType Directory -Path $CategoryDir -Force | Out-Null
        Write-Host "Created directory: $CategoryDir" -ForegroundColor Yellow
    }
    
    # Generate each family in the category
    foreach ($FamilySpec in $Families[$Category]) {
        $Template = New-FamilyTemplate -FamilySpec $FamilySpec -Category $Category
        $FilePath = "$CategoryDir\$($FamilySpec.name)_family.json"
        
        $Template | Out-File -FilePath $FilePath -Encoding UTF8
        Write-Host "Created: $FilePath" -ForegroundColor Green
    }
}

Write-Host "`n All family templates generated successfully!" -ForegroundColor Cyan
Write-Host " Family structure ready for bot organization" -ForegroundColor Green