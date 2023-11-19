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

enum class MenuSelection
{
    start,
    controls,
    exit,
};

namespace Color
{
    enum ColorPair : short
    {
        default_color,
        red_black,
        cyan_black,
        pink_black,
        blue_black,
        yellow_black,
        white_black,
        orange_black,
        null,
    };
}

struct Vec
{
    int y{};
    int x{};
};

