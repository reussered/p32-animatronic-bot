# P32 Universal Component System - Individual component files
# Each component can be commented out for memory analysis

# Core P32 source (always included)
set(P32_CORE_SOURCES
    main.cpp
)

# P32 Individual Components - Auto-generated to include all components
set(P32_COMPONENT_SOURCES
    component_tables.cpp            # Generated dispatch tables (initTable, actTable, hitCountTable)
    p32_component_functions.cpp     # Aggregates all individual component implementations
    p32_eye_display.cpp             # Eye animation system
    p32_web_client.cpp              # Web client for PC streaming (test mode)
    Mood.cpp                        # C++ Mood system with efficient delta calculations
    FrameProcessor.cpp              # C++ Frame processing with mood-based optimization
    core/memory/SharedMemory.cpp    # SharedMemory ESP-NOW mesh communication
    components/arm_controller.cpp
    components/audio.cpp
    components/bluetooth_central.cpp
    components/center_eye.cpp
    components/component_2_5x1_5_inch_tft_lcd_display.cpp
    components/elbow_flexion.cpp
    components/esp_now_mesh.cpp
    components/foot_controller.cpp
    components/forehead_leds.cpp
    components/gc9a01.cpp
    components/gc9a01_circular_lcd_display.cpp
    components/gc9a01_circular_lcd_display_verbose.cpp
    components/goblin_behavior_engine.cpp
    components/goblin_eye.cpp
    components/goblin_head.cpp
    components/goblin_mood.cpp
    components/goblin_mouth.cpp
    components/goblin_mouth_STUB.cpp
    components/goblin_nose.cpp
    components/goblin_personality.cpp
    components/goblin_pirate_head.cpp
    components/goblin_pirate_personality.cpp
    components/goblin_right_eye.cpp
    components/goblin_speaker.cpp
    components/goblin_speaker_STUB.cpp
    components/goblin_torso.cpp
    components/hand_controller.cpp
    components/hc_sr04_ultrasonic_distance_sensor.cpp
    components/hc_sr04_ultrasonic_distance_sensor_verbose.cpp
    components/heartbeat.cpp
    components/hip_flexion.cpp
    components/i2s_digital_speaker_module.cpp
    components/i2s_digital_speaker_module_verbose.cpp
    components/index_finger.cpp
    components/knee_flexion.cpp
    components/left_ear.cpp
    components/left_ear_microphone.cpp
    components/left_eye.cpp
    components/leg_controller.cpp
    components/master_controller.cpp
    components/mesh_coordinator.cpp
    components/mesh_software_coordinator.cpp
    components/mg996r_servo.cpp
    components/mouth.cpp
    components/network_monitor.cpp
    components/nose_sensor.cpp
    components/p32_comp_master_controller.cpp
    components/p32_comp_mesh_coordinator.cpp
    components/p32_comp_mesh_software_coordinator.cpp
    components/power_manager.cpp
    components/power_monitor.cpp
    components/right_ear_microphone.cpp
    components/right_eye.cpp
    components/serial_console.cpp
    components/servo_sg90_micro.cpp
    components/shoulder_flexion.cpp
    components/speaker.cpp
    components/spine_lower.cpp
    components/system_core.cpp
    components/system_init.cpp
    components/telemetry_hub.cpp
    components/thumb.cpp
    components/torso_controller.cpp
    components/torso_integrated_power_system.cpp
    components/unknown.cpp
    components/watchdog.cpp
    components/wifi_station.cpp
    components/wrist_flexion.cpp
)

# For memory analysis - uncomment this minimal set:
# set(P32_COMPONENT_SOURCES
#     components/system_init.c
#     # Add only one component at a time for precise measurement
# )

# Build complete source list
set(app_sources ${P32_CORE_SOURCES})
foreach(component ${P32_COMPONENT_SOURCES})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${component})
        list(APPEND app_sources ${component})
        get_filename_component(comp_name ${component} NAME_WE)
        message(STATUS "P32 Component: ${comp_name}")
    else()
        message(WARNING "Component not found: ${component}")
    endif()
endforeach()

message(STATUS "P32 Total sources: ${app_sources}")

idf_component_register(SRCS ${app_sources})
