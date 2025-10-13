# P32 Family Variant Expander - 15+ variants per family
# Creates comprehensive variant sets within existing families

param(
    [switch]$DryRun = $false
)

Write-Host "=== P32 FAMILY VARIANT EXPANDER ===" -ForegroundColor Cyan
Write-Host "Target: 15+ variants per family" -ForegroundColor Yellow

$Date = Get-Date -Format "yyyy-MM-dd"

# Family-specific variant patterns - each family has unique 15+ variants
$familyVariants = @{
    "orcs" = @(
        "orc_shaman", "orc_brute", "orc_berserker", "orc_chieftain", "orc_warrior", 
        "orc_scout", "orc_necromancer", "orc_hunter", "orc_raider", "orc_warlord",
        "orc_grunt", "orc_sniper", "orc_bomber", "orc_assassin", "orc_captain"
    )
    "goblins" = @(
        "goblin_tinkerer", "goblin_bomber", "goblin_rider", "goblin_witch", "goblin_merchant", 
        "goblin_thief", "goblin_guard", "goblin_archer", "goblin_miner", "goblin_cook",
        "goblin_alchemist", "goblin_spy", "goblin_engineer", "goblin_overlord", "goblin_jester"
    )
    "cats" = @(
        "siamese_cat", "persian_cat", "maine_coon", "bengal_cat", "ragdoll_cat",
        "sphinx_cat", "british_shorthair", "russian_blue", "norwegian_forest", "scottish_fold",
        "abyssinian_cat", "birman_cat", "exotic_shorthair", "devon_rex", "savannah_cat"
    )
    "bears" = @(
        "kodiak_bear", "sun_bear", "sloth_bear", "spectacled_bear", "asiatic_bear",
        "brown_bear", "kermode_bear", "glacier_bear", "cinnamon_bear", "blonde_bear",
        "eurasian_bear", "syrian_bear", "himalayan_bear", "american_bear", "russian_bear"
    )
    "dragons" = @(
        "red_dragon", "blue_dragon", "green_dragon", "black_dragon", "white_dragon",
        "bronze_dragon", "silver_dragon", "gold_dragon", "copper_dragon", "brass_dragon",
        "amethyst_dragon", "crystal_dragon", "emerald_dragon", "sapphire_dragon", "topaz_dragon"
    )
    "robots" = @(
        "mk1_robot", "mk2_robot", "mk3_robot", "alpha_robot", "beta_robot",
        "gamma_robot", "delta_robot", "omega_robot", "titan_robot", "sentinel_robot",
        "guardian_robot", "nexus_robot", "quantum_robot", "plasma_robot", "neural_robot"
    )
    "androids" = @(
        "genesis_android", "nexus_android", "apex_android", "vertex_android", "matrix_android",
        "cipher_android", "vector_android", "neural_android", "quantum_android", "plasma_android",
        "photon_android", "electron_android", "proton_android", "neutron_android", "fusion_android"
    )
    "elves" = @(
        "moonlight_elf", "starlight_elf", "twilight_elf", "dawn_elf", "dusk_elf",
        "forest_guardian", "tree_singer", "leaf_dancer", "wind_walker", "stone_speaker",
        "flame_keeper", "frost_weaver", "storm_caller", "earth_shaper", "void_touched"
    )
    "undead" = @(
        "plague_zombie", "frost_zombie", "fire_zombie", "shadow_zombie", "crystal_zombie",
        "bone_lord", "grave_knight", "soul_reaper", "death_mage", "dark_priest",
        "blood_wraith", "spirit_walker", "nightmare", "doom_herald", "void_spawn"
    )
    "steampunk" = @(
        "clockwork_gentleman", "steam_baroness", "gear_engineer", "boiler_mechanic", "brass_inventor",
        "copper_artisan", "iron_industrialist", "steel_magnate", "bronze_tinkerer", "golden_aristocrat",
        "silver_scholar", "platinum_professor", "cobalt_chemist", "titanium_tycoon", "chrome_captain"
    )
    "mechanical" = @(
        "servo_unit", "hydraulic_unit", "pneumatic_unit", "electromagnetic_unit", "gyroscopic_unit",
        "rotary_engine", "piston_driver", "gear_assembly", "cam_mechanism", "linkage_system",
        "spring_actuator", "motor_controller", "sensor_array", "processing_core", "power_module"
    )
    "humanoids" = @(
        "noble_human", "merchant_human", "warrior_human", "scholar_human", "artisan_human",
        "farmer_human", "sailor_human", "mountain_human", "desert_human", "forest_human",
        "city_dweller", "nomad_wanderer", "tribal_leader", "royal_guard", "court_mage"
    )
}

function New-FamilyVariant($Family, $VariantName) {
    # Determine characteristics based on variant name
    $characteristics = Get-VariantCharacteristics $VariantName
    
    $variant = @{
        "relative_filename" = "config/bots/bot_families/$Family/${VariantName}.json"
        "version" = "1.0.0"
        "author" = "config/author.json"
        "bot_type" = $VariantName.ToUpper()
        "bot_id" = "${VariantName}_v1"
        "description" = "Animatronic $VariantName - specialized variant of $Family family"
        "created" = $Date
        "family" = $Family
        "variant_type" = ($VariantName -split "_")[1]
        
        "family_template" = "config/bots/bot_families/$Family/${Family}_family.json"
        
        "coordinate_system" = if ($Family -in @("robots", "androids", "humanoids")) { "planar_2d" } else { "skull_3d" }
        "reference_point" = "nose_center"
        
        "positioned_components" = @(
            "config/components/positioned/${VariantName}_eye_left.json",
            "config/components/positioned/${VariantName}_eye_right.json",
            "config/components/positioned/${VariantName}_mouth.json",
            "config/components/positioned/${VariantName}_speaker.json"
        )
        
        "mood_defaults" = $characteristics.moods
        
        "behavior_config" = @{
            "threat_response_enabled" = $characteristics.aggressive
            "auto_sleep_timeout_seconds" = $characteristics.sleepTimeout
            "interaction_range_cm" = $characteristics.range
            "aggressive_threshold" = $characteristics.aggressionThreshold
            "personality_traits" = $characteristics.traits
        }
        
        "aesthetic_profile" = @{
            "size_class" = $characteristics.size
            "color_theme" = $characteristics.colors
            "surface_texture" = $characteristics.texture
        }
        
        "prototype_status" = "family_variant_generated"
        "tested" = $false
    }
    
    return $variant
}

function Get-VariantCharacteristics($VariantName) {
    # Analyze variant name to determine characteristics
    $type = ($VariantName -split "_")[1]
    
    $baseChars = @{
        moods = @{
            FEAR = 15; ANGER = 10; IRRITATION = 15; HAPPINESS = 20
            CONTENTMENT = 20; HUNGER = 15; CURIOSITY = 25; AFFECTION = 15
        }
        aggressive = $true
        sleepTimeout = 300
        range = 150
        aggressionThreshold = 50
        traits = @("standard_behavior")
        size = "medium"
        colors = "natural"
        texture = "organic"
    }
    
    # Modify based on variant type
    switch -Regex ($type) {
        "brute|warrior|berserker" { 
            $baseChars.moods.ANGER = 35
            $baseChars.moods.FEAR = 5
            $baseChars.aggressive = $true
            $baseChars.aggressionThreshold = 30
            $baseChars.size = "large"
            $baseChars.traits = @("aggressive", "strong", "fearless")
        }
        "shaman|mage|witch" { 
            $baseChars.moods.CURIOSITY = 40
            $baseChars.moods.CONTENTMENT = 30
            $baseChars.aggressive = $false
            $baseChars.traits = @("intelligent", "mystical", "wise")
            $baseChars.colors = "mystical"
        }
        "scout|spy|assassin" { 
            $baseChars.moods.CURIOSITY = 35
            $baseChars.moods.IRRITATION = 25
            $baseChars.range = 200
            $baseChars.traits = @("stealthy", "observant", "quick")
            $baseChars.size = "small"
        }
        "guard|captain|lord" { 
            $baseChars.moods.CONTENTMENT = 35
            $baseChars.moods.AFFECTION = 25
            $baseChars.aggressive = $true
            $baseChars.traits = @("protective", "loyal", "disciplined")
        }
        "hunter|ranger|sniper" { 
            $baseChars.moods.CURIOSITY = 30
            $baseChars.range = 250
            $baseChars.traits = @("patient", "accurate", "focused")
        }
        default { 
            $baseChars.traits = @("balanced", "adaptable")
        }
    }
    
    return $baseChars
}

# Process each family
$totalCreated = 0
foreach ($family in $familyVariants.Keys) {
    $variants = $familyVariants[$family]
    
    Write-Host "`nCreating $family family variants..." -ForegroundColor Yellow
    
    # Create family directory if it doesn't exist
    $familyDir = "config\bots\bot_families\$family"
    if (-not $DryRun -and -not (Test-Path $familyDir)) {
        New-Item -ItemType Directory -Path $familyDir -Force | Out-Null
        Write-Host "  Created directory: $familyDir" -ForegroundColor Green
    }
    
    foreach ($variantName in $variants) {
        $variantFile = "$familyDir\${variantName}.json"
        
        # Skip if variant already exists
        if (Test-Path $variantFile) {
            Write-Host "  Exists: $variantName" -ForegroundColor Gray
            continue
        }
        
        $variant = New-FamilyVariant $family $variantName
        
        if (-not $DryRun) {
            $variant | ConvertTo-Json -Depth 10 | Out-File -FilePath $variantFile -Encoding UTF8
            Write-Host "  Created: $variantName" -ForegroundColor Green
        } else {
            Write-Host "  [DRY RUN] Would create: $variantName" -ForegroundColor Cyan
        }
        
        $totalCreated++
    }
    
    Write-Host "  $family family: $($variants.Count) variants total" -ForegroundColor White
}

Write-Host "`n=== FAMILY VARIANT EXPANSION COMPLETE ===" -ForegroundColor Magenta
Write-Host "Families processed: $($familyVariants.Keys.Count)" -ForegroundColor Green
Write-Host "New variants created: $totalCreated" -ForegroundColor Green

$estimatedTotal = $totalCreated + 55
Write-Host "Estimated total creatures: $estimatedTotal" -ForegroundColor Yellow

if ($DryRun) {
    Write-Host "`nRun without -DryRun to actually create the files." -ForegroundColor Cyan
}