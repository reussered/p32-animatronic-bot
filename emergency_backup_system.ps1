?#  EMERGENCY BACKUP SYSTEM - PREVENT WORK LOSS
# This script MUST be run before any major operations
# Created: October 21, 2025
# Purpose: Prevent loss of dozens of hours of work

param(
    [string]$BackupReason = "Manual backup",
    [switch]$ForceBackup = $false
)

$ErrorActionPreference = "Stop"

# Define backup locations
$BACKUP_BASE = "F:\EMERGENCY_BACKUPS\P32_ANIMATRONIC_BOT"
$GITHUB_SOURCE = "F:\GitHub\p32-animatronic-bot"
$ROOT_SOURCE = "F:\p32-animatronic-bot"
$TIMESTAMP = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"

Write-Host " EMERGENCY BACKUP SYSTEM ACTIVATED" -ForegroundColor Red -BackgroundColor Yellow
Write-Host "Reason: $BackupReason" -ForegroundColor Yellow
Write-Host "Timestamp: $TIMESTAMP" -ForegroundColor Green

# Create backup directory structure
$BACKUP_DIR = "$BACKUP_BASE\BACKUP_$TIMESTAMP"
New-Item -ItemType Directory -Path $BACKUP_DIR -Force | Out-Null
New-Item -ItemType Directory -Path "$BACKUP_DIR\GITHUB_VERSION" -Force | Out-Null
New-Item -ItemType Directory -Path "$BACKUP_DIR\ROOT_VERSION" -Force | Out-Null

# Backup GitHub version (current working version)
Write-Host " Backing up GitHub version (F:\GitHub\p32-animatronic-bot)..." -ForegroundColor Cyan
if (Test-Path $GITHUB_SOURCE) {
    robocopy "$GITHUB_SOURCE" "$BACKUP_DIR\GITHUB_VERSION" /E /XD .git .pio .venv __pycache__ .vs /XF *.log *.tmp /NDL /NJH /NJS
    $githubFiles = (Get-ChildItem -Path "$BACKUP_DIR\GITHUB_VERSION" -Recurse -File | Measure-Object).Count
    Write-Host " GitHub version: $githubFiles files backed up" -ForegroundColor Green
} else {
    Write-Host " GitHub version not found!" -ForegroundColor Yellow
}

# Backup root version (potential lost work location)
Write-Host " Backing up root version (F:\p32-animatronic-bot)..." -ForegroundColor Cyan
if (Test-Path $ROOT_SOURCE) {
    robocopy "$ROOT_SOURCE" "$BACKUP_DIR\ROOT_VERSION" /E /XD .git .pio .venv __pycache__ .vs /XF *.log *.tmp /NDL /NJH /NJS
    $rootFiles = (Get-ChildItem -Path "$BACKUP_DIR\ROOT_VERSION" -Recurse -File | Measure-Object).Count
    Write-Host " Root version: $rootFiles files backed up" -ForegroundColor Green
} else {
    Write-Host " Root version not found!" -ForegroundColor Yellow
}

# Create backup manifest
$manifest = @"
EMERGENCY BACKUP MANIFEST
========================
Backup Date: $(Get-Date)
Backup Reason: $BackupReason
Backup Location: $BACKUP_DIR

GITHUB VERSION (F:\GitHub\p32-animatronic-bot):
- Files: $githubFiles
- Last Modified: $(if (Test-Path $GITHUB_SOURCE) { (Get-Item $GITHUB_SOURCE).LastWriteTime } else { "NOT FOUND" })

ROOT VERSION (F:\p32-animatronic-bot):
- Files: $rootFiles
- Last Modified: $(if (Test-Path $ROOT_SOURCE) { (Get-Item $ROOT_SOURCE).LastWriteTime } else { "NOT FOUND" })

CRITICAL FILES BACKED UP:
- All source code (.cpp, .hpp, .c, .h)
- All configuration files (.json)
- All documentation (.md)
- All build scripts (.ps1, .bat)
- All project files (platformio.ini, CMakeLists.txt)

EXCLUDED FROM BACKUP:
- .git directories (version control)
- .pio build artifacts
- .venv virtual environments
- __pycache__ Python cache
- .vs Visual Studio cache
- Temporary files (.log, .tmp)

To restore from this backup:
1. Copy desired version to working location
2. Verify all files are present
3. Run validation scripts to ensure integrity
"@

$manifest | Out-File -FilePath "$BACKUP_DIR\BACKUP_MANIFEST.txt" -Encoding UTF8

Write-Host " Backup manifest created: $BACKUP_DIR\BACKUP_MANIFEST.txt" -ForegroundColor Green
Write-Host " BACKUP COMPLETE!" -ForegroundColor Green -BackgroundColor Black
Write-Host "Location: $BACKUP_DIR" -ForegroundColor Yellow

# Create quick access shortcut
Write-Host " Creating backup shortcuts for easy access..." -ForegroundColor Cyan
$desktop = [Environment]::GetFolderPath("Desktop")
$shortcutPath = "$desktop\P32_LATEST_BACKUP.lnk"
$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut($shortcutPath)
$Shortcut.TargetPath = $BACKUP_DIR
$Shortcut.Description = "Latest P32 Animatronic Bot Emergency Backup - $TIMESTAMP"
$Shortcut.Save()

Write-Host " Desktop shortcut created: P32_LATEST_BACKUP.lnk" -ForegroundColor Green
Write-Host ""
Write-Host " NEVER LOSE WORK AGAIN! " -ForegroundColor Red -BackgroundColor Yellow
Write-Host "This backup is saved outside all repositories and build systems." -ForegroundColor Yellow