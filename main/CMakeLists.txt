idf_component_register(
    SRCS "main.cpp" "component_tables.cpp" "p32_components.cmake"
    INCLUDE_DIRS "../include" "../shared"
)

# Conditional source file inclusion based on subsystem
if(CONFIG_SUBSYSTEM_GOBLIN_HEAD_ONLY)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../components/p32_component_functions.cpp"
        "../components/SharedMemory.cpp"
        "../components/Mood.cpp"
        "../components/p32_web_client.cpp"
    )
endif()

# Dual eye display test configuration
if(CONFIG_TEST_DUAL_EYES)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../src/component_tables.cpp"
        "../src/components/SharedMemory.cpp"
        "../src/components/Mood.cpp"
        "../src/p32_web_client.cpp"
        "../test_display_interface.cpp"
    )
endif()