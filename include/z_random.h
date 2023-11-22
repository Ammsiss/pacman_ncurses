#pragma once

#include <random>
#include "z_aggregate.h"

namespace Random
{
    extern std::mt19937 mt;

    extern std::uniform_int_distribution<int> randomDirection;
    extern std::uniform_int_distribution<int> randomGhostTargetY;
    extern std::uniform_int_distribution<int> randomGhostTargetX;
}