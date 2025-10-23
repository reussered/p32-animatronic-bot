# P32 Component Dispatch Tables - Auto-generated CMake
# Includes all component source files for build system

set(P32_COMPONENT_SOURCES
    components/heartbeat.cpp
    components/network_monitor.cpp
    components/test_head.cpp
    components/goblin_left_eye.cpp
    components/goblin_eye.cpp
    components/gc9a01.cpp
    components/goblin_right_eye.cpp
    components/goblin_eye.cpp
    components/gc9a01.cpp
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