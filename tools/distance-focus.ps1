# P32 Distance-Based Eye Focusing Animation Generator
# Creates eye animations that focus based on HC-SR04 proximity sensor data

param(
    [string]$Character = "goblin",
    [switch]$GenerateAll,
    [switch]$Demo
)

# Distance-based eye focusing configurations
$EyeFocusConfig = @{
    goblin = @{
        eye_spacing_inches = 2.1  # Distance between eye centers
        convergence_ranges = @{
            # Very close - maximum convergence (cross-eyed)
            very_close = @{
                distance_cm_min = 0
                distance_cm_max = 30  
                left_eye_angle = 25    # Degrees inward
                right_eye_angle = -25  # Degrees inward
                pupil_size = 0.9       # Large pupils for close focus
                expression = "cross_eyed_intense"
            }
            # Close - strong convergence
            close = @{
                distance_cm_min = 30
                distance_cm_max = 60
                left_eye_angle = 15
                right_eye_angle = -15
                pupil_size = 0.8
                expression = "focused_close"
            }
            # Medium - moderate convergence  
            medium = @{
                distance_cm_min = 60
                distance_cm_max = 120
                left_eye_angle = 8
                right_eye_angle = -8
                pupil_size = 0.7
                expression = "attentive"
            }
            # Far - slight convergence
            far = @{
                distance_cm_min = 120
                distance_cm_max = 200
                left_eye_angle = 3
                right_eye_angle = -3
                pupil_size = 0.6
                expression = "distant_gaze"
            }
            # Very far - parallel gaze
            very_far = @{
                distance_cm_min = 200
                distance_cm_max = 400
                left_eye_angle = 0
                right_eye_angle = 0
                pupil_size = 0.5
                expression = "parallel_stare"
            }
        }
        
        # Smooth transition parameters
        transition_speed_ms = 500
        focus_tracking_enabled = $true
        blink_rate_modifier = @{
            very_close = 0.5  # Blink less when focused close
            close = 0.7
            medium = 1.0      # Normal blink rate
            far = 1.2
            very_far = 1.5    # Blink more when looking far
        }
    }
}

# Animation templates for distance-based focusing
$DistanceFocusPatterns = @{
    distance_transition = @{
        description = "Smooth transition between distance focus ranges"
        duration_ms = 800
        keyframes = @(
            @{ time_ms = 0; transition_phase = "start" },
            @{ time_ms = 200; transition_phase = "pupil_adjust" },
            @{ time_ms = 500; transition_phase = "angle_adjust" },
            @{ time_ms = 800; transition_phase = "settle" }
        )
    }
    
    focus_lock = @{
        description = "Lock focus on detected object with micro-adjustments"
        duration_ms = 2000
        loop = $true
        keyframes = @(
            @{ time_ms = 0; focus_intensity = 1.0; micro_adjust = 0.0 },
            @{ time_ms = 500; focus_intensity = 1.1; micro_adjust = 0.02 },
            @{ time_ms = 1000; focus_intensity = 1.0; micro_adjust = -0.01 },
            @{ time_ms = 1500; focus_intensity = 1.05; micro_adjust = 0.01 },
            @{ time_ms = 2000; focus_intensity = 1.0; micro_adjust = 0.0 }
        )
    }
    
    proximity_startle = @{
        description = "Startle response when object gets too close"
        duration_ms = 1200
        loop = $false
        keyframes = @(
            @{ time_ms = 0; eye_openness = 0.8; convergence = "normal" },
            @{ time_ms = 100; eye_openness = 1.0; convergence = "extreme"; startle = $true },
            @{ time_ms = 300; eye_openness = 0.6; convergence = "extreme" },
            @{ time_ms = 600; eye_openness = 0.9; convergence = "high" },
            @{ time_ms = 1200; eye_openness = 0.8; convergence = "normal" }
        )
    }
    
    tracking_search = @{
        description = "Search pattern when object moves or disappears"
        duration_ms = 3000
        loop = $true
        keyframes = @(
            @{ time_ms = 0; search_angle = 0; focus_confidence = 0.8 },
            @{ time_ms = 600; search_angle = -15; focus_confidence = 0.4 },
            @{ time_ms = 1200; search_angle = 15; focus_confidence = 0.3 },
            @{ time_ms = 1800; search_angle = 0; focus_confidence = 0.6 },
            @{ time_ms = 2400; search_angle = -8; focus_confidence = 0.5 },
            @{ time_ms = 3000; search_angle = 0; focus_confidence = 0.8 }
        )
    }
}

function New-DistanceFocusAnimation {
    param(
        [string]$Character,
        [string]$DistanceRange,
        [hashtable]$FocusConfig,
        [hashtable]$RangeConfig
    )
    
    $animation = @{
        animation_name = "$Character" + "_eyes_focus_" + "$DistanceRange"
        version = "1.0.0"
        author = "config/author.json"
        description = "Eye focusing for $DistanceRange distance range ($($RangeConfig.distance_cm_min)-$($RangeConfig.distance_cm_max)cm)"
        duration_ms = $FocusConfig.transition_speed_ms
        loop = $false
        trigger_type = "proximity_sensor"
        trigger_conditions = @{
            distance_min_cm = $RangeConfig.distance_cm_min
            distance_max_cm = $RangeConfig.distance_cm_max
            sensor_component = "goblin_nose"
        }
        components = @()
    }
    
    # Left eye component
    $leftEye = @{
        component_id = "$Character" + "_eye_left"
        keyframes = @(
            @{
                time_ms = 0
                display_content = @{
                    type = "eye_expression"
                    pupil_size = $RangeConfig.pupil_size
                    eye_openness = 0.8
                    color = "#44ff44"
                    expression = $RangeConfig.expression
                    gaze_angle_horizontal = $RangeConfig.left_eye_angle
                    focus_distance_cm = ($RangeConfig.distance_cm_min + $RangeConfig.distance_cm_max) / 2
                }
            }
        )
    }
    
    # Right eye component  
    $rightEye = @{
        component_id = "$Character" + "_eye_right"
        keyframes = @(
            @{
                time_ms = 0
                display_content = @{
                    type = "eye_expression"
                    pupil_size = $RangeConfig.pupil_size
                    eye_openness = 0.8
                    color = "#44ff44"
                    expression = $RangeConfig.expression
                    gaze_angle_horizontal = $RangeConfig.right_eye_angle
                    focus_distance_cm = ($RangeConfig.distance_cm_min + $RangeConfig.distance_cm_max) / 2
                }
            }
        )
    }
    
    $animation.components += $leftEye, $rightEye
    return $animation
}

Write-Host "P32 Distance-Based Eye Focusing Generator" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

if ($Demo) {
    Write-Host "Generating distance-based eye focusing animations..." -ForegroundColor Green
    
    $config = $EyeFocusConfig[$Character]
    $generatedCount = 0
    
    foreach ($rangeName in $config.convergence_ranges.Keys) {
        $rangeConfig = $config.convergence_ranges[$rangeName]
        
        $outputPath = "assets\animations\$Character\eyes_focus_$rangeName.json"
        New-Item -Path (Split-Path $outputPath) -ItemType Directory -Force | Out-Null
        
        $animation = New-DistanceFocusAnimation -Character $Character -DistanceRange $rangeName -FocusConfig $config -RangeConfig $rangeConfig
        
        $animation | ConvertTo-Json -Depth 10 | Out-File $outputPath -Encoding UTF8
        Write-Host "Created: eyes_focus_$rangeName ($($rangeConfig.distance_cm_min)-$($rangeConfig.distance_cm_max)cm)" -ForegroundColor Green
        $generatedCount++
    }
    
    Write-Host "`nGenerated $generatedCount distance-based eye focus animations!" -ForegroundColor Green
    Write-Host "`nIntegration Notes:" -ForegroundColor Yellow
    Write-Host "- Animations triggered by HC-SR04 sensor data from goblin_nose component" -ForegroundColor White
    Write-Host "- Eye convergence angles: 0° (parallel) to ±25° (cross-eyed)" -ForegroundColor White  
    Write-Host "- Pupil size varies: 0.5 (far) to 0.9 (very close)" -ForegroundColor White
    Write-Host "- Smooth transitions between distance ranges" -ForegroundColor White
} else {
    Write-Host "`nUsage:" -ForegroundColor Green
    Write-Host "  Demo focus animations: .\tools\distance-focus.ps1 -Demo"
    Write-Host "`nDistance Ranges:" -ForegroundColor Cyan
    
    $config = $EyeFocusConfig[$Character]
    foreach ($rangeName in $config.convergence_ranges.Keys) {
        $range = $config.convergence_ranges[$rangeName]
        Write-Host "  $rangeName`: $($range.distance_cm_min)-$($range.distance_cm_max)cm (±$($range.left_eye_angle)°)" -ForegroundColor White
    }
}