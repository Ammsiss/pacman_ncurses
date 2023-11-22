#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pacman.h"
#include "z_blinky.h"

//std
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Pacman;

class Pinky
{
public:
    bool timeToMove(Window& win, Pacman& pacman, Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4, Blinky& blinky);

private:
    //private variables

    //ghost attributes
    Vec m_pinkyVec{15, 12};
    Color::ColorPair m_pinkyColor{Color::pink_black};
    //portal coorinates
    int m_rightPortalX{26};
    int m_leftPortalX{1};
    int m_portalY{14};
    //timer variables
    std::chrono::milliseconds m_pinkyInterval{300ms};
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    bool setDirection(Window& win, Pacman& pacman, Blinky& blinky);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Ghost& g1, Ghost& g2, Ghost& g3, Ghost& g4);
    void printOverLap(Window& win, Color::ColorPair overLapColor);
    void printAndRefreshGhost(Window& win);
};