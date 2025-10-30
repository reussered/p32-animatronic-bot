// ESP32 Pin Assignment Arrays
// Generated for: ESP32-C3
// Chip: ESP32_C3

// Runtime pin assignment tracking
static int assigned_pins[16];
static size_t assigned_pins_count = 0;

const int spi_assignable[] = {
    GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17
};
const size_t spi_assignable_count = sizeof(spi_assignable) / sizeof(spi_assignable[0]);

const int i2c_assignable[] = {
    GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17
};
const size_t i2c_assignable_count = sizeof(i2c_assignable) / sizeof(i2c_assignable[0]);

const int i2s_assignable[] = {
    GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17
};
const size_t i2s_assignable_count = sizeof(i2s_assignable) / sizeof(i2s_assignable[0]);

const int adc_assignable[] = {
    GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4
};
const size_t adc_assignable_count = sizeof(adc_assignable) / sizeof(adc_assignable[0]);

const int pwm_assignable[] = {
    GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17
};
const size_t pwm_assignable_count = sizeof(pwm_assignable) / sizeof(pwm_assignable[0]);

const int gpio_assignable[] = {
    GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17
};
const size_t gpio_assignable_count = sizeof(gpio_assignable) / sizeof(gpio_assignable[0]);

// Pin assignment helper function
static int assign_pin(const int* assignable_array, size_t array_size) {
    for (size_t i = 0; i < array_size; i++) {
        int pin = assignable_array[i];
        // Check if pin is already assigned
        bool already_assigned = false;
        for (size_t j = 0; j < assigned_pins_count; j++) {
            if (assigned_pins[j] == pin) {
                already_assigned = true;
                break;
            }
        }
        if (!already_assigned && assigned_pins_count < sizeof(assigned_pins)/sizeof(assigned_pins[0])) {
            assigned_pins[assigned_pins_count++] = pin;
            return pin;
        }
    }
    return -1; // No available pins
}
