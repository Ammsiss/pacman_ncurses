#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pacman.h"
#include "z_pinky.h"
#include "z_blinky.h"
#include "z_ghost.h"
#include "typealiases.h"

//std
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Pacman;
class Blinky;
class Pinky;
class Ghost;
class Inky
{
public:
    bool timeToMove(Window& win, Pacman& pacman, Pinky& pinky, Blinky& blinky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, 
        int& score, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval);
        
    Vec getInkyVec();
    Color::ColorPair getInkyColor();
    void printGhost(Window& win);

    void setSpeed();
    void setGhostVec();

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
    std::chrono::milliseconds m_inkyInterval{185ms};
    std::chrono::milliseconds m_inkyPathInterval{7s};
    std::chrono::milliseconds m_inkyIntervalStorage{};
    TypeAlias::timepoint m_lastTime;
    TypeAlias::timepoint m_pathLastTime;

    //private methods

    //setting valid directionss
    bool setDirection(Window& win, Pacman& pacman, bool powerPelletActive);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Pinky& pinky, Blinky& blinky, Ghost& clyde, bool powerPelletActive);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Pinky& pinky, Blinky& blinky, Ghost& clyde);
    void printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletActie);
    void printAndRefreshGhost(Window& win, bool powerPelletActive);
    void ghostFlashing(Window& win, TypeAlias::timepoint& lastTime, std::chrono::milliseconds& interval, TypeAlias::timepoint& currentTime);
};
