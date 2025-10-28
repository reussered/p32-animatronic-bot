#ifndef BEHAVIOR_CONTROL_HPP
#define BEHAVIOR_CONTROL_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class BehaviorControl {
public:
    uint32_t version;

    // Unified creature behavior state
    uint8_t current_behavior;
    uint8_t behavior_intensity;
    bool behavior_active;
    uint32_t behavior_start_time;

    // Coordination flags
    bool master_override;
    uint8_t coordination_priority;

};

#endif // BEHAVIOR_CONTROL_HPP