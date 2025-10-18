// GlobalSharedMemory Test Header
// Function declarations for testing the GSM system

#ifndef GLOBAL_SHARED_MEMORY_TEST_HPP
#define GLOBAL_SHARED_MEMORY_TEST_HPP

#include "GlobalSharedMemory.hpp"

// Test function declarations
void test_global_shared_memory();
void test_mesh_peers(GlobalSharedMemory& gsm);

// Simple display test function - call from display component act() function
// Returns true if tests pass and component should continue, false if tests fail
bool run_gsm_display_tests();

#endif // GLOBAL_SHARED_MEMORY_TEST_HPP