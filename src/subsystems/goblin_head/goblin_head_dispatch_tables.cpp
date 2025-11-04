#include "subsystems/goblin_head/goblin_head_dispatch_tables.hpp"
#include "subsystems/goblin_head/goblin_head_component_functions.hpp"

// Auto-generated dispatch table implementation for subsystem goblin_head

const init_function_t goblin_head_init_table[] = {
    &goblin_left_eye_init,
    &goblin_right_eye_init,
    &goblin_nose_init,
    &goblin_mouth_init,
    &goblin_speaker_init,
    &goblin_left_ear_init,
    &goblin_right_ear_init
};

const act_function_t goblin_head_act_table[] = {
    &goblin_left_eye_act,
    &goblin_right_eye_act,
    &goblin_nose_act,
    &goblin_mouth_act,
    &goblin_speaker_act,
    &goblin_left_ear_act,
    &goblin_right_ear_act
};

const uint32_t goblin_head_hitcount_table[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1
};

const std::size_t goblin_head_init_table_size = sizeof(goblin_head_init_table) / sizeof(init_function_t);
const std::size_t goblin_head_act_table_size = sizeof(goblin_head_act_table) / sizeof(act_function_t);

