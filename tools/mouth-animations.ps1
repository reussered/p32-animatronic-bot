# P32 Animatronic Bot - Mouth Display Animation Generator  
# Creates grinding maw and mouth animations for GC9A01 circular displays

param(
    [string]$Character = "goblin",
    [string]$MouthType = "grinding_maw",
    [switch]$GenerateAll,
    [switch]$Demo
)

# Mouth animation configurations
$MouthAnimations = @{
    goblin = @{
        grinding_maw = @{
            base_color = "#882222"
            teeth_color = "#ffffff"
            gum_color = "#440000" 
            saliva_color = "#004400"
            animation_intensity = "high"
        }
        snarl = @{
            base_color = "#aa1111"
            teeth_color = "#ffffaa"
            lip_curl = "aggressive"
            fang_emphasis = $true
        }
        neutral = @{
            base_color = "#228822"
            teeth_visible = $false
            lip_position = "relaxed"
        }
        speaking = @{
            base_color = "#336633"
            teeth_visible = $true
            mouth_movement = "articulation"
        }
        evil_grin = @{
            base_color = "#114411"
            teeth_color = "#ffff88"
            corner_curl = "sinister"
            eye_coordination = $true
        }
    }
}

# Complex animation patterns for mouth expressions
$MouthPatterns = @{
    grinding_maw = @{
        duration_ms = 3000
        loop = $true
        description = "Menacing grinding jaw motion with visible teeth and drool"
        keyframes = @(
            @{ 
                time_ms = 0
                mouth_shape = "closed_menacing"
                teeth_visible = $false
                jaw_position = 0.0
                drool_amount = 0.1
            },
            @{ 
                time_ms = 500
                mouth_shape = "slight_open"
                teeth_visible = $true
                jaw_position = 0.3
                drool_amount = 0.2
                grinding_motion = "left"
            },
            @{ 
                time_ms = 1000
                mouth_shape = "wide_open"
                teeth_visible = $true
                jaw_position = 0.8
                drool_amount = 0.4
                grinding_motion = "center"
                fang_glint = $true
            },
            @{ 
                time_ms = 1500
                mouth_shape = "grinding_right" 
                teeth_visible = $true
                jaw_position = 0.6
                drool_amount = 0.5
                grinding_motion = "right"
            },
            @{ 
                time_ms = 2000
                mouth_shape = "chomping"
                teeth_visible = $true
                jaw_position = 0.2
                drool_amount = 0.3
                grinding_motion = "clamp"
            },
            @{ 
                time_ms = 2500
                mouth_shape = "snarl_left"
                teeth_visible = $true  
                jaw_position = 0.4
                drool_amount = 0.2
                grinding_motion = "left"
            },
            @{ 
                time_ms = 3000
                mouth_shape = "closed_menacing"
                teeth_visible = $false
                jaw_position = 0.0
                drool_amount = 0.1
            }
        )
    }
    
    evil_chuckle = @{
        duration_ms = 2000
        loop = $false
        description = "Sinister chuckling motion"
        keyframes = @(
            @{ time_ms = 0; mouth_shape = "closed_grin"; teeth_visible = $false },
            @{ time_ms = 300; mouth_shape = "slight_open"; teeth_visible = $true },
            @{ time_ms = 600; mouth_shape = "wide_grin"; teeth_visible = $true; corner_curl = "high" },
            @{ time_ms = 900; mouth_shape = "chuckle_open"; teeth_visible = $true },
            @{ time_ms = 1200; mouth_shape = "wide_grin"; teeth_visible = $true },
            @{ time_ms = 1500; mouth_shape = "slight_open"; teeth_visible = $true },
            @{ time_ms = 2000; mouth_shape = "evil_smile"; teeth_visible = $true }
        )
    }
    
    snarl_threat = @{
        duration_ms = 1500
        loop = $true
        description = "Aggressive threatening snarl"
        keyframes = @(
            @{ time_ms = 0; mouth_shape = "closed_angry"; teeth_visible = $false },
            @{ time_ms = 200; mouth_shape = "lip_curl_left"; teeth_visible = $true; fang_show = "left" },
            @{ time_ms = 500; mouth_shape = "full_snarl"; teeth_visible = $true; fang_show = "both" },
            @{ time_ms = 800; mouth_shape = "lip_curl_right"; teeth_visible = $true; fang_show = "right" },
            @{ time_ms = 1200; mouth_shape = "aggressive_open"; teeth_visible = $true },
            @{ time_ms = 1500; mouth_shape = "closed_angry"; teeth_visible = $false }
        )
    }
    
    speaking_mouth = @{
        duration_ms = 1000
        loop = $false
        description = "Natural speaking mouth movements"
        keyframes = @(
            @{ time_ms = 0; mouth_shape = "closed_neutral"; teeth_visible = $false },
            @{ time_ms = 100; mouth_shape = "vowel_a"; teeth_visible = $false },
            @{ time_ms = 200; mouth_shape = "consonant_m"; teeth_visible = $false },
            @{ time_ms = 350; mouth_shape = "vowel_o"; teeth_visible = $false },
            @{ time_ms = 500; mouth_shape = "consonant_th"; teeth_visible = $true },
            @{ time_ms = 650; mouth_shape = "vowel_e"; teeth_visible = $false },
            @{ time_ms = 800; mouth_shape = "consonant_r"; teeth_visible = $false },
            @{ time_ms = 1000; mouth_shape = "closed_neutral"; teeth_visible = $false }
        )
    }
    
    idle_breathing = @{
        duration_ms = 4000
        loop = $true
        description = "Subtle breathing motion for idle state"
        keyframes = @(
            @{ time_ms = 0; mouth_shape = "closed_relaxed"; breath_phase = "inhale_start" },
            @{ time_ms = 1000; mouth_shape = "slightly_parted"; breath_phase = "inhale_peak" },
            @{ time_ms = 2000; mouth_shape = "closed_relaxed"; breath_phase = "hold" },
            @{ time_ms = 3000; mouth_shape = "barely_open"; breath_phase = "exhale" },
            @{ time_ms = 4000; mouth_shape = "closed_relaxed"; breath_phase = "rest" }
        )
    }
}

function New-MouthAnimationJSON {
    param(
        [string]$AnimationName,
        [string]$Character,
        [hashtable]$Config,
        [hashtable]$Pattern
    )
    
    $animation = @{
        animation_name = "$Character" + "_mouth_" + "$AnimationName"
        version = "1.0.0"
        author = "config/author.json"
        description = $Pattern.description
        duration_ms = $Pattern.duration_ms
        loop = $Pattern.loop
        mood_trigger = $AnimationName.ToUpper().Replace("_", "")
        components = @()
    }
    
    # Generate mouth component animation
    $component = @{
        component_id = "$Character" + "_mouth"
        keyframes = @()
    }
    
    foreach ($keyframe in $Pattern.keyframes) {
        $displayContent = @{
            type = "mouth_expression"
            mouth_shape = $keyframe.mouth_shape
            teeth_visible = if ($keyframe.teeth_visible -ne $null) { $keyframe.teeth_visible } else { $false }
            color = $Config.base_color
            expression = $AnimationName
        }
        
        # Add specialized mouth animation properties
        if ($keyframe.jaw_position) { $displayContent.jaw_position = $keyframe.jaw_position }
        if ($keyframe.drool_amount) { $displayContent.drool_amount = $keyframe.drool_amount }
        if ($keyframe.grinding_motion) { $displayContent.grinding_motion = $keyframe.grinding_motion }
        if ($keyframe.fang_glint) { $displayContent.fang_glint = $keyframe.fang_glint }
        if ($keyframe.fang_show) { $displayContent.fang_show = $keyframe.fang_show }
        if ($keyframe.corner_curl) { $displayContent.corner_curl = $keyframe.corner_curl }
        if ($keyframe.breath_phase) { $displayContent.breath_phase = $keyframe.breath_phase }
        
        # Add color variations
        if ($Config.teeth_color -and $keyframe.teeth_visible) { 
            $displayContent.teeth_color = $Config.teeth_color 
        }
        if ($Config.gum_color) { $displayContent.gum_color = $Config.gum_color }
        if ($Config.saliva_color -and $keyframe.drool_amount) { 
            $displayContent.saliva_color = $Config.saliva_color 
        }
        
        $component.keyframes += @{
            time_ms = $keyframe.time_ms
            display_content = $displayContent
        }
    }
    
    $animation.components += $component
    return $animation
}

Write-Host "P32 Mouth Display Animation Generator" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan

if ($GenerateAll) {
    Write-Host "Generating all mouth animations..." -ForegroundColor Green
    
    $totalAnimations = 0
    foreach ($char in $MouthAnimations.Keys) {
        Write-Host "`nGenerating $char mouth animations..." -ForegroundColor Yellow
        
        foreach ($mouthType in $MouthAnimations[$char].Keys) {
            foreach ($patternName in $MouthPatterns.Keys) {
                $animationName = "$mouthType" + "_" + "$patternName"
                $outputPath = "assets\animations\$char\mouth_$animationName.json"
                
                New-Item -Path (Split-Path $outputPath) -ItemType Directory -Force | Out-Null
                
                $animation = New-MouthAnimationJSON -AnimationName $animationName -Character $char -Config $MouthAnimations[$char][$mouthType] -Pattern $MouthPatterns[$patternName]
                
                $animation | ConvertTo-Json -Depth 10 | Out-File $outputPath -Encoding UTF8
                Write-Host "Created: $animationName" -ForegroundColor Green
                $totalAnimations++
            }
        }
    }
    
    Write-Host "`nGenerated $totalAnimations mouth animation files!" -ForegroundColor Green
} elseif ($Demo) {
    Write-Host "Generating demo mouth animations..." -ForegroundColor Green
    
    # Generate key demo animations including GRINDING MAW
    $demoAnimations = @(
        @{ character = "goblin"; mouth_type = "grinding_maw"; pattern = "grinding_maw" },
        @{ character = "goblin"; mouth_type = "snarl"; pattern = "snarl_threat" },
        @{ character = "goblin"; mouth_type = "evil_grin"; pattern = "evil_chuckle" },
        @{ character = "goblin"; mouth_type = "speaking"; pattern = "speaking_mouth" },
        @{ character = "goblin"; mouth_type = "neutral"; pattern = "idle_breathing" }
    )
    
    foreach ($demo in $demoAnimations) {
        $animationName = "$($demo.mouth_type)_$($demo.pattern)"
        $outputPath = "assets\animations\$($demo.character)\mouth_$animationName.json"
        
        New-Item -Path (Split-Path $outputPath) -ItemType Directory -Force | Out-Null
        
        $config = $MouthAnimations[$demo.character][$demo.mouth_type]
        $pattern = $MouthPatterns[$demo.pattern]
        
        $animation = New-MouthAnimationJSON -AnimationName $animationName -Character $demo.character -Config $config -Pattern $pattern
        
        $animation | ConvertTo-Json -Depth 10 | Out-File $outputPath -Encoding UTF8
        Write-Host "Demo: $animationName" -ForegroundColor Green
    }
} else {
    Write-Host "`nUsage:" -ForegroundColor Green
    Write-Host "  Demo animations:     .\tools\mouth-animations.ps1 -Demo"  
    Write-Host "  All animations:      .\tools\mouth-animations.ps1 -GenerateAll"
    Write-Host "`nSupported characters: $($MouthAnimations.Keys -join ', ')" -ForegroundColor Cyan
    Write-Host "Animation patterns: $($MouthPatterns.Keys -join ', ')" -ForegroundColor Cyan
    Write-Host "`nSpecial Features:" -ForegroundColor Yellow
    Write-Host "  GRINDING MAW - Menacing jaw motion with drool and teeth grinding" -ForegroundColor Red
    Write-Host "  SNARL THREAT - Aggressive lip curling with fang display" -ForegroundColor Red  
    Write-Host "  EVIL CHUCKLE - Sinister laughing mouth movement" -ForegroundColor Red
}