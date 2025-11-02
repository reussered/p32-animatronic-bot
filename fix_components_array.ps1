# Fix JSON files to use components:[] array instead of embedded interfaces
# Updates all recently created component JSON files

$baseDir = "f:\GitHub\p32-animatronic-bot"
$fixed = 0
$skipped = 0

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Fix Components Array" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# Get all JSON files in positioned directory
Get-ChildItem -Path "$baseDir\config\components\positioned" -Recurse -Filter "*.json" -File | ForEach-Object {
    $file = $_
    $filename = $file.BaseName
    
    try {
        # Read JSON
        $content = Get-Content $file.FullName -Raw -ErrorAction Stop
        $json = $content | ConvertFrom-Json -ErrorAction Stop
        
        $needsUpdate = $false
        
        # Check if it has the old structure (no components array but has subsystem)
        if (-not $json.components -and $json.subsystem) {
            $components = @()
            
            # Determine interface based on subsystem or name patterns
            if ($json.subsystem -eq "ACTUATOR" -or $filename -match "servo") {
                $components += "config/components/interfaces/esp32_servo_controller.json"
                $needsUpdate = $true
            }
            elseif ($json.subsystem -eq "AUDIO" -or $filename -match "speaker") {
                $components += "config/components/interfaces/i2s_bus.json"
                $needsUpdate = $true
            }
            elseif ($json.subsystem -eq "VISION" -or $filename -match "eye|camera") {
                $components += "config/components/interfaces/spi_bus.json"
                $needsUpdate = $true
            }
            elseif ($json.subsystem -eq "SENSOR" -or $filename -match "sensor") {
                $components += "config/components/interfaces/i2c_bus.json"
                $needsUpdate = $true
            }
            elseif ($json.subsystem -eq "INDICATOR" -or $filename -match "led|beacon") {
                # LEDs typically use GPIO or I2C depending on type
                if ($filename -match "matrix|strip|array") {
                    $components += "config/components/interfaces/i2c_bus.json"
                } else {
                    $components += "config/components/interfaces/spi_bus.json"
                }
                $needsUpdate = $true
            }
            
            # Add components array if we have interfaces
            if ($components.Count -gt 0) {
                $json | Add-Member -MemberType NoteProperty -Name "components" -Value $components -Force
                
                # Reorder properties: name, version, author, component_type, subsystem, components, then rest
                $orderedJson = [ordered]@{
                    name = $json.name
                    version = $json.version
                    author = $json.author
                    component_type = $json.component_type
                    subsystem = $json.subsystem
                    components = $json.components
                }
                
                # Add remaining properties
                $json.PSObject.Properties | Where-Object { $_.Name -notin @('name','version','author','component_type','subsystem','components') } | ForEach-Object {
                    $orderedJson[$_.Name] = $_.Value
                }
                
                # Convert to JSON and save
                $orderedJson | ConvertTo-Json -Depth 10 | Set-Content $file.FullName -Encoding UTF8
                Write-Host "  [FIX] $filename" -ForegroundColor Green
                $fixed++
            }
        }
        
        if (-not $needsUpdate) {
            $skipped++
        }
    }
    catch {
        Write-Host "  [ERROR] $filename : $_" -ForegroundColor Red
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  Fixed: $fixed files" -ForegroundColor Green
Write-Host "  Skipped: $skipped files" -ForegroundColor Gray
Write-Host "========================================`n" -ForegroundColor Cyan
