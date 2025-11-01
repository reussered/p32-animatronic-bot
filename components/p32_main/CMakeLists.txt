idf_component_register(
    SRCS "component_tables.cpp" "main.cpp" "core/memory/SharedMemory.cpp"
    INCLUDE_DIRS "../../include" "../../shared"
    REQUIRES esp_wifi
)

# Include generated component sources
include("p32_components.cmake")

# Conditional source file inclusion based on subsystem
if(SUBSYSTEM_GOBLIN_HEAD)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../p32_component_functions.cpp"
        "../p32_web_client.cpp"
    )
endif()

if(SUBSYSTEM_GOBLIN_HEAD_ONLY)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../p32_component_functions.cpp"
        "core/memory/SharedMemory.cpp"
        "../p32_web_client.cpp"
    )
endif()

# Dual eye display test configuration
if(TEST_DUAL_EYES)
    target_sources(${COMPONENT_LIB} PRIVATE
        "component_tables.cpp"
        "core/memory/SharedMemory.cpp"
        "../p32_web_client.cpp"
        "../dual_eye_display_test.cpp"
    )
endif()
