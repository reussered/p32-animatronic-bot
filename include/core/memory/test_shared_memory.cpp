#include "SharedMemory.hpp"
#include <iostream>
#include <string>

// All communication logic is now internal to SharedMemory

struct TestStruct {
    int a;
    float b;
};

int main() {
    SharedMemory shm;
    TestStruct t1 = {42, 3.14f};
    shm.write<TestStruct>("test", &t1);

    TestStruct* t2 = shm.read<TestStruct>("test");
    std::cout << "Read values: a=" << t2->a << ", b=" << t2->b << std::endl;

    // Overwrite and read again
    TestStruct t3 = {7, 2.71f};
    shm.write<TestStruct>("test", &t3);
    TestStruct* t4 = shm.read<TestStruct>("test");
    std::cout << "After overwrite: a=" << t4->a << ", b=" << t4->b << std::endl;

    // Test new entry
    int val = 123;
    shm.write<int>("intval", &val);
    int* valptr = shm.read<int>("intval");
    std::cout << "Read int: " << *valptr << std::endl;

    return 0;
}
