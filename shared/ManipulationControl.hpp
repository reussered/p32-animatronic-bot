#ifndef MANIPULATION_CONTROL_HPP
#define MANIPULATION_CONTROL_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class ManipulationControl {
public:
    uint32_t version;

    // Arm manipulation state
    uint8_t manipulation_mode;
    uint8_t grip_strength;
    bool manipulation_active;

};

#endif // MANIPULATION_CONTROL_HPP