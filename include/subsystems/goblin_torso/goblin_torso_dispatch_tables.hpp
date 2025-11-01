#ifndef GOBLIN_TORSO_DISPATCH_TABLES_HPP
#define GOBLIN_TORSO_DISPATCH_TABLES_HPP

#include <cstddef>
#include <cstdint>
#include "esp_err.h"

// Auto-generated dispatch table header for subsystem goblin_torso

using init_function_t = esp_err_t (*)(void);
using act_function_t = void (*)(void);

extern const init_function_t goblin_torso_init_table[];
extern const act_function_t goblin_torso_act_table[];
extern const uint32_t goblin_torso_hitcount_table[];
extern const std::size_t goblin_torso_init_table_size;
extern const std::size_t goblin_torso_act_table_size;

#endif // GOBLIN_TORSO_DISPATCH_TABLES_HPP
