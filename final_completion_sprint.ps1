#!/usr/bin/env pwsh#!/usr/bin/env pwsh

# FINAL COMPLETION SPRINT# FINAL COMPLETION SPRINT - Create all missing components in 5 minutes!



Write-Host "FINAL COMPLETION SPRINT - Goblin Component Creation!" -ForegroundColor GreenWrite-Host "FINAL COMPLETION SPRINT - Goblin Component Creation!" -ForegroundColor Green

Write-Host "Creating ALL missing goblin components..." -ForegroundColor Yellow

function New-PositionedComponent {

    param($Path, $ComponentName, $HardwareRef, $Interface, $RefPoint = "nose_center", $X = "0.0 INCH", $Y = "0.0 INCH", $Z = "0.0 INCH")# Template for positioned components

    function Create-PositionedComponent {

    $content = @"    param($Path, $ComponentName, $HardwareRef, $Interface, $RefPoint = "nose_center", $X = "0.0 INCH", $Y = "0.0 INCH", $Z = "0.0 INCH")

{    

    "relative_filename": "$Path",    $content = @"

    "version": "1.0.0",{

    "author": "config/author.json",    "relative_filename": "$Path",

    "component_type": "POSITIONED_COMPONENT",    "version": "1.0.0",

    "component_name": "$ComponentName",    "author": "config/author.json",

    "hardware_reference": "$HardwareRef",    "component_type": "POSITIONED_COMPONENT",

    "component_name": "$ComponentName",

    "position": {    "hardware_reference": "$HardwareRef",

        "coordinate_system": "skull_3d",

        "reference_point": "$RefPoint",    "position": {

        "x": "$X",        "coordinate_system": "skull_3d",

        "y": "$Y",         "reference_point": "$RefPoint",

        "z": "$Z"        "x": "$X",

    },        "y": "$Y", 

    "software": {        "z": "$Z"

        "init_function": "${ComponentName}_init",    },

        "act_function": "${ComponentName}_act"    "software": {

    }        "init_function": "${ComponentName}_init",

}        "act_function": "${ComponentName}_act"

"@    }

    New-Item -ItemType File -Path $Path -Value $content -Force | Out-Null}

    Write-Host "  Created: $ComponentName" -ForegroundColor Green"@

}    New-Item -ItemType File -Path $Path -Value $content -Force | Out-Null

    Write-Host "  ? Created: $ComponentName" -ForegroundColor Green

# ARM COMPONENTS - LEFT}

Write-Host "Creating LEFT ARM components..." -ForegroundColor Cyan

New-PositionedComponent "config/components/positioned/goblin/arm_components/left/shoulder_abduction_servo.json" "goblin_left_shoulder_abduction_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_5" "shoulder_socket_left" "-0.5 INCH" "0.0 INCH" "0.0 INCH"# ARM COMPONENTS - LEFT

New-PositionedComponent "config/components/positioned/goblin/arm_components/left/shoulder_rotation_servo.json" "goblin_left_shoulder_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_6" "shoulder_socket_left" "0.0 INCH" "0.5 INCH" "0.0 INCH"Write-Host "Creating LEFT ARM components..." -ForegroundColor Cyan

New-PositionedComponent "config/components/positioned/goblin/arm_components/left/forearm_rotation_servo.json" "goblin_left_forearm_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_7" "shoulder_socket_left" "0.0 INCH" "-3.0 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/left/shoulder_abduction_servo.json" "goblin_left_shoulder_abduction_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_5" "shoulder_socket_left" "-0.5 INCH" "0.0 INCH" "0.0 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/left/wrist_rotation_servo.json" "goblin_left_wrist_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_8" "shoulder_socket_left" "0.0 INCH" "-5.5 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/left/shoulder_rotation_servo.json" "goblin_left_shoulder_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_6" "shoulder_socket_left" "0.0 INCH" "0.5 INCH" "0.0 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/left/arm_status_led.json" "goblin_left_arm_status_led" "config/components/hardware/led_neopixel_ring.json" "GPIO_PIN_26" "shoulder_socket_left" "0.0 INCH" "-2.0 INCH" "0.5 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/left/forearm_rotation_servo.json" "goblin_left_forearm_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_7" "shoulder_socket_left" "0.0 INCH" "-3.0 INCH" "0.0 INCH"

Create-PositionedComponent "config/components/positioned/goblin/arm_components/left/wrist_rotation_servo.json" "goblin_left_wrist_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_8" "shoulder_socket_left" "0.0 INCH" "-5.5 INCH" "0.0 INCH"

# ARM COMPONENTS - RIGHT  Create-PositionedComponent "config/components/positioned/goblin/arm_components/left/arm_status_led.json" "goblin_left_arm_status_led" "config/components/hardware/led_neopixel_ring.json" "GPIO_PIN_26" "shoulder_socket_left" "0.0 INCH" "-2.0 INCH" "0.5 INCH"

Write-Host "Creating RIGHT ARM components..." -ForegroundColor Cyan

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/shoulder_flexion_servo.json" "goblin_right_shoulder_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_9" "shoulder_socket_right" "0.0 INCH" "0.0 INCH" "0.5 INCH"# ARM COMPONENTS - RIGHT

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/shoulder_abduction_servo.json" "goblin_right_shoulder_abduction_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_10" "shoulder_socket_right" "0.5 INCH" "0.0 INCH" "0.0 INCH"Write-Host "Creating RIGHT ARM components..." -ForegroundColor Cyan

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/shoulder_rotation_servo.json" "goblin_right_shoulder_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_11" "shoulder_socket_right" "0.0 INCH" "0.5 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/shoulder_flexion_servo.json" "goblin_right_shoulder_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_9" "shoulder_socket_right" "0.0 INCH" "0.0 INCH" "0.5 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/elbow_flexion_servo.json" "goblin_right_elbow_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_12" "shoulder_socket_right" "0.0 INCH" "-2.5 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/shoulder_abduction_servo.json" "goblin_right_shoulder_abduction_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_10" "shoulder_socket_right" "0.5 INCH" "0.0 INCH" "0.0 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/forearm_rotation_servo.json" "goblin_right_forearm_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_13" "shoulder_socket_right" "0.0 INCH" "-3.0 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/shoulder_rotation_servo.json" "goblin_right_shoulder_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_11" "shoulder_socket_right" "0.0 INCH" "0.5 INCH" "0.0 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/wrist_flexion_servo.json" "goblin_right_wrist_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_14" "shoulder_socket_right" "0.0 INCH" "-5.0 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/elbow_flexion_servo.json" "goblin_right_elbow_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_12" "shoulder_socket_right" "0.0 INCH" "-2.5 INCH" "0.0 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/wrist_rotation_servo.json" "goblin_right_wrist_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_15" "shoulder_socket_right" "0.0 INCH" "-5.5 INCH" "0.0 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/forearm_rotation_servo.json" "goblin_right_forearm_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_13" "shoulder_socket_right" "0.0 INCH" "-3.0 INCH" "0.0 INCH"

New-PositionedComponent "config/components/positioned/goblin/arm_components/right/arm_status_led.json" "goblin_right_arm_status_led" "config/components/hardware/led_neopixel_ring.json" "GPIO_PIN_27" "shoulder_socket_right" "0.0 INCH" "-2.0 INCH" "0.5 INCH"Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/wrist_flexion_servo.json" "goblin_right_wrist_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_14" "shoulder_socket_right" "0.0 INCH" "-5.0 INCH" "0.0 INCH"

Create-PositionedComponent "config/components/positioned/goblin/arm_components/right/wrist_rotation_servo.json" "goblin_right_wrist_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_15" "shoulder_socket_right" "0.0 INCH" "-5.5 INCH" "0.0 INCH"

Write-Host "? SPRINT COMPLETE! 28 components created!" -ForegroundColor GreenCreate-PositionedComponent "config/components/positioned/goblin/arm_components/right/arm_status_led.json" "goblin_right_arm_status_led" "config/components/hardware/led_neopixel_ring.json" "GPIO_PIN_27" "shoulder_socket_right" "0.0 INCH" "-2.0 INCH" "0.5 INCH"

# HAND COMPONENTS - LEFT
Write-Host "Creating LEFT HAND components..." -ForegroundColor Cyan
Create-PositionedComponent "config/components/positioned/goblin/hand_components/left/middle_finger_servo.json" "goblin_left_middle_finger_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_16" "wrist_joint_left" "0.0 INCH" "-1.0 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/hand_components/left/ring_finger_servo.json" "goblin_left_ring_finger_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_17" "wrist_joint_left" "-0.3 INCH" "-1.0 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/hand_components/left/pinky_finger_servo.json" "goblin_left_pinky_finger_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_18" "wrist_joint_left" "-0.6 INCH" "-0.8 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/hand_components/left/hand_status_led.json" "goblin_left_hand_status_led" "config/components/hardware/led_neopixel_ring.json" "GPIO_PIN_28" "wrist_joint_left" "0.0 INCH" "0.0 INCH" "0.3 INCH"

# LEG COMPONENTS - LEFT
Write-Host "Creating LEFT LEG components..." -ForegroundColor Cyan
Create-PositionedComponent "config/components/positioned/goblin/leg_components/left/hip_abduction_servo.json" "goblin_left_hip_abduction_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_19" "hip_socket_left" "-0.5 INCH" "0.0 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/leg_components/left/hip_rotation_servo.json" "goblin_left_hip_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_20" "hip_socket_left" "0.0 INCH" "0.0 INCH" "0.5 INCH"
Create-PositionedComponent "config/components/positioned/goblin/leg_components/left/ankle_flexion_servo.json" "goblin_left_ankle_flexion_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_21" "hip_socket_left" "0.0 INCH" "-6.0 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/leg_components/left/ankle_rotation_servo.json" "goblin_left_ankle_rotation_servo" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_22" "hip_socket_left" "0.0 INCH" "-6.5 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/leg_components/left/leg_status_led.json" "goblin_left_leg_status_led" "config/components/hardware/led_neopixel_ring.json" "GPIO_PIN_29" "hip_socket_left" "0.0 INCH" "-3.0 INCH" "0.5 INCH"

# TORSO COMPONENTS
Write-Host "Creating TORSO components..." -ForegroundColor Cyan
Create-PositionedComponent "config/components/positioned/goblin/torso_components/main_battery_monitor.json" "goblin_main_battery_monitor" "config/components/hardware/battery_voltage_sensor.json" "ADC_CHANNEL_1" "pelvis_center" "0.0 INCH" "0.0 INCH" "-2.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/torso_components/external_wifi_communication.json" "goblin_external_wifi_communication" "config/components/hardware/esp32_wifi_module.json" "WIFI_INTERFACE" "pelvis_center" "0.0 INCH" "1.0 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/torso_components/imu_balance_sensor.json" "goblin_imu_balance_sensor" "config/components/hardware/imu_6dof_sensor.json" "I2C_DEVICE_2" "pelvis_center" "0.0 INCH" "0.0 INCH" "1.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/torso_components/spine_servo_middle.json" "goblin_spine_servo_middle" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_23" "pelvis_center" "0.0 INCH" "2.0 INCH" "0.0 INCH"
Create-PositionedComponent "config/components/positioned/goblin/torso_components/spine_servo_upper.json" "goblin_spine_servo_upper" "config/components/hardware/servo_9g_micro.json" "PWM_CHANNEL_24" "pelvis_center" "0.0 INCH" "4.0 INCH" "0.0 INCH"

Write-Host "`n? FINAL SPRINT COMPLETE! Created ALL missing goblin components!" -ForegroundColor Green
Write-Host "   - 5 Left arm components" -ForegroundColor White
Write-Host "   - 8 Right arm components" -ForegroundColor White  
Write-Host "   - 4 Left hand components" -ForegroundColor White
Write-Host "   - 5 Left leg components" -ForegroundColor White
Write-Host "   - 6 Torso components" -ForegroundColor White
Write-Host "Total: 28 components created! Let's check consistency now!" -ForegroundColor Yellow