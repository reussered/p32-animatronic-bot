#ifndef PERSONALITY_HPP
#define PERSONALITY_HPP

#include <cstdint>
#include <cstring>

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class Personality {
public:
    // Family-level traits (set by torso personality module, broadcast to all subsystems)
    int8_t base_aggression;      // 0-127: how strongly aggression is displayed (-128 to +127)
    int8_t base_curiosity;       // 0-127: how strongly curiosity is displayed
    int8_t base_fear;            // 0-127: how strongly fear is displayed
    int8_t base_affection;       // 0-127: how strongly affection is displayed
    
    Personality() 
        : base_aggression(60), base_curiosity(70), base_fear(20), base_affection(40)
    {
    }
    
    // Copy constructor
    Personality(const Personality& other) {
        std::memcpy(this, &other, sizeof(Personality));
    }
    
    // Assignment operator
    Personality& operator=(const Personality& other) {
        std::memcpy(this, &other, sizeof(Personality));
        return *this;
    }
    
    // Equality operator
    bool operator==(const Personality& other) const {
        return (base_aggression == other.base_aggression &&
                base_curiosity == other.base_curiosity &&
                base_fear == other.base_fear &&
                base_affection == other.base_affection);
    }
    
    bool operator!=(const Personality& other) const {
        return !(*this == other);
    }
    
    /**
     * Get intensity multiplier (0.5 to 2.0) based on trait value.
     * 
     * Used to scale how strongly mood effects are displayed:
     * - Negative values: dampen effects (0.5x to 1.0x)
     * - Zero: neutral (1.0x)
     * - Positive values: amplify effects (1.0x to 2.0x)
     * 
     * @param trait_value Personality trait (-128 to +127)
     * @return Multiplier (0.5 to 2.0)
     */
    float getIntensityMultiplier(int8_t trait_value) const {
        if (trait_value < 0) {
            // Negative: dampen effects (ranges from 0.5x at -128 to 1.0x at 0)
            return 0.5f + (trait_value / -128.0f) * 0.5f;
        } else {
            // Positive: amplify effects (ranges from 1.0x at 0 to 2.0x at +127)
            return 1.0f + (trait_value / 127.0f) * 1.0f;
        }
    }
    
    /**
     * Get saturation multiplier for RGB color effects.
     * Used to scale the vividness of mood-based color overlays.
     * 
     * @param trait_value Personality trait
     * @return Multiplier for RGB intensity (0.5 to 2.0)
     */
    float getRGBMultiplier(int8_t trait_value) const {
        return getIntensityMultiplier(trait_value);
    }
};

#endif // PERSONALITY_HPP
