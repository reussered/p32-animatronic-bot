# Generate Family-Aware Mounting System for P32 Animatronic Bot
# Extended version supporting bot families and inheritance
# Works with config/bots/bot_families/ system

param(
    [string]$BotConfig,
    [string]$BotFamily,
    [string]$BotType = "goblin",
    [switch]$GenerateBasicMounts = $true,
    [switch]$GenerateBotShells = $true,
    [switch]$OpenSCADPreview = $false,
    [switch]$ResolveFamily = $false
)

Write-Host "=== P32 Family-Aware Mounting System Generator ===" -ForegroundColor Cyan

# Function to load and resolve bot family configuration
function Resolve-BotFamily {
    param([string]$FamilyPath)
    
    if (-not (Test-Path $FamilyPath)) {
        Write-Host "ERROR: Family template not found: $FamilyPath" -ForegroundColor Red
        return $null
    }
    
    $familyConfig = Get-Content $FamilyPath | ConvertFrom-Json
    Write-Host "Loaded family: $($familyConfig.family_type) ($($familyConfig.family_category))" -ForegroundColor Green
    return $familyConfig
}

# Function to resolve individual bot with family inheritance
function Resolve-BotConfig {
    param([string]$BotPath)
    
    if (-not (Test-Path $BotPath)) {
        Write-Host "ERROR: Bot config not found: $BotPath" -ForegroundColor Red
        return $null
    }
    
    $botConfig = Get-Content $BotPath | ConvertFrom-Json
    
    # If no family template, return as-is
    if (-not $botConfig.family_template) {
        Write-Host "Bot has no family template - using direct configuration" -ForegroundColor Yellow
        return $botConfig
    }
    
    # Load family template
    $familyConfig = Resolve-BotFamily $botConfig.family_template
    if (-not $familyConfig) {
        return $null
    }
    
    # Merge family config with bot overrides
    $resolvedConfig = $familyConfig.PSObject.Copy()
    
    # Apply bot-specific overrides
    if ($botConfig.overrides) {
        foreach ($key in $botConfig.overrides.PSObject.Properties.Name) {
            if ($resolvedConfig.$key -is [PSCustomObject] -and $botConfig.overrides.$key -is [PSCustomObject]) {
                # Merge nested objects
                foreach ($subKey in $botConfig.overrides.$key.PSObject.Properties.Name) {
                    $resolvedConfig.$key.$subKey = $botConfig.overrides.$key.$subKey
                }
            } else {
                # Direct replacement
                $resolvedConfig.$key = $botConfig.overrides.$key
            }
        }
    }
    
    # Apply subfamily variant if specified
    if ($botConfig.subfamily_selection -and $familyConfig.subfamily_variants) {
        $subfamily = $familyConfig.subfamily_variants.($botConfig.subfamily_selection)
        if ($subfamily) {
            Write-Host "Applying subfamily variant: $($botConfig.subfamily_selection)" -ForegroundColor Cyan
            
            # Apply personality modifiers
            if ($subfamily.personality_modifiers -and $resolvedConfig.family_mood_profile) {
                foreach ($mood in $subfamily.personality_modifiers.PSObject.Properties.Name) {
                    $modifier = $subfamily.personality_modifiers.$mood
                    $currentValue = $resolvedConfig.family_mood_profile.$mood
                    
                    if ($modifier -match '^\+(\d+)$') {
                        $resolvedConfig.family_mood_profile.$mood = $currentValue + [int]$matches[1]
                    } elseif ($modifier -match '^-(\d+)$') {
                        $resolvedConfig.family_mood_profile.$mood = $currentValue - [int]$matches[1]
                    } else {
                        $resolvedConfig.family_mood_profile.$mood = [int]$modifier
                    }
                }
            }
            
            # Apply other subfamily properties
            foreach ($key in $subfamily.PSObject.Properties.Name) {
                if ($key -ne "personality_modifiers") {
                    $resolvedConfig.$key = $subfamily.$key
                }
            }
        }
    }
    
    # Add bot-specific properties
    foreach ($key in $botConfig.PSObject.Properties.Name) {
        if ($key -notin @("family_template", "overrides", "subfamily_selection")) {
            $resolvedConfig.$key = $botConfig.$key
        }
    }
    
    Write-Host "Resolved bot configuration with family inheritance" -ForegroundColor Green
    return $resolvedConfig
}

# Function to extract aesthetic profile from resolved config
function Get-AestheticProfile {
    param($ResolvedConfig)
    
    $aesthetic = @{
        "eye_style" = "default_socket"
        "nose_style" = "default_ridge"
        "mouth_style" = "default_line"
        "surface_texture" = "smooth"
        "color_scheme" = "neutral"
    }
    
    if ($ResolvedConfig.aesthetic_profile) {
        foreach ($key in $ResolvedConfig.aesthetic_profile.PSObject.Properties.Name) {
            $aesthetic[$key] = $ResolvedConfig.aesthetic_profile.$key
        }
    }
    
    return $aesthetic
}

# Main execution logic
if ($BotConfig) {
    Write-Host "Bot Config: $BotConfig" -ForegroundColor Yellow
    $resolvedConfig = Resolve-BotConfig $BotConfig
    if (-not $resolvedConfig) { exit 1 }
    
    $aestheticProfile = Get-AestheticProfile $resolvedConfig
    $BotType = $resolvedConfig.family_type.ToLower()
    
} elseif ($BotFamily) {
    Write-Host "Bot Family: $BotFamily" -ForegroundColor Yellow
    $familyConfig = Resolve-BotFamily $BotFamily
    if (-not $familyConfig) { exit 1 }
    
    $aestheticProfile = Get-AestheticProfile $familyConfig
    $BotType = $familyConfig.family_type.ToLower()
    
} else {
    Write-Host "Bot Type: $BotType (legacy mode)" -ForegroundColor Yellow
    # Use legacy hardcoded bot shells
    $BotShells = @{
        "goblin" = @{
            "eye_style" = "craggy_socket"
            "nose_style" = "warty_protrusion"
            "mouth_style" = "fanged_maw"
            "surface_texture" = "rough_organic"
            "color_scheme" = "dark_green"
        }
    }
    $aestheticProfile = $BotShells[$BotType]
}

if (-not $aestheticProfile) {
    Write-Host "ERROR: Could not determine aesthetic profile for bot type: $BotType" -ForegroundColor Red
    exit 1
}

Write-Host "Aesthetic Profile:" -ForegroundColor Cyan
$aestheticProfile.GetEnumerator() | ForEach-Object { 
    Write-Host "  $($_.Key): $($_.Value)" -ForegroundColor White
}

# Hardware mounting specifications (unchanged from original)
$MountingSpecs = @{
    "gc9a01_display" = @{
        "diameter" = 32.512
        "thickness" = 2.032
        "mount_holes" = 4
        "hole_diameter" = 2.54
        "hole_spacing" = 27.94
        "bracket_name" = "display_basic_mount"
    }
    "hc_sr04_sensor" = @{
        "width" = 45.0
        "height" = 20.0
        "depth" = 15.0
        "transducer_diameter" = 16.0
        "transducer_spacing" = 24.0
        "bracket_name" = "sensor_basic_mount"
    }
    "speaker" = @{
        "diameter" = 40.0
        "depth" = 20.0
        "mount_holes" = 4
        "hole_diameter" = 3.0
        "hole_spacing" = 35.0
        "bracket_name" = "speaker_basic_mount"
    }
}

# Create directories
$ScadBasicDir = "assets\shapes\scad\basic_mounts"
$ScadBotShellsDir = "assets\shapes\scad\bot_shells"
$StlBasicDir = "assets\shapes\stl\basic_mounts"
$StlBotShellsDir = "assets\shapes\stl\bot_shells"

@($ScadBasicDir, $ScadBotShellsDir, $StlBasicDir, $StlBotShellsDir) | ForEach-Object {
    if (-not (Test-Path $_)) {
        New-Item -ItemType Directory -Path $_ -Force | Out-Null
        Write-Host "Created directory: $_" -ForegroundColor Green
    }
}

# Generate family-aware bot shells
if ($GenerateBotShells) {
    Write-Host "`n=== Generating Family-Aware Bot Shells ===" -ForegroundColor Cyan
    
    # Generate eye shells based on aesthetic profile
    $EyeShellTemplate = @"
// $($BotType.ToUpper()) Eye Shell - $($aestheticProfile.eye_style) incorporating display_basic_mount
// Family-aware decorative shell for $BotType eye displays

use <../basic_mounts/display_basic_mount.scad>

module $($BotType)_eye_shell(is_left_eye = true) {
    eye_socket_diameter = 45;
    socket_depth = 12;
    
    // Eye style: $($aestheticProfile.eye_style)
    // Surface texture: $($aestheticProfile.surface_texture)
    // Color scheme: $($aestheticProfile.color_scheme)
    
    difference() {
        union() {
            // Main eye socket based on family aesthetic
            generate_eye_socket();
            
            // Family-specific surface details
            apply_surface_texture();
        }
        
        // Cutout for basic mount
        translate([0,0,socket_depth*0.4])
            mount_cutout();
    }
    
    // Include the basic mount (positioned inside shell)
    translate([0,0,socket_depth*0.4])
        display_basic_mount();
}

module generate_eye_socket() {
    // Implementation varies by eye_style
    if ("$($aestheticProfile.eye_style)" == "craggy_socket") {
        // Goblin-style craggy implementation
        difference() {
            cylinder(h=socket_depth, d=eye_socket_diameter);
            translate([0,0,socket_depth*0.3])
                cylinder(h=socket_depth*0.7+1, d=eye_socket_diameter*0.8);
        }
    } else if ("$($aestheticProfile.eye_style)" == "rounded_socket") {
        // Bear-style rounded implementation  
        hull() {
            cylinder(h=socket_depth, d=eye_socket_diameter);
            translate([0,0,socket_depth*0.5])
                sphere(d=eye_socket_diameter*0.9);
        }
    } else {
        // Default socket
        cylinder(h=socket_depth, d=eye_socket_diameter);
    }
}

module apply_surface_texture() {
    // Surface texture based on family profile
    if ("$($aestheticProfile.surface_texture)" == "rough_organic") {
        // Add organic warty details
        for(i = [0:5]) {
            rotate([0, 0, i*60 + random(30)])
                translate([eye_socket_diameter*0.3, 0, random(socket_depth)])
                    sphere(d=random(4)+2);
        }
    }
}

module mount_cutout() {
    cylinder(h=socket_depth*0.6+1, d=35);
}

// Generate left and right eye shells
translate([-30, 0, 0]) $($BotType)_eye_shell(true);   // Left eye
translate([30, 0, 0]) $($BotType)_eye_shell(false);   // Right eye

// Helper function for random values
function random(max_val) = max_val * (sin(`$t*137.5 + max_val*7.3) + 1) / 2;
"@
    
    $EyeShellPath = "$ScadBotShellsDir\$($BotType)_eye_shells.scad"
    $EyeShellTemplate | Out-File -FilePath $EyeShellPath -Encoding UTF8
    Write-Host "Generated: $EyeShellPath" -ForegroundColor Green
}

Write-Host "`n=== Family-Aware Generation Complete ===" -ForegroundColor Green
Write-Host "Bot Type: $BotType" -ForegroundColor White
Write-Host "Eye Style: $($aestheticProfile.eye_style)" -ForegroundColor White
Write-Host "Surface Texture: $($aestheticProfile.surface_texture)" -ForegroundColor White
Write-Host "Color Scheme: $($aestheticProfile.color_scheme)" -ForegroundColor White