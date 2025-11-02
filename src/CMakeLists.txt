# Configure the core application component to build only the
# generated subsystem sources alongside shared infrastructure.

set(app_sources SharedMemory.cpp)

get_filename_component(P32_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(P32_GENERATED_FILE "${P32_PROJECT_ROOT}/p32_generated_sources.cmake")

if(EXISTS "${P32_GENERATED_FILE}")
	include("${P32_GENERATED_FILE}")
else()
	message(FATAL_ERROR "Missing ${P32_GENERATED_FILE}. Run tools/generate_tables.py first.")
endif()

if(DEFINED P32_GENERATED_SOURCES AND P32_GENERATED_SOURCES)
    foreach(source_file ${P32_GENERATED_SOURCES})
        string(REGEX REPLACE "^src/" "" relative_path "${source_file}")
        list(APPEND app_sources ${relative_path})
    endforeach()
else()
	message(FATAL_ERROR "P32_GENERATED_SOURCES is not defined after include. Run tools/generate_tables.py.")
endif()

list(REMOVE_DUPLICATES app_sources)

idf_component_register(
	SRCS ${app_sources}
	INCLUDE_DIRS
		.
		../include
		../shared
)
