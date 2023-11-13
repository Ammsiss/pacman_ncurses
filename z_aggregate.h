#pragma once

enum class DirectionEval
{
    increment,
    decrement,
};

enum class Direction
{
    up,
    down,
    left,
    right,
    null,
};

struct Vec
{
    int y{};
    int x{};
};