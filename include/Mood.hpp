// P32 Mood System - C++ Class with Operator Overloading
// Efficient delta-based mood transitions with single calculation per frame

#ifndef MOOD_HPP
#define MOOD_HPP

#include <cstdint>
#include <cstring>

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

// Helper function to clamp int16_t values to int8_t range (-128 to +127)
inline int8_t clampToInt8(int16_t value) {
    if (value > 127) return 127;
    if (value < -128) return -128;
    return static_cast<int8_t>(value);
}

class Mood {
public:
    static const int componentCount = 9;
    
    // Mood components (-128 to +127 intensity)
    enum Component {
        ANGER = 0,
        FEAR = 1,
        HAPPINESS = 2,
        SADNESS = 3,
        CURIOSITY = 4,
        AFFECTION = 5,
        IRRITATION = 6,
        CONTENTMENT = 7,
        EXCITEMENT = 8
    };
    
    int8_t components[componentCount] = {0};
    
    // Named accessors for convenience (sensible -128 to +127 range)
    int8_t& anger() { return components[ANGER]; }
    int8_t& fear() { return components[FEAR]; }
    int8_t& happiness() { return components[HAPPINESS]; }
    int8_t& sadness() { return components[SADNESS]; }
    int8_t& curiosity() { return components[CURIOSITY]; }
    int8_t& affection() { return components[AFFECTION]; }
    int8_t& irritation() { return components[IRRITATION]; }
    int8_t& contentment() { return components[CONTENTMENT]; }
    int8_t& excitement() { return components[EXCITEMENT]; }
    
    const int8_t& anger() const { return components[ANGER]; }
    const int8_t& fear() const { return components[FEAR]; }
    const int8_t& happiness() const { return components[HAPPINESS]; }
    const int8_t& sadness() const { return components[SADNESS]; }
    const int8_t& curiosity() const { return components[CURIOSITY]; }
    const int8_t& affection() const { return components[AFFECTION]; }
    const int8_t& irritation() const { return components[IRRITATION]; }
    const int8_t& contentment() const { return components[CONTENTMENT]; }
    const int8_t& excitement() const { return components[EXCITEMENT]; }
    
    // Operator overloading - checks if ANY component changed
    bool operator!=(const Mood& other) const {
        for(int i = 0; i < componentCount; ++i) {
            if(components[i] != other.components[i]) return true;
        }
        return false;
    }
    
    bool operator==(const Mood& other) const {
        return !(*this != other);
    }
    
    // Assignment operator
    Mood& operator=(const Mood& other) {
        std::memcpy(components, other.components, sizeof(components));
        return *this;
    }
    
    // Constructors
    Mood() = default;
    
    // Copy constructor
    Mood(const Mood& other) {
        std::memcpy(components, other.components, sizeof(components));
    }
    
    Mood(int8_t anger_val, int8_t fear_val, int8_t happiness_val, 
         int8_t sadness_val, int8_t curiosity_val, int8_t affection_val,
         int8_t irritation_val, int8_t contentment_val, int8_t excitement_val = 0) {
        components[ANGER] = anger_val;
        components[FEAR] = fear_val;
        components[HAPPINESS] = happiness_val;
        components[SADNESS] = sadness_val;
        components[CURIOSITY] = curiosity_val;
        components[AFFECTION] = affection_val;
        components[IRRITATION] = irritation_val;
        components[CONTENTMENT] = contentment_val;
        components[EXCITEMENT] = excitement_val;
    }
    
    // Utility methods
    void clear() {
        std::memset(components, 0, sizeof(components));
    }
    
    bool hasAnyMood() const {
        for(int i = 0; i < componentCount; ++i) {
            if(components[i] != 0) return true;  // Allow negative mood values
        }
        return false;
    }
    
    // Add mood values with scaling to prevent overflow
    void addMood(int component_index, int16_t delta) {
        if(component_index >= 0 && component_index < componentCount) {
            int16_t temp = static_cast<int16_t>(components[component_index]) + delta;
            components[component_index] = clampToInt8(temp);
        }
    }
    
    // Add mood by name with scaling
    void addAnger(int16_t delta) { addMood(ANGER, delta); }
    void addFear(int16_t delta) { addMood(FEAR, delta); }
    void addHappiness(int16_t delta) { addMood(HAPPINESS, delta); }
    void addSadness(int16_t delta) { addMood(SADNESS, delta); }
    void addCuriosity(int16_t delta) { addMood(CURIOSITY, delta); }
    void addAffection(int16_t delta) { addMood(AFFECTION, delta); }
    void addIrritation(int16_t delta) { addMood(IRRITATION, delta); }
    void addContentment(int16_t delta) { addMood(CONTENTMENT, delta); }
    void addExcitement(int16_t delta) { addMood(EXCITEMENT, delta); }
    
    static const char* getComponentName(int index) {
        static const char* names[componentCount] = {
            "ANGER", "FEAR", "HAPPINESS", "SADNESS", 
            "CURIOSITY", "AFFECTION", "IRRITATION", "CONTENTMENT", "EXCITEMENT"
        };
        return (index >= 0 && index < componentCount) ? names[index] : "UNKNOWN";
    }
};

// Color delta for mood effects
struct MoodColorDelta {
    int8_t red_delta = 0;
    int8_t green_delta = 0;
    int8_t blue_delta = 0;
    
    MoodColorDelta() = default;
    MoodColorDelta(int8_t r, int8_t g, int8_t b) : red_delta(r), green_delta(g), blue_delta(b) {}
    
    // Add deltas together with scaling to prevent overflow
    MoodColorDelta& operator+=(const MoodColorDelta& other) {
        // Use int16_t for intermediate calculations to prevent overflow
        int16_t temp_red = static_cast<int16_t>(red_delta) + static_cast<int16_t>(other.red_delta);
        int16_t temp_green = static_cast<int16_t>(green_delta) + static_cast<int16_t>(other.green_delta);
        int16_t temp_blue = static_cast<int16_t>(blue_delta) + static_cast<int16_t>(other.blue_delta);
        
        // Scale results to fit in int8_t range (-128 to +127)
        red_delta = clampToInt8(temp_red);
        green_delta = clampToInt8(temp_green);
        blue_delta = clampToInt8(temp_blue);
        
        return *this;
    }
    

};

// Mood-to-color mapping for each mood component
struct MoodColorEffect {
    float red_multiplier = 0.0f;    // How much red per mood unit
    float green_multiplier = 0.0f;  // How much green per mood unit
    float blue_multiplier = 0.0f;   // How much blue per mood unit
    
    MoodColorEffect() = default;
    MoodColorEffect(float r, float g, float b) : red_multiplier(r), green_multiplier(g), blue_multiplier(b) {}
};

// Global mood color effects (defined in implementation)
extern const MoodColorEffect moodColorEffects[Mood::componentCount];

#endif // MOOD_HPP