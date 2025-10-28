# P32 Animatronic Bot - ESP-IDF CMakeLists.txt
# Main component for goblin_head environment

idf_component_register(
    SRCS
        "main.cpp"
        "../src/component_tables.cpp"
        "../src/p32_component_functions.cpp"
        "../src/components/goblin_head.src"
        "../src/components/goblin_left_eye.src"
        "../src/components/goblin_right_eye.src"
        "../src/components/goblin_mouth.src"
        "../src/components/goblin_speaker.src"
        "../src/components/goblin_nose.src"
        "../src/components/goblin_left_ear.src"
        "../src/components/goblin_right_ear.src"
        "../src/components/system_core.src"
        "../src/components/heartbeat.src"
        "../src/components/network_monitor.src"
        "../src/components/hc_sr04_ultrasonic_distance_sensor.src"
        "../src/components/speaker.src"
        "../src/components/spi_bus.src"
        "../src/components/gc9a01.src"
        "../src/components/generic_spi_display/generic_spi_display.src"
        "../src/components/SharedMemory.cpp"
        "../src/components/Mood.cpp"
        "../src/p32_web_client.cpp"
        "../src/goblin_full/heartbeat.src"
        "../src/goblin_full/network_monitor.src"
        "../src/goblin_full/test_head.src"
        "../src/goblin_full/goblin_eye.src"
        "../src/goblin_full/goblin_left_eye.src"
        "../src/goblin_full/goblin_right_eye.src"
        "../src/goblin_full/goblin_nose.src"
        "../src/goblin_full/goblin_head.src"
        "../src/goblin_full/goblin_mouth.src"
        "../src/goblin_full/goblin_speaker.src"
        "../src/goblin_full/goblin_left_ear.src"
        "../src/goblin_full/goblin_right_ear.src"
        "../src/goblin_full/mg996r_servo.src"
        "../src/goblin_full/servo_sg90_micro.src"
        "../src/goblin_full/torso_integrated_power_system.src"
        "../src/goblin_full/hc_sr04_ultrasonic_distance_sensor.src"
        "../src/goblin_full/speaker.src"

    INCLUDE_DIRS
        "../include"
        "../shared"

    REQUIRES
        arduinojson
)