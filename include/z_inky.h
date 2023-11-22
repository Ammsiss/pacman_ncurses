#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pacman.h"
#include "z_blinky.h"
#include "z_ghost.h"
#include "z_pinky.h"

//std
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Pacman;
class Pinky;
class Blinky;
class Ghost;

class Inky
{
public:
    bool timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Blinky& blinky, Ghost& clyde);
    Vec getInkyVec();
    Color::ColorPair getInkyColor();

private:
    //private variables

    //ghost attributes
    Vec m_inkyVec{15, 12};
    Color::ColorPair m_inkyColor{Color::cyan_black};
    //portal coorinates
    int m_rightPortalX{26};
    int m_leftPortalX{1};
    int m_portalY{14};
    //timer variables
    std::chrono::milliseconds m_inkyInterval{175ms};
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    bool setDirection(Window& win, Pacman& pacman);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Pinky& pinky, Blinky& blinky, Ghost& clyde);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Pinky& pinky, Blinky& blinky, Ghost& clyde);
    void printOverLap(Window& win, Color::ColorPair overLapColor);
    void printAndRefreshGhost(Window& win);
};