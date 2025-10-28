# P32 Component Dispatch Tables - Auto-generated CMake
# Includes all component source files for build system

set(P32_COMPONENT_SOURCES
    components/heartbeat.cpp
    components/network_monitor.cpp
    components/goblin_head.cpp
    components/spi_bus.cpp
    components/goblin_left_eye.cpp
    components/goblin_eye.cpp
    components/gc9a01.cpp
    components/generic_spi_display.cpp
    components/goblin_right_eye.cpp
    components/goblin_eye.cpp
    components/gc9a01.cpp
    components/generic_spi_display.cpp
    components/goblin_nose.cpp
    components/hc_sr04_ultrasonic_distance_sensor.cpp
    components/goblin_mouth.cpp
    components/goblin_mouth.cpp
    components/gc9a01.cpp
    components/generic_spi_display.cpp
    components/goblin_speaker.cpp
    components/speaker.cpp
    components/goblin_left_ear.cpp
    components/servo_sg90_micro.cpp
    components/goblin_right_ear.cpp
    components/servo_sg90_micro.cpp
    components/goblin_torso.cpp
    components/spine_flexion_servo.cpp
    components/servo_sg90_micro.cpp
    components/spine_extension_servo.cpp
    components/servo_sg90_micro.cpp
    components/waist_rotation_servo.cpp
    components/servo_sg90_micro.cpp
    components/torso_status_led.cpp
    components/servo_sg90_micro.cpp
    components/torso_speaker.cpp
    components/servo_sg90_micro.cpp
    component_tables.cpp
)

# Add component source files to build
target_sources(${CMAKE_PROJECT_NAME}.elf PRIVATE
    ${P32_COMPONENT_SOURCES}
)

# Component include directories
target_include_directories(${CMAKE_PROJECT_NAME}.elf PRIVATE
    include/components
    include
)