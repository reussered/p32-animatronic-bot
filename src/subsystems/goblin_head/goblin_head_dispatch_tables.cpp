#include "subsystems/goblin_head/goblin_head_dispatch_tables.hpp"
#include "subsystems/goblin_head/goblin_head_component_functions.hpp"

// Auto-generated dispatch table implementation for subsystem goblin_head

const init_function_t goblin_head_init_table[] = {
    goblin_left_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_display_bus_init,
    generic_spi_display_init,
    goblin_right_eye_init,
    goblin_eye_init,
    gc9a01_init,
    spi_display_bus_init,
    generic_spi_display_init,
    goblin_nose_init,
    hc_sr04_ultrasonic_distance_sensor_init,
    goblin_mouth_init,
    goblin_mouth_init,
    gc9a01_init,
    spi_display_bus_init,
    generic_spi_display_init,
    goblin_speaker_init,
    speaker_init,
    goblin_left_ear_init,
    servo_sg90_micro_init,
    hw496_microphone_init,
    goblin_right_ear_init,
    servo_sg90_micro_init,
    hw496_microphone_init
};

const act_function_t goblin_head_act_table[] = {
    goblin_left_eye_act,
    goblin_eye_act,
    gc9a01_act,
    spi_display_bus_act,
    generic_spi_display_act,
    goblin_right_eye_act,
    goblin_eye_act,
    gc9a01_act,
    spi_display_bus_act,
    generic_spi_display_act,
    goblin_nose_act,
    hc_sr04_ultrasonic_distance_sensor_act,
    goblin_mouth_act,
    goblin_mouth_act,
    gc9a01_act,
    spi_display_bus_act,
    generic_spi_display_act,
    goblin_speaker_act,
    speaker_act,
    goblin_left_ear_act,
    servo_sg90_micro_act,
    hw496_microphone_act,
    goblin_right_ear_act,
    servo_sg90_micro_act,
    hw496_microphone_act
};

const uint32_t goblin_head_hitcount_table[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    180000,
    1,
    36000,
    1,
    1,
    1,
    1,
    84000,
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

