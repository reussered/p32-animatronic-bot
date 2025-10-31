# Generate All Family Templates - Working Version
Write-Host "=== P32 Family Template Generator ===" -ForegroundColor Cyan

$Date = Get-Date -Format "yyyy-MM-dd"

# Define families
$AnimalFamilies = @("cats", "bears", "wolves")
$FantasyFamilies = @("goblins", "dragons", "elves", "orcs") 
$MechanicalFamilies = @("robots", "androids", "steampunk")
$HorrorFamilies = @("zombies", "skeletons", "ghosts")

function Create-FamilyTemplate($FamilyName, $Category) {
    $FamilyUpper = $FamilyName.ToUpper()
    
    # Set coordinate system based on category
    $CoordSystem = "skull_3d"
    $DimConfig = @"
    "eye_spacing": "3.0 INCH",
    "compression_factor": 0.8,
    "skull_scale": 1.0"@
    
    if ($Category -eq "mechanical") {
        $CoordSystem = "planar_2d"
        $DimConfig = @"
    "face_width": "6.0 INCH",
    "face_height": "8.0 INCH",
    "component_spacing": "1.5 INCH""@
    }
    
    $Template = @"
{
  "relative_filename": "config/bots/bot_families/$Category/${FamilyName}_family.json",
  "version": "1.0.0",
  "author": "config/author.json",
  "family_type": "$FamilyUpper",
  "family_category": "$Category",
  "description": "$FamilyName family with $Category characteristics",
  "created": "$Date",
  "template_type": "family_template",
  
  "coordinate_system": "$CoordSystem",
  "reference_point": "nose_center",
  "baseline_dimensions": {
$DimConfig
  },
  
  "default_components": [
    "config/components/positioned/${FamilyName}_eye_left.json",
    "config/components/positioned/${FamilyName}_eye_right.json",
    "config/components/positioned/${FamilyName}_nose.json",
    "config/components/positioned/${FamilyName}_mouth.json",
    "config/components/positioned/${FamilyName}_speaker.json"
  ],
  
  "family_mood_profile": {
    "FEAR": 15,
    "ANGER": 10,
    "IRRITATION": 15,
    "HAPPINESS": 20,
    "CONTENTMENT": 20,
    "HUNGER": 15,
    "CURIOSITY": 25,
    "AFFECTION": 15
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
      "${FamilyName}_trait_3"
    ]
  },
  
  "hardware_preferences": {
    "coordinate_system": "$CoordSystem"
  },
  
  "aesthetic_profile": {
    "surface_texture": "${FamilyName}_texture",
    "color_scheme": "${FamilyName}_colors",
    "eye_style": "${FamilyName}_eyes",
    "nose_style": "${FamilyName}_nose",
    "mouth_style": "${FamilyName}_mouth"
  },
  
  "sound_characteristics": {
    "voice_pitch": "${FamilyName}_voice",
    "breathing_pattern": "${FamilyName}_breathing",
    "comfort_sounds": ["sound1", "sound2"],
    "alert_sounds": ["alert1", "alert2"]
  },
  
  "animation_style": {
    "movement_speed": "${FamilyName}_movement",
    "eye_tracking": "${FamilyName}_tracking",
    "idle_behaviors": ["idle1", "idle2"],
    "expression_intensity": "moderate"
  },
  
  "prototype_status": "template_generated",
  "tested": false
}
"@
    
    return $Template
}

# Create all families
$AllCategories = @{
    "animals" = $AnimalFamilies
    "fantasy" = $FantasyFamilies
    "mechanical" = $MechanicalFamilies
    "horror" = $HorrorFamilies
}

foreach ($Category in $AllCategories.Keys) {
    # Create category directory
    $CategoryDir = "config\bots\bot_families\$Category"
    if (-not (Test-Path $CategoryDir)) {
        New-Item -ItemType Directory -Path $CategoryDir -Force | Out-Null
        Write-Host "Created: $CategoryDir" -ForegroundColor Yellow
    }
    
    # Create family templates
    foreach ($FamilyName in $AllCategories[$Category]) {
        $Template = Create-FamilyTemplate $FamilyName $Category
        $FilePath = "$CategoryDir\${FamilyName}_family.json"
        
        $Template | Out-File -FilePath $FilePath -Encoding UTF8
        Write-Host "Created: $FilePath" -ForegroundColor Green
    }
}

Write-Host "`nAll family templates generated!" -ForegroundColor Cyan