#pragma once

enum class PositionChange
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

enum class GhostColor
{
    red,
    orange,
    pink,
    cyan,
    null,
};

struct Vec
{
    int y{};
    int x{};
};

