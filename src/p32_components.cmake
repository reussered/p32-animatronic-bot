# P32 Individual Component List - Auto-generated
# Use this to selectively enable/disable components for memory analysis

set(P32_COMPONENT_SOURCES
    components/system_core.c
    components/network_monitor.c
    components/goblin_eye_left.c
    components/goblin_eye_right.c
    components/goblin_nose.c
    p32_component_tables.c
)

# Individual component targets for memory analysis
foreach(component_file ${P32_COMPONENT_SOURCES})
    get_filename_component(component_name ${component_file} NAME_WE)
    # message(STATUS "P32 Component: ${component_name}")  
endforeach()