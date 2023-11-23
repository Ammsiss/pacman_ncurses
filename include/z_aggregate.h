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
        yellow_blue,
        pink_blue,
        red_blue,
        orange_blue,
        cyan_blue,
        null,
    };
}

namespace CellName
{
    enum CellNames
    {
        pelletEaten = 10000,
        perimeterValue,
        obstacleValue,
        notEaten,
        ghostBox,
        portalLeft,
        portalRight,
    };
}

enum class GhostState
{
    validDirection,
    invalidDirection,
    insidePacman,
};

enum class LevelState
{
    levelClear,
    pacmanDead,
    normalMovement,
};
struct Vec
{
    int y{};
    int x{};
};

