# P32 Individual Component List - Auto-generated
# Use this to selectively enable/disable components for memory analysis

set(P32_COMPONENT_SOURCES
    components/system_core.cpp
    components/network_monitor.cpp
    components/unknown_component.cpp
    components/unknown_component.cpp
    p32_component_tables.cpp
)

# Individual component targets for memory analysis
foreach(component_file ${P32_COMPONENT_SOURCES})
    get_filename_component(component_name ${component_file} NAME_WE)
    # message(STATUS "P32 Component: ${component_name}")  
endforeach()