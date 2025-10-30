cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(p32-animatronic-bot)

# Only include source files from main directory
set(EXTRA_COMPONENT_DIRS main)
