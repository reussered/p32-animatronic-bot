#!/usr/bin/env pwsh
<#
.SYNOPSIS
Complete Missing Components Creator - Creates ALL missing hardware and creature components

.DESCRIPTION  
Creates all remaining missing files that are preventing JSON consistency:
- All missing hardware components 
- All missing creature-specific components
- Missing interface definitions

.PARAMETER ProjectRoot
Root directory for JSON file operations (default: "config")

.PARAMETER DryRun
Show what would be created without making files

.PARAMETER Verbose  
Show detailed operation information

.EXAMPLE
.\tools\complete_missing_components.ps1
.\tools\complete_missing_components.ps1 -DryRun -Verbose
#>

param(
    [string]$ProjectRoot = "config",
    [switch]$DryRun,
    [switch]$Verbose
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# Color functions for output
function Write-Success { param($Message) Write-Host "? $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "??  $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "? $Message" -ForegroundColor Red }
function Write-Info { param($Message) Write-Host "??  $Message" -ForegroundColor Cyan }
function Write-Progress { param($Message) Write-Host "? $Message" -ForegroundColor Blue }

Write-Info "P32 Animatronic Bot - Complete Missing Components Creator"
Write-Info "Target: $ProjectRoot | DryRun: $DryRun | Verbose: $Verbose"
Write-Info "="*70

# Extended Hardware Component Definitions
$HardwareDefinitions = @{
    "hc_sr04_sensor.json" = @{
        relative_filename = "components/hardware/hc_sr04_sensor.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "hc_sr04_sensor"
        hardware_type = "ULTRASONIC_SENSOR"
        description = "HC-SR04 ultrasonic distance sensor"
        specifications = @{
            sensing_range = "2-400cm"
            accuracy = "?3mm"
            measuring_angle = "15?"
            frequency = "40kHz"
            power_consumption = @{
                voltage = "5V"
                current_draw = "15mA"
                power_watts = "0.075W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 3.99
            availability = "COMMON"
            lead_time_days = 2
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
        }
        cost_performance = @{
            unit_cost_usd = 45.99
            availability = "COMMON"
            lead_time_days = 7
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "esp32_s3_wifi.json" = @{
        relative_filename = "components/hardware/esp32_s3_wifi.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "esp32_s3_wifi"
        hardware_type = "WIRELESS_CONTROLLER"
        description = "ESP32-S3 WiFi and Bluetooth controller"
        specifications = @{
            wifi_standard = "802.11 b/g/n"
            bluetooth = "Bluetooth LE 5.0"
            esp_now = "Supported"
            mesh_networking = "Supported"
            power_consumption = @{
                voltage = "3.3V"
                current_draw = "240mA"
                power_watts = "0.792W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 8.99
            availability = "COMMON"
            lead_time_days = 3
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
            operating_speed = "0.17sec/60?"
            power_consumption = @{
                voltage = "6V"
                current_draw = "900mA"
                power_watts = "5.4W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 18.99
            availability = "COMMON"
            lead_time_days = 5
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "directional_microphone.json" = @{
        relative_filename = "components/hardware/directional_microphone.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "directional_microphone"
        hardware_type = "AUDIO_INPUT"
        description = "Directional electret microphone for enhanced audio pickup"
        specifications = @{
            sensitivity = "-38dB"
            frequency_response = "20Hz-20kHz"
            directional_pattern = "Cardioid"
            snr = "58dB"
            power_consumption = @{
                voltage = "3.3V"
                current_draw = "0.5mA"
                power_watts = "0.00165W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 12.99
            availability = "COMMON"
            lead_time_days = 4
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "mechanical_actuator.json" = @{
        relative_filename = "components/hardware/mechanical_actuator.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "mechanical_actuator"
        hardware_type = "LINEAR_ACTUATOR"
        description = "Small linear actuator for mechanical animation"
        specifications = @{
            stroke_length = "20mm"
            force_capacity = "50N"
            speed = "5mm/s"
            control_signal = "PWM"
            power_consumption = @{
                voltage = "12V"
                current_draw = "300mA"
                power_watts = "3.6W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 35.99
            availability = "COMMON"
            lead_time_days = 7
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "led_matrix_small.json" = @{
        relative_filename = "components/hardware/led_matrix_small.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "led_matrix_small"
        hardware_type = "LED_MATRIX"
        description = "8x8 WS2812B LED matrix for decorative lighting"
        specifications = @{
            matrix_size = "8x8"
            led_type = "WS2812B"
            color_depth = "24-bit RGB"
            control_signal = "Digital"
            power_consumption = @{
                voltage = "5V"
                current_draw = "2A"
                power_watts = "10W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 22.99
            availability = "COMMON"
            lead_time_days = 5
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "ws2812b_strip.json" = @{
        relative_filename = "components/hardware/ws2812b_strip.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "ws2812b_strip"
        hardware_type = "LED_STRIP"
        description = "WS2812B addressable LED strip"
        specifications = @{
            led_count = 30
            led_density = "30/meter"
            color_depth = "24-bit RGB"
            control_signal = "Digital"
            power_consumption = @{
                voltage = "5V"
                current_draw = "1.8A"
                power_watts = "9W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 15.99
            availability = "COMMON"
            lead_time_days = 3
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "aesthetic_overlay.json" = @{
        relative_filename = "components/hardware/aesthetic_overlay.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "aesthetic_overlay"
        hardware_type = "DECORATIVE"
        description = "3D printed aesthetic overlay for character customization"
        specifications = @{
            material = "PLA"
            layer_height = "0.2mm"
            infill = "20%"
            mounting_system = "Snap-fit"
            power_consumption = @{
                voltage = "0V"
                current_draw = "0mA"
                power_watts = "0W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 5.99
            availability = "CUSTOM"
            lead_time_days = 2
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    "power_management_unit.json" = @{
        relative_filename = "components/hardware/power_management_unit.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "power_management_unit"
        hardware_type = "POWER_MANAGEMENT"
        description = "Battery management and power distribution system"
        specifications = @{
            input_voltage = "7-30V"
            output_voltages = @("5V", "3.3V")
            max_current = "5A"
            efficiency = "85%"
            power_consumption = @{
                voltage = "12V"
                current_draw = "50mA"
                power_watts = "0.6W"
            }
        }
        cost_performance = @{
            unit_cost_usd = 89.99
            availability = "COMMON"
            lead_time_days = 10
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
}

# Complete Creature Components Definitions  
$CreatureComponents = @{
    "goblin_eye.json" = @{
        relative_filename = "components/creature_specific/goblin_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "goblin_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "GOBLIN"
        description = "Goblin-specific eye rendering with menacing characteristics"
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
    
    "goblin_ear.json" = @{
        relative_filename = "components/creature_specific/goblin_ear.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "goblin_ear"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "GOBLIN"
        description = "Goblin-specific ear rendering with pointed characteristics"
        rendering = @{
            base_shape = "ear_pointed"
            creature_modifications = @{
                texture = "goblin_skin_warty"
                color_scheme = "goblin_green_brown"
                scale_factor = 1.4
                distinctive_features = @("pointed_tips", "tufted_hair", "veined_surface")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.7
            animation_style = "twitching_alert"
            interaction_patterns = @("goblin_idle_twitch", "goblin_active_swivel")
        }
        hardware_reference = "config/components/hardware/directional_microphone.json"
        software = @{
            init_function = "goblin_ear_init"
            act_function = "goblin_ear_act"
            include_path = "src/components/goblin_ear.h"
        }
    }
    
    "cat_eye.json" = @{
        relative_filename = "components/creature_specific/cat_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "cat_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "CAT"
        description = "Cat-specific eye rendering with feline characteristics"
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
        description = "Bear-specific eye rendering with ursine characteristics"
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
    
    "android_eye.json" = @{
        relative_filename = "components/creature_specific/android_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "android_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "ANDROID"
        description = "Android-specific eye rendering with robotic characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "android_metal_brushed"
                color_scheme = "android_blue_white"
                scale_factor = 0.9
                distinctive_features = @("led_ring", "digital_iris", "scanning_patterns")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.8
            animation_style = "precise_mechanical"
            interaction_patterns = @("android_idle_scan", "android_active_focus")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "android_eye_init"
            act_function = "android_eye_act"
            include_path = "src/components/android_eye.h"
        }
    }
    
    "cyclops_eye.json" = @{
        relative_filename = "components/creature_specific/cyclops_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "cyclops_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "CYCLOPS"
        description = "Cyclops-specific single large eye rendering"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "cyclops_skin_rough"
                color_scheme = "cyclops_red_orange"
                scale_factor = 1.8
                distinctive_features = @("massive_pupil", "burning_iris", "intimidating_stare")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.95
            animation_style = "intense_focus"
            interaction_patterns = @("cyclops_idle_glower", "cyclops_active_track")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "cyclops_eye_init"
            act_function = "cyclops_eye_act"
            include_path = "src/components/cyclops_eye.h"
        }
    }
    
    "pixie_eye.json" = @{
        relative_filename = "components/creature_specific/pixie_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "pixie_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "PIXIE"
        description = "Pixie-specific eye rendering with magical characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "pixie_skin_luminous"
                color_scheme = "pixie_purple_silver"
                scale_factor = 1.3
                distinctive_features = @("sparkle_effect", "large_pupils", "magical_glow")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.9
            animation_style = "playful_darting"
            interaction_patterns = @("pixie_idle_twinkle", "pixie_active_dance")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "pixie_eye_init"
            act_function = "pixie_eye_act"
            include_path = "src/components/pixie_eye.h"
        }
    }
    
    "robot_eye.json" = @{
        relative_filename = "components/creature_specific/robot_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "robot_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "ROBOT"
        description = "Robot-specific eye rendering with industrial characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "robot_metal_industrial"
                color_scheme = "robot_red_black"
                scale_factor = 0.8
                distinctive_features = @("targeting_reticle", "status_leds", "diagnostic_display")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.5
            animation_style = "systematic_scan"
            interaction_patterns = @("robot_idle_standby", "robot_active_target")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "robot_eye_init"
            act_function = "robot_eye_act"
            include_path = "src/components/robot_eye.h"
        }
    }
    
    "zombie_eye.json" = @{
        relative_filename = "components/creature_specific/zombie_eye.json"
        version = "1.0.0"
        author = "config/author.json"
        component_name = "zombie_eye"
        component_type = "CREATURE_SPECIFIC"
        creature_family = "ZOMBIE"
        description = "Zombie-specific eye rendering with undead characteristics"
        rendering = @{
            base_shape = "eye_basic"
            creature_modifications = @{
                texture = "zombie_skin_decayed"
                color_scheme = "zombie_white_grey"
                scale_factor = 1.1
                distinctive_features = @("clouded_iris", "bloodshot_veins", "vacant_stare")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.3
            animation_style = "sluggish_drift"
            interaction_patterns = @("zombie_idle_wander", "zombie_active_hunger")
        }
        hardware_reference = "config/components/hardware/gc9a01_display.json"
        software = @{
            init_function = "zombie_eye_init"
            act_function = "zombie_eye_act"
            include_path = "src/components/zombie_eye.h"
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
Write-Info "Complete Missing Components Creation Complete"
if ($DryRun) {
    Write-Warning "DRY RUN MODE - No files were actually created"
} else {
    Write-Success "Created $totalCreated missing reference files"
}
Write-Info "  Hardware Components: $createdHardware"
Write-Info "  Creature Components: $createdCreatures"
Write-Info ""
Write-Info "Run repair_json_consistency.ps1 again to continue repairs"