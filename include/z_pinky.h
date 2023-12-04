#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"
#include "z_pacman.h"
#include "z_inky.h"
#include "z_ghost.h"
#include "z_blinky.h"

//std
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Pacman;
class Inky;
class Ghost;
class Blinky;
class Pinky
{
public:
    bool timeToMove(Window& win, Pacman& pacman, Inky& inky, Blinky& blinky, Ghost& clyde, bool powerPelletTimer, LevelState ateWhichGhost, 
        int& score, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime, std::chrono::milliseconds& interval);
        
    Vec getPinkyVec();
    Color::ColorPair getPinkyColor();
    void printGhost(Window& win);

    void setSpeed();
    void setGhostVec();

private:
    //private variables

    //ghost attributes
    Vec m_pinkyVec{15, 15};
    Color::ColorPair m_pinkyColor{Color::pink_black};
    //portal coorinates
    int m_rightPortalX{26};
    int m_leftPortalX{1};
    int m_portalY{14};
    //timer variables
    std::chrono::milliseconds m_pinkyInterval{300ms};
    std::chrono::milliseconds m_pinkyIntervalStorage{};
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    bool setDirection(Window& win, Pacman& pacman, Blinky& blinky, bool powerPelletActive);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Inky& inky, Blinky& blinky, Ghost& clyde, bool powerPelletActive);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Inky& inky, Blinky& blinky, Ghost& clyde);
    void printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletActie);
    void printAndRefreshGhost(Window& win, bool powerPelletActive);
    void ghostFlashing(Window& win, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime, std::chrono::milliseconds& interval, std::chrono::time_point<std::chrono::high_resolution_clock>& currentTime);
};
