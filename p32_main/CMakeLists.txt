idf_component_register(
    SRCS "core/memory/SharedMemory.cpp"
    INCLUDE_DIRS "../include" "../shared"
)

# Include (possibly-empty) component list generated or managed elsewhere.
include("p32_components.cmake")

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
        "core/memory/SharedMemory.cpp"
        "components/Mood.cpp"
        "p32_web_client.cpp"
        "../test_display_interface.cpp"
    )
endif()
