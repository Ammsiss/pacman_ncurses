#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pacman.h"
#include "z_pinky.h"
#include "z_inky.h"
#include "z_ghost.h"

//std
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Pacman;
class Inky;
class Ghost;
class Pinky;
class Blinky
{
public:
    bool timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Inky& inky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, int& score);
    Vec getBlinkyVec();
    Color::ColorPair getBlinkyColor();
    void printGhost(Window& win);

    void setSpeed();
    void setGhostVec();

private:
    //private variables

    //ghost attributes
    Vec m_blinkyVec{15, 14};
    Color::ColorPair m_blinkyColor{Color::red_black};
    //portal coorinates
    int m_rightPortalX{26};
    int m_leftPortalX{1};
    int m_portalY{14};
    //timer variables
    std::chrono::milliseconds m_blinkyInterval{200ms};
    std::chrono::milliseconds m_blinkyIntervalStorage{};
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    bool setDirection(Window& win, Pacman& pacman, bool powerPelletActive);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Pinky& pinky, Inky& inky, Ghost& clyde);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Pinky& pinky, Inky& inky, Ghost& clyde);
    void printOverLap(Window& win, Color::ColorPair overLapColor);
    void printAndRefreshGhost(Window& win, bool powerPelletActive);
};