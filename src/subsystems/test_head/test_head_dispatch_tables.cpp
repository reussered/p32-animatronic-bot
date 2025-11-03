#include "subsystems/test_head/test_head_dispatch_tables.hpp"
#include "subsystems/test_head/test_head_component_functions.hpp"

// Auto-generated dispatch table implementation for subsystem test_head

const init_function_t test_head_init_table[] = {
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_display_bus_init,
    generic_spi_display_init,
    spi_bus_init,
    generic_spi_data_driver_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_display_bus_init,
    generic_spi_display_init,
    spi_bus_init,
    generic_spi_data_driver_init
};

const act_function_t test_head_act_table[] = {
    goblin_left_eye_act,
    goblin_eye_act,
    gc9a01_act,
    spi_display_bus_act,
    generic_spi_display_act,
    spi_bus_act,
    generic_spi_data_driver_act,
    goblin_right_eye_act,
    goblin_eye_act,
    gc9a01_act,
    spi_display_bus_act,
    generic_spi_display_act,
    spi_bus_act,
    generic_spi_data_driver_act
};

const uint32_t test_head_hitcount_table[] = {
    200,
    1,
    1,
    1,
    1,
    1,
    1,
    200,
    1,
    1,
    1,
    1,
    1,
    1
};

const std::size_t test_head_init_table_size = sizeof(test_head_init_table) / sizeof(init_function_t);
const std::size_t test_head_act_table_size = sizeof(test_head_act_table) / sizeof(act_function_t);

