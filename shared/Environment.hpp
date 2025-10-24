
class Environment {
public:
    uint32_t version;
   
    // Sensor data
    uint8_t distance_cm;
    bool touch_detected;
    int8_t temperature_c;
	int8_t humidity; // (0 - 99%)
    
    // System status
    uint8_t battery_percent;
    bool wifi_connected;
    
    // Debug control flags
    bool debugging_enabled;
    bool verbose_logging;
    bool performance_monitoring;
    bool display_diagnostics;
    
};

