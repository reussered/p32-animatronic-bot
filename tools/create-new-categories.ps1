# P32 New Creature Category Generator
# Creates entirely new creature families to double the creature count

param(
    [switch]$DryRun = $false
)

Write-Host "=== P32 NEW CREATURE CATEGORY GENERATOR ===" -ForegroundColor Cyan

$Date = Get-Date -Format "yyyy-MM-dd"

# New creature categories with family definitions
$newCategories = @{
    "aquatic" = @{
        "sharks" = @("great_white", "hammerhead", "tiger_shark", "bull_shark", "mako_shark")
        "whales" = @("blue_whale", "humpback", "orca_killer", "sperm_whale", "beluga")
        "dolphins" = @("bottlenose", "spinner", "pacific_white", "common", "striped")
        "octopi" = @("giant_pacific", "blue_ringed", "mimic", "dumbo", "common")
        "jellyfish" = @("moon_jelly", "lion_mane", "crystal", "upside_down", "immortal")
    }
    "insects" = @{
        "spiders" = @("black_widow", "tarantula", "jumping", "wolf_spider", "orb_weaver")
        "beetles" = @("hercules", "stag_beetle", "rhinoceros", "firefly", "scarab")
        "butterflies" = @("monarch", "swallowtail", "morpho", "glasswing", "painted_lady")
        "mantis" = @("praying_mantis", "orchid", "dead_leaf", "ghost", "chinese")
        "ants" = @("fire_ant", "carpenter", "bullet_ant", "leaf_cutter", "army_ant")
    }
    "birds" = @{
        "eagles" = @("bald_eagle", "golden", "harpy", "sea_eagle", "martial")
        "owls" = @("great_horned", "barn_owl", "screech", "snowy", "eagle_owl")
        "ravens" = @("common_raven", "crow", "magpie", "jay", "jackdaw")
        "parrots" = @("macaw", "cockatoo", "amazon", "african_grey", "budgie")
        "peacocks" = @("indian_peacock", "green", "congo", "white", "spalding")
    }
    "reptiles" = @{
        "snakes" = @("python", "cobra", "viper", "anaconda", "rattlesnake")
        "lizards" = @("iguana", "gecko", "monitor", "chameleon", "bearded_dragon")
        "turtles" = @("sea_turtle", "snapping", "box_turtle", "tortoise", "slider")
        "crocodiles" = @("nile_croc", "alligator", "saltwater", "caiman", "gharial")
        "iguanas" = @("green_iguana", "marine", "rock", "desert", "fiji_banded")
    }
    "mythical" = @{
        "phoenixes" = @("fire_phoenix", "ice_phoenix", "storm", "shadow", "crystal")
        "griffins" = @("royal_griffin", "war_griffin", "sky_rider", "mountain", "sea")
        "unicorns" = @("pure_unicorn", "shadow", "rainbow", "forest", "crystal")
        "pegasus" = @("white_pegasus", "storm", "night", "golden", "silver")
        "chimeras" = @("lion_chimera", "dragon", "serpent", "eagle", "wolf")
    }
    "elemental" = @{
        "fire_spirits" = @("flame_dancer", "inferno", "ember", "phoenix_spirit", "lava")
        "water_spirits" = @("wave_rider", "ice_maiden", "mist", "tide_caller", "rain")
        "earth_golems" = @("stone_giant", "crystal", "metal", "sand", "clay")
        "air_elementals" = @("wind_walker", "storm", "cloud", "lightning", "breeze")
        "ice_crystals" = @("frost_shard", "blizzard", "glacier", "snow", "hail")
    }
}

function New-CreatureDefinition($Category, $Family, $Species) {
    $creatureName = "${species}_${family}"
    $coordinateSystem = if ($Category -eq "aquatic") { "planar_2d" } else { "skull_3d" }
    
    $creature = @{
        "relative_filename" = "config/bots/bot_families/$Category/$Family/${creatureName}.json"
        "version" = "1.0.0"
        "author" = "config/author.json"
        "bot_type" = $creatureName.ToUpper()
        "bot_id" = "${creatureName}_v1"
        "description" = "Animatronic $Species from the $Family family in $Category category"
        "created" = $Date
        "category" = $Category
        "family" = $Family
        "species" = $Species
        
        "family_template" = "config/bots/bot_families/$Category/${Family}_family.json"
        "subfamily_variant" = $Species
        
        "coordinate_system" = $coordinateSystem
        "reference_point" = "nose_center"
        
        "positioned_components" = @(
            "config/components/positioned/${creatureName}_eye_left.json",
            "config/components/positioned/${creatureName}_eye_right.json",
            "config/components/positioned/${creatureName}_mouth.json",
            "config/components/positioned/${creatureName}_speaker.json"
        )
        
        "mood_defaults" = @{
            "FEAR" = Get-Random -Minimum 5 -Maximum 25
            "ANGER" = Get-Random -Minimum 5 -Maximum 20
            "IRRITATION" = Get-Random -Minimum 10 -Maximum 25
            "HAPPINESS" = Get-Random -Minimum 15 -Maximum 30
            "CONTENTMENT" = Get-Random -Minimum 15 -Maximum 25
            "HUNGER" = Get-Random -Minimum 10 -Maximum 20
            "CURIOSITY" = Get-Random -Minimum 20 -Maximum 35
            "AFFECTION" = Get-Random -Minimum 10 -Maximum 25
        }
        
        "behavior_config" = @{
            "threat_response_enabled" = $true
            "auto_sleep_timeout_seconds" = Get-Random -Minimum 240 -Maximum 600
            "interaction_range_cm" = Get-Random -Minimum 100 -Maximum 200
        }
        
        "prototype_status" = "auto_generated"
        "tested" = $false
    }
    
    return $creature
}

# Generate all new creatures
$totalCreated = 0
foreach ($categoryName in $newCategories.Keys) {
    $category = $newCategories[$categoryName]
    
    Write-Host "`nCreating $($categoryName.ToUpper()) category..." -ForegroundColor Yellow
    
    # Create category directory
    $categoryDir = "config\bots\bot_families\$categoryName"
    if (-not $DryRun -and -not (Test-Path $categoryDir)) {
        New-Item -ItemType Directory -Path $categoryDir -Force | Out-Null
    }
    
    foreach ($familyName in $category.Keys) {
        $species = $category[$familyName]
        
        Write-Host "  Creating $familyName family..." -ForegroundColor Green
        
        # Create family directory  
        $familyDir = "$categoryDir\$familyName"
        if (-not $DryRun -and -not (Test-Path $familyDir)) {
            New-Item -ItemType Directory -Path $familyDir -Force | Out-Null
        }
        
        # Create each species
        foreach ($speciesName in $species) {
            $creature = New-CreatureDefinition $categoryName $familyName $speciesName
            $creatureFile = "$familyDir\$($creature.bot_id).json"
            
            if (-not $DryRun) {
                $creature | ConvertTo-Json -Depth 10 | Out-File -FilePath $creatureFile -Encoding UTF8
                Write-Host "    Created: $($creature.bot_id)" -ForegroundColor White
            } else {
                Write-Host "    [DRY RUN] Would create: $($creature.bot_id)" -ForegroundColor Cyan
            }
            
            $totalCreated++
        }
    }
}

Write-Host "`n=== NEW CATEGORY GENERATION COMPLETE ===" -ForegroundColor Magenta
Write-Host "New categories created: $($newCategories.Keys.Count)" -ForegroundColor Green
Write-Host "New families created: $(($newCategories.Values | ForEach-Object { $_.Keys.Count } | Measure-Object -Sum).Sum)" -ForegroundColor Green
Write-Host "New creatures created: $totalCreated" -ForegroundColor Green

if ($DryRun) {
    Write-Host "`nRun without -DryRun to actually create the files." -ForegroundColor Cyan
}