// P32 System Test Class - Hardware Component Testing and Validation
// Provides controlled testing capabilities for individual hardware components

#ifndef SYSTEST_HPP
#define SYSTEST_HPP

#include <cstdint>
#include <cstring>

class SysTest {
public:
    static const int testComponentCount = 8;
    
    // Test component types
    enum TestComponent {
        LEFT_EYE_DISPLAY = 0,
        RIGHT_EYE_DISPLAY = 1,
        NOSE_SENSOR = 2,
        MOUTH_DISPLAY = 3,
        AUDIO_OUTPUT = 4,
        HEAD_SERVO_YAW = 5,
        HEAD_SERVO_PITCH = 6,
        STATUS_LEDS = 7
    };
    
    // Test states for each component
    enum TestState {
        TEST_DISABLED = 0,
        TEST_INIT = 1,
        TEST_RUNNING = 2,
        TEST_PASSED = 3,
        TEST_FAILED = 4,
        TEST_MANUAL = 5  // Manual control mode
    };
    
    // Component test states (1 byte each)
    uint8_t componentStates[testComponentCount] = {TEST_DISABLED};
    
    // Test control flags
    bool globalTestEnabled = false;
    bool continuousMode = false;
    bool verboseLogging = false;
    
    // Test cycle control
    uint32_t testCycleCount = 0;
    uint32_t maxTestCycles = 0;  // 0 = unlimited
    
    // Test timing
    uint32_t testStartTime = 0;
    uint32_t testDuration = 0;
    
    // Named accessors for component states
    uint8_t& leftEyeState() { return componentStates[LEFT_EYE_DISPLAY]; }
    uint8_t& rightEyeState() { return componentStates[RIGHT_EYE_DISPLAY]; }
    uint8_t& noseSensorState() { return componentStates[NOSE_SENSOR]; }
    uint8_t& mouthDisplayState() { return componentStates[MOUTH_DISPLAY]; }
    uint8_t& audioOutputState() { return componentStates[AUDIO_OUTPUT]; }
    uint8_t& headServoYawState() { return componentStates[HEAD_SERVO_YAW]; }
    uint8_t& headServoPitchState() { return componentStates[HEAD_SERVO_PITCH]; }
    uint8_t& statusLedsState() { return componentStates[STATUS_LEDS]; }
    
    const uint8_t& leftEyeState() const { return componentStates[LEFT_EYE_DISPLAY]; }
    const uint8_t& rightEyeState() const { return componentStates[RIGHT_EYE_DISPLAY]; }
    const uint8_t& noseSensorState() const { return componentStates[NOSE_SENSOR]; }
    const uint8_t& mouthDisplayState() const { return componentStates[MOUTH_DISPLAY]; }
    const uint8_t& audioOutputState() const { return componentStates[AUDIO_OUTPUT]; }
    const uint8_t& headServoYawState() const { return componentStates[HEAD_SERVO_YAW]; }
    const uint8_t& headServoPitchState() const { return componentStates[HEAD_SERVO_PITCH]; }
    const uint8_t& statusLedsState() const { return componentStates[STATUS_LEDS]; }
    
    // Operator overloading - checks if ANY component state changed
    bool operator!=(const SysTest& other) const {
        for(int i = 0; i < testComponentCount; ++i) {
            if(componentStates[i] != other.componentStates[i]) return true;
        }
        return globalTestEnabled != other.globalTestEnabled ||
               continuousMode != other.continuousMode ||
               verboseLogging != other.verboseLogging ||
               testCycleCount != other.testCycleCount ||
               maxTestCycles != other.maxTestCycles;
    }
    
    bool operator==(const SysTest& other) const {
        return !(*this != other);
    }
    
    // Assignment operator
    SysTest& operator=(const SysTest& other) {
        std::memcpy(componentStates, other.componentStates, sizeof(componentStates));
        globalTestEnabled = other.globalTestEnabled;
        continuousMode = other.continuousMode;
        verboseLogging = other.verboseLogging;
        testCycleCount = other.testCycleCount;
        maxTestCycles = other.maxTestCycles;
        testStartTime = other.testStartTime;
        testDuration = other.testDuration;
        return *this;
    }
    
    // Constructors
    SysTest() = default;
    
    // Copy constructor
    SysTest(const SysTest& other) {
        *this = other;
    }
    
    // Utility methods
    void clearAllTests() {
        std::memset(componentStates, TEST_DISABLED, sizeof(componentStates));
        globalTestEnabled = false;
        continuousMode = false;
        verboseLogging = false;
        testCycleCount = 0;
        maxTestCycles = 0;
        testStartTime = 0;
        testDuration = 0;
    }
    
    void enableAllTests() {
        for(int i = 0; i < testComponentCount; ++i) {
            componentStates[i] = TEST_INIT;
        }
        globalTestEnabled = true;
    }
    
    void disableAllTests() {
        for(int i = 0; i < testComponentCount; ++i) {
            componentStates[i] = TEST_DISABLED;
        }
        globalTestEnabled = false;
    }
    
    // Start specific component test
    void startComponentTest(TestComponent component) {
        if(component < testComponentCount) {
            componentStates[component] = TEST_INIT;
        }
    }
    
    // Stop specific component test
    void stopComponentTest(TestComponent component) {
        if(component < testComponentCount) {
            componentStates[component] = TEST_DISABLED;
        }
    }
    
    // Set component to manual control mode
    void setManualMode(TestComponent component) {
        if(component < testComponentCount) {
            componentStates[component] = TEST_MANUAL;
        }
    }
    
    // Check if any test is running
    bool isAnyTestRunning() const {
        for(int i = 0; i < testComponentCount; ++i) {
            if(componentStates[i] == TEST_RUNNING || componentStates[i] == TEST_INIT) {
                return true;
            }
        }
        return false;
    }
    
    // Check if all tests passed
    bool allTestsPassed() const {
        for(int i = 0; i < testComponentCount; ++i) {
            if(componentStates[i] != TEST_PASSED && componentStates[i] != TEST_DISABLED) {
                return false;
            }
        }
        return true;
    }
    
    // Get count of failed tests
    uint8_t getFailedTestCount() const {
        uint8_t count = 0;
        for(int i = 0; i < testComponentCount; ++i) {
            if(componentStates[i] == TEST_FAILED) {
                count++;
            }
        }
        return count;
    }
    
    // Get count of passed tests
    uint8_t getPassedTestCount() const {
        uint8_t count = 0;
        for(int i = 0; i < testComponentCount; ++i) {
            if(componentStates[i] == TEST_PASSED) {
                count++;
            }
        }
        return count;
    }
    
    // Start global test sequence
    void startGlobalTest(uint32_t maxCycles = 0, bool continuous = false, bool verbose = false) {
        globalTestEnabled = true;
        continuousMode = continuous;
        verboseLogging = verbose;
        maxTestCycles = maxCycles;
        testCycleCount = 0;
        testStartTime = 0;  // Set by calling code with actual time
        enableAllTests();
    }
    
    // Stop global test sequence
    void stopGlobalTest() {
        globalTestEnabled = false;
        continuousMode = false;
        disableAllTests();
    }
    
    // Increment test cycle
    void incrementCycle() {
        testCycleCount++;
        if(maxTestCycles > 0 && testCycleCount >= maxTestCycles) {
            stopGlobalTest();
        }
    }
};

#endif // SYSTEST_HPP