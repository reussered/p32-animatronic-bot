# P32 Universal Component System - Individual component files
# Each component can be commented out for memory analysis

# Always include both main.cpp and simple_display_test.cpp
# main.cpp will decide which one to call based on SIMPLE_DISPLAY_TEST define
set(app_sources
    main.cpp
    simple_display_test.cpp
)

message(STATUS "P32 sources: ${app_sources}")

idf_component_register(SRCS ${app_sources})
