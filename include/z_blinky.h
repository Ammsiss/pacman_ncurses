#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pacman.h"

//std
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Pacman;

class Blinky
{
public:
    bool timeToMove(Window& win, Pacman& pacman, Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);

private:
    //private variables

    //ghost attributes
    Direction m_direction;
    Vec m_blinkyVec{15, 12};
    Color::ColorPair m_blinkyColor{Color::red_black};
    //portal coorinates
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;
    //timer variables
    std::chrono::milliseconds m_blinkyInterval{200ms};
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    bool setDirection(Window& win, Pacman& pacman);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);
    void printOverLap(Window& win, Color::ColorPair overLapColor);
    void printAndRefreshGhost(Window& win);
};