# Update component references from old positioned paths to new family/subsystem paths
Get-ChildItem -Path "f:\GitHub\p32-animatronic-bot\config" -Filter "*.json" -Recurse | ForEach-Object {
    $filePath = $_.FullName
    $content = Get-Content $filePath -Raw

    # Replace goblin component references
    $content = $content -replace 'config/components/positioned/goblin_left_eye\.json', 'config/bots/bot_families/goblins/head/goblin_left_eye.json'
    $content = $content -replace 'config/components/positioned/goblin_right_eye\.json', 'config/bots/bot_families/goblins/head/goblin_right_eye.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_left_eye\.json', 'config/bots/bot_families/goblins/head/goblin_left_eye.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_right_eye\.json', 'config/bots/bot_families/goblins/head/goblin_right_eye.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_mouth\.json', 'config/bots/bot_families/goblins/head/goblin_mouth.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_nose\.json', 'config/bots/bot_families/goblins/head/goblin_nose.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_speaker\.json', 'config/bots/bot_families/goblins/head/goblin_speaker.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_left_ear\.json', 'config/bots/bot_families/goblins/head/goblin_left_ear.json'
    $content = $content -replace 'config/components/positioned/goblin/head_components/goblin_right_ear\.json', 'config/bots/bot_families/goblins/head/goblin_right_ear.json'

    # Replace other positioned component references (these might need to go to system/ or other locations)
    $content = $content -replace 'config/components/positioned/neck_pan_servo\.json', 'config/components/system/neck_pan_servo.json'
    $content = $content -replace 'config/components/positioned/neck_tilt_servo\.json', 'config/components/system/neck_tilt_servo.json'
    $content = $content -replace 'config/components/positioned/spine_flexion_servo\.json', 'config/components/system/spine_flexion_servo.json'
    $content = $content -replace 'config/components/positioned/spine_extension_servo\.json', 'config/components/system/spine_extension_servo.json'
    $content = $content -replace 'config/components/positioned/waist_rotation_servo\.json', 'config/components/system/waist_rotation_servo.json'
    $content = $content -replace 'config/components/positioned/torso_status_led\.json', 'config/components/system/torso_status_led.json'
    $content = $content -replace 'config/components/positioned/torso_speaker\.json', 'config/components/system/torso_speaker.json'

    # Replace humanoid component references
    $content = $content -replace 'config/components/positioned/humanoid_head_assembly\.json', 'config/bots/bot_families/humanoids/head/humanoid_head_assembly.json'
    $content = $content -replace 'config/components/positioned/humanoid_neck_assembly\.json', 'config/bots/bot_families/humanoids/head/humanoid_neck_assembly.json'
    $content = $content -replace 'config/components/positioned/humanoid_pelvis_assembly\.json', 'config/bots/bot_families/humanoids/torso/humanoid_pelvis_assembly.json'
    $content = $content -replace 'config/components/positioned/humanoid_spine_vertebrae\.json', 'config/bots/bot_families/humanoids/torso/humanoid_spine_vertebrae.json'
    $content = $content -replace 'config/components/positioned/humanoid_shoulder_left\.json', 'config/bots/bot_families/humanoids/arms/humanoid_shoulder_left.json'
    $content = $content -replace 'config/components/positioned/humanoid_shoulder_right\.json', 'config/bots/bot_families/humanoids/arms/humanoid_shoulder_right.json'
    $content = $content -replace 'config/components/positioned/humanoid_elbow_left\.json', 'config/bots/bot_families/humanoids/arms/humanoid_elbow_left.json'
    $content = $content -replace 'config/components/positioned/humanoid_elbow_right\.json', 'config/bots/bot_families/humanoids/arms/humanoid_elbow_right.json'
    $content = $content -replace 'config/components/positioned/humanoid_wrist_left\.json', 'config/bots/bot_families/humanoids/arms/humanoid_wrist_left.json'
    $content = $content -replace 'config/components/positioned/humanoid_wrist_right\.json', 'config/bots/bot_families/humanoids/arms/humanoid_wrist_right.json'
    $content = $content -replace 'config/components/positioned/humanoid_hip_left\.json', 'config/bots/bot_families/humanoids/legs/humanoid_hip_left.json'
    $content = $content -replace 'config/components/positioned/humanoid_hip_right\.json', 'config/bots/bot_families/humanoids/legs/humanoid_hip_right.json'
    $content = $content -replace 'config/components/positioned/humanoid_knee_left\.json', 'config/bots/bot_families/humanoids/legs/humanoid_knee_left.json'
    $content = $content -replace 'config/components/positioned/humanoid_knee_right\.json', 'config/bots/bot_families/humanoids/legs/humanoid_knee_right.json'

    Set-Content -Path $filePath -Value $content
    Write-Host "Updated references in $filePath"
}