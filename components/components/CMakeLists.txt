# Goblin Full Subsystem - CMakeLists.txt
# Contains compiled source files specific to the goblin_full bot configuration

set(goblin_full_sources
    SharedMemory.cpp
    Mood.cpp
)

# Add these sources to the parent scope so they get compiled
set(app_sources ${app_sources} ${goblin_full_sources} PARENT_SCOPE)