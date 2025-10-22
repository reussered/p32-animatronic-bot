# P32 Individual Component List - Auto-generated
# Use this to selectively enable/disable components for memory analysis

set(P32_COMPONENT_SOURCES
    components/system_core.c
    components/network_monitor.c
    components/unknown_component.c
    components/goblin_left_eye.c
    components/goblin_right_eye.c
    components/goblin_nose.c
    components/goblin_mouth.c
    components/goblin_speaker.c
    components/goblin_ear_left.c
    components/goblin_ear_right.c
    components/unknown_component.c
    p32_component_tables.c
)

# Individual component targets for memory analysis
foreach(component_file ${P32_COMPONENT_SOURCES})
    get_filename_component(component_name ${component_file} NAME_WE)
    # message(STATUS "P32 Component: ${component_name}")  
endforeach()