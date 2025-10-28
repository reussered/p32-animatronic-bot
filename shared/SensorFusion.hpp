#ifndef SENSOR_FUSION_HPP
#define SENSOR_FUSION_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class SensorFusion {
public:
    uint32_t version;

    // Combined sensor data from all chips
    uint8_t fused_distance_cm;
    bool fused_touch_detected;
    int8_t fused_temperature_c;
    int8_t fused_humidity;

    // Fusion metadata
    uint8_t sensor_count;
    uint32_t last_fusion_time;
    bool fusion_valid;

};

#endif // SENSOR_FUSION_HPP