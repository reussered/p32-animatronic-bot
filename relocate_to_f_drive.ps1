# ================================================================
# F: Drive Relocation Script for PlatformIO/Python/ESP-IDF
# ================================================================
# This script permanently moves all build/cache operations from 
# C: drive to F: drive to prevent disk space issues
# ================================================================

Write-Host "? Relocating PlatformIO/Python/ESP-IDF to F: Drive..." -ForegroundColor Green

# Create F: drive directories
$FDriveDirectories = @(
    "F:\platformio_core",
    "F:\platformio_home", 
    "F:\python_user",
    "F:\python_cache",
    "F:\esp_idf_tools",
    "F:\temp",
    "F:\build_cache"
)

Write-Host "`n? Creating F: drive directories..." -ForegroundColor Yellow
foreach ($dir in $FDriveDirectories) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "   Created: $dir" -ForegroundColor Cyan
    } else {
        Write-Host "   Exists: $dir" -ForegroundColor Gray
    }
}

# Set environment variables for current session
Write-Host "`n? Setting environment variables for current session..." -ForegroundColor Yellow
$env:PLATFORMIO_CORE_DIR = "F:\platformio_core"
$env:PLATFORMIO_HOME_DIR = "F:\platformio_home"
$env:PYTHON_USER_BASE = "F:\python_user"
$env:PYTHONUSERBASE = "F:\python_user"
$env:PIP_CACHE_DIR = "F:\python_cache\pip"
$env:IDF_TOOLS_PATH = "F:\esp_idf_tools"
$env:TEMP = "F:\temp"
$env:TMP = "F:\temp"
$env:TMPDIR = "F:\temp"

# Permanently set environment variables (requires admin for system-wide)
Write-Host "`n??  Setting permanent user environment variables..." -ForegroundColor Yellow
$EnvVars = @{
    "PLATFORMIO_CORE_DIR" = "F:\platformio_core"
    "PLATFORMIO_HOME_DIR" = "F:\platformio_home"
    "PYTHON_USER_BASE" = "F:\python_user"
    "PYTHONUSERBASE" = "F:\python_user"
    "PIP_CACHE_DIR" = "F:\python_cache\pip"
    "IDF_TOOLS_PATH" = "F:\esp_idf_tools"
    "TEMP" = "F:\temp"
    "TMP" = "F:\temp"
}

foreach ($var in $EnvVars.GetEnumerator()) {
    [Environment]::SetEnvironmentVariable($var.Key, $var.Value, [EnvironmentVariableTarget]::User)
    Write-Host "   Set $($var.Key) = $($var.Value)" -ForegroundColor Cyan
}

# Copy existing PlatformIO data if it exists
Write-Host "`n? Migrating existing PlatformIO data..." -ForegroundColor Yellow
$OldPlatformIODir = "$env:USERPROFILE\.platformio"
if (Test-Path $OldPlatformIODir) {
    Write-Host "   Found existing PlatformIO directory: $OldPlatformIODir" -ForegroundColor Cyan
    Write-Host "   Copying to F:\platformio_core..." -ForegroundColor Cyan
    try {
        Copy-Item $OldPlatformIODir -Destination "F:\platformio_core" -Recurse -Force
        Write-Host "   ? Migration complete!" -ForegroundColor Green
        Write-Host "   ? You can safely delete $OldPlatformIODir after verifying builds work" -ForegroundColor Yellow
    } catch {
        Write-Host "   ??  Migration failed: $($_.Exception.Message)" -ForegroundColor Red
        Write-Host "   Continuing with fresh setup..." -ForegroundColor Yellow
    }
} else {
    Write-Host "   No existing PlatformIO directory found - fresh setup" -ForegroundColor Gray
}

# Create platformio.ini override for this project
Write-Host "`n? Creating project-specific configuration..." -ForegroundColor Yellow
$PlatformIOOverride = @"
; F: Drive Build Configuration Override
; This ensures all build artifacts go to F: drive

[platformio]
build_dir = F:/build_cache/.pio/build
libdeps_dir = F:/build_cache/.pio/libdeps
globallib_dir = F:/platformio_core/lib
platforms_dir = F:/platformio_core/platforms
packages_dir = F:/platformio_core/packages

"@

Add-Content -Path "platformio_f_drive.ini" -Value $PlatformIOOverride
Write-Host "   Created: platformio_f_drive.ini (include this in main platformio.ini)" -ForegroundColor Cyan

# Verification
Write-Host "`n? Verifying configuration..." -ForegroundColor Yellow
Write-Host "   PLATFORMIO_CORE_DIR = $env:PLATFORMIO_CORE_DIR" -ForegroundColor Cyan
Write-Host "   TEMP = $env:TEMP" -ForegroundColor Cyan
Write-Host "   IDF_TOOLS_PATH = $env:IDF_TOOLS_PATH" -ForegroundColor Cyan

Write-Host "`n? F: Drive relocation complete!" -ForegroundColor Green
Write-Host "? Restart your terminal for permanent changes to take effect" -ForegroundColor Yellow
Write-Host "??  Next: Run 'pio run -e test_bot_minimal' to test the new configuration" -ForegroundColor Yellow