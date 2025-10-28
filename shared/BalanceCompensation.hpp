#ifndef BALANCE_COMPENSATION_HPP
#define BALANCE_COMPENSATION_HPP

// Type ID definition - matches SharedMemory.hpp
typedef int shared_type_id_t;

class BalanceCompensation {
public:
    uint32_t version;

    // Balance compensation state
    int8_t balance_offset_x;
    int8_t balance_offset_y;
    bool compensation_active;

};

#endif // BALANCE_COMPENSATION_HPP