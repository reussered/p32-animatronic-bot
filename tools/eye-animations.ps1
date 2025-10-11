# P32 Animatronic Bot - Eye Display Animation Generator
# Creates visual animations for GC9A01 circular displays used as eyes

param(
    [string]$Character = "goblin",
    [string]$Emotion = "normal",
    [switch]$GenerateAll,
    [switch]$Demo
)

# Eye animation configurations for different characters
$EyeAnimations = @{
    goblin = @{
        normal = @{
            pupil_size = 0.6
            eye_openness = 0.8
            color = "#44ff44"
            blink_rate = 3000
            gaze_wander = $true
        }
        angry = @{
            pupil_size = 0.8
            eye_openness = 0.4
            color = "#ff4444"
            blink_rate = 1000
            gaze_intensity = "high"
        }
        curious = @{
            pupil_size = 0.9
            eye_openness = 1.0
            color = "#22ff22"
            blink_rate = 2000
            gaze_tracking = $true
        }
        fear = @{
            pupil_size = 0.3
            eye_openness = 1.0
            color = "#ffff44"
            blink_rate = 500
            darting_movement = $true
        }
        sleepy = @{
            pupil_size = 0.4
            eye_openness = 0.3
            color = "#4444aa"
            blink_rate = 8000
            droopy_lids = $true
        }
    }
}

# Animation pattern templates
$AnimationPatterns = @{
    blink = @{
        duration_ms = 300
        keyframes = @(
            @{ time_ms = 0; eye_openness = 0.8 },
            @{ time_ms = 100; eye_openness = 0.0 },
            @{ time_ms = 200; eye_openness = 0.0 },
            @{ time_ms = 300; eye_openness = 0.8 }
        )
    }
    
    wink = @{
        duration_ms = 500
        keyframes = @(
            @{ time_ms = 0; eye_openness = 0.8 },
            @{ time_ms = 150; eye_openness = 0.0 },
            @{ time_ms = 350; eye_openness = 0.0 },
            @{ time_ms = 500; eye_openness = 0.8 }
        )
    }
    
    angry_squint = @{
        duration_ms = 1500
        loop = $true
        keyframes = @(
            @{ time_ms = 0; eye_openness = 0.4; pupil_size = 0.6 },
            @{ time_ms = 750; eye_openness = 0.3; pupil_size = 0.8 },
            @{ time_ms = 1500; eye_openness = 0.4; pupil_size = 0.6 }
        )
    }
    
    look_around = @{
        duration_ms = 4000
        loop = $true
        keyframes = @(
            @{ time_ms = 0; gaze_x = 0.0; gaze_y = 0.0 },
            @{ time_ms = 1000; gaze_x = -0.3; gaze_y = 0.1 },
            @{ time_ms = 2000; gaze_x = 0.3; gaze_y = -0.1 },
            @{ time_ms = 3000; gaze_x = 0.0; gaze_y = 0.2 },
            @{ time_ms = 4000; gaze_x = 0.0; gaze_y = 0.0 }
        )
    }
    
    fear_darting = @{
        duration_ms = 2000
        loop = $true
        keyframes = @(
            @{ time_ms = 0; gaze_x = 0.0; gaze_y = 0.0; eye_openness = 1.0 },
            @{ time_ms = 200; gaze_x = -0.4; gaze_y = 0.2; eye_openness = 1.0 },
            @{ time_ms = 400; gaze_x = 0.4; gaze_y = -0.3; eye_openness = 0.9 },
            @{ time_ms = 600; gaze_x = -0.2; gaze_y = 0.4; eye_openness = 1.0 },
            @{ time_ms = 800; gaze_x = 0.3; gaze_y = 0.1; eye_openness = 0.8 },
            @{ time_ms = 1000; gaze_x = 0.0; gaze_y = 0.0; eye_openness = 1.0 }
        )
    }
}

function New-EyeAnimationJSON {
    param(
        [string]$AnimationName,
        [string]$Character,
        [hashtable]$Config,
        [hashtable]$Pattern
    )
    
    $animation = @{
        animation_name = "$Character" + "_eye_" + "$AnimationName"
        version = "1.0.0"
        author = "config/author.json"
        description = "Eye animation for $AnimationName expression"
        duration_ms = $Pattern.duration_ms
        loop = if ($Pattern.loop) { $Pattern.loop } else { $false }
        mood_trigger = $AnimationName.ToUpper()
        components = @()
    }
    
    # Generate left and right eye components
    foreach ($side in @("left", "right")) {
        $component = @{
            component_id = "$Character" + "_eye_" + "$side"
            keyframes = @()
        }
        
        foreach ($keyframe in $Pattern.keyframes) {
            $displayContent = @{
                type = "eye_expression"
                pupil_size = if ($keyframe.pupil_size) { $keyframe.pupil_size } else { $Config.pupil_size }
                eye_openness = if ($keyframe.eye_openness) { $keyframe.eye_openness } else { $Config.eye_openness }
                color = $Config.color
                expression = $AnimationName
            }
            
            # Add gaze tracking if specified
            if ($keyframe.gaze_x -or $keyframe.gaze_y) {
                $displayContent.gaze_x = if ($keyframe.gaze_x) { $keyframe.gaze_x } else { 0.0 }
                $displayContent.gaze_y = if ($keyframe.gaze_y) { $keyframe.gaze_y } else { 0.0 }
            }
            
            $component.keyframes += @{
                time_ms = $keyframe.time_ms
                display_content = $displayContent
            }
        }
        
        $animation.components += $component
    }
    
    return $animation
}

Write-Host "P32 Eye Display Animation Generator" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

if ($GenerateAll) {
    Write-Host "Generating all eye animations for all characters..." -ForegroundColor Green
    
    $totalAnimations = 0
    foreach ($char in $EyeAnimations.Keys) {
        Write-Host "`nGenerating $char eye animations..." -ForegroundColor Yellow
        
        foreach ($emotion in $EyeAnimations[$char].Keys) {
            foreach ($patternName in $AnimationPatterns.Keys) {
                $animationName = "$emotion" + "_" + "$patternName"
                $outputPath = "assets\animations\$char\eye_$animationName.json"
                
                New-Item -Path (Split-Path $outputPath) -ItemType Directory -Force | Out-Null
                
                $animation = New-EyeAnimationJSON -AnimationName $animationName -Character $char -Config $EyeAnimations[$char][$emotion] -Pattern $AnimationPatterns[$patternName]
                
                $animation | ConvertTo-Json -Depth 10 | Out-File $outputPath -Encoding UTF8
                Write-Host "Created: $animationName" -ForegroundColor Green
                $totalAnimations++
            }
        }
    }
    
    Write-Host "`nGenerated $totalAnimations eye animation files!" -ForegroundColor Green
} elseif ($Demo) {
    Write-Host "Generating demo eye animations..." -ForegroundColor Green
    
    # Generate key demo animations
    $demoAnimations = @(
        @{ character = "goblin"; emotion = "normal"; pattern = "blink" },
        @{ character = "goblin"; emotion = "angry"; pattern = "angry_squint" },
        @{ character = "goblin"; emotion = "curious"; pattern = "look_around" },
        @{ character = "goblin"; emotion = "fear"; pattern = "fear_darting" }
    )
    
    foreach ($demo in $demoAnimations) {
        $animationName = "$($demo.emotion)_$($demo.pattern)"
        $outputPath = "assets\animations\$($demo.character)\eye_$animationName.json"
        
        New-Item -Path (Split-Path $outputPath) -ItemType Directory -Force | Out-Null
        
        $config = $EyeAnimations[$demo.character][$demo.emotion]  
        $pattern = $AnimationPatterns[$demo.pattern]
        
        $animation = New-EyeAnimationJSON -AnimationName $animationName -Character $demo.character -Config $config -Pattern $pattern
        
        $animation | ConvertTo-Json -Depth 10 | Out-File $outputPath -Encoding UTF8
        Write-Host "Demo: $animationName" -ForegroundColor Green
    }
} else {
    Write-Host "`nUsage:" -ForegroundColor Green
    Write-Host "  Demo animations:     .\tools\eye-animations.ps1 -Demo"
    Write-Host "  All animations:      .\tools\eye-animations.ps1 -GenerateAll"
    Write-Host "`nSupported characters: $($EyeAnimations.Keys -join ', ')" -ForegroundColor Cyan
    Write-Host "Animation patterns: $($AnimationPatterns.Keys -join ', ')" -ForegroundColor Cyan
}