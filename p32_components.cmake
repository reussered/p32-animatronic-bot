# P32 Component Dispatch Tables - Auto-generated CMake
# Includes all component source files for build system

set(P32_COMPONENT_SOURCES
    components/heartbeat.cpp
    components/network_monitor.cpp
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