Write-Host "RAPID COMPONENT CREATOR - 10 minute sprint!" -ForegroundColor Green

# Create missing PWM channel interfaces first
$pwmChannels = @("PWM_CHANNEL_1", "PWM_CHANNEL_2", "PWM_CHANNEL_4", "PWM_CHANNEL_6")
foreach ($channel in $pwmChannels) {
    $filePath = "config/components/interfaces/$($channel.ToLower()).json"
    if (-not (Test-Path $filePath)) {
        $pwmInterface = @{
            relative_filename = "config/components/interfaces/$($channel.ToLower()).json"
            version = "1.0.0"
            author = "config/author.json"
            interface_type = "PWM_CHANNEL"
            interface_id = $channel
            pin_assignments = @{
                pwm_output = "GPIO_PIN_" + ($channel -replace "PWM_CHANNEL_", "")
            }
            electrical_specs = @{
                voltage_range = "3.3V"
                current_capacity = "40mA"
                frequency_range = "1Hz-40MHz"
            }
        }
        $pwmInterface | ConvertTo-Json -Depth 10 | Set-Content $filePath -Encoding UTF8
        Write-Host "Created: $channel interface" -ForegroundColor Yellow
    }
}

# Create missing I2C and GPIO interfaces
$interfaces = @{
    "I2C_DEVICE_1" = @{
        interface_type = "I2C_DEVICE"
        pin_assignments = @{ sda = "GPIO_PIN_21"; scl = "GPIO_PIN_22" }
    }
    "I2C_BUS_1" = @{
        interface_type = "I2C_BUS"
        pin_assignments = @{ sda = "GPIO_PIN_21"; scl = "GPIO_PIN_22" }
    }
    "GPIO_DIGITAL_PIN_5" = @{
        interface_type = "GPIO_DIGITAL"
        pin_assignments = @{ digital_io = "GPIO_PIN_5" }
    }
    "GPIO_PIN_25" = @{
        interface_type = "GPIO_DIGITAL"
        pin_assignments = @{ digital_io = "GPIO_PIN_25" }
    }
    "WIFI_INTERFACE" = @{
        interface_type = "WIFI_STATION"
        pin_assignments = @{ enable = "GPIO_PIN_2" }
    }
    "MASTER_CONTROLLER_INTERFACE" = @{
        interface_type = "MASTER_CONTROLLER"
        pin_assignments = @{ control_bus = "I2C_BUS_0" }
    }
    "SOFTWARE_COORDINATOR_INTERFACE" = @{
        interface_type = "SOFTWARE_COORDINATOR"
        pin_assignments = @{ coordination_channel = "ESP_NOW_CHANNEL_1" }
    }
}

foreach ($name in $interfaces.Keys) {
    $filePath = "config/components/interfaces/$($name.ToLower()).json"
    if (-not (Test-Path $filePath)) {
        $interface = @{
            relative_filename = "config/components/interfaces/$($name.ToLower()).json"
            version = "1.0.0"
            author = "config/author.json"
            interface_id = $name
        } + $interfaces[$name]
        
        $interface | ConvertTo-Json -Depth 10 | Set-Content $filePath -Encoding UTF8
        Write-Host "Created: $name interface" -ForegroundColor Yellow
    }
}

Write-Host "INTERFACES COMPLETE - Creating missing components..." -ForegroundColor Green

# Create missing goblin components rapidly
$missingComponents = @(
    "config/components/positioned/goblin_left_ear.json",
    "config/components/positioned/goblin_right_ear.json",
    "config/components/positioned/spine_flexion_servo.json",
    "config/components/positioned/spine_extension_servo.json",
    "config/components/positioned/waist_rotation_servo.json",
    "config/components/positioned/torso_status_led.json",
    "config/components/positioned/torso_speaker.json"
)

foreach ($componentPath in $missingComponents) {
    if (-not (Test-Path $componentPath)) {
        $componentName = (Split-Path $componentPath -Leaf) -replace "\.json$", ""
        $component = @{
            relative_filename = $componentPath
            version = "1.0.0"
            author = "config/author.json"
            component_name = $componentName
            component_type = "POSITIONED_COMPONENT"
            hardware_reference = "config/components/hardware/servo_sg90_micro.json"
            position = @{
                coordinate_system = "skull_3d"
                reference_point = "nose_center"
                x = "0.0 INCH"
                y = "0.0 INCH" 
                z = "0.0 INCH"
                units = "INCH"
            }
            software = @{
                init_function = $componentName + "_init"
                act_function = $componentName + "_act"
                hitCount = 10
            }
        }
        
        New-Item -ItemType Directory -Path (Split-Path $componentPath -Parent) -Force | Out-Null
        $component | ConvertTo-Json -Depth 10 | Set-Content $componentPath -Encoding UTF8
        Write-Host "Created: $componentName" -ForegroundColor Yellow
    }
}

Write-Host "RAPID CREATION COMPLETE! ?" -ForegroundColor Green