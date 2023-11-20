//user
#include "z_aggregate.h"

//std
#include <random>
namespace Random
{
    std::mt19937 mt{ std::random_device{}() };

    std::uniform_int_distribution<int> randomDirection{ 0, 3 };
}