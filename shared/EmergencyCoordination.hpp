#ifndef EMERGENCY_COORDINATION_HPP
#define EMERGENCY_COORDINATION_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class EmergencyCoordination {
public:
    uint32_t version;

    // Safety/emergency response state
    uint8_t emergency_level;
    bool emergency_active;
    uint32_t emergency_start_time;

    // Coordination data
    uint8_t affected_subsystems;
    bool master_coordination_required;

};

#endif // EMERGENCY_COORDINATION_HPP