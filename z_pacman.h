#pragma once

// external
#include <ncurses.h>

// user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_vectorspace.h"

// std
#include <vector>
#include <chrono>
class Pacman
{
public:
    // constructor
    Pacman();
    // public method
    void timeToMove(Window& win);

private:
    // private variables

    // pacman attributes
    Direction m_direction;
    Vec m_pacVec;
    char m_userInput;
    int m_score{0};

    // portal coordinates
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;

    // timing
    std::chrono::milliseconds m_interval;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    // printing
    void erase(Window& win);
    void printScore(Window& win);

    // setting direction
    bool obstacleBoundsCheck(Window& win);
    void printPacmanBasedOnDirection(Window& win);
    void movePacmanBasedOnDirection(Window& win);
    void setDirection();

    // getters
    void getUserInputAndSetDirection(Window& win);

    // setters
    void setY(PositionChange direction);
    void setX(PositionChange direction);
    void updateEatenPellets(Window& win);
};