idf_component_register(
    SRCS "component_tables.cpp" "p32_components.cmake"
    INCLUDE_DIRS "../include" "../shared"
)

# Conditional source file inclusion based on subsystem
if(CONFIG_SUBSYSTEM_GOBLIN_HEAD)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../components/p32_component_functions.cpp"
        "core/memory/SharedMemory.cpp"
        "../components/Mood.cpp"
        "../components/p32_web_client.cpp"
    )
endif()

if(CONFIG_SUBSYSTEM_GOBLIN_HEAD_ONLY)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../components/p32_component_functions.cpp"
        "core/memory/SharedMemory.cpp"
        "../components/Mood.cpp"
        "../components/p32_web_client.cpp"
    )
endif()

# Dual eye display test configuration
if(CONFIG_TEST_DUAL_EYES)
    target_sources(${COMPONENT_LIB} PRIVATE
        "component_tables.cpp"
        "core/memory/SharedMemory.cpp"
        "components/Mood.cpp"
        "p32_web_client.cpp"
        "../test_display_interface.cpp"
    )
endif()
