#!/usr/bin/env pwsh
<#
.SYNOPSIS
Creates all missing hardware and component files referenced in JSON configurations

.DESCRIPTION  
Systematically creates missing files that are referenced but don't exist:
- Missing hardware components (gc9a01_display.json, hc_sr04_sensor.json, etc.)
- Missing creature-specific components (goblin_eye.json, cat_eye.json, etc.)
- Missing interface definitions for complex assignments

.PARAMETER ProjectRoot
Root directory for JSON file operations (default: "config")

.PARAMETER DryRun
Show what would be created without making files

.PARAMETER Verbose  
Show detailed operation information

.EXAMPLE
.\tools\create_missing_references.ps1
.\tools\create_missing_references.ps1 -DryRun -Verbose
#>

param(
    [string]$ProjectRoot = "config",
    [switch]$DryRun,
    [switch]$Verbose
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# Color functions for output
function Write-Success { param($Message) Write-Host "✅ $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "⚠️  $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "❌ $Message" -ForegroundColor Red }
function Write-Info { param($Message) Write-Host "ℹ️  $Message" -ForegroundColor Cyan }
function Write-Progress { param($Message) Write-Host "🔧 $Message" -ForegroundColor Blue }

Write-Info "P32 Animatronic Bot - Missing References Creator"
Write-Info "Target: $ProjectRoot | DryRun: $DryRun | Verbose: $Verbose"
Write-Info "="*70

# Standard hardware component definitions
$HardwareDefinitions = @{
    "gc9a01_display.json" = @{
        relative_filename = "components/hardware/gc9a01_display.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "gc9a01_display"
        hardware_type = "ROUND_DISPLAY"
        description = "GC9A01 240x240 round color display"
        specifications = @{
            display_type = "TFT_LCD"
            resolution = @{ width = 240; height = 240 }
            color_depth = "16-bit RGB565"
            interface = "SPI"
            power_consumption = @{
                voltage = "3.3V"
                current_draw = "80mA"
                power_watts = "0.264W"
            }
            physical_dimensions = @{
                diameter = "28mm"
                thickness = "3.5mm"
                weight = "8g"
            }
        }
        cost_performance = @{
            unit_cost_usd = 12.99
            availability = "COMMON"
            lead_time_days = 5
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "hc_sr04_sensor.json" = @{
        relative_filename = "components/hardware/hc_sr04_sensor.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "hc_sr04_sensor"
        hardware_type = "ULTRASONIC_SENSOR"
        description = "HC-SR04 ultrasonic distance sensor"
        specifications = @{
            sensing_range = "2-400cm"
            accuracy = "±3mm"
            measuring_angle = "15°"
            frequency = "40kHz"
            power_consumption = @{
                voltage = "5V"
                current_draw = "15mA"
                power_watts = "0.075W"
            }
            physical_dimensions = @{
                width = "45mm"
                height = "20mm"
                depth = "15mm"
                weight = "8g"
            }
        }
        cost_performance = @{
            unit_cost_usd = 3.99
            availability = "COMMON"
            lead_time_days = 2
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "speaker.json" = @{
        relative_filename = "components/hardware/speaker.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "speaker"
        hardware_type = "AUDIO_SPEAKER"
        description = "40mm 8Ω full-range speaker"
        specifications = @{
            impedance = "8Ω"
            power_rating = "3W"
            frequency_response = "100Hz-18kHz"
            sensitivity = "85dB"
            power_consumption = @{
                voltage = "3.3V"
                current_draw = "500mA"
                power_watts = "1.65W"
            }
            physical_dimensions = @{
                diameter = "40mm"
                depth = "20mm"
                weight = "25g"
            }
        }
        cost_performance = @{
            unit_cost_usd = 8.99
            availability = "COMMON"
            lead_time_days = 3
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "esp32_s3_devkit.json" = @{
        relative_filename = "components/hardware/esp32_s3_devkit.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "esp32_s3_devkit"
        hardware_type = "MICROCONTROLLER"
        description = "ESP32-S3-DevKitC-1 development board"
        specifications = @{
            processor = "ESP32-S3"
            memory_flash_mb = 16
            memory_ram_kb = 512
            wifi_standard = "802.11 b/g/n"
            bluetooth = "Bluetooth LE 5.0"
            gpio_pins = 45
            power_consumption = @{
                voltage = "5V"
                current_draw = "500mA"
                power_watts = "2.5W"
            }
            physical_dimensions = @{
                width = "25.5mm"
                height = "69.8mm"
                depth = "13.2mm"
                weight = "25g"
            }
        }
        cost_performance = @{
            unit_cost_usd = 45.99
            availability = "COMMON"
            lead_time_days = 7
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "mg996r_servo.json" = @{
        relative_filename = "components/hardware/mg996r_servo.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "mg996r_servo"
        hardware_type = "SERVO_MOTOR"
        description = "MG996R high-torque metal gear servo"
        specifications = @{
            torque_kg_cm = 10.0
            rotation_degrees = 180
            control_signal = "PWM"
            operating_speed = "0.17sec/60°"
            power_consumption = @{
                voltage = "6V"
                current_draw = "900mA"
                power_watts = "5.4W"
            }
            physical_dimensions = @{
                width = "20mm"
                height = "40mm"
                depth = "37mm"
                weight = "55g"
            }
        }
        cost_performance = @{
            unit_cost_usd = 18.99
            availability = "COMMON"
            lead_time_days = 5
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
}

# Creature-specific component definitions
$CreatureComponents = @{
    "goblin_eye.json" = @{
        relative_filename = "components/creature_specific/goblin_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "goblin_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "GOBLIN"
        description = "Goblin-specific eye rendering component with menacing characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "goblin_skin_warty"
                color_scheme = "goblin_red_yellow"
                scale_factor = 1.2
                distinctive_features = @("bloodshot_veins", "vertical_pupils", "yellow_glow")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.9
            animation_style = "aggressive_darting"
            interaction_patterns = @("goblin_idle_glare", "goblin_active_tracking")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "goblin_eye_init"
            act_function = "goblin_eye_act"
            include_path = "src/components/goblin_eye.h"
        }
    }
    
    "cat_eye.json" = @{
        relative_filename = "components/creature_specific/cat_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "cat_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "CAT"
        description = "Cat-specific eye rendering component with feline characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "cat_fur_soft"
                color_scheme = "cat_green_gold"
                scale_factor = 1.0
                distinctive_features = @("vertical_pupils", "reflective_layer", "soft_blink")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.7
            animation_style = "smooth_tracking"
            interaction_patterns = @("cat_idle_slow_blink", "cat_active_hunting")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "cat_eye_init"
            act_function = "cat_eye_act"
            include_path = "src/components/cat_eye.h"
        }
    }
    
    "bear_eye.json" = @{
        relative_filename = "components/creature_specific/bear_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "bear_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "BEAR"
        description = "Bear-specific eye rendering component with ursine characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "bear_fur_thick"
                color_scheme = "bear_brown_black"
                scale_factor = 1.1
                distinctive_features = @("round_pupils", "kind_expression", "slow_movement")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.6
            animation_style = "gentle_tracking"
            interaction_patterns = @("bear_idle_calm", "bear_active_protective")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "bear_eye_init"
            act_function = "bear_eye_act"
            include_path = "src/components/bear_eye.h"
        }
    }
}

# Create missing hardware components
$createdHardware = 0
foreach ($filename in $HardwareDefinitions.Keys) {
    $definition = $HardwareDefinitions[$filename]
    $fullPath = Join-Path $ProjectRoot $definition.relative_filename
    
    if (-not (Test-Path $fullPath)) {
        if ($Verbose) { Write-Progress "Creating hardware component: $filename" }
        
        if (-not $DryRun) {
            $directory = Split-Path $fullPath -Parent
            if (-not (Test-Path $directory)) {
                New-Item -ItemType Directory -Path $directory -Force | Out-Null
            }
            
            $definition | ConvertTo-Json -Depth 20 | Set-Content -Path $fullPath -Encoding UTF8
        }
        $createdHardware++
    }
}

# Create missing creature components
$createdCreatures = 0
foreach ($filename in $CreatureComponents.Keys) {
    $definition = $CreatureComponents[$filename]
    $fullPath = Join-Path $ProjectRoot $definition.relative_filename
    
    if (-not (Test-Path $fullPath)) {
        if ($Verbose) { Write-Progress "Creating creature component: $filename" }
        
        if (-not $DryRun) {
            $directory = Split-Path $fullPath -Parent
            if (-not (Test-Path $directory)) {
                New-Item -ItemType Directory -Path $directory -Force | Out-Null
            }
            
            $definition | ConvertTo-Json -Depth 20 | Set-Content -Path $fullPath -Encoding UTF8
        }
        $createdCreatures++
    }
}

# Summary
$totalCreated = $createdHardware + $createdCreatures

Write-Info "="*70
Write-Info "Missing References Creation Complete"
if ($DryRun) {
    Write-Warning "DRY RUN MODE - No files were actually created"
} else {
    Write-Success "Created $totalCreated missing reference files"
}
Write-Info "  Hardware Components: $createdHardware"
Write-Info "  Creature Components: $createdCreatures"
Write-Info ""
Write-Info "Run repair_json_consistency.ps1 again to continue repairs"