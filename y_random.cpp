#include <random>
#include "z_aggregate.h"

namespace Random
{
    std::mt19937 mt{ std::random_device{}() };

    std::uniform_int_distribution<int> randomDirection{ 0, 3 };
}