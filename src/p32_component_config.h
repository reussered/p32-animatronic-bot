#ifndef P32_COMPONENT_CONFIG_H
#define P32_COMPONENT_CONFIG_H

// Component enable flags for conditional compilation
// Use #ifdef VARIABLE_NAME (defined = enabled, undefined = disabled)

// Simple components for basic testing (ENABLED - defined)
#define ENABLE_SIMPLE_DISPLAY
#define ENABLE_SIMPLE_AUDIO
#define ENABLE_SIMPLE_SENSOR
#define ENABLE_SIMPLE_MOOD
#define ENABLE_SIMPLE_GOBLIN
#define ENABLE_SIMPLE_WIFI
#define ENABLE_GOBLIN_COMPONENTS  // Enable goblin component implementations
// #define ENABLE_SIMPLE_SERIAL  // DISABLED - has UART dependency issues

// Standalone components (DISABLED - undefined/commented out)
// #define ENABLE_SIMPLE_GOBLIN_STANDALONE
// #define ENABLE_P32_CORE
// #define ENABLE_SIMPLE_CORE

// Complex components (DISABLED - undefined/commented out)
// #define ENABLE_GOBLIN_PERSONALITY
// #define ENABLE_ANDROID_PERSONALITY
// #define ENABLE_ZOMBIE_PERSONALITY
// #define ENABLE_PERSONALITY_COMPONENT
// #define ENABLE_PERSONALITY_VARIANTS
// #define ENABLE_SIMPLE_CORE
// #define ENABLE_COMPONENT_TABLE
// #define ENABLE_COMPLEX_DISPLAY
// #define ENABLE_COMPLEX_AUDIO
// #define ENABLE_COMPLEX_SENSORS
// #define ENABLE_COMPLEX_MOOD
// #define ENABLE_BEHAVIOR_SYSTEM
// #define ENABLE_COMPONENT_SYSTEM

#endif // P32_COMPONENT_CONFIG_H