# P32 Universal Component System - Individual component files
# Each component can be commented out for memory analysis

# Core P32 source (always included)
set(P32_CORE_SOURCES
    main.c
)

# P32 Individual Components - Comment out to exclude from build
set(P32_COMPONENT_SOURCES
    p32_component_tables.c          # Generated table definitions (always needed)
    p32_eye_display.c               # Eye animation system
    p32_web_client.c                # Web client for PC streaming (test mode)
    Mood.cpp                        # C++ Mood system with efficient delta calculations
    FrameProcessor.cpp              # C++ Frame processing with mood-based optimization
    components/system_core.c
    components/network_monitor.c  
    components/goblin_eye_left.c
    components/goblin_eye_right.c
    components/goblin_nose.c
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
