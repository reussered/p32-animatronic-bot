@echo off
echo Starting Canon Printer Keep-Alive...
echo This will ping your LBP632C every 90 seconds to keep it awake
echo Press Ctrl+C to stop
echo.
powershell.exe -ExecutionPolicy Bypass -File "%~dp0printer-keepalive.ps1"
pause