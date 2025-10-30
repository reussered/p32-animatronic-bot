@echo off
echo ========================================
echo P32 Dual Display Test Builder
echo ========================================
cd /d f:\GitHub\p32-animatronic-bot

echo.
echo Attempting to build dual display test...
echo.

pio run -e goblin_head --target upload --target monitor

if %errorlevel% neq 0 (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    echo.
    echo Alternative: Use the Arduino sketch!
    echo.
    echo 1. Open simple_dual_test.ino in Arduino IDE
    echo 2. Select ESP32-S3 board
    echo 3. Upload to your ESP32
    echo.
    echo The sketch will test both displays with GPIO 15/16
    echo.
    pause
) else (
    echo.
    echo ========================================
    echo SUCCESS! Displays should be testing now!
    echo ========================================
    echo.
    echo Check your displays - they should show test patterns.
    echo.
    pause
)