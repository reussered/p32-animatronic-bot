#include "subsystems/test_head/test_head_dispatch_tables.hpp"
#include "subsystems/test_head/test_head_component_functions.hpp"

// Auto-generated dispatch table implementation for subsystem test_head

const init_function_t test_head_init_table[] = {
    &goblin_left_eye_init
};

const act_function_t test_head_act_table[] = {
    &goblin_left_eye_act
};

const uint32_t test_head_hitcount_table[] = {
    1
};

const std::size_t test_head_init_table_size = sizeof(test_head_init_table) / sizeof(init_function_t);
const std::size_t test_head_act_table_size = sizeof(test_head_act_table) / sizeof(act_function_t);

