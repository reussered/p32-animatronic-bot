# P32 Creature Variant Generator - Quadruple existing creatures
# Creates 3 additional variants for each existing creature

param(
    [string]$SourceDir = "config\bots\bot_families",
    [switch]$DryRun = $false
)

Write-Host "=== P32 CREATURE VARIANT GENERATOR ===" -ForegroundColor Cyan

# Variant naming patterns
$variantSuffixes = @(
    @{ Name = "alpha"; Description = "Enhanced strength and aggression"; MoodMod = @{ ANGER = 1.5; FEAR = 0.7 } }
    @{ Name = "stealth"; Description = "Silent and cunning variant"; MoodMod = @{ CURIOSITY = 1.3; IRRITATION = 0.8 } }
    @{ Name = "guardian"; Description = "Protective and loyal variant"; MoodMod = @{ AFFECTION = 1.4; CONTENTMENT = 1.2 } }
)

# Size/scale variants
$scaleVariants = @(
    @{ Name = "mini"; Scale = 0.6; Description = "Compact 60% scale version" }
    @{ Name = "giant"; Scale = 1.8; Description = "Oversized 180% scale version" }
)

function Create-CreatureVariant($SourcePath, $VariantType, $VariantData) {
    $sourceContent = Get-Content $SourcePath -Raw | ConvertFrom-Json
    
    # Generate new variant filename
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($SourcePath)
    $directory = [System.IO.Path]::GetDirectoryName($SourcePath)
    $variantName = "${baseName}_$($VariantData.Name)"
    $variantPath = Join-Path $directory "$variantName.json"
    
    # Skip if variant already exists
    if (Test-Path $variantPath) {
        Write-Host "  Variant already exists: $variantName" -ForegroundColor Yellow
        return
    }
    
    # Clone and modify source creature
    $variant = $sourceContent | ConvertTo-Json -Depth 10 | ConvertFrom-Json
    
    # Update basic info
    $variant.relative_filename = $variantPath -replace '^.*\\config\\', 'config/'
    $variant.bot_id = $variantName.ToUpper() -replace '_', '_'
    $variant.description = "$($sourceContent.description) - $($VariantData.Description)"
    $variant.created = Get-Date -Format "yyyy-MM-dd"
    $variant.variant_type = $VariantData.Name
    
    # Apply mood modifications
    if ($VariantData.MoodMod -and $variant.mood_defaults) {
        foreach ($mood in $VariantData.MoodMod.Keys) {
            if ($variant.mood_defaults.$mood) {
                $variant.mood_defaults.$mood = [int]($variant.mood_defaults.$mood * $VariantData.MoodMod[$mood])
            }
        }
    }
    
    # Apply scale modifications
    if ($VariantData.Scale -and $variant.horizontal_scale) {
        $variant.horizontal_scale = [string]([float]$variant.horizontal_scale * $VariantData.Scale)
        $variant.vertical_scale = [string]([float]$variant.vertical_scale * $VariantData.Scale)
    }
    
    # Save variant
    if (-not $DryRun) {
        $variant | ConvertTo-Json -Depth 10 | Out-File -FilePath $variantPath -Encoding UTF8
        Write-Host "  Created: $variantName" -ForegroundColor Green
    } else {
        Write-Host "  [DRY RUN] Would create: $variantName" -ForegroundColor Yellow
    }
}

# Process all existing creatures
$totalCreated = 0
Get-ChildItem $SourceDir -Recurse -Filter "*.json" | Where-Object { 
    $_.Name -notlike "*_family.json" -and $_.Name -notlike "*_alpha.json" -and $_.Name -notlike "*_stealth.json" -and $_.Name -notlike "*_guardian.json" -and $_.Name -notlike "*_mini.json" -and $_.Name -notlike "*_giant.json"
} | ForEach-Object {
    $sourceFile = $_.FullName
    $creatureName = $_.BaseName
    
    Write-Host "`nProcessing: $creatureName" -ForegroundColor White
    
    # Create 3 personality variants
    foreach ($variant in $variantSuffixes) {
        Create-CreatureVariant $sourceFile "personality" $variant
        $totalCreated++
    }
}

Write-Host "`n=== VARIANT GENERATION COMPLETE ===" -ForegroundColor Magenta
Write-Host "Total variants created: $totalCreated" -ForegroundColor Green
Write-Host "Estimated new creature count: $($totalCreated + 55)" -ForegroundColor Yellow

if ($DryRun) {
    Write-Host "`nRun without -DryRun to actually create the files." -ForegroundColor Cyan
}