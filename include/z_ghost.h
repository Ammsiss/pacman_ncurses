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
#include "z_inky.h"

//std
#include <vector>
#include <chrono>

class Pacman;
class Pinky;
class Inky;
class Blinky;
class Ghost
{
public:
    //Constructor
    Ghost();

    //public methods
    bool timeToMove(Window& win, Pinky& pinky, Inky& inky, Blinky& blinky, Pacman& pacman, bool powerPelletTimer, LevelState ateWhichGhost, 
        int& score, std::chrono::time_point<std::chrono::high_resolution_clock>& lastTime, std::chrono::milliseconds& interval); 
        
    void printGhost(Window& win);

    // getters
    Color::ColorPair getGhostColor();
    Vec getGhostVec();
    
    void setSpeed();
    void setGhostVec();

private:
    //private variables

    //ghost attributes
    Direction m_direction;
    Vec m_ghostVec;
    Color::ColorPair m_ghostColor;
    //portal coorinates
    int m_rightPortalX;
    int m_leftPortalX;
    int m_portalY;
    //timer variables
    std::chrono::milliseconds m_interval;
    std::chrono::milliseconds m_ghostIntervalStorage;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    bool setValidDirection(Window& win, Pacman& pacman, bool powerPelletTimer);
    std::vector<Vec> createGhostPath(Vec start, Window& win, Vec target);
    bool oppositeDirectionCheck(Direction directionCheck);
    GhostState moveGhostInValidDirection(Window& win, Pacman& pacman);
    void setY(PositionChange direction);
    void setX(PositionChange direction);
    bool obstacleBoundsCheck(Window& win);
    bool perimeterBoundsCheck(Window& win);
    bool pacmanCollisionCheck(Pacman& pacman);

    // printing
    void eraseLastPosition(Window& win);
    void checkForAndPrintOverLaps(Window& win, Pinky& pinky, Inky& inky, Blinky& blinky, bool powerPelletTimer);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Pinky& pinky, Inky& inky, Blinky& blinky);
    void printOverLap(Window& win, Color::ColorPair overLapColor, bool powerPelletTimer);
    void printAndRefreshGhost(Window& win, bool powerPelletActive);
};