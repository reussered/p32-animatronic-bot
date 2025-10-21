?#  WORK PROTECTION SYSTEM
# AUTOMATIC BACKUP BEFORE ANY RISKY OPERATIONS
# This script prevents loss of work by creating backups before dangerous operations

param(
    [Parameter(Mandatory=$true)]
    [string]$Operation,
    [string]$AdditionalInfo = ""
)

$ErrorActionPreference = "Stop"

Write-Host " WORK PROTECTION SYSTEM ACTIVATED" -ForegroundColor Green -BackgroundColor Black
Write-Host "Operation: $Operation" -ForegroundColor Yellow
Write-Host "Additional Info: $AdditionalInfo" -ForegroundColor Cyan

# List of operations that REQUIRE backup
$RISKY_OPERATIONS = @(
    "git clone",
    "git submodule",
    "Remove-Item",
    "rm -rf",
    "git rm",
    "git reset --hard",
    "git clean",
    "directory cleanup",
    "repository operations",
    "major refactoring",
    "component generation",
    "file renaming",
    "validation fixes"
)

# Check if operation is risky
$isRisky = $false
foreach ($riskyOp in $RISKY_OPERATIONS) {
    if ($Operation -like "*$riskyOp*") {
        $isRisky = $true
        break
    }
}

if ($isRisky -or $Operation -eq "force") {
    Write-Host " RISKY OPERATION DETECTED!" -ForegroundColor Red
    Write-Host "Creating emergency backup before proceeding..." -ForegroundColor Yellow
    
    # Run emergency backup
    & "$PSScriptRoot\EMERGENCY_BACKUP_SYSTEM.ps1" -BackupReason "BEFORE: $Operation - $AdditionalInfo"
    
    Write-Host " Backup complete. Operation can proceed safely." -ForegroundColor Green
} else {
    Write-Host " Operation appears safe, but backup recommended." -ForegroundColor Green
    Write-Host "To force backup, run: .\WORK_PROTECTION.ps1 -Operation 'force'" -ForegroundColor Cyan
}

Write-Host ""
Write-Host " YOUR WORK IS PROTECTED" -ForegroundColor Green -BackgroundColor Black