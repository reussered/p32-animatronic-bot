# P32 MEGA CREATURE EXPANSION - Master Script
# Orchestrates the complete expansion from 55 to 440 creatures

param(
    [switch]$DryRun = $false,
    [switch]$VariantsOnly = $false,
    [switch]$NewCategoriesOnly = $false
)

Write-Host "=== P32 MEGA CREATURE EXPANSION ===" -ForegroundColor Magenta
Write-Host "Target: 55  440 creatures (8x expansion)" -ForegroundColor Yellow

# Get current count
$currentCount = (Get-ChildItem "config\bots\bot_families" -Recurse -Name "*.json" | Where-Object { $_ -notlike "*_family.json" } | Measure-Object).Count
Write-Host "Current creature count: $currentCount" -ForegroundColor Green

if (-not $NewCategoriesOnly) {
    Write-Host "`n=== PHASE 1: QUADRUPLE EXISTING CREATURES ===" -ForegroundColor Cyan
    Write-Host "Creating 3 variants for each existing creature..." -ForegroundColor White
    
    if ($DryRun) {
        & ".\tools\create-creature-variants.ps1" -DryRun
    } else {
        & ".\tools\create-creature-variants.ps1"
    }
    
    # Count after variants
    $afterVariants = (Get-ChildItem "config\bots\bot_families" -Recurse -Name "*.json" | Where-Object { $_ -notlike "*_family.json" } | Measure-Object).Count
    Write-Host "After variants: $afterVariants creatures" -ForegroundColor Green
}

if (-not $VariantsOnly) {
    Write-Host "`n=== PHASE 2: ADD NEW CREATURE CATEGORIES ===" -ForegroundColor Cyan  
    Write-Host "Creating new aquatic, insects, birds, reptiles, mythical, elemental categories..." -ForegroundColor White
    
    if ($DryRun) {
        & ".\tools\create-new-categories.ps1" -DryRun
    } else {
        & ".\tools\create-new-categories.ps1"
    }
    
    # Final count
    $finalCount = (Get-ChildItem "config\bots\bot_families" -Recurse -Name "*.json" | Where-Object { $_ -notlike "*_family.json" } | Measure-Object).Count
    Write-Host "Final creature count: $finalCount creatures" -ForegroundColor Green
}

Write-Host "`n=== EXPANSION SUMMARY ===" -ForegroundColor Magenta

if (-not $DryRun) {
    $newCount = (Get-ChildItem "config\bots\bot_families" -Recurse -Name "*.json" | Where-Object { $_ -notlike "*_family.json" } | Measure-Object).Count
    $increase = $newCount - $currentCount
    $multiplier = [math]::Round($newCount / $currentCount, 1)
    
    Write-Host "Starting creatures: $currentCount" -ForegroundColor White
    Write-Host "Final creatures: $newCount" -ForegroundColor Green
    Write-Host "Increase: +$increase creatures" -ForegroundColor Yellow
    Write-Host "Expansion factor: ${multiplier}x" -ForegroundColor Cyan
    
    if ($newCount -ge 400) {
        Write-Host " TARGET ACHIEVED! 400+ creatures created!" -ForegroundColor Magenta
    } elseif ($newCount -ge 300) {
        Write-Host " EXCELLENT PROGRESS! 300+ creatures created!" -ForegroundColor Green
    } else {
        Write-Host " Good progress, consider running additional expansions." -ForegroundColor Yellow
    }
} else {
    Write-Host "[DRY RUN] No files were created. Run without -DryRun to execute." -ForegroundColor Yellow
}

Write-Host "`n=== NEXT STEPS ===" -ForegroundColor Cyan
Write-Host "1. Generate family templates for new categories" -ForegroundColor White
Write-Host "2. Create positioned components for new creatures" -ForegroundColor White  
Write-Host "3. Generate animation assets" -ForegroundColor White
Write-Host "4. Update component tables and build system" -ForegroundColor White
Write-Host "5. Commit the expanded creature ecosystem" -ForegroundColor White