idf_component_register(
    SRCS "main.cpp" "component_tables.cpp" "p32_components.cmake"
    INCLUDE_DIRS "../include" "../shared"
)

# Conditional source file inclusion based on subsystem
if(CONFIG_SUBSYSTEM_GOBLIN_HEAD_ONLY)
    target_sources(${COMPONENT_LIB} PRIVATE
        "../components/component_tables.cpp"
        "../components/p32_component_functions.cpp"
        "../components/components/goblin_head_left_eye_only.src"
        "../components/goblin_full/goblin_left_eye.src"
        "../components/goblin_full/goblin_eye.src"
        "../components/components/gc9a01.src"
        "../components/components/spi_bus.src"
        "../components/components/generic_spi_display/generic_spi_display.src"
        "../components/components/SharedMemory.cpp"
        "../components/components/Mood.cpp"
        "../components/p32_web_client.cpp"
    )
endif()