# Rename all *_components fields to "components" (except family_components)
# This script will update all JSON files in the config directory

$ConfigPath = "f:\GitHub\p32-animatronic-bot\config"
$FilesChanged = 0

# Get all JSON files recursively
$JsonFiles = Get-ChildItem -Path $ConfigPath -Recurse -Filter "*.json"

foreach ($file in $JsonFiles) {
    $content = Get-Content $file.FullName -Raw
    $originalContent = $content

    # Replace various *_components fields with "components" (but not family_components)
    $content = $content -replace '"specialized_components"', '"components"'
    $content = $content -replace '"default_components"', '"components"'
    $content = $content -replace '"additional_components"', '"components"'
    $content = $content -replace '"common_components"', '"components"'
    $content = $content -replace '"optional_components"', '"components"'
    $content = $content -replace '"sensor_components"', '"components"'
    $content = $content -replace '"skeletal_power_components"', '"components"'
    $content = $content -replace '"mood_components"', '"components"'
    $content = $content -replace '"gpio_expansion_using_existing_components"', '"components"'
    $content = $content -replace '"internal_components"', '"components"'
    $content = $content -replace '"3d_printed_components"', '"components"'
    $content = $content -replace '"filtering_and_components"', '"components"'
    $content = $content -replace '"electrical_components"', '"components"'
    $content = $content -replace '"leveraging_existing_components"', '"components"'
    $content = $content -replace '"contained_components"', '"components"'
    $content = $content -replace '"includes_components"', '"components"'
    $content = $content -replace '"positioned_components"', '"components"'
    $content = $content -replace '"subsystem_components"', '"components"'
    $content = $content -replace '"interface_components"', '"components"'
    $content = $content -replace '"hardware_components"', '"components"'
    $content = $content -replace '"functional_components"', '"components"'
    $content = $content -replace '"scaling_components"', '"components"'
    $content = $content -replace '"creature_components"', '"components"'
    $content = $content -replace '"manufacturing_components"', '"components"'
    $content = $content -replace '"template_components"', '"components"'
    $content = $content -replace '"validation_components"', '"components"'
    $content = $content -replace '"resolved_components"', '"components"'

    # Check if content changed
    if ($content -ne $originalContent) {
        $content | Set-Content -Path $file.FullName -Encoding UTF8
        $FilesChanged++
        Write-Host "Updated: $($file.FullName)" -ForegroundColor Green
    }
}

Write-Host "`nCompleted! Updated $FilesChanged JSON files." -ForegroundColor Cyan