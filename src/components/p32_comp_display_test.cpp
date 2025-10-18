/**
 * Display Test Component - Simple GSM Test Integration
 * 
 * Purpose: Add GlobalSharedMemory test names to SystemTest and run them from display components
 * Hardware: Uses existing GC9A01 display components for test execution
 * 
 * Expected Behavior:
 * - Adds GSM test names to SystemTest global variable during init
 * - Tests are executed by display components during their act() functions
 */

#include "esp_log.h"
#include "esp_err.h"
#include "GlobalSharedMemoryTest.hpp"
#include <driver/spi_master.h>
#include <driver/gpio.h>

namespace {
    constexpr const char* TAG = "DISPLAY_TEST";
    
    // SPI Device 1 pins (from spi_device_1.json and spi_bus_vspi.json)
    constexpr gpio_num_t DISPLAY_CS_PIN  = GPIO_NUM_35;  // Chip Select
    constexpr gpio_num_t DISPLAY_RST_PIN = GPIO_NUM_15;  // Reset
    constexpr gpio_num_t DISPLAY_DC_PIN  = GPIO_NUM_16;  // Data/Command
    constexpr gpio_num_t DISPLAY_SCL_PIN = GPIO_NUM_17;  // Clock
    constexpr gpio_num_t DISPLAY_SDA_PIN = GPIO_NUM_18;  // Data
    
    spi_device_handle_t display_spi = nullptr;
    bool display_initialized = false;
}

// Mock SystemTest structure - this would normally be defined in shared headers
typedef struct {
    char test_names[10][64];  // Array of test name strings
    int test_count;
    bool tests_added;
} SystemTest;

// Global SystemTest variable that display components can access
SystemTest g_system_test = {0};

/**
 * Initialize SPI and display hardware
 */
static esp_err_t init_display_spi(void)
{
    ESP_LOGI(TAG, "🔧 Initializing SPI for Display 1...");
    
    // Configure SPI bus (if not already done)
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = DISPLAY_SDA_PIN,
        .miso_io_num = -1,  // No MISO for displays
        .sclk_io_num = DISPLAY_SCL_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
    
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "❌ SPI bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure display device
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 10 * 1000 * 1000,  // 10MHz
        .mode = 0,
        .spics_io_num = DISPLAY_CS_PIN,
        .queue_size = 7,
    };
    
    ret = spi_bus_add_device(SPI2_HOST, &dev_cfg, &display_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "❌ SPI device add failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure GPIO pins
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DISPLAY_RST_PIN) | (1ULL << DISPLAY_DC_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "✅ SPI initialized successfully");
    return ESP_OK;
}

/**
 * Send a basic test pattern to wake up the display
 */
static void test_display_communication(void)
{
    if (!display_spi) return;
    
    ESP_LOGI(TAG, "🔥 TESTING DISPLAY COMMUNICATION...");
    
    // Reset sequence
    gpio_set_level(DISPLAY_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(DISPLAY_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Try basic commands that work on most displays
    uint8_t test_commands[] = {
        0x01,  // Software reset
        0x11,  // Sleep out
        0x29,  // Display on
    };
    
    for (int i = 0; i < sizeof(test_commands); i++) {
        spi_transaction_t trans = {
            .length = 8,
            .tx_data = {test_commands[i], 0, 0, 0},
            .flags = SPI_TRANS_USE_TXDATA
        };
        
        gpio_set_level(DISPLAY_DC_PIN, 0);  // Command mode
        spi_device_transmit(display_spi, &trans);
        vTaskDelay(pdMS_TO_TICKS(10));
        
        ESP_LOGI(TAG, "📤 Sent command: 0x%02X", test_commands[i]);
    }
    
    ESP_LOGI(TAG, "🚀 Display commands sent! Check your screen!");
}

/**
 * Initialize the display test component
 * Adds GSM test names to the SystemTest global variable AND initializes display
 */
extern "C" esp_err_t p32_comp_display_test_init(void)
{
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "🖥️  DISPLAY TEST - INITIALIZING HARDWARE");
    ESP_LOGI(TAG, "========================================");
    
    // Initialize SPI and display hardware FIRST
    esp_err_t ret = init_display_spi();
    if (ret == ESP_OK) {
        display_initialized = true;
        test_display_communication();
    } else {
        ESP_LOGE(TAG, "❌ Display initialization failed!");
    }
    
    // Add GSM test names to the system test registry
    if (!g_system_test.tests_added) {
        strncpy(g_system_test.test_names[0], "gsm_basic_allocation", 63);
        strncpy(g_system_test.test_names[1], "gsm_read_write", 63);
        strncpy(g_system_test.test_names[2], "gsm_find_pointer", 63);
        strncpy(g_system_test.test_names[3], "gsm_type_safety", 63);
        strncpy(g_system_test.test_names[4], "gsm_memory_stats", 63);
        
        g_system_test.test_count = 5;
        g_system_test.tests_added = true;
        
        ESP_LOGI(TAG, "✅ Added %d GSM test names to SystemTest", g_system_test.test_count);
    }
    
    ESP_LOGI(TAG, "========================================");
    return ESP_OK;
}

/**
 * Display test component action - minimal operation
 * The actual tests are run by the display components themselves
 */
extern "C" void p32_comp_display_test_act(void)
{
    // This component just manages test names - actual execution happens in display components
    // Log periodically to show this component is active
    static uint32_t log_counter = 0;
    log_counter++;
    
    if (log_counter % 50000 == 0) {  // Log every ~50000 calls
        ESP_LOGD(TAG, "Display test component active - %d tests registered", g_system_test.test_count);
    }
}

/**
 * Helper function for display components to run GSM tests
 * This can be called from GC9A01 act() functions
 */
extern "C" bool run_display_gsm_tests(void)
{
    if (!g_system_test.tests_added) {
        ESP_LOGW(TAG, "SystemTest not initialized - cannot run tests");
        return false;
    }
    
    ESP_LOGI(TAG, "=== Running GSM Tests for Display Component ===");
    ESP_LOGI(TAG, "Available tests: %d", g_system_test.test_count);
    
    // Call the actual GSM test function from GlobalSharedMemoryTest
    bool result = run_gsm_display_tests();
    
    if (result) {
        ESP_LOGI(TAG, "✅ All GSM display tests passed");
    } else {
        ESP_LOGE(TAG, "❌ Some GSM display tests failed");
    }
    
    return result;
}