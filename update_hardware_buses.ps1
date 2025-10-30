# Update hardware components to include bus references
# Run this script to add bus components to all hardware devices

# SPI devices (displays) - add spi_bus
$spiDevices = @(
    "gmt12864_display.json",
    "ili9341_3p5inch_tft.json",
    "ili9481_5inch_tft.json",
    "ili9486_4inch_tft.json",
    "ili9486_tft_800x480.json",
    "ili9488_tft_480x320.json",
    "ips_round_rgb_display.json",
    "ra8875_tft_1024x600.json",
    "ra8875_tft_800x480.json",
    "ssd1331_micro_rgb_oled.json",
    "ssd1351_rgb_oled.json",
    "ssd1963_tft_800x600.json",
    "st7735_tft_rgb.json",
    "st7789_premium_tft_rgb.json",
    "st7796s_tft_320x480.json",
    "st7796_tft_320x480.json",
    "tft_2p5x1p5_display.json"
)

# PWM devices (servos, LEDs) - add pwm_bus
$pwmDevices = @(
    "mg996r_servo.json",
    "color_changing_leds.json",
    "ws2812b_strip.json",
    "ws2812_led_strip.json"
)

# I2C devices - add i2c_bus
$i2cDevices = @(
    "hw496_microphone.json"
)

# GPIO devices - add gpio_bus
$gpioDevices = @(
    "pir_motion_sensor.json",
    "capacitive_touch_sensor.json"
)

# ADC devices - add adc_bus
$adcDevices = @(
    # Add ADC devices here when identified
)

function Add-BusComponent {
    param(
        [string]$filePath,
        [string]$busComponent
    )

    $content = Get-Content $filePath -Raw | ConvertFrom-Json

    if (-not $content.software) {
        $content | Add-Member -MemberType NoteProperty -Name "software" -Value @{}
    }

    if (-not $content.software.components) {
        $content.software | Add-Member -MemberType NoteProperty -Name "components" -Value @()
    }

    # Check if bus component is already present
    $busPresent = $content.software.components | Where-Object { $_ -eq $busComponent }
    if (-not $busPresent) {
        $content.software.components += $busComponent
        Write-Host "Added $busComponent to $filePath"
    } else {
        Write-Host "$busComponent already present in $filePath"
    }

    $content | ConvertTo-Json -Depth 10 | Set-Content $filePath
}

# Update SPI devices
foreach ($device in $spiDevices) {
    $filePath = "config/components/hardware/$device"
    if (Test-Path $filePath) {
        Add-BusComponent -filePath $filePath -busComponent "config/components/drivers/spi_bus.json"
    }
}

# Update PWM devices
foreach ($device in $pwmDevices) {
    $filePath = "config/components/hardware/$device"
    if (Test-Path $filePath) {
        Add-BusComponent -filePath $filePath -busComponent "config/components/drivers/pwm_bus.json"
    }
}

# Update I2C devices
foreach ($device in $i2cDevices) {
    $filePath = "config/components/hardware/$device"
    if (Test-Path $filePath) {
        Add-BusComponent -filePath $filePath -busComponent "config/components/drivers/i2c_bus.json"
    }
}

# Update GPIO devices
foreach ($device in $gpioDevices) {
    $filePath = "config/components/hardware/$device"
    if (Test-Path $filePath) {
        Add-BusComponent -filePath $filePath -busComponent "config/components/drivers/gpio_bus.json"
    }
}

# Update ADC devices
foreach ($device in $adcDevices) {
    $filePath = "config/components/hardware/$device"
    if (Test-Path $filePath) {
        Add-BusComponent -filePath $filePath -busComponent "config/components/drivers/adc_bus.json"
    }
}

Write-Host "Hardware component updates completed!"