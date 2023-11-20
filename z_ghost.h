#pragma once

//external
#include <ncurses.h>

//user
#include "z_aggregate.h"
#include "z_window.h"
#include "z_obstacle.h"

//std
#include <vector>
#include <chrono>
class Ghost
{
public:
    //Constructor
    Ghost(std::chrono::milliseconds speed, Color::ColorPair ghostColor);

    //public methods
    void timeToMove(Window& win, Ghost& g1, Ghost& g2, Ghost& g3); 

    // getters
    Color::ColorPair getGhostColor();
    Vec getGhostVec();

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
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

    //private methods

    //setting valid directionss
    void setValidDirection(Window& win);
    bool oppositeDirectionCheck(Direction directionCheck);
    bool moveGhostInValidDirection(Window& win);
    void setY(PositionChange direction);
    void setX(PositionChange direction);
    bool obstacleBoundsCheck(Window& win);
    bool perimeterBoundsCheck(Window& win);

    // printing
    void eraseLastPosition(Window& win);
    void CheckForAndPrintOverLaps(Window& win, Ghost& g1, Ghost& g2, Ghost& g3);
    void printPelletBackIfNotEaten(Window& win);
    Color::ColorPair checkGhostOverLap(Ghost& g1, Ghost& g2, Ghost& g3);
    void printOverLap(Window& win, Color::ColorPair overLapColor);
    void printAndRefreshGhost(Window& win);
};