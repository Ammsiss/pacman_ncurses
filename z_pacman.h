#pragma once

// external
#include <ncurses.h>

// user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_vectorspace.h"
#include "z_ghost.h"

// std
#include <vector>
#include <chrono>
class Pacman
{
public:
    // constructor
    Pacman();
    // public method
    bool timeToMove(Window& win, Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);

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
    void printPacmanBasedOnDirection(Window& win);

    // setting direction
    void setDirection();
    bool movePacmanBasedOnDirection(Window& win, Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);
    bool obstacleBoundsCheck(Window& win);
    bool ghostCollisionCheck(Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);

    // getters
    void getUserInputAndSetDirection(Window& win);

    // setters
    void setY(PositionChange direction);
    void setX(PositionChange direction);
    void updateEatenPellets(Window& win);
};