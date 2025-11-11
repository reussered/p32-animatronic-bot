# Build and Upload Script for P32 Animatronic Bot
# Usage: .\build_and_upload.ps1 <subsystem_name>
# Example: .\build_and_upload.ps1 goblin_head

param(
    [Parameter(Mandatory=$true)]
    [string]$Subsystem
)

$ErrorActionPreference = "Stop"

Write-Host "=== P32 Animatronic Bot Build Pipeline ===" -ForegroundColor Cyan
Write-Host "Subsystem: $Subsystem" -ForegroundColor Yellow
Write-Host ""

# Step 1: Generate component tables
Write-Host "[1/4] Generating component tables..." -ForegroundColor Green
python tools\generate_tables.py $Subsystem src
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Component table generation failed!" -ForegroundColor Red
    exit 1
}
Write-Host "Component tables generated successfully." -ForegroundColor Green
Write-Host ""

# Step 2: Clean build artifacts
Write-Host "[2/4] Cleaning build artifacts..." -ForegroundColor Green
pio run -e $Subsystem -t clean
if ($LASTEXITCODE -ne 0) {
    Write-Host "WARNING: Clean operation had issues, continuing anyway..." -ForegroundColor Yellow
}
Write-Host "Build artifacts cleaned." -ForegroundColor Green
Write-Host ""

# Step 3: Compile
Write-Host "[3/4] Compiling..." -ForegroundColor Green
$ErrorActionPreference = "Continue"
pio run -e $Subsystem *> build.log
$ErrorActionPreference = "Stop"
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Compilation failed!" -ForegroundColor Red
    Write-Host "Showing last 30 error lines:" -ForegroundColor Yellow
    Select-String -Path build.log -Pattern "error|failed" -CaseSensitive | Select-Object -Last 30
    exit 1
}
Write-Host "Compilation successful!" -ForegroundColor Green
Select-String -Path build.log -Pattern "success|Linking|RAM:|Flash:" | Select-Object -Last 10
Write-Host ""

# Step 4: Upload
Write-Host "[4/4] Uploading to device..." -ForegroundColor Green
$ErrorActionPreference = "Continue"
pio run -e $Subsystem -t upload *> upload.log
$ErrorActionPreference = "Stop"
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Upload failed!" -ForegroundColor Red
    Select-String -Path upload.log -Pattern "error|failed" -CaseSensitive | Select-Object -Last 20
    exit 1
}
Write-Host "Upload successful!" -ForegroundColor Green
Select-String -Path upload.log -Pattern "success|Writing|Wrote" | Select-Object -Last 10
Write-Host ""

Write-Host "=== Build and Upload Complete ===" -ForegroundColor Cyan
Write-Host "Logs saved to build.log and upload.log" -ForegroundColor Yellow
