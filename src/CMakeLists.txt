# P32 Universal Component System - Individual component files
# Each component can be commented out for memory analysis

# Include auto-generated component sources
include(p32_components.cmake)

# Always include both main.cpp and simple_display_test.cpp
# main.cpp will decide which one to call based on SIMPLE_DISPLAY_TEST define
set(app_sources
    main.cpp
    simple_display_test.cpp
    ${P32_COMPONENT_SOURCES}
)

message(STATUS "P32 sources: ${app_sources}")

idf_component_register(SRCS ${app_sources})
