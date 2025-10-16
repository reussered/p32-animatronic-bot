# P32 Universal Component System - PURE C++ SYSTEM
# All files converted to C++ for type-safe shared memory

# Core P32 source (always included)
set(P32_CORE_SOURCES
    main.cpp
)

# P32 Individual Components - Comment out to exclude from build
set(P32_COMPONENT_SOURCES
    p32_component_tables.cpp        # Generated table definitions (always needed)
    system_components.cpp           # Generated system components (heartbeat, network_monitor)
    bot_components.cpp              # Generated bot-specific components (eyes, nose, mouth, etc.)
    p32_eye_display.cpp             # Eye animation system
    p32_web_client.cpp              # Web client for PC streaming (test mode)
    p32_shared_memory.cpp           # C++ Shared memory with automatic mesh synchronization
    Mood.cpp                        # C++ Mood system with efficient delta calculations
    FrameProcessor.cpp              # C++ Frame processing with mood-based optimization
    components/system_core.cpp
    components/network_monitor.cpp  
    components/goblin_eye_left.cpp
    components/goblin_eye_right.cpp
    components/goblin_nose.cpp
)

# For memory analysis - uncomment this minimal set:
# set(P32_COMPONENT_SOURCES
#     components/system_init.cpp
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
