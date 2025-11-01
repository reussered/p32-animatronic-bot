#include "goblin_torso_dispatch_tables.hpp"
#include "goblin_torso_component_functions.hpp"

// Auto-generated dispatch table implementation for subsystem goblin_torso

const init_function_t goblin_torso_init_table[] = {
    goblin_torso_init,
    system_core_init,
    debug_controller_init,
    network_monitor_init,
    wifi_station_init,
    bluetooth_central_init,
    telemetry_hub_init,
    watchdog_init,
    power_monitor_init,
    spine_flexion_servo_init,
    servo_sg90_micro_init,
    spine_extension_servo_init,
    servo_sg90_micro_init,
    waist_rotation_servo_init,
    servo_sg90_micro_init,
    torso_status_led_init,
    servo_sg90_micro_init,
    torso_speaker_init,
    servo_sg90_micro_init
};

const act_function_t goblin_torso_act_table[] = {
    goblin_torso_act,
    system_core_act,
    debug_controller_act,
    network_monitor_act,
    wifi_station_act,
    bluetooth_central_act,
    telemetry_hub_act,
    watchdog_act,
    power_monitor_act,
    spine_flexion_servo_act,
    servo_sg90_micro_act,
    spine_extension_servo_act,
    servo_sg90_micro_act,
    waist_rotation_servo_act,
    servo_sg90_micro_act,
    torso_status_led_act,
    servo_sg90_micro_act,
    torso_speaker_act,
    servo_sg90_micro_act
};

const uint32_t goblin_torso_hitcount_table[] = {
    50,
    100,
    1,
    50,
    50,
    75,
    75,
    500,
    200,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1
};

const std::size_t goblin_torso_init_table_size = sizeof(goblin_torso_init_table) / sizeof(init_function_t);
const std::size_t goblin_torso_act_table_size = sizeof(goblin_torso_act_table) / sizeof(act_function_t);

