# P32 Multi-ESP32 Build and Test Script
# This script builds and validates all ESP32 variant configurations

param(
    [switch]$BuildAll,
    [switch]$TestBuilds,
    [switch]$ShowCosts,
    [switch]$ValidateGPIO,
    [string]$Environment = "",
    [string]$UploadPort = ""
)

Write-Host "🤖 P32 Animatronic Bot - Multi-ESP32 Build System" -ForegroundColor Cyan
Write-Host "=================================================" -ForegroundColor Cyan

# ESP32 Variant Information
$ESP32Variants = @{
    "ESP32-C3" = @{
        "Cost" = 3
        "GPIO" = 22
        "SRAM" = "400KB"
        "Cores" = 1
        "Environments" = @("left_arm", "right_arm", "left_leg", "right_leg")
    }
    "ESP32" = @{
        "Cost" = 5
        "GPIO" = 34
        "SRAM" = "520KB"
        "Cores" = 2
        "Environments" = @("left_arm_advanced", "right_arm_advanced")
    }
    "ESP32-S3" = @{
        "Cost" = 8
        "GPIO" = 45
        "SRAM" = "512KB+8MB"
        "Cores" = 2
        "Environments" = @("goblin_head", "goblin_torso", "dev_all_s3")
    }
}

# Function to display cost analysis
function Show-CostAnalysis {
    Write-Host "`n💰 Cost Analysis for Different Configurations" -ForegroundColor Green
    Write-Host "=============================================" -ForegroundColor Green
    
    # Production Optimized (2×S3 + 4×C3)
    $ProdOptimized = (2 * 8) + (4 * 3)
    Write-Host "Production Optimized: 2×ESP32-S3 + 4×ESP32-C3 = $($ProdOptimized) USD" -ForegroundColor Yellow
    
    # All ESP32-S3 (Development)
    $AllS3 = 6 * 8
    Write-Host "All ESP32-S3 (Dev):   6×ESP32-S3             = $($AllS3) USD" -ForegroundColor Yellow
    
    # Maximum Cost Optimized (1×S3 + 5×C3)
    $MaxCostOpt = (1 * 8) + (5 * 3)
    Write-Host "Maximum Cost-Opt:     1×ESP32-S3 + 5×ESP32-C3 = $($MaxCostOpt) USD" -ForegroundColor Yellow
    
    Write-Host "`nSavings vs All-S3: $($AllS3 - $ProdOptimized) USD (Production Optimized)" -ForegroundColor Green
}

# Function to validate GPIO usage
function Test-GPIOUsage {
    Write-Host "`n🔍 Validating GPIO Usage for Each Subsystem" -ForegroundColor Green
    Write-Host "===========================================" -ForegroundColor Green
    
    $GPIOUsage = @{
        "goblin_head" = @{ "Used" = 25; "Chip" = "ESP32-S3"; "Limit" = 45 }
        "goblin_torso" = @{ "Used" = 20; "Chip" = "ESP32-S3"; "Limit" = 45 }
        "left_arm" = @{ "Used" = 18; "Chip" = "ESP32-C3"; "Limit" = 22 }
        "right_arm" = @{ "Used" = 18; "Chip" = "ESP32-C3"; "Limit" = 22 }
        "left_leg" = @{ "Used" = 22; "Chip" = "ESP32-C3"; "Limit" = 22 }
        "right_leg" = @{ "Used" = 22; "Chip" = "ESP32-C3"; "Limit" = 22 }
    }
    
    foreach ($env in $GPIOUsage.Keys) {
        $usage = $GPIOUsage[$env]
        $utilization = [math]::Round(($usage.Used / $usage.Limit) * 100, 1)
        $status = if ($utilization -le 80) { "✅ GOOD" } elseif ($utilization -le 95) { "⚠️  HIGH" } else { "❌ OVER" }
        
        Write-Host "$($env.PadRight(15)) | $($usage.Chip.PadRight(10)) | $($usage.Used.ToString().PadLeft(2))/$($usage.Limit) pins | $($utilization.ToString().PadLeft(5))% | $status"
    }
}

# Function to build specific environment
function Build-Environment {
    param([string]$EnvName)
    
    Write-Host "`n🔨 Building environment: $EnvName" -ForegroundColor Yellow
    Write-Host "=========================" -ForegroundColor Yellow
    
    try {
        $result = pio run -e $EnvName 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ $EnvName built successfully" -ForegroundColor Green
            return $true
        } else {
            Write-Host "❌ $EnvName build failed:" -ForegroundColor Red
            Write-Host $result -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Host "❌ Error building $EnvName`: $_" -ForegroundColor Red
        return $false
    }
}

# Function to test all builds
function Test-AllBuilds {
    Write-Host "`n🧪 Testing All Build Configurations" -ForegroundColor Green
    Write-Host "====================================" -ForegroundColor Green
    
    $allEnvironments = @(
        "goblin_head", "goblin_torso",
        "left_arm", "right_arm", "left_leg", "right_leg",
        "left_arm_advanced", "right_arm_advanced",
        "dev_all_s3", "legacy_single_chip"
    )
    
    $buildResults = @()
    $successCount = 0
    
    foreach ($env in $allEnvironments) {
        $success = Build-Environment $env
        $buildResults += @{ "Environment" = $env; "Success" = $success }
        if ($success) { $successCount++ }
    }
    
    Write-Host "`n📊 Build Results Summary:" -ForegroundColor Cyan
    Write-Host "========================" -ForegroundColor Cyan
    Write-Host "Successful builds: $successCount/$($allEnvironments.Count)" -ForegroundColor Green
    
    foreach ($result in $buildResults) {
        $status = if ($result.Success) { "✅" } else { "❌" }
        Write-Host "$status $($result.Environment)"
    }
    
    return $successCount -eq $allEnvironments.Count
}

# Function to upload to specific environment
function Upload-Environment {
    param([string]$EnvName, [string]$Port = "")
    
    Write-Host "`n📤 Uploading to: $EnvName" -ForegroundColor Yellow
    
    $uploadCmd = "pio run -e $EnvName -t upload"
    if ($Port) {
        $uploadCmd += " --upload-port $Port"
    }
    
    try {
        Invoke-Expression $uploadCmd
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ $EnvName uploaded successfully" -ForegroundColor Green
        } else {
            Write-Host "❌ $EnvName upload failed" -ForegroundColor Red
        }
    } catch {
        Write-Host "❌ Error uploading $EnvName`: $_" -ForegroundColor Red
    }
}

# Main execution logic
if ($ShowCosts) {
    Show-CostAnalysis
}

if ($ValidateGPIO) {
    Test-GPIOUsage
}

if ($TestBuilds) {
    $allSuccess = Test-AllBuilds
    if ($allSuccess) {
        Write-Host "`n🎉 All builds successful! System is ready for deployment." -ForegroundColor Green
    } else {
        Write-Host "`n⚠️  Some builds failed. Please check the errors above." -ForegroundColor Yellow
    }
}

if ($BuildAll) {
    Write-Host "`n🔨 Building All Production Environments" -ForegroundColor Green
    Write-Host "=======================================" -ForegroundColor Green
    
    $productionEnvs = @("goblin_head", "goblin_torso", "left_arm", "right_arm", "left_leg", "right_leg")
    
    foreach ($env in $productionEnvs) {
        Build-Environment $env
    }
}

if ($Environment) {
    if ($UploadPort) {
        Upload-Environment $Environment $UploadPort
    } else {
        Build-Environment $Environment
    }
}

# Show usage if no parameters provided
if (-not ($BuildAll -or $TestBuilds -or $ShowCosts -or $ValidateGPIO -or $Environment)) {
    Write-Host "`n📖 Usage Examples:" -ForegroundColor Cyan
    Write-Host "==================" -ForegroundColor Cyan
    Write-Host "Show cost analysis:           .\build_multi_esp32.ps1 -ShowCosts" -ForegroundColor White
    Write-Host "Validate GPIO usage:          .\build_multi_esp32.ps1 -ValidateGPIO" -ForegroundColor White
    Write-Host "Test all build configs:       .\build_multi_esp32.ps1 -TestBuilds" -ForegroundColor White
    Write-Host "Build all production:         .\build_multi_esp32.ps1 -BuildAll" -ForegroundColor White
    Write-Host "Build specific environment:   .\build_multi_esp32.ps1 -Environment goblin_head" -ForegroundColor White
    Write-Host "Upload to specific port:      .\build_multi_esp32.ps1 -Environment goblin_head -UploadPort COM3" -ForegroundColor White
    
    Write-Host "`n🚀 Available Environments:" -ForegroundColor Cyan
    Write-Host "ESP32-S3: goblin_head, goblin_torso, dev_all_s3" -ForegroundColor Yellow
    Write-Host "ESP32-C3: left_arm, right_arm, left_leg, right_leg" -ForegroundColor Yellow  
    Write-Host "ESP32:    left_arm_advanced, right_arm_advanced" -ForegroundColor Yellow
    Write-Host "Legacy:   legacy_single_chip" -ForegroundColor Gray
}