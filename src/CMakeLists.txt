# P32 Universal Component System - Individual component files
# Each component can be commented out for memory analysis

# Always include both main.cpp and simple_display_test.cpp
# main.cpp will decide which one to call based on SIMPLE_DISPLAY_TEST define
set(app_sources
    main.cpp
    simple_display_test.cpp
    component_tables.cpp
    components/heartbeat.cpp
    components/network_monitor.cpp
    components/test_head.cpp
    components/goblin_left_eye.cpp
    components/spi_bus.cpp
    components/goblin_eye.cpp
    components/gc9a01.cpp
    components/generic_spi_display.cpp
    components/goblin_right_eye.cpp
)

message(STATUS "P32 sources: ${app_sources}")

idf_component_register(SRCS ${app_sources})
