# Auto-retry serial monitor script
# Keeps trying to open COM3 until ESP32 releases the port

Write-Host "Waiting for ESP32 to release COM3 port..."
Write-Host "Hold BOOT, press RST, then release BOOT when monitoring starts"
Write-Host ""

while ($true) {
    Write-Host "Attempting to open COM3..." -NoNewline

    # Try to run the serial monitor
    $process = Start-Process -FilePath "pio" -ArgumentList "device", "monitor", "--port", "COM3", "--baud", "115200" -NoNewWindow -PassThru -Wait

    if ($process.ExitCode -eq 0) {
        Write-Host "Success!"
        break
    } else {
        Write-Host "Port busy, retrying in 500ms..."
        Start-Sleep -Milliseconds 500
    }
}

Write-Host "Serial monitor started successfully!"