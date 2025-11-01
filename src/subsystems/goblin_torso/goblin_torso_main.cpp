#include "goblin_torso_main.hpp"
#include "goblin_torso_dispatch_tables.hpp"

#include <cstddef>
#include <cstdint>

// Auto-generated subsystem main loop for goblin_torso

uint32_t g_loopCount = 0;

extern "C" void app_main(void) {
    for (std::size_t i = 0; i < goblin_torso_init_table_size; ++i) {
        if (goblin_torso_init_table[i]) {
            goblin_torso_init_table[i]();
        }
    }

    while (true) {
        for (std::size_t i = 0; i < goblin_torso_act_table_size; ++i) {
            const auto func = goblin_torso_act_table[i];
            const auto hit = goblin_torso_hitcount_table[i];
            if (func && hit > 0U && (g_loopCount % hit) == 0U) {
                func();
            }
        }
        ++g_loopCount;
    }
}
