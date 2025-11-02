cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(p32-animatronic-bot)
include(p32_generated_sources.cmake OPTIONAL)

target_sources(app PRIVATE 
    "components/p32_main/main.cpp"
    "src/p32_component_functions.cpp"
)
