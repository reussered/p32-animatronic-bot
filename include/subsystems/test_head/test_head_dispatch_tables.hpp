#ifndef TEST_HEAD_DISPATCH_TABLES_HPP
#define TEST_HEAD_DISPATCH_TABLES_HPP

#include <cstddef>
#include <cstdint>
#include "esp_err.h"

// Auto-generated dispatch table header for subsystem test_head

using init_function_t = esp_err_t (*)(void);
using act_function_t = void (*)(void);

extern const init_function_t test_head_init_table[];
extern const act_function_t test_head_act_table[];
extern const uint32_t test_head_hitcount_table[];
extern const std::size_t test_head_init_table_size;
extern const std::size_t test_head_act_table_size;

#endif // TEST_HEAD_DISPATCH_TABLES_HPP
