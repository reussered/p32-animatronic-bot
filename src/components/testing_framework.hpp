#pragma once

/**
 * @brief Runtime Testing Framework - Class-Based with String Test Names
 * 
 * This provides a clean alternative to #ifdef SIMPLE_TEST preprocessor hell.
 * The simple_test component (if present) controls g_TestFramework globally.
 * Components can check specific test conditions using human-readable names.
 */

#include <string>
#include <set>

class TestFramework {
public:
    TestFramework() : production_mode(true) {}
    
    // Core test state management
    void enable_testing() { production_mode = false; }
    void disable_testing() { production_mode = true; active_tests.clear(); }
    bool is_production_mode() const { return production_mode; }
    bool is_testing() const { return !production_mode; }
    
    // String-based test control
    void start_test(const std::string& test_name) { 
        production_mode = false;
        active_tests.insert(test_name); 
    }
    void stop_test(const std::string& test_name) { 
        active_tests.erase(test_name); 
        if (active_tests.empty()) production_mode = true;
    }
    void clear_all_tests() { active_tests.clear(); production_mode = true; }
    
    // Test queries
    bool is_test_active(const std::string& test_name) const {
        return active_tests.find(test_name) != active_tests.end();
    }
    
    // Common test patterns
    bool skip_hardware() const { 
        return is_test_active("simple") || is_test_active("animation_only") || is_test_active("mock_hardware");
    }
    bool verbose_logging() const { 
        return is_test_active("hardware_debug") || is_test_active("verbose") || is_test_active("trace_all");
    }
    bool use_mock_data() const { 
        return is_test_active("simulation") || is_test_active("mock_sensors") || is_test_active("synthetic_data");
    }
    
    // Component-agnostic test specifications (any component can implement these)
    bool solid_color_display_test() const { return is_test_active("solid_color_display_test"); }
    bool color_cycle_test() const { return is_test_active("color_cycle_test"); }
    bool brightness_test() const { return is_test_active("brightness_test"); }
    bool animation_smoothness_test() const { return is_test_active("animation_smoothness_test"); }
    bool pixel_accuracy_test() const { return is_test_active("pixel_accuracy_test"); }
    bool refresh_rate_test() const { return is_test_active("refresh_rate_test"); }
    
    // Hardware subsystem tests (multiple components may implement)
    bool spi_bus_test() const { return is_test_active("spi_bus_test"); }
    bool gpio_test() const { return is_test_active("gpio_test"); }
    bool i2c_bus_test() const { return is_test_active("i2c_bus_test"); }
    bool sensor_data_test() const { return is_test_active("sensor_data_test"); }
    
    // Legacy component-specific helpers (deprecated - use component-agnostic tests above)
    bool test_display() const { return solid_color_display_test() || color_cycle_test() || brightness_test(); }
    bool test_animations() const { return animation_smoothness_test() || is_test_active("blink_test"); }
    bool test_spi() const { return spi_bus_test() || is_test_active("spi_loopback"); }
    bool test_sensors() const { return sensor_data_test() || is_test_active("proximity_test"); }
    
    // Get active test list for debugging
    std::set<std::string> get_active_tests() const { return active_tests; }
    
private:
    bool production_mode;
    std::set<std::string> active_tests;
};

// Global testing framework instance
extern TestFramework g_TestFramework;

// Weak symbol fallback - provides default when simple_test component is not linked
__attribute__((weak)) TestFramework g_TestFramework;

// Convenience macros using the class interface
#define IS_PRODUCTION() (g_TestFramework.is_production_mode())
#define IS_TESTING() (g_TestFramework.is_testing())
#define IS_TEST_ACTIVE(name) (g_TestFramework.is_test_active(name))
#define SKIP_HARDWARE() (g_TestFramework.skip_hardware())
#define VERBOSE_LOGGING() (g_TestFramework.verbose_logging())
#define USE_MOCK_DATA() (g_TestFramework.use_mock_data())

// Component-agnostic test macros (any display component can implement these)
#define SOLID_COLOR_DISPLAY_TEST() (g_TestFramework.solid_color_display_test())
#define COLOR_CYCLE_TEST() (g_TestFramework.color_cycle_test())
#define BRIGHTNESS_TEST() (g_TestFramework.brightness_test())
#define ANIMATION_SMOOTHNESS_TEST() (g_TestFramework.animation_smoothness_test())
#define PIXEL_ACCURACY_TEST() (g_TestFramework.pixel_accuracy_test())
#define REFRESH_RATE_TEST() (g_TestFramework.refresh_rate_test())

// Hardware subsystem test macros  
#define SPI_BUS_TEST() (g_TestFramework.spi_bus_test())
#define GPIO_TEST() (g_TestFramework.gpio_test())
#define I2C_BUS_TEST() (g_TestFramework.i2c_bus_test())
#define SENSOR_DATA_TEST() (g_TestFramework.sensor_data_test())

// Legacy macros (deprecated)
#define TEST_DISPLAY() (g_TestFramework.test_display())
#define TEST_ANIMATIONS() (g_TestFramework.test_animations())
#define TEST_SPI() (g_TestFramework.test_spi())
#define TEST_SENSORS() (g_TestFramework.test_sensors())