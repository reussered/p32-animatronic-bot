# Create missing component files (JSON, .hdr, .src)
# This script generates stub components with minimum required fields

param(
    [string]$MissingCsvPath = "f:\GitHub\p32-animatronic-bot\still_missing.csv"
)

$baseDir = "f:\GitHub\p32-animatronic-bot"
$created = 0
$skipped = 0

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Component Generator" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# Load missing files
$missing = Import-Csv $MissingCsvPath

foreach ($item in $missing) {
    $relPath = $item.ReferencedFile
    $fullPath = Join-Path $baseDir $relPath
    $filename = [System.IO.Path]::GetFileNameWithoutExtension((Split-Path $relPath -Leaf))
    $dir = Split-Path $fullPath -Parent
    
    # Skip if already exists
    if (Test-Path $fullPath) {
        Write-Host "  [SKIP] $filename (already exists)" -ForegroundColor Gray
        $skipped++
        continue
    }
    
    # Create directory if needed
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    
    # Determine component type and interface based on name
    $componentType = "POSITIONED_COMPONENT"
    $subsystem = "UNKNOWN"
    $components = @()
    
    # Categorize by name patterns
    if ($filename -match "servo") {
        $subsystem = "ACTUATOR"
        $components += "config/components/interfaces/esp32_servo_controller.json"
    }
    elseif ($filename -match "speaker") {
        $subsystem = "AUDIO"
        $components += "config/components/interfaces/i2s_bus.json"
    }
    elseif ($filename -match "eye") {
        $subsystem = "VISION"
        $components += "config/components/interfaces/spi_bus.json"
    }
    elseif ($filename -match "sensor") {
        $subsystem = "SENSOR"
        $components += "config/components/interfaces/i2c_bus.json"
    }
    elseif ($filename -match "led") {
        $subsystem = "INDICATOR"
        $components += "config/components/interfaces/gpio_pair_vibration_sensor.json"
    }
    elseif ($filename -match "base_") {
        $componentType = "BASE_TEMPLATE"
        $subsystem = "TEMPLATE"
    }
    
    # Create JSON file
    $json = @{
        name = $filename
        version = "1.0.0"
        author = "config/author.json"
        component_type = $componentType
        subsystem = $subsystem
    }
    
    # Add components array if we have interfaces
    if ($components.Count -gt 0) {
        $json.components = $components
    }
    
    # Add software section
    $json.software = @{
        init_function = "${filename}_init"
        act_function = "${filename}_act"
        hitCount = 10
    }
    
    # Add position for positioned components
    if ($componentType -eq "POSITIONED_COMPONENT") {
        $json.position = @{
            coordinate_system = "skull_3d"
            reference_point = "nose_center"
            x = "0.0 INCH"
            y = "0.0 INCH"
            z = "0.0 INCH"
            units = "INCH"
        }
    }
    
    # Write JSON
    $json | ConvertTo-Json -Depth 10 | Set-Content $fullPath -Encoding ASCII
    
    # Create .hdr file
    $hdrPath = Join-Path "f:\GitHub\p32-animatronic-bot\config\components" "$filename.hdr"
    $hdrContent = @"
// $filename Header
// Auto-generated component header

#ifndef ${filename}_HDR
#define ${filename}_HDR

#include "esp_err.h"

/**
 * @brief Initialize $filename component
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ${filename}_init(void);

/**
 * @brief Act function for $filename component
 * Called periodically by main loop
 */
void ${filename}_act(void);

#endif // ${filename}_HDR
"@
    $hdrContent | Set-Content $hdrPath -Encoding ASCII
    
    # Create .src file
    $srcPath = Join-Path "f:\GitHub\p32-animatronic-bot\config\components" "$filename.src"
    $srcContent = @"
// $filename Implementation
// Auto-generated component source

#include "esp_log.h"

static const char *TAG = "$filename";

esp_err_t ${filename}_init(void) {
#ifdef DEBUG
    ESP_LOGI(TAG, "${filename}_init() called");
#endif
    // TODO: Add initialization code
    return ESP_OK;
}

void ${filename}_act(void) {
#ifdef DEBUG
    ESP_LOGD(TAG, "${filename}_act() called");
#endif
    // TODO: Add action code
}
"@
    $srcContent | Set-Content $srcPath -Encoding ASCII
    
    Write-Host "  [CREATE] $filename" -ForegroundColor Green
    $created++
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  Created: $created components" -ForegroundColor Green
Write-Host "  Skipped: $skipped components" -ForegroundColor Gray
Write-Host "========================================`n" -ForegroundColor Cyan
