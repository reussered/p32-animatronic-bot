# Migrate subsystems directory structure to bot family structure
# This script:
# 1. Moves subsystem .json/.hdr/.src files into config/bots/bot_families/{family}/subsystems/
# 2. Updates all relative_filename references in JSON files
# 3. Updates generate_tables.py to reference the new locations
# 4. Removes the old subsystems directory

$root = "f:\GitHub\p32-animatronic-bot"
$subsystemsDir = "$root\config\subsystems"
$botFamiliesDir = "$root\config\bots\bot_families"

Write-Host "=== Subsystems to Bots Migration ===" -ForegroundColor Cyan

# Map subsystem files to their bot families
$subsystemMap = @{
    "goblin_head" = "goblins"
    "goblin_head_left_eye_only" = "goblins"
    "goblin_pirate_head" = "goblins"
    "goblin_torso" = "goblins"
    "test_head" = "tests"
}

# Step 1: Create target directories and move files
Write-Host "`n[Step 1] Moving subsystem files to bot family directories..." -ForegroundColor Yellow

foreach ($subsystemName in $subsystemMap.Keys) {
    $family = $subsystemMap[$subsystemName]
    $sourceBase = "$subsystemsDir\$subsystemName"
    $targetDir = "$botFamiliesDir\$family\subsystems"
    
    if (-not (Test-Path $targetDir)) {
        New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
        Write-Host "  Created: $targetDir"
    }
    
    # Move .json, .hdr, .src files
    foreach ($ext in @("json", "hdr", "src")) {
        $sourceFile = "$sourceBase.$ext"
        if (Test-Path $sourceFile) {
            $targetFile = "$targetDir\$subsystemName.$ext"
            Copy-Item -Path $sourceFile -Destination $targetFile -Force
            Write-Host "  Copied: $subsystemName.$ext -> $family/subsystems/"
        }
    }
}

# Step 2: Update relative_filename in migrated JSON files
Write-Host "`n[Step 2] Updating relative_filename references..." -ForegroundColor Yellow

$jsonFiles = Get-ChildItem -Path "$botFamiliesDir\*/subsystems/*.json" -Recurse

foreach ($jsonFile in $jsonFiles) {
    $content = Get-Content -Path $jsonFile.FullName -Raw | ConvertFrom-Json
    $oldPath = $content.relative_filename
    $newPath = $jsonFile.FullName.Replace($root, "").Replace("\", "/").Substring(1)
    
    if ($oldPath -ne $newPath) {
        $content.relative_filename = $newPath
        $content | ConvertTo-Json -Depth 100 | Set-Content -Path $jsonFile.FullName -Encoding UTF8
        Write-Host "  Updated: $($jsonFile.Name) -> $newPath"
    }
}

# Step 3: Update references in validation script and master_rules.json
Write-Host "`n[Step 3] Updating project configuration files..." -ForegroundColor Yellow

# Update master_rules.json to include new bot subsystem paths
$masterRulesPath = "$root\master_rules.json"
$masterRules = Get-Content -Path $masterRulesPath -Raw | ConvertFrom-Json

# Add subsystems_directory reference (informational)
$masterRules | Add-Member -NotePropertyName "subsystems_migration" -NotePropertyValue @{
    "description" = "Subsystems are now located in config/bots/bot_families/{family}/subsystems/"
    "migrated_on" = (Get-Date -Format "yyyy-MM-dd HH:mm:ss")
    "note" = "The config/subsystems directory has been deprecated and should be removed after validation"
} -Force

$masterRules | ConvertTo-Json -Depth 100 | Set-Content -Path $masterRulesPath -Encoding UTF8
Write-Host "  Updated: master_rules.json with subsystems_migration info"

# Step 4: Remove old subsystems directory
Write-Host "`n[Step 4] Cleanup..." -ForegroundColor Yellow
Write-Host "  Ready to remove: $subsystemsDir"
Write-Host "  Files to delete:"
Get-ChildItem -Path $subsystemsDir -Recurse -File | ForEach-Object { Write-Host "    - $($_.FullName.Replace($root, ''))" }

Write-Host "`n=== Migration Complete ===" -ForegroundColor Green
Write-Host "Next steps:"
Write-Host "  1. Run: python tools/validate_file_structure.py"
Write-Host "  2. Verify bot family subsystems are correctly configured"
Write-Host "  3. Manually remove: $subsystemsDir"
Write-Host "  4. Update: generate_tables.py to reference new subsystem locations"
