@echo off
REM P32 Auto-Build Batch Script
REM Usage: build_bot.bat <bot_name>
REM Example: build_bot.bat test_bot

if "%~1"=="" (
    echo Usage: build_bot.bat ^<bot_name^>
    echo Example: build_bot.bat test_bot
    exit /b 1
)

echo Building bot: %1
python auto_build.py %1

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build completed successfully!
) else (
    echo.
    echo Build failed!
    exit /b 1
)