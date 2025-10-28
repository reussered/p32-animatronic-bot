#ifndef COLLISION_AVOIDANCE_HPP
#define COLLISION_AVOIDANCE_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class CollisionAvoidance {
public:
    uint32_t version;

    // Collision avoidance state
    uint8_t avoidance_level;
    bool obstacle_detected;
    uint8_t safe_distance_cm;

};

#endif // COLLISION_AVOIDANCE_HPP