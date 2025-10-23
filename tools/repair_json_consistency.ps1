#!/usr/bin/env pwsh
<#
.SYNOPSIS
Comprehensive JSON Consistency Repair Script for P32 Animatronic Bot

.DESCRIPTION  
Systematically repairs all consistency issues found by check_json_consistency.ps1:
- PATH_MISMATCH: Fix relative_filename to match actual file paths
- MISSING_HARDWARE_REF: Create missing hardware component files
- MISSING_COMPONENT_REF: Create missing creature-specific component files  
- UNDEFINED_INTERFACE: Create missing interface definitions
- DUPLICATE_INTERFACE: Resolve interface assignment conflicts
- DUPLICATE_COMPONENT: Rename duplicate components uniquely
- INVALID_COORDINATE_SYSTEM: Standardize coordinate systems
- Missing versions: Add standardized version fields

.PARAMETER ProjectRoot
Root directory for JSON file operations (default: "config")

.PARAMETER DryRun
Show what would be done without making changes

.PARAMETER Verbose  
Show detailed operation information

.EXAMPLE
.\tools\repair_json_consistency.ps1
.\tools\repair_json_consistency.ps1 -DryRun -Verbose
.\tools\repair_json_consistency.ps1 -ProjectRoot "config"
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

# Repair counters
$RepairStats = @{
    PathMismatches = 0
    MissingHardware = 0
    MissingComponents = 0
    UndefinedInterfaces = 0
    DuplicateInterfaces = 0
    DuplicateComponents = 0
    InvalidCoordinateSystems = 0
    MissingVersions = 0
    JsonSyntaxFixes = 0
}

Write-Info "P32 Animatronic Bot - JSON Consistency Repair System"
Write-Info "Target: $ProjectRoot | DryRun: $DryRun | Verbose: $Verbose"
Write-Info "="*70

#region Helper Functions

function Get-JsonFiles {
    param([string]$RootPath)
    return Get-ChildItem -Path $RootPath -Recurse -Filter "*.json" | 
           Where-Object { 
               $_.Name -notmatch "^(\..*|temp_.*|backup_.*)" -and
               $_.FullName -notmatch '\.pio' -and 
               $_.FullName -notmatch '\.cmake' -and 
               $_.FullName -notmatch 'build' -and
               $_.FullName -notmatch 'bootloader' -and
               $_.FullName -notmatch '__pycache__' -and
               $_.FullName -notmatch 'CMakeFiles'
           }
}

function Read-JsonSafe {
    param([string]$FilePath)
    try {
        $content = Get-Content -Path $FilePath -Raw -Encoding UTF8
        return $content | ConvertFrom-Json
    } catch {
        Write-Warning "Invalid JSON syntax in $FilePath"
        return $null
    }
}

function Write-JsonSafe {
    param($Object, [string]$FilePath)
    if (-not $DryRun) {
        $Object | ConvertTo-Json -Depth 20 | Set-Content -Path $FilePath -Encoding UTF8
    }
    if ($Verbose) { Write-Progress "Updated: $FilePath" }
}

function Get-RelativePathFromConfig {
    param([string]$FullPath, [string]$ConfigRoot)
    try {
        $configPath = Resolve-Path $ConfigRoot -ErrorAction Stop
        $fullPath = Resolve-Path $FullPath -ErrorAction Stop
        return $fullPath.Path.Replace($configPath.Path + [IO.Path]::DirectorySeparatorChar, "").Replace([IO.Path]::DirectorySeparatorChar, "/")
    } catch {
        # If resolve fails, just strip the config root manually
        $normalizedFull = $FullPath.Replace("\", "/")
        $normalizedConfig = $ConfigRoot.Replace("\", "/")
        if ($normalizedFull.StartsWith($normalizedConfig + "/")) {
            return $normalizedFull.Substring($normalizedConfig.Length + 1)
        } else {
            return Split-Path $FullPath -Leaf
        }
    }
}

function New-MissingDirectory {
    param([string]$FilePath)
    $directory = Split-Path $FilePath -Parent
    if (-not (Test-Path $directory) -and -not $DryRun) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
        if ($Verbose) { Write-Progress "Created directory: $directory" }
    }
}

#endregion

#region Standard Templates

function Get-HardwareTemplate {
    param([string]$ComponentType, [string]$ComponentName)
    
    $baseTemplate = @{
        relative_filename = ""
        version = "1.0.0"
        author = "config/author.json"
        component_name = $ComponentName
        hardware_type = $ComponentType.ToUpper()
        description = "Generated hardware component for $ComponentName"
        specifications = @{
            power_consumption = @{
                voltage = "3.3V"
                current_draw = "50mA"
                power_watts = "0.165W"
            }
            physical_dimensions = @{
                width = "20mm"
                height = "15mm"
                depth = "10mm"
                weight = "5g"
            }
            interface_requirements = @{
                communication_protocol = "SPI"
                data_pins = 1
                control_pins = 2
                power_pins = 2
            }
        }
        cost_performance = @{
            unit_cost_usd = 15.99
            availability = "COMMON"
            lead_time_days = 7
            last_updated = (Get-Date -Format "yyyy-MM-dd")
        }
    }
    
    # Customize based on component type
    switch -Regex ($ComponentType) {
        "display|screen|lcd|oled" {
            $baseTemplate.specifications.resolution = @{ width = 240; height = 240 }
            $baseTemplate.specifications.color_depth = "16-bit RGB565"
            $baseTemplate.cost_performance.unit_cost_usd = 25.99
        }
        "servo|motor" {
            $baseTemplate.specifications.torque_kg_cm = 1.8
            $baseTemplate.specifications.rotation_degrees = 180
            $baseTemplate.specifications.control_signal = "PWM"
            $baseTemplate.cost_performance.unit_cost_usd = 12.99
        }
        "sensor" {
            $baseTemplate.specifications.sensing_range = "2-400cm"
            $baseTemplate.specifications.accuracy = "?3mm"
            $baseTemplate.cost_performance.unit_cost_usd = 8.99
        }
        "esp32|controller" {
            $baseTemplate.specifications.processor = "ESP32-S3"
            $baseTemplate.specifications.memory_flash_mb = 16
            $baseTemplate.specifications.memory_ram_kb = 512
            $baseTemplate.cost_performance.unit_cost_usd = 45.99
        }
    }
    
    return $baseTemplate
}

function Get-CreatureComponentTemplate {
    param([string]$CreatureType, [string]$ComponentType, [string]$ComponentName)
    
    return @{
        relative_filename = ""
        version = "1.0.0"
        author = "config/author.json"
        component_name = $ComponentName
        component_type = "CREATURE_SPECIFIC"
        creature_family = $CreatureType.ToUpper()
        description = "Generated $CreatureType-specific $ComponentType component"
        rendering = @{
            base_shape = "${ComponentType}_basic"
            creature_modifications = @{
                texture = "${CreatureType}_texture"
                color_scheme = "${CreatureType}_colors"
                scale_factor = 1.0
                distinctive_features = @("${CreatureType}_characteristic_1", "${CreatureType}_characteristic_2")
            }
        }
        behavior_modifiers = @{
            mood_responsiveness = 0.8
            animation_style = "${CreatureType}_style"
            interaction_patterns = @("${CreatureType}_idle", "${CreatureType}_active")
        }
        hardware_reference = "config/components/hardware/gc9a01_round_display.json"
        software = @{
            init_function = "${ComponentName}_init"
            act_function = "${ComponentName}_act"
            include_path = "src/components/${ComponentName}.h"
        }
    }
}

function Get-InterfaceTemplate {
    param([string]$InterfaceId, [string]$InterfaceType)
    
    $baseTemplate = @{
        relative_filename = ""
        version = "1.0.0"
        author = "config/author.json"
        interface_id = $InterfaceId
        interface_type = $InterfaceType.ToUpper()
        description = "Generated interface definition for $InterfaceId"
    }
    
    # Customize based on interface type
    switch -Regex ($InterfaceType) {
        "SPI_DEVICE" {
            $deviceNum = [regex]::Match($InterfaceId, '\d+').Value
            $baseTemplate.bus_reference = "config/components/interfaces/spi_bus_vspi.json"
            $baseTemplate.device_config = @{
                chip_select_pin = [int]$deviceNum + 14  # GPIO 15, 16, 17, etc.
                spi_mode = 0
                clock_speed_mhz = 20
                bit_order = "MSB_FIRST"
            }
        }
        "I2C_DEVICE" {
            $deviceNum = [regex]::Match($InterfaceId, '\d+').Value
            $baseTemplate.bus_reference = "config/components/interfaces/i2c_bus_primary.json"
            $baseTemplate.device_config = @{
                device_address = "0x" + ([int]$deviceNum + 60).ToString("X2")
                clock_speed_khz = 400
                address_size = 7
            }
        }
        "PWM_CHANNEL" {
            $channelNum = [regex]::Match($InterfaceId, '\d+').Value
            $baseTemplate.pwm_config = @{
                channel_number = [int]$channelNum
                gpio_pin = [int]$channelNum + 20  # GPIO 21, 22, 23, etc.
                frequency_hz = 50
                duty_cycle_range = @{ min = 5; max = 10 }
                resolution_bits = 12
            }
        }
        "GPIO" {
            $pinNum = [regex]::Match($InterfaceId, '\d+').Value
            $baseTemplate.gpio_config = @{
                pin_number = [int]$pinNum
                direction = "OUTPUT"
                pull_mode = "NONE"
                drive_strength = "NORMAL"
            }
        }
        "I2S_DEVICE" {
            $deviceNum = [regex]::Match($InterfaceId, '\d+').Value
            $baseTemplate.bus_reference = "config/components/interfaces/i2s_bus_0.json"
            $baseTemplate.device_config = @{
                data_pin = [int]$deviceNum + 25  # GPIO 26, 27, etc.
                sample_rate = 44100
                bits_per_sample = 16
                channel_format = "RIGHT_LEFT"
            }
        }
        default {
            $baseTemplate.generic_config = @{
                connection_type = "DIGITAL"
                voltage_level = "3V3"
                current_capability = "10mA"
            }
        }
    }
    
    return $baseTemplate
}

#endregion

#region Repair Functions

function Repair-PathMismatches {
    Write-Progress "Phase 1: Repairing PATH_MISMATCH issues..."
    
    $jsonFiles = Get-JsonFiles $ProjectRoot
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        # Check if the object has a relative_filename property
        $hasRelativeFilename = $json.PSObject.Properties.Name -contains "relative_filename"
        if (-not $hasRelativeFilename -or -not $json.relative_filename) { continue }
        
        $actualPath = Get-RelativePathFromConfig $file.FullName $ProjectRoot
        if ($json.relative_filename -ne $actualPath) {
            if ($Verbose) { 
                Write-Warning "PATH_MISMATCH: $($file.Name) has '$($json.relative_filename)' should be '$actualPath'" 
            }
            
            $json.relative_filename = $actualPath
            Write-JsonSafe $json $file.FullName
            $RepairStats.PathMismatches++
        }
    }
    
    Write-Success "Fixed $($RepairStats.PathMismatches) path mismatches"
}

function Repair-MissingHardwareComponents {
    Write-Progress "Phase 2: Creating missing hardware components..."
    
    $missingHardware = @()
    $jsonFiles = Get-JsonFiles $ProjectRoot
    
    # Collect all missing hardware references
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasHardwareRef = $json.PSObject.Properties.Name -contains "hardware_reference"
        if (-not $hasHardwareRef -or -not $json.hardware_reference) { continue }
        
        $hardwarePath = Join-Path $ProjectRoot $json.hardware_reference.Replace("config/", "")
        if (-not (Test-Path $hardwarePath)) {
            $missingHardware += @{
                Path = $hardwarePath
                Reference = $json.hardware_reference
                SourceFile = $file.Name
            }
        }
    }
    
    # Create missing hardware files
    $uniqueHardware = $missingHardware | Group-Object Path | ForEach-Object { $_.Group[0] }
    foreach ($hw in $uniqueHardware) {
        $componentName = (Split-Path $hw.Path -Leaf).Replace(".json", "")
        $componentType = Split-Path (Split-Path $hw.Path -Parent) -Leaf
        
        if ($Verbose) { Write-Progress "Creating missing hardware: $componentName" }
        
        $template = Get-HardwareTemplate $componentType $componentName
        $template.relative_filename = Get-RelativePathFromConfig $hw.Path $ProjectRoot
        
        New-MissingDirectory $hw.Path
        Write-JsonSafe $template $hw.Path
        $RepairStats.MissingHardware++
    }
    
    Write-Success "Created $($RepairStats.MissingHardware) missing hardware components"
}

function Repair-MissingCreatureComponents {
    Write-Progress "Phase 3: Creating missing creature-specific components..."
    
    $missingComponents = @()
    $jsonFiles = Get-JsonFiles $ProjectRoot
    
    # Collect all missing component references
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasComponentRef = $json.PSObject.Properties.Name -contains "component_reference"
        if (-not $hasComponentRef -or -not $json.component_reference) { continue }
        
        $componentPath = Join-Path $ProjectRoot $json.component_reference.Replace("config/", "")
        if (-not (Test-Path $componentPath)) {
            $missingComponents += @{
                Path = $componentPath
                Reference = $json.component_reference
                SourceFile = $file.Name
            }
        }
    }
    
    # Create missing creature component files
    $uniqueComponents = $missingComponents | Group-Object Path | ForEach-Object { $_.Group[0] }
    foreach ($comp in $uniqueComponents) {
        $componentName = (Split-Path $comp.Path -Leaf).Replace(".json", "")
        
        # Extract creature type from path
        $creatureType = "goblin"  # default
        if ($comp.Path -match "/(goblin|cat|bear|cyclops|dragon|android|robot|pixie|zombie|orc)/") {
            $creatureType = $matches[1]
        }
        
        # Extract component type from name
        $componentType = "eye"  # default
        if ($componentName -match "(eye|mouth|nose|ear|speaker)") {
            $componentType = $matches[1]
        }
        
        if ($Verbose) { Write-Progress "Creating missing component: $creatureType $componentType" }
        
        $template = Get-CreatureComponentTemplate $creatureType $componentType $componentName
        $template.relative_filename = Get-RelativePathFromConfig $comp.Path $ProjectRoot
        
        New-MissingDirectory $comp.Path
        Write-JsonSafe $template $comp.Path
        $RepairStats.MissingComponents++
    }
    
    Write-Success "Created $($RepairStats.MissingComponents) missing creature components"
}

function Repair-UndefinedInterfaces {
    Write-Progress "Phase 4: Creating missing interface definitions..."
    
    $undefinedInterfaces = @()
    $jsonFiles = Get-JsonFiles $ProjectRoot
    
    # Collect all undefined interface references
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasInterfaceId = $json.PSObject.Properties.Name -contains "interface_id"
        if (-not $hasInterfaceId -or -not $json.interface_id) { continue }
        
        $interfaceId = $json.interface_id
        
        # Skip complex object interfaces for now
        if ($interfaceId -match "^@\{.*\}$" -or $interfaceId -match "config/") { continue }
        
        # Check if interface definition exists
        $interfacePath = Join-Path $ProjectRoot "components/interfaces/$($interfaceId.ToLower()).json"
        if (-not (Test-Path $interfacePath)) {
            $undefinedInterfaces += @{
                InterfaceId = $interfaceId
                Path = $interfacePath
                SourceFile = $file.Name
            }
        }
    }
    
    # Create missing interface files
    $uniqueInterfaces = $undefinedInterfaces | Group-Object InterfaceId | ForEach-Object { $_.Group[0] }
    foreach ($iface in $uniqueInterfaces) {
        $interfaceType = ""
        if ($iface.InterfaceId -match "^(SPI|I2C|PWM|GPIO|I2S)_") {
            $interfaceType = $matches[1] + "_DEVICE"
        } else {
            $interfaceType = "GENERIC"
        }
        
        if ($Verbose) { Write-Progress "Creating missing interface: $($iface.InterfaceId)" }
        
        $template = Get-InterfaceTemplate $iface.InterfaceId $interfaceType
        $template.relative_filename = Get-RelativePathFromConfig $iface.Path $ProjectRoot
        
        New-MissingDirectory $iface.Path
        Write-JsonSafe $template $iface.Path
        $RepairStats.UndefinedInterfaces++
    }
    
    Write-Success "Created $($RepairStats.UndefinedInterfaces) missing interface definitions"
}

function Repair-DuplicateInterfaces {
    Write-Progress "Phase 5: Resolving duplicate interface assignments..."
    
    $interfaceAssignments = @{}
    $jsonFiles = Get-JsonFiles $ProjectRoot
    
    # Map all interface assignments
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasInterfaceId = $json.PSObject.Properties.Name -contains "interface_id"
        if (-not $hasInterfaceId -or -not $json.interface_id) { continue }
        
        $interfaceId = $json.interface_id
        if (-not $interfaceAssignments.ContainsKey($interfaceId)) {
            $interfaceAssignments[$interfaceId] = @()
        }
        $interfaceAssignments[$interfaceId] += $file.FullName
    }
    
    # Resolve duplicates by reassigning interfaces
    foreach ($interfaceId in $interfaceAssignments.Keys) {
        $assignments = $interfaceAssignments[$interfaceId]
        if ($assignments.Count -le 1) { continue }
        
        if ($Verbose) { 
            Write-Warning "DUPLICATE_INTERFACE: $interfaceId assigned to $($assignments.Count) components" 
        }
        
        # Reassign all but the first assignment
        for ($i = 1; $i -lt $assignments.Count; $i++) {
            $file = $assignments[$i]
            $json = Read-JsonSafe $file
            
            # Generate new interface ID
            $baseId = $interfaceId -replace '\d+$', ''
            $newNumber = [int]($interfaceId -replace '.*?(\d+)$', '$1') + $i + 10
            $newInterfaceId = "$baseId$newNumber"
            
            if ($Verbose) { Write-Progress "Reassigning $interfaceId -> $newInterfaceId in $(Split-Path $file -Leaf)" }
            
            $json.interface_id = $newInterfaceId
            Write-JsonSafe $json $file
            $RepairStats.DuplicateInterfaces++
        }
    }
    
    Write-Success "Resolved $($RepairStats.DuplicateInterfaces) duplicate interface assignments"
}

function Repair-DuplicateComponents {
    Write-Progress "Phase 6: Resolving duplicate component names..."
    
    $componentNames = @{}
    $jsonFiles = Get-JsonFiles $ProjectRoot
    
    # Map all component names
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasComponentName = $json.PSObject.Properties.Name -contains "component_name"
        if (-not $hasComponentName -or -not $json.component_name) { continue }
        
        $componentName = $json.component_name
        if (-not $componentNames.ContainsKey($componentName)) {
            $componentNames[$componentName] = @()
        }
        $componentNames[$componentName] += $file.FullName
    }
    
    # Resolve duplicates by renaming components
    foreach ($componentName in $componentNames.Keys) {
        $files = $componentNames[$componentName]
        if ($files.Count -le 1) { continue }
        
        if ($Verbose) { 
            Write-Warning "DUPLICATE_COMPONENT: $componentName found in $($files.Count) files" 
        }
        
        # Rename all but the first occurrence
        for ($i = 1; $i -lt $files.Count; $i++) {
            $file = $files[$i]
            $json = Read-JsonSafe $file
            
            # Generate unique name based on file path
            $pathElements = (Split-Path $file -Parent).Split([IO.Path]::DirectorySeparatorChar)
            $uniquePrefix = $pathElements[-1]  # Use parent directory as prefix
            $newComponentName = "${uniquePrefix}_${componentName}"
            
            if ($Verbose) { Write-Progress "Renaming $componentName -> $newComponentName in $(Split-Path $file -Leaf)" }
            
            $json.component_name = $newComponentName
            Write-JsonSafe $json $file
            $RepairStats.DuplicateComponents++
        }
    }
    
    Write-Success "Resolved $($RepairStats.DuplicateComponents) duplicate component names"
}

function Repair-CoordinateSystems {
    Write-Progress "Phase 7: Standardizing coordinate systems..."
    
    $validCoordinateSystems = @("skull_3d", "planar_2d", "torso_3d")
    $standardMappings = @{
        "humanoid_3d" = "skull_3d"
        "quadruped_3d" = "skull_3d" 
        "unknown" = "skull_3d"
    }
    
    $jsonFiles = Get-JsonFiles $ProjectRoot
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasCoordinateSystem = $json.PSObject.Properties.Name -contains "coordinate_system"
        if (-not $hasCoordinateSystem -or -not $json.coordinate_system) { continue }
        
        $currentSystem = $json.coordinate_system
        
        # Handle complex coordinate system objects
        if ($currentSystem -is [string] -and $currentSystem.StartsWith("@{")) {
            $newSystem = "skull_3d"  # Default for complex systems
            if ($Verbose) { 
                Write-Progress "Converting complex coordinate system in $(Split-Path $file -Leaf): $currentSystem -> $newSystem" 
            }
            $json.coordinate_system = $newSystem
            Write-JsonSafe $json $file.FullName
            $RepairStats.InvalidCoordinateSystems++
        }
        elseif ($currentSystem -notin $validCoordinateSystems) {
            $newSystem = $standardMappings[$currentSystem]
            if (-not $newSystem) { $newSystem = "skull_3d" }
            
            if ($Verbose) { 
                Write-Progress "Standardizing coordinate system in $(Split-Path $file -Leaf): $currentSystem -> $newSystem" 
            }
            $json.coordinate_system = $newSystem
            Write-JsonSafe $json $file.FullName
            $RepairStats.InvalidCoordinateSystems++
        }
    }
    
    Write-Success "Standardized $($RepairStats.InvalidCoordinateSystems) coordinate systems"
}

function Repair-MissingVersions {
    Write-Progress "Phase 8: Adding missing version fields..."
    
    $jsonFiles = Get-JsonFiles $ProjectRoot
    foreach ($file in $jsonFiles) {
        $json = Read-JsonSafe $file.FullName
        if (-not $json) { continue }
        
        $hasVersion = $json.PSObject.Properties.Name -contains "version"
        if (-not $hasVersion -or -not $json.version -or $json.version -eq "" -or $null -eq $json.version) {
            if ($Verbose) { Write-Progress "Adding version to $(Split-Path $file -Leaf)" }
            if (-not $hasVersion) {
                $json | Add-Member -MemberType NoteProperty -Name "version" -Value "1.0.0" -Force
            } else {
                $json.version = "1.0.0"
            }
            Write-JsonSafe $json $file.FullName
            $RepairStats.MissingVersions++
        }
    }
    
    Write-Success "Added version fields to $($RepairStats.MissingVersions) files"
}

#endregion

#region Main Execution

try {
    $startTime = Get-Date
    
    if (-not (Test-Path $ProjectRoot)) {
        Write-Error "Project root '$ProjectRoot' does not exist"
        exit 1
    }
    
    # Execute all repair phases
    Repair-PathMismatches
    Repair-MissingHardwareComponents  
    Repair-MissingCreatureComponents
    Repair-UndefinedInterfaces
    Repair-DuplicateInterfaces
    Repair-DuplicateComponents
    Repair-CoordinateSystems
    Repair-MissingVersions
    
    # Summary
    $duration = (Get-Date) - $startTime
    $totalRepairs = ($RepairStats.Values | Measure-Object -Sum).Sum
    
    Write-Info "="*70
    Write-Info "JSON Consistency Repair Complete"
    Write-Info "Duration: $($duration.TotalSeconds.ToString('F1')) seconds"
    Write-Info "Total Repairs: $totalRepairs"
    Write-Info ""
    Write-Info "Repair Breakdown:"
    foreach ($key in $RepairStats.Keys) {
        Write-Info "  $key`: $($RepairStats[$key])"
    }
    
    if ($DryRun) {
        Write-Warning "DRY RUN MODE - No files were actually modified"
    } else {
        Write-Success "All repairs applied successfully"
        Write-Info "Run check_json_consistency.ps1 again to verify fixes"
    }
    
    exit 0
    
} catch {
    Write-Error "Repair failed: $($_.Exception.Message)"
    Write-Error "Stack Trace: $($_.ScriptStackTrace)"
    exit 1
}

#endregion