#ifndef MICROPHONE_DATA_HPP
#define MICROPHONE_DATA_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class MicrophoneData {
public:
    uint32_t version;

    // Raw microphone data
    int raw_sample;
    int processed_sample;

    // Audio processing results
    int voltage_mv;
    float gain_applied;
    bool noise_gate_active;

    // Audio analysis
    int peak_level;
    int average_level;
    bool sound_detected;

    // Status
    bool driver_initialized;
    uint32_t sample_count;

    // Default constructor
    MicrophoneData() :
        version(1),
        raw_sample(0),
        processed_sample(0),
        voltage_mv(0),
        gain_applied(1.0f),
        noise_gate_active(false),
        peak_level(0),
        average_level(0),
        sound_detected(false),
        driver_initialized(false),
        sample_count(0)
    {}
};

#endif // MICROPHONE_DATA_HPP